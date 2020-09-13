/**
* @file :	vtl.h
* @author :	El-Fadel Bonfoh, Cedric Tape
* @date :	03/2019 -
* @version :	1.0
* @brief :
*/

#pragma once

#include <linux/types.h>
#include <net/if.h> 			// struct ifreq
#include <netinet/ip.h> 		// struct ip and IP_MAXPACKET (which is 65535)
#include <stdint.h> 			// For uintX_t

//deps
#include "../../../nDPI/src/include/ndpi_api.h"
#include "../../../nDPI/src/include/ndpi_main.h"

#include "./common/xdp_user_helpers.h"
#include "./common/xsk_user_helpers.h"

/* Arbitrary numbers */
#define IPPROTO_VTL 				0xfd
#define VTL_ERRBUF_SIZE 			0x100
#define VTL_HDR_LEN 				0x14 
#define IP4_HDR_LEN 				0x14
#define VTL_DATA_SIZE 				0x400
#define MAX_HANDLED_TCP_STREAMS 		0x10000 /* 65536 */
#define MAX_PROFILING_TRIES 			5

/****************
 APP Tx/Rx Mode
*****************/
typedef enum {
	VTL_SENDER_ROLE, // old VTL_MODE_OUT
	VTL_RECVER_ROLE, // old VTL_MODE_IN
	VTL_BOTH_ROLE, 	// old VTL_MODE_INOUT
} vtl_host_role;

enum tcp_host_role {
	TCP_CLIENT = 1, /* <==> RECEIVER role in VTL */
	TCP_SERVER, 	/* <==> SENDER role in VTL */
};

/*************
 MAPS values
**************/
typedef enum {
	N_ACCEPT,
	N_REFUSE,
	N_IDLE,
} negotiation_state;
// TODO: Use by Go-back-N. May be changed. VTL should provides all 
// necessary support for negotiation only, not for internal TF algo.
// ==> move this #defines to the code of the TFs.
#define RECV_ACK 					0x0
#define TIMEOUT 					0x1
#define IDLE 						0x2

/*************
 packet types
**************/
typedef enum {
	NEGO,
	DATA,
	ACK,
	NACK,
	NEGO_ACK,
	NEGO_NACK,
} vtl_pkt_type;

/***************************
 VTL layer paquet structure
****************************/
typedef struct {
	int8_t gid;
	vtl_pkt_type pkt_type;
	uint16_t checksum;
	uint16_t seq_num; // TODO: Should be moved inside TF's algos. 
} vtl_hdr_t;

typedef struct {
	size_t data_len;
		uint8_t *data; // TODO: flexible array data[0]
} vtl_payload_t;

typedef struct {
	vtl_hdr_t hdr;
	vtl_payload_t payload;
} vtl_pkt_t;

/*********************************
 Structure of access point to VTL
**********************************/
// TODO: is it possible to keep unique var for (recv_data and send_data)
typedef struct {
	// XSK_sock
	struct xsk_socket_info *xsk_socket; // contain umem
	struct xdp_config xdp_cfg; 	// TODO: find where it is used
	uint8_t *recv_data; 		// <==> Rx buff
	size_t recv_data_len;

	// RAW_sock
	int af_inet_sock;
	uint8_t *send_pkt;  // <==> Sk buff
	uint8_t *send_data; // <==> Tx buff
	size_t send_data_len;
	// TODO: add tc_cfg

	// VTL layer
	vtl_hdr_t vtlh;
	//IP layer
	struct ip iphdr;
	int *ip_flags;
	char src_ip[INET_ADDRSTRLEN];
	char dst_ip[INET_ADDRSTRLEN];
	char ifname[IF_NAMESIZE];
	
	// others
	struct ifreq ifr;
	uint32_t cnt_pkts;
	uint32_t cnt_bytes;
	char err_buf[VTL_ERRBUF_SIZE];
} vtl_socket_t;

struct vtl_qos_params {
	uint32_t delay;     // ms
	uint32_t thpt; 	    // Mbps
	uint32_t loss_rate; // % 
};

struct sock_state_t { // change the name
	__u32 sk_fd;
	int event;
}__attribute__((packed));

typedef enum {
	// TODO: Add OPT at the end of all vars
	VTL_COMPLIANT,
	NEGO_OPT,
	NEGO_ACK_OPT,
	NEGO_NACK_OPT,
	CLOSE_OPT,
	CLOSE_ACK_OPT,
	VTL_PURE_DATA,
} vtl_tcp_options;

struct tcp_opt {
	uint8_t kind;
	uint8_t len;
	uint16_t data;
};

struct stream_tuple {
	__u32 src_ip;
	__u32 dst_ip;
	__u32 src_port;
	__u32 dst_port;
};

enum tcp_stream_profil {
	LATENCY_SENSITIVE,
	LATENCY_LOSS_SENSITIVE,
	LOSS_SENSITIVE,
	INSENSITIVE,
	UNKNOWN_STREAM_PROFIL,
};

static __always_inline char* vtl_opt_to_string(uint16_t v_opt_data) {
	char s[] = "UNKNOWN_VTL_OPTION"; // TODO: Not safe. Fix !
	switch(v_opt_data) {
		case 0x0001:
			strcpy(s, "VTL_COMPLIANT");
			break;
		
		case 0x0002:
			strcpy(s, "NEGO_OPT");
			break;

		case 0x0003:
			strcpy(s, "NEGO_ACK_OPT");
			break;

		case 0x0004:
			strcpy(s, "NEGO_NACK_OPT");
			break;

		case 0x0005:
			strcpy(s, "CLOSE_OPT");
			break;

		case 0x0006:
			strcpy(s, "CLOSE_ACK_OPT");
			break;

		case 0x0007:
			strcpy(s, "VTL_PURE_DATA");
			break;

		default:
			break;
	}

	char *ret = s;

	return ret;
}

static __always_inline uint16_t vtl_compute_tcp_stream_cookie(__u32 src_ip, __u32 dst_ip, __u32 src_port, __u32 dst_port) {
	return (src_ip + dst_ip + src_port + dst_port)%MAX_HANDLED_TCP_STREAMS;
}

/*typedef struct {
	uint8_t payload[403]; // x + 40
} ugly_t;
*/

/*******************************************************************************
 Some VTL helpers... Note that VTL integrates many others directly to the kern.
*********************************************************************************/
