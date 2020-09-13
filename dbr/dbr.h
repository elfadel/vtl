/*
* @file:		dbr.h
* @authors:		El-Fadel Bonfoh
* @date:		12/2019
* @version:		0.1
* @brief:
*/
#pragma once

#include <bpf/bpf.h>
#include <bpf/xsk.h>
#include <net/if.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/resource.h>

#include "../include/vtl.h"
#include "../include/common/util.h"
#include "../include/common/xsk_user_helpers.h"

/* TODO: reduce the number of arguments */
int dbr_send(int sock_fd, uint8_t *send_pkt, 
		vtl_hdr_t *vtlh, struct ip *iphdr, 
		char *dst_ip, char *src_ip, int *ip_flags,
                uint8_t *send_data, size_t send_data_len, char *err_buf);

void dbr_recv(struct xsk_socket_info *xsk_socket, 
		uint8_t *rx_data, size_t *rx_data_len, 
		uint32_t *cnt_pkts, uint32_t *cnt_bytes);
