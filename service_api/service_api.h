//SPDX-License-Identifier: GPL-2.0

/*
* @file :		service_api.h
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		12/2019
* @version :		0.1
* @brief :
*/

#ifndef __SERVICE_API_H
#define __SERVICE_API_H

#include "../include/vtl.h"

vtl_socket_t* vtl_init(vtl_host_role role, char *src_ip, char *dst_ip, char *ifname, char *err_buf);
int vtl_send_data(vtl_socket_t *vtl_sock, uint8_t *data, size_t data_len, char *err_buf);
void vtl_recv_data(vtl_socket_t *vtl_sock, FILE *rx_file);

#endif // __SERVICE_API_H
