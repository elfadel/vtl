/*
* @file:		dbr.c
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

#include "dbr.h"

static int __create_ip4_hdr(struct ip *iphdr, char *dst_ip, char *src_ip, 
				int *ip_flags, size_t send_data_len) 
{
	int status;
	void *tmp;
	char *target = dst_ip;

	struct addrinfo hints, *res;
	struct sockaddr_in *ipv4;

	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = 0;
	hints.ai_flags = hints.ai_flags | AI_CANONNAME;

	// Resolve target==dst_ip using getaddrinfo()
	if((status = getaddrinfo(target, NULL, &hints, &res)) != 0) {
		fprintf(stderr, "[DBR]: Error - getaddrinfo() failed: %s\n", 
			gai_strerror(status));
		return -1;
	}

	// struct sockaddr_in *ipv4
	ipv4 = (struct sockaddr_in *) res->ai_addr;
	tmp = &(ipv4->sin_addr);
	if(inet_ntop(AF_INET, tmp, dst_ip, INET_ADDRSTRLEN) == NULL) {
		status = errno;
		fprintf(stderr, "[DBR]: Error - inet_ntop() failed.\n Error message: %s", 
			strerror(status));
		return -1;
	}
	freeaddrinfo(res);

	// IPv4 *header* length (4 bits): Number of 32-bit words in header = 5
	iphdr->ip_hl = IP4_HDR_LEN / sizeof(uint32_t);

	// IP version (4 bits): IPv4
	iphdr->ip_v = 4;

	//Type of Service (8 bits)
	iphdr->ip_tos = 0;

	// Total length of datagram (16 bits): IP header + VTL header + VTL payload
	iphdr->ip_len = htons(IP4_HDR_LEN + sizeof(vtl_hdr_t) + send_data_len);

	// ID sequence number (16 bits): unused, since single datagram
	iphdr->ip_id = htons(0);

	/* Flags, and fragmentation offset (3, 13 bits): 0 since single datagram*/
	ip_flags[0] = 0; // Zero (1 bit)
	ip_flags[1] = 0; // Do not fragment flag (1 bit)
	ip_flags[2] = 0; // More fragments following flag (1 bit)
	ip_flags[3] = 0; // Fragmentation offset(13 bits)
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
		fprintf(stderr, "[DBR]: Error - inet_pton() failed.\n Error message: %s", 
			strerror(status));
		return -1;
	}

	// Destination IPv4 address (32 bits)
	if((status = inet_pton(AF_INET, dst_ip, &iphdr->ip_dst)) != 1) {
		fprintf(stderr, "[DBR]: Error - inet_pton() failed\n Error message: %s", 
			strerror(status));
		return -1;
	}

	// IPv4 header checksum (16 bits): set to 0 when calculating checksum
	iphdr->ip_sum = 0;
	iphdr->ip_sum = checksum((uint16_t *) iphdr, IP4_HDR_LEN);

	return 0;
}

static void __fill_sockaddr_in(struct sockaddr_in *to, struct ip *iphdr) {

	memset(to, 0, sizeof(struct sockaddr_in));
	to->sin_family = AF_INET;
	to->sin_addr.s_addr = iphdr->ip_dst.s_addr;
}

static void __ip4_pkt_assemble(uint8_t *send_pkt, struct ip *iphdr,
				vtl_hdr_t *vtlh, uint8_t *send_data, size_t send_data_len) 
{
	memcpy(send_pkt, iphdr, IP4_HDR_LEN);

	// Next part of packet is upper layer protocol header: VTL header
	memcpy((send_pkt + IP4_HDR_LEN), vtlh, sizeof(vtl_hdr_t));

	// Finally, add the VTL data, i.e. application payload
	memcpy(send_pkt + IP4_HDR_LEN + sizeof(vtl_hdr_t), send_data, send_data_len);
}

static int __send_packet(int sock_fd, struct sockaddr_in *to, 
			 uint8_t *send_pkt, size_t send_data_len) 
{
	size_t ip_pkt_size = IP4_HDR_LEN + sizeof(vtl_hdr_t) + send_data_len;
	if(sendto(sock_fd, send_pkt, ip_pkt_size, 
		    0, (struct sockaddr *) to, sizeof(struct sockaddr)) < 0) 
	{
		perror("[DBR]: Error - sendto() failed.");
		return -1;
	}

	return 0;
}

int dbr_send(int sock_fd, uint8_t *send_pkt, 
		vtl_hdr_t *vtlh, struct ip *iphdr, 
		char *dst_ip, char *src_ip, int *ip_flags, 
		uint8_t *send_data, size_t send_data_len, char *err_buf)
{
	int ret;
	struct sockaddr_in to;

	ret = __create_ip4_hdr(iphdr, dst_ip, src_ip, ip_flags, send_data_len);
	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "[DBR]: Error - create_ip4_hdr() failed.\n");
		return ret;
	}

	// Fill destination sock_addr_in
	__fill_sockaddr_in(&to, iphdr);

	__ip4_pkt_assemble(send_pkt, iphdr, vtlh, send_data, send_data_len);

	ret = __send_packet(sock_fd, &to, send_pkt, send_data_len);
	if(ret != 0) {
		snprintf(err_buf, VTL_ERRBUF_SIZE, "[DBR]: Error - send_packet() failed.\n");
		return ret;
	}

	return 0;
}

static bool __process_packet(struct xsk_socket_info *xsk, uint64_t addr, uint32_t len,
				uint8_t *rx_data, size_t *rx_data_len, 
				uint32_t *cnt_pkts, uint32_t *cnt_bytes) 
{
	uint32_t hdr_size, data_size;

	uint8_t *pkt = xsk_umem__get_data(xsk->umem->buffer, addr);

	struct ethhdr *eth = (struct ethhdr *) pkt;
	struct iphdr *iph = (struct iphdr *)(eth + 1);
	vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
	uint8_t *data = (uint8_t *)(vtlh + 1);

	hdr_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(vtl_hdr_t);
	data_size = len - hdr_size;

	// TODO: Make a linked list or ring buffer
	memcpy(rx_data + *rx_data_len, data, data_size);
	*rx_data += (size_t) data_size;
	
	*cnt_pkts += 1;
	*cnt_bytes += data_size;

	return true;
}

static void __handle_receive_packets(struct xsk_socket_info *xsk, uint8_t *rx_data, 
					size_t *rx_data_len, uint32_t *cnt_pkts, uint32_t *cnt_bytes) 
{
	unsigned int recvd, stock_frames, i;
	uint32_t idx_rx = 0, idx_fq = 0;
	int ret;

	recvd = xsk_ring_cons__peek(&xsk->rx, RX_BATCH_SIZE, &idx_rx);
	if(!recvd)
		return;

	// Stuff the ring with as much frames as possible
	stock_frames = xsk_prod_nb_free(&xsk->umem->fq, xsk_umem_free_frames(xsk));

	if(stock_frames > 0) {
		ret = xsk_ring_prod__reserve(&xsk->umem->fq, stock_frames, &idx_fq);

		// This should not happen, but just in case
		while(ret != stock_frames)
			ret = xsk_ring_prod__reserve(&xsk->umem->fq, recvd, &idx_fq);

		for(i = 0; i < stock_frames; i++)
			*xsk_ring_prod__fill_addr(&xsk->umem->fq, idx_fq++) =
				xsk_alloc_umem_frame(xsk);

		xsk_ring_prod__submit(&xsk->umem->fq, stock_frames);
	}

	*rx_data_len = 0;
	// Process received packets
	for(i = 0; i < recvd; i++) {
		uint64_t addr = xsk_ring_cons__rx_desc(&xsk->rx, idx_rx)->addr;
		uint32_t len = xsk_ring_cons__rx_desc(&xsk->rx, idx_rx++)->len;

		if(!__process_packet(xsk, addr, len, rx_data, rx_data_len, cnt_pkts, cnt_bytes))
			xsk_free_umem_frame(xsk, addr);
	}

	xsk_ring_cons__release(&xsk->rx, recvd);
}

void dbr_recv(struct xsk_socket_info *xsk_socket, 
		uint8_t *rx_data, size_t *rx_data_len,
		uint32_t *cnt_pkts, uint32_t *cnt_bytes) 
{
	// TODO: remove poll code to save resource
  	struct pollfd fds[2];
	memset(fds, 0, sizeof(fds));
	fds[0].fd = xsk_socket__fd(xsk_socket->xsk);
	fds[0].events = POLLIN;

	__handle_receive_packets(xsk_socket, rx_data, rx_data_len, cnt_pkts, cnt_bytes);
}