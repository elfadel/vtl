//SPDX-License-Identifier: GPL-2.0

/*
* @file :		adaptor_recv.h
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		12/2019
* @version :		0.1
* @brief :
*/

#pragma once

#include "../include/vtl.h"
#include "../include/common/xsk_user_helpers.h"

struct xsk_socket_info* adaptor_create_xsk_sock(char *ifname, __u32 xdp_flags,
                                __u16 xsk_bind_flags, int xsk_if_queue,
                                struct xsk_umem_info *umem, char *err_buf);
void adaptor_recv_data(struct xsk_socket_info *xsk_socket, FILE *rx_file, 
						uint32_t *cnt_pkts, uint32_t *cnt_bytes);

