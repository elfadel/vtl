//SPDX-License-Identifier: GPL-2.0

/*
* @file :		service_api.c
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		12/2019
* @version :		0.1
* @brief :
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <linux/if_link.h> // XDP flags

#include "service_api.h"
#include "../include/common/util.h"
#include "../adaptor/adaptor_recv.h"
#include "../adaptor/adaptor_send.h"

vtl_socket_t* vtl_init(vtl_host_role role, char *src_ip, char *dst_ip, char *ifname, char *err_buf) {

	int ret;
	__u32 xdp_flags = 0;
	__u16 xsk_bind_flags = 0;
	int xsk_if_queue = 0;

	struct xsk_umem_info umem = {0}; // always initialize
	vtl_socket_t *vtl_sock = NULL;
	vtl_sock = (vtl_socket_t *)malloc(sizeof(vtl_socket_t));
	memset(vtl_sock, 0, sizeof(vtl_socket_t));

	switch(role){
		case VTL_RECVER_ROLE:
			// xdp ==> config af_xdp sock
			xdp_flags &= ~XDP_FLAGS_MODES;
			xdp_flags |= XDP_FLAGS_SKB_MODE;
			xsk_bind_flags &= XDP_ZEROCOPY;
			xsk_bind_flags |= XDP_COPY;

			vtl_sock->xsk_socket = adaptor_create_xsk_sock(ifname, xdp_flags, xsk_bind_flags, xsk_if_queue, &umem, err_buf);
			if(vtl_sock->xsk_socket == NULL) { // VTL socket init failed !
				return NULL;
			}
			vtl_sock->recv_data = allocate_ustrmem(VTL_DATA_SIZE);
			break;

		case VTL_SENDER_ROLE:
			// tc ==> config raw socket
			vtl_sock->af_inet_sock = adaptor_create_raw_sock(AF_INET, IPPROTO_RAW, err_buf);
			if(vtl_sock->af_inet_sock < 0) { // VTL socket init failed !
				return NULL;
			}
			ret = adaptor_config_raw_sock(vtl_sock->af_inet_sock, ifname, err_buf);
			if(ret < 0)
				return NULL;
			vtl_sock->send_data = allocate_ustrmem(VTL_DATA_SIZE);
			vtl_sock->send_pkt = allocate_ustrmem(IP_MAXPACKET);
			vtl_sock->ip_flags = allocate_intmem(4);
			break;

		case VTL_BOTH_ROLE:
			xdp_flags &= ~XDP_FLAGS_MODES;
			xdp_flags |= XDP_FLAGS_SKB_MODE;
			xsk_bind_flags &= XDP_ZEROCOPY;
			xsk_bind_flags |= XDP_COPY;

			vtl_sock->xsk_socket = adaptor_create_xsk_sock(ifname, xdp_flags, xsk_bind_flags,
																xsk_if_queue, &umem, err_buf);
			if(vtl_sock->xsk_socket == NULL) { // VTL socket init failed !
				return NULL;
			}
			vtl_sock->recv_data = allocate_ustrmem(VTL_DATA_SIZE);
			vtl_sock->af_inet_sock = adaptor_create_raw_sock(AF_INET, IPPROTO_RAW, err_buf);
			if(vtl_sock->af_inet_sock < 0) { // VTL socket init failed !
				return NULL;
			}
			ret = adaptor_config_raw_sock(vtl_sock->af_inet_sock, ifname, err_buf);
			if(ret < 0)
				return NULL;
			vtl_sock->send_data = allocate_ustrmem(VTL_DATA_SIZE);
			vtl_sock->send_pkt = allocate_ustrmem(IP_MAXPACKET);
			vtl_sock->ip_flags = allocate_intmem(4);
			break;
		default:
			snprintf(err_buf, VTL_ERRBUF_SIZE, "ERR: unknown mode\n");
			return NULL;
			break;
	}

	strcpy(vtl_sock->src_ip, src_ip);
	if(dst_ip != NULL) {
		strcpy(vtl_sock->dst_ip, dst_ip);
	}
	if(ifname == NULL) {
		strcpy(ifname, "enp0s3"); // TODO: search the first available ifname; look at ui/vtl_ui.c
	}
	strcpy(vtl_sock->ifname, ifname);

	vtl_sock->vtlh.gid = -1;

	return vtl_sock;
}

int vtl_send_data(vtl_socket_t *vtl_sock, uint8_t *data, size_t data_len, char *err_buf) {
	int ret;

	// Fill application payload
	vtl_sock->send_data = data;
	vtl_sock->send_data_len = data_len;

	// Sent VTL packet
	ret = adaptor_send_packet(vtl_sock->af_inet_sock, vtl_sock->send_pkt, &vtl_sock->vtlh, &vtl_sock->iphdr, 
					vtl_sock->dst_ip, vtl_sock->src_ip, vtl_sock->ip_flags,
					vtl_sock->send_data, vtl_sock->send_data_len, err_buf);

	if(ret < 0) { // Failed to send data
		return -1;
	}
	return 0;
}

void vtl_recv_data(vtl_socket_t *vtl_sock, FILE *rx_file) {
	adaptor_recv_data(vtl_sock->xsk_socket, rx_file,
						&vtl_sock->cnt_pkts, &vtl_sock->cnt_bytes);
}
