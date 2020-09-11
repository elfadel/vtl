//SPDX-License-Identifier: GPL-2.0

/*
* @file :		adaptor_send.c
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		12/2019
* @version :		1.0
* @brief :
*/

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/types.h>

#include "../include/common/util.h"
#include "../include/vtl.h"
#include "adaptor_send.h"

// TODO: place this vars in .h
static const char *TC_GLOBAL_NS = "/sys/fs/bpf/tc/globals";
static const char *BPF_MAP_NAME = "EGRESS_SOCKS_MAP";
//static const char *BPF_ACKS_MAP = "EGRESS_SOCKS_WIN_MAP";
static const char *BPF_SEQ_MAPS = "NUM_SEQ_MAP";

long get_map_fd(void) {
	char pinned_file[256];
	snprintf(pinned_file, sizeof(pinned_file), "%s/%s", TC_GLOBAL_NS, BPF_MAP_NAME);

	return bpf_obj_get(pinned_file);
}

long get_seq_maps_fd(void) {
	char pinned_file[256];
	snprintf(pinned_file, sizeof(pinned_file), "%s/%s", TC_GLOBAL_NS, BPF_SEQ_MAPS);

	return bpf_obj_get(pinned_file);
}

int adaptor_create_raw_sock(int domain, int proto, char *err_buf) {
	int sock_fd;

	// Submit request for a new raw socket descriptor
	if((sock_fd = socket(domain, SOCK_RAW, proto)) < 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "ERR: socket() failed \"%s\"\n",
							strerror(errno));
		return -1;
	}

	long map_fd = get_map_fd();
	if(map_fd < 0) {
		fprintf(stderr, "ADAPTOR_SEND: Unable to get EGRESS_SOCKS_MAP\n");
	}
	else {
		int index = 0;
		struct sock_state_t sk_state = {};
		sk_state.sk_fd = (__u32)sock_fd;
		sk_state.event = IDLE;

		long ret = bpf_map_update_elem(map_fd, &index, &sk_state, BPF_ANY);

		if(ret != 0)
			fprintf(stderr, "ADAPTOR_SEND: Unable to init EGRESS_SOCKS_MAP\n");
		else
			fprintf(stderr, "ADAPTOR_SEND: Init OK in EGRESS_SOCKS_MAP\n");
	}

	long seq_maps_fd = get_seq_maps_fd();
	if(seq_maps_fd < 0) {
		fprintf(stderr, "ADAPTOR_SEND: Unable to get NUM_SEQ_MAP\n");
	}
	else {

		int index = 0;
		uint16_t num_seq = 0;

		long ret = bpf_map_update_elem(seq_maps_fd, &index, &num_seq, BPF_ANY);

		if(ret != 0)
			fprintf(stderr, "ADAPTOR_SEND: Unable to init value in NUM_SEQ_MAP\n");
		else
			fprintf(stderr, "ADAPTOR_SEND: Init OK in NUM_SEQ_MAP\n");
	}

	//long acks_map_fd = get_acks_map_fd();

	return sock_fd;
}

// Set flag so socket expects us to provide IPv4 header
static int enable_ip4_hdr_gen(int sock_fd) {

	const int on = 1;
	if(setsockopt(sock_fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on)) < 0) {
		perror("ERR: setsockopt() failed to set IP_HDRINCL");
	return -1;
	}

	return 0;
}

static int bind_raw_sock_to_interface(char *interface, int sock_fd) {

	int sd;
	struct ifreq ifr;

	// Submit request to a socket descriptor to look up interface
	if((sd = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
		perror("ERR: socket() failed to get socket descriptor for using ioctl().");
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
		perror("ERR: ioctl() failed to find interface.");
		return -1; // TODO:
	}
	close(sd);

	/*printf("Index for interface %s is %i\n",
			interface, ifr.ifr_ifindex);*/

	// Bind socket to interface
	if(setsockopt(sock_fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof(ifr)) < 0) {
		perror("ERR: setsockopt() failed to bind to interface.");
		return -1; // TODO:
	}

	return 0;
}

int adaptor_config_raw_sock(int sock_fd, char *interface, char *err_buf) {

	int ret = enable_ip4_hdr_gen(sock_fd);

	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "ERR: enable_ip4_hdr_gen() failed.\n");
		return ret;
	}

	ret = bind_raw_sock_to_interface(interface, sock_fd);

	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "ERR: bind_raw_sock_to_interface() failed.\n");
		return ret;
	}

	return 0;
}

static int create_ip4_hdr(struct ip *iphdr, char *dst_ip, char *src_ip, int *ip_flags, size_t send_data_len) {

	int status;
	void *tmp;

	struct addrinfo hints, *res;
	struct sockaddr_in *ipv4;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;					// TODO: Should be SOCK_RAW
	hints.ai_flags = hints.ai_flags | AI_CANONNAME;

	// Resolve target==dst_ip using getaddrinfo()
	if((status = getaddrinfo(dst_ip, NULL, &hints, &res)) != 0) {
		fprintf(stderr, "ERR: getaddrinfo() failed: %s\n", gai_strerror(status));
		return -1;
	}

	// struct sockaddr_in *ipv4
	ipv4 = (struct sockaddr_in *) res->ai_addr;
	tmp = &(ipv4->sin_addr);
	if(inet_ntop(AF_INET, tmp, dst_ip, INET_ADDRSTRLEN) == NULL) {
		status = errno;
		fprintf(stderr, "ERR: inet_ntop() failed.\n Error message: %s", strerror(status));
		return -1;
	}
	freeaddrinfo(res);

	/* IPv4 header */

	// IPv4 *header* length (4 bits): Number of 32-bit words in header = 5
	iphdr->ip_hl = IP4_HDR_LEN / sizeof(uint32_t);

	// IP version (4 bits): IPv4
	iphdr->ip_v = 4;

	//Type of Service (8 bits)
	iphdr->ip_tos = 0;

	// Total length of datagram (16 bits): IP header + VTL header + VTL payload
	iphdr->ip_len = htons(IP4_HDR_LEN + sizeof(vtl_hdr_t) + send_data_len); // previous name of payload_len = snd_datalen

	// ID sequence number (16 bits): unused, since single datagram
	iphdr->ip_id = htons(0);

	/* Flags, and fragmentation offset (3, 13 bits): 0 since single datagram*/

	// Zero (1 bit)
	ip_flags[0] = 0;

	// Do not fragment flag (1 bit)
	ip_flags[1] = 0;

	// More fragments following flag (1 bit)
	ip_flags[2] = 0;

	// Fragmentation offset(13 bits)
	ip_flags[3] = 0;

	iphdr->ip_off = htons((ip_flags[0] << 15)
								+ (ip_flags[1] << 14)
								+ (ip_flags[2] << 13)
								+  ip_flags[3]);

	// TTL (8 bits): default to maximum value
	iphdr->ip_ttl = 255;

	// Transport layer protocol (8 bits)
	iphdr->ip_p = IPPROTO_VTL;

	// Source IPv4 address (32 bits)
	if((status = inet_pton(AF_INET, src_ip, &iphdr->ip_src)) != 1) {
		fprintf(stderr, "ERR: inet_pton() failed.\n Error message: %s", strerror(status));
		return -1;
	}

	// Destination IPv4 address (32 bits)
	if((status = inet_pton(AF_INET, dst_ip, &iphdr->ip_dst)) != 1) {
		fprintf(stderr, "ERR: inet_pton() failed\n Error message: %s", strerror(status));
		return -1;
	}

	// IPv4 header checksum (16 bits): set to 0 when calculating checksum
	iphdr->ip_sum = 0;
	iphdr->ip_sum = checksum((uint16_t *) iphdr, IP4_HDR_LEN);

	return 0;
}

static void fill_sockaddr_in(struct sockaddr_in *to, struct ip *iphdr) {

	memset(to, 0, sizeof(struct sockaddr_in));
	to->sin_family = AF_INET;
	to->sin_addr.s_addr = iphdr->ip_dst.s_addr;
}

static void ip4_pkt_assemble(uint8_t *send_pkt, struct ip *iphdr,
								vtl_hdr_t *vtlh, uint8_t *send_data, size_t send_data_len) {

	memcpy(send_pkt, iphdr, IP4_HDR_LEN);

	// Next part of packet is upper layer protocol header: VTL header
	memcpy((send_pkt + IP4_HDR_LEN), vtlh, sizeof(vtl_hdr_t));

	// Finally, add the VTL data, i.e. application payload
	memcpy(send_pkt + IP4_HDR_LEN + sizeof(vtl_hdr_t), send_data, send_data_len);
}

static int send_packet(int sock_fd, struct sockaddr_in *to, uint8_t *send_pkt,
												size_t send_data_len) {

	size_t ip_pkt_size = IP4_HDR_LEN + sizeof(vtl_hdr_t) + send_data_len;
	if(sendto(sock_fd, send_pkt, ip_pkt_size, 0, (struct sockaddr *) to,
				sizeof(struct sockaddr)) < 0) {
		perror("ERR: sendto() failed.");
		return -1;
	}

	return 0;
}

int adaptor_send_packet(int sock_fd, uint8_t *send_pkt, vtl_hdr_t *vtlh,
							struct ip *iphdr, char *dst_ip, char *src_ip,
							int *ip_flags, uint8_t *send_data, size_t send_data_len, char *err_buf)
							{

	int ret;
	struct sockaddr_in to;

	ret = create_ip4_hdr(iphdr, dst_ip, src_ip, ip_flags, send_data_len);
	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "ERR: create_ip4_hdr() failed.\n");
		return ret;
	}

	// Fill destination sock_addr_in
	fill_sockaddr_in(&to, iphdr);

	ip4_pkt_assemble(send_pkt, iphdr, vtlh, send_data, send_data_len);

	ret = send_packet(sock_fd, &to, send_pkt, send_data_len);
	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "ERR: send_packet() failed.\n");
		return ret;
	}

	return 0;
}
