/*
* @file:		cbr.h
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

/* egress path routines */
int cbr_create_raw_sock(int domain, int proto, char *err_buf);

int cbr_config_raw_sock(int sock_fd, char *interface, char *err_buf);

/* Must return the appropriate *gid* */
int cbr_select_graft(struct vtl_qos_params qos_values /*, struct vtl_l4_services l4_services */);

/* ingress path routines */
struct xsk_socket_info* 
   cbr_create_and_config_xsk_sock(char *ifname, __u32 xdp_flags,
                        		__u16 xsk_bind_flags, int xsk_if_queue,
                        		struct xsk_umem_info *umem, char *err_buf);

int cbr_find_graft(int8_t gid);