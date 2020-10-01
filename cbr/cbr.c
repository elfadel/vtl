/*
* @file:		cbr.c
* @authors:		El-Fadel Bonfoh
* @date:		12/2019
* @version:		1.0
* @brief:
*/

#include <errno.h>
#include <poll.h>
#include <netdb.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if_link.h>
#include <sys/socket.h>

#include "cbr.h"

// Globals
static const char *TC_GLOBAL_NS = "/sys/fs/bpf/tc/globals";
static const char *BPF_ACK_MAP = "ACK_WND_MAP";
static const char *BPF_SEQ_MAP = "NUM_SEQ_MAP";
static const char *BPF_WND_MAP = "LEN_WND_MAP";

static long __get_map_fd(const char* map_filename) {

	char pinned_file[256];
	snprintf(pinned_file, sizeof(pinned_file), "%s/%s", TC_GLOBAL_NS, map_filename);

	return bpf_obj_get(pinned_file);
}

int cbr_create_raw_sock(int domain, int proto, char *err_buf) {

	printf("[CBR]: cbr_create_raw_sock() called !\n");

	int sock_fd;
	long ret, ack_map_fd, seq_map_fd, wnd_map_fd;

	if((sock_fd = socket(domain, SOCK_RAW, proto)) < 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, 
			"[CBR]: Error - socket() failed \"%s\"\n", strerror(errno));
		return -1;
	}

	ack_map_fd = __get_map_fd(BPF_ACK_MAP);
	if(ack_map_fd < 0)
		fprintf(stderr, "[CBR]: Warning - unable to get ACK_WND_MAP\n");
	else {

		for(int id = 0; id < 4; id++) {
			struct sock_state_t sk_state = {};
			sk_state.sk_fd = (__u32)sock_fd;
			sk_state.event = TIMEOUT;

			ret = bpf_map_update_elem(ack_map_fd, &id, &sk_state, BPF_ANY);

			if(ret != 0)
				fprintf(stderr, "[CBR]: Warning - unable to init ACK_WND_MAP at id = %d\n", id);
		}

		fprintf(stderr, "[CBR]: ACK_WND_MAP init !\n");
	}

	seq_map_fd = __get_map_fd(BPF_SEQ_MAP);
	if(seq_map_fd < 0)
		fprintf(stderr, "[CBR]: Warning - unable to get NUM_SEQ_MAP\n");
	else {
		int index = 0;
		uint16_t num_seq = 0;

		ret = bpf_map_update_elem(seq_map_fd, &index, &num_seq, BPF_ANY);

		if(ret != 0)
			fprintf(stderr, "[CBR]: Warning - unable to init NUM_SEQ_MAP\n");
		else
			fprintf(stderr, "[CBR]: NUM_SEQ_MAP init !\n");
	}

	wnd_map_fd = __get_map_fd(BPF_WND_MAP);
	if(wnd_map_fd < 0)
		fprintf(stderr, "[CBR]: Warning - unable to get LEN_WND_MAP\n");
	else {
		int index = 0;
		uint16_t init_len = 1;

		ret = bpf_map_update_elem(wnd_map_fd, &index, &init_len, BPF_ANY);

		if(ret != 0)
			fprintf(stderr, "[CBR]: Warning - unable to init LEN_WND_MAP\n");
		else
			fprintf(stderr, "[CBR]: LEN_WND_MAP init !\n");
	}

	return sock_fd;
}

// Set flag so socket expects us to provide IPv4 header
static int __enable_ip4_hdr_gen(int sock_fd) {

	const int on = 1;
	if(setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
		perror("[CBR]: Error - setsockopt() failed to set IP_HDRINCL");
		return -1;
	}

	return 0;
}

static int __bind_raw_sock_to_interface(char *interface, int sock_fd) {

	int sd;
	struct ifreq ifr;

	// Submit request to a socket descriptor to look up interface
	if((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("ERR: socket() failed to get socket descriptor.");
		return -1;
	}

	/**
	* Use ioctl() to lookup interface index which we will use to
	* bind socket descriptor sd to specified interface with setsockopt() since
	* none of the other arguments of sendto() specify which interface to use.
	*/
	memset(&ifr, 0, sizeof(ifr));
	snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", interface);
	if(ioctl(sd, SIOCGIFINDEX, &ifr) < 0) {
		perror("[CBR]: Error - ioctl() failed to find interface.");
		return -1;
	}
	close(sd);

	// Bind socket to interface
	if(setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
		perror("[CBR]: Error - setsockopt() failed to bind to interface.");
		return -1; 
	}

	return 0;
}

int cbr_config_raw_sock(int sock_fd, char *interface, char *err_buf) {

	int ret;

	ret = __enable_ip4_hdr_gen(sock_fd);
	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "[CBR]: Error - enable_ip4_hdr_gen() failed.\n");
		return ret;
	}

	ret = __bind_raw_sock_to_interface(interface, sock_fd);
	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "[CBR]: Error - bind_raw_sock_to_interface() failed.\n");
		return ret;
	}

	return 0;
}

struct xsk_socket_info* 
	cbr_create_and_config_xsk_sock(char *ifname, __u32 xdp_flags,
                        		__u16 xsk_bind_flags, int xsk_if_queue,
                        		struct xsk_umem_info *umem, char *err_buf) 
{
	void *packet_buffer;
	uint64_t packet_buffer_size;
	struct xsk_socket_info *xsk_socket = NULL;
	struct rlimit rlim = {RLIM_INFINITY, RLIM_INFINITY};

	/*
	* Allow unlimited locking of memory, so all memory needed for packet
	* buffers can be locked.
	*/
	if(setrlimit(RLIMIT_MEMLOCK, &rlim)) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, 
			"[CBR]: Error - setrlimit() \"%s\"\n", strerror(errno));
		goto bad;
	}

	// Allocate memory for NUM_FRAMES of the default XDP frame size
	packet_buffer_size = NUM_FRAMES * FRAME_SIZE;
	if(posix_memalign(&packet_buffer, getpagesize(), packet_buffer_size)) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, 
			"[CBR]: Error - Can't allocate buffer memory \"%s\"\n", strerror(errno));
		goto bad;
	}

	// Initialize shared packet_buffer for umem usage
	umem = configure_xsk_umem(packet_buffer, packet_buffer_size);
	if(umem == NULL) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, 
			"[CBR]: Error - Can't create umem \"%s\"\n", strerror(errno));
		goto bad;
	}

	// Open and configure the AF_XDP(xsk) socket
	xsk_socket = xsk_configure_socket(ifname, xdp_flags, xsk_bind_flags, xsk_if_queue, umem);
	if(xsk_socket == NULL) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, 
			"[CBR]: Error - Can't setup AF_XDP socket \"%s\"\n", strerror(errno));
		goto bad;
	}

	return xsk_socket;
bad:
	//TODO: cleanup reserved mem
	return NULL;
}