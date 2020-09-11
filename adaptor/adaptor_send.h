//SPDX-License-Identifier: GPL-2.0

/*
* @file :		adaptor_send.h
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		12/2019
* @version :		0.1
* @brief :
*/

#ifndef __ADAPTOR_SEND_H
#define __ADAPTOR_SEND_H

#include <net/if.h>
#include <netinet/ip.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <bpf/bpf.h>

#include "../include/vtl.h"

int adaptor_create_raw_sock(int domain, int proto, char *err_buf);
int adaptor_config_raw_sock(int sock_fd, char *interface, char *err_buf);

int adaptor_send_packet(int sock_fd, uint8_t *send_pkt, vtl_hdr_t *vtlh, struct ip *iphdr, 
						char *dst_ip, char *src_ip, int *ip_flags,
                        uint8_t *send_data, size_t send_data_len, char *err_buf);

#endif // __ADAPTOR_SEND_H
