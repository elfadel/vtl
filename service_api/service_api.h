/*
* @file:		service_api.h
* @authors:		El-Fadel Bonfoh, Cedric Tape
* @date:		12/2019
* @version:		0.1
* @brief:
*/
#pragma once

#include "../include/vtl.h"

vtl_socket_t* vtl_init(vtl_host_role role, char *src_ip, char *dst_ip, char *ifname, char *err_buf);

int vtl_send_data(vtl_socket_t *vtl_sock, uint8_t *data, size_t data_len, char *err_buf);

int vtl_negotiate(vtl_socket_t *vtl_sock, struct vtl_qos_params qos_values, char *err_buf);

int vtl_validate(vtl_socket_t *vtl_sock, char *err_buf);

void vtl_recv_data(vtl_socket_t *vtl_sock, uint8_t *rx_data, size_t *rx_data_len, char *err_buf);

void vtl_close(vtl_socket_t *vtl_sock, char *err_buf);