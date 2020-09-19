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
#include <unistd.h>

#include <linux/if_link.h> // XDP flags

#include "service_api.h"
#include "../include/common/util.h"
#include "../cbr/cbr.h"
#include "../dbr/dbr.h"

static const char *TC_GLOBAL_NS = "/sys/fs/bpf/tc/globals";
//static const char *path_to_nego_map = "/sys/fs/bpf/ngmap";
static const char *path_to_graftid_map = "/sys/fs/bpf/graftidmap";
static const char *BPF_NEGO_MAP = "QOS_NEGO_MAP";

static long __get_map_fd(const char* map_filename) {

	char pinned_file[256];
	snprintf(pinned_file, sizeof(pinned_file), "%s/%s", TC_GLOBAL_NS, map_filename);

	return bpf_obj_get(pinned_file);
}

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

			vtl_sock->xsk_socket = cbr_create_and_config_xsk_sock(ifname, xdp_flags, xsk_bind_flags, 
										xsk_if_queue, &umem, err_buf);
			if(vtl_sock->xsk_socket == NULL)
				return NULL;
			vtl_sock->recv_data = allocate_ustrmem(VTL_DATA_SIZE);
			break;

		case VTL_SENDER_ROLE:
			// tc ==> config raw socket
			vtl_sock->af_inet_sock = cbr_create_raw_sock(AF_INET, IPPROTO_RAW, err_buf);
			if(vtl_sock->af_inet_sock < 0) { // VTL socket init failed !
				return NULL;
			}
			ret = cbr_config_raw_sock(vtl_sock->af_inet_sock, ifname, err_buf);
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

			vtl_sock->xsk_socket = cbr_create_and_config_xsk_sock(ifname, xdp_flags, xsk_bind_flags,
										xsk_if_queue, &umem, err_buf);
			if(vtl_sock->xsk_socket == NULL)
				return NULL;
			vtl_sock->recv_data = allocate_ustrmem(VTL_DATA_SIZE);
			vtl_sock->af_inet_sock = cbr_create_raw_sock(AF_INET, IPPROTO_RAW, err_buf);
			if(vtl_sock->af_inet_sock < 0)
				return NULL;
			
			ret = cbr_config_raw_sock(vtl_sock->af_inet_sock, ifname, err_buf);
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

	vtl_sock->vtlh.payload_len = data_len;

	// Send VTL packet
	ret = dbr_send(vtl_sock->af_inet_sock, vtl_sock->send_pkt, &vtl_sock->vtlh, &vtl_sock->iphdr, 
			vtl_sock->dst_ip, vtl_sock->src_ip, vtl_sock->ip_flags,
			vtl_sock->send_data, vtl_sock->send_data_len, err_buf);
	if(ret < 0) {
		return -1;
	}

	return 0;
}

void vtl_recv_data(vtl_socket_t *vtl_sock, uint8_t *rx_data, size_t *rx_data_len, char *err_buf) {
	*rx_data_len = 0;
	dbr_recv(vtl_sock->xsk_socket, rx_data, rx_data_len, &vtl_sock->cnt_pkts, &vtl_sock->cnt_bytes, NULL);
}

int vtl_negotiate(vtl_socket_t *vtl_sock, struct vtl_qos_params qos_values, char *err_buf) {

	int index = 0;
	long qos_nego_map;
	negotiation_state *nego_state = NULL;

	if(1/* TODO: replace by matching rules between qos_values and KTF/Grafts store */) {
		vtl_sock->vtlh.gid = 10; // cbr_select_graft(qos_values,  l4_services)
		int ret;

		if(vtl_sock->vtlh.gid <= 0) {
			printf("[SERVICE API]: Warning - can't find a graft. Keep sending with canonical.\n");
			return 0; 
		}

		ret = dbr_send(vtl_sock->af_inet_sock, vtl_sock->send_pkt, &vtl_sock->vtlh, &vtl_sock->iphdr,
				vtl_sock->dst_ip, vtl_sock->src_ip, vtl_sock->ip_flags, NULL, 0, err_buf);
		if(ret < 0)
			return -1;
	}

	/* TODO: use select() or poll() instead */
	usleep(1000); //1ms
	qos_nego_map = __get_map_fd(BPF_NEGO_MAP);
	if(qos_nego_map < 0)
		fprintf(stderr, "[SERVICE API]: Warning - unable to get QOS_NEGO_MAP\n");
	else {
		bpf_map_lookup_elem(qos_nego_map, &index, nego_state);
      		if(!nego_state) {
      			fprintf(stderr, "[SERVICE API]: Error - unable to get negotiation state. Skipping.\n");
        		return -1;
      		}

      		if(*nego_state == N_ACCEPT) {
        		printf("[SERVICE API]: Negotiation succes. Unload Egress Canonical Graft.\n");
        		// TODO: Add code to perform reconf
        		return vtl_sock->vtlh.gid;
        	}
        	else if(*nego_state == N_REFUSE) {
        		printf("[SERVICE API]: Negotiation rejected by Receiver. Keep sending with canonical.\n");
        		/* 
         		 * TODO: Add any useful customize code 
         		 * (e.g. to close connection instead of keep cano graft)
        		*/
        		return 0; // Cano graft id
      		}
	}

	return -1;
}

int vtl_validate(vtl_socket_t *vtl_sock, char *err_buf) {

	int graft_available = 0; 
	int rx_graft_id = -1; 
	int ret;
	int index = 0;
	
	int graft_id_map = bpf_obj_get(path_to_graftid_map);
	if(graft_id_map < 0) {
		fprintf(stderr, "[SERVICE API]: Error - unable to get QOS_NEGO_MAP\n");
		return -1;
	}
	do {
		bpf_map_lookup_elem(graft_id_map, &index, &rx_graft_id);
		if(rx_graft_id != -100)
			break;
	} while(1);
		
	graft_available = rx_graft_id; //cbr_find_graft(rx_graft_id);

	if(graft_available == 0)
		return 0;
	else if(graft_available > 0) {
		vtl_sock->vtlh.gid = rx_graft_id;
		vtl_sock->vtlh.pkt_type = NEGO_ACK;
	}
	else { // Graft not available in XTF pool

		vtl_sock->vtlh.gid = 0; // Fallback to cano; not mandatory
		vtl_sock->vtlh.pkt_type = NEGO_NACK;
	}

	ret = dbr_send(vtl_sock->af_inet_sock, vtl_sock->send_pkt, &vtl_sock->vtlh, &vtl_sock->iphdr,
			vtl_sock->dst_ip, vtl_sock->src_ip, vtl_sock->ip_flags, NULL, 0, err_buf);
			
  	return rx_graft_id;
}