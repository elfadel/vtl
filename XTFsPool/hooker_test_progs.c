/*
 * @file: 	hooker_test_progs.c
 * @author: 	El-Fadel Bonfoh
 * @date: 	06/2020
 * @version: 	1.0
 * @brief: 	Hooker kernel part
*/

#include <linux/bpf.h>
#include <linux/if_ether.h>

#include "bpf/bpf_helpers.h"
#include "bpf/bpf_endian.h"
#include "bpf/tc_bpf_util.h"
#include "../include/vtl.h"

#define TCPOPT_NOP 			1
#define SERVER_PORT 		/*4443*/2222
#define CLIENT_PORT 		/*4443*/2223
#define HK_REDIR_PORT       		10000

struct tcp_opt v_opt = {

	.kind 	= 253, // Experimental value (iana.org RF2780)
	.len 	= sizeof(v_opt),
	.data 	= bpf_htons(0x0001),
};

enum vtl_ndpi_l3_type {
	L3_IP4,
	L3_IP6, 		/* Not yet supported */
};

struct vtl_tcp_stream_info { // Unique stream

	uint8_t gid; 			/* Graft id associated to the stream */
	uint32_t profil;
	uint32_t stream_id;
	unsigned long long int packets_processed;
	uint64_t first_seen;
	uint64_t last_seen;
	uint64_t hash_val;

	enum vtl_ndpi_l3_type l3_type;

	union {
		struct {
			uint32_t src;
			uint32_t dst;
		} v4;
		struct {
			uint64_t src[2];
			uint64_t dst[2];
		} v6;
	} ip_tuple;

	unsigned long long int total_tcp_data_len;
	uint16_t src_port;
	uint16_t dst_port;

	uint8_t is_mid_stream:1;
	uint8_t stream_fin_ack_seen:1;
	uint8_t stream_ack_seen:1;
	uint8_t detection_completed:1;
	uint8_t tls_client_hello_seen:1; 
	uint8_t tls_server_hello_seen:1;
	uint8_t reserved_00:2;
	/* This field is put to fix ndpi_detection_get_l4() invokation. 
	 * We already knew that we are processing TCP stream
	*/
	uint8_t l4_proto;

	struct ndpi_proto detected_l7_proto;
	struct ndpi_proto guessed_proto;

	struct ndpi_flow_struct *ndpi_flow;
	struct ndpi_id_struct *ndpi_src;
	struct ndpi_id_struct *ndpi_dst;
};

/* PLEASE, keep the below MAPS at the top in this order. */
struct  bpf_map_def SEC("maps") HK_SOCK_MAP = {
	.type 		= BPF_MAP_TYPE_SOCKHASH,
	.key_size 	= sizeof(int),
	.value_size 	= sizeof(int),
	.max_entries 	= 20,
};

struct bpf_map_def SEC("maps") APP_HASH_ID_MAP = {
	.type 		= BPF_MAP_TYPE_ARRAY,
	.key_size 	= sizeof(int),
	.value_size 	= sizeof(int),
	.max_entries 	= 1,
};
/* PLEASE, add additional MAPS here below */

struct bpf_map_def SEC("maps") TCP_STREAMS_CACHE_MAP = {
	.type 		= BPF_MAP_TYPE_ARRAY,
	.key_size 	= sizeof(unsigned int),
	.value_size 	= sizeof(struct vtl_tcp_stream_info),
	.max_entries 	= MAX_HANDLED_TCP_STREAMS,
};

struct bpf_map_def SEC("maps") TCP_HOST_ROLE_MAP = {
	.type 		= BPF_MAP_TYPE_ARRAY,
	.key_size 	= sizeof(unsigned int),
	.value_size 	= sizeof(enum tcp_host_role),
	.max_entries 	= 1,
};

struct bpf_map_def SEC("maps") PRFLNG_TRIES_NUM_MAP = {
	.type 		= BPF_MAP_TYPE_ARRAY,
	.key_size 	= sizeof(unsigned int),
	.value_size 	= sizeof(unsigned int),
	.max_entries 	= 1,
};

struct bpf_map_def SEC("maps") SYN_HDR_INFO_MAP = {
	.type 		= BPF_MAP_TYPE_ARRAY,
	.key_size 	= sizeof(unsigned int),
	.value_size 	= sizeof(struct stream_tuple),
	.max_entries 	= 1,
};

static __always_inline 
void __hk_record_app_info(struct bpf_sock_ops *sk_ops, enum tcp_host_role h_role) {

	struct stream_tuple app_info = {0};
	int index = 0, app_hash, app_hash_cpy, port_num;

	app_info.src_ip = bpf_ntohl(sk_ops->local_ip4);
	app_info.dst_ip = bpf_ntohl(sk_ops->remote_ip4);
	app_info.src_port = sk_ops->local_port; /* Already in HBO order */
	app_info.dst_port = bpf_ntohl(sk_ops->remote_port);

	app_hash = vtl_compute_tcp_stream_cookie(app_info.src_ip, app_info.dst_ip,
						 app_info.src_port, app_info.dst_port);
	app_hash %= 20; /* size of hk_sock_map */

	bpf_printk("[HK-SK]: CLIENT_PORT=%d | SERVER_PORT=%d\n", 
		   app_info.src_port, app_info.dst_port);
	switch(h_role) {
		case TCP_SERVER:
			port_num = SERVER_PORT;
			break;
		case TCP_CLIENT:
			port_num = (CLIENT_PORT == SERVER_PORT) ? 
				    app_info.src_port : CLIENT_PORT;
			break;
		default:
			return;
	}
	bpf_printk("[HK-SK]: port to monitor = %d\n", port_num);

	if(app_info.src_port == port_num) {
		// Don't use directly app_hash var;
		app_hash_cpy = app_hash;
		
		int ret = bpf_map_update_elem(&APP_HASH_ID_MAP, &index, &app_hash_cpy, BPF_ANY);
		if(ret < 0)
			bpf_printk("[HK-SK]: WARN - failed to update APP_HASH_ID_MAP.\n");
		else
			bpf_printk("[HK-SK]: APP_HASH_ID_MAP updated !\n");
	}

	// Fill the second entry of hk_sock_map
	int ret = bpf_sock_hash_update(sk_ops, &HK_SOCK_MAP, &app_hash, BPF_NOEXIST);
	if(ret < 0)
		bpf_printk("[HK-SK]: WARN - failed to update SOCKMAP. ret=%d\n", ret);
	else
		bpf_printk("[HK-SK]: SOCKMAP updated !\n");
}

SEC("hooker_sockops/0")
int hooker_monitor_apps(struct bpf_sock_ops *sk_ops) {

	int op, rv = -1, opt_buff, profiling_tries;
	unsigned int index, index0, index1;
	__u32 local_ip4, remote_ip4;
	__u32 local_port, remote_port;
	uint16_t option_type = -1, stream_cookie_id;
	struct vtl_tcp_stream_info *tcp_stream = NULL;
	enum tcp_host_role h_role = -1;

	local_ip4 = sk_ops->local_ip4;
	remote_ip4 = sk_ops->remote_ip4;
	local_port = sk_ops->local_port;
	remote_port = sk_ops->remote_port;

	if(local_port != SERVER_PORT && local_port != CLIENT_PORT &&
		bpf_ntohl(remote_port) != SERVER_PORT && 
		bpf_ntohl(remote_port) != CLIENT_PORT) {
		/* Appli not supported. Don't play with packet :-; */
		sk_ops->reply = -1;
		return 1;
	}

	op = (int) sk_ops->op;
	switch(op) {
		case BPF_SOCK_OPS_TCP_CONNECT_CB:
			bpf_printk("[HK-SK]: setting VTL_FLAGS to activate Hooker.\n");
			rv = bpf_sock_ops_cb_flags_set(sk_ops, BPF_SOCK_OPS_VTL_OPT_WRITE_FLAG);
			index0 = 0;
			h_role = TCP_CLIENT;
			bpf_map_update_elem(&TCP_HOST_ROLE_MAP, &index0, &h_role, BPF_ANY);
			break;

		case BPF_SOCK_OPS_TCP_LISTEN_CB:
			bpf_printk("[HK-SK]: setting VTL_FLAGS to activate Hooker.\n");
			rv = bpf_sock_ops_cb_flags_set(sk_ops, BPF_SOCK_OPS_VTL_OPT_WRITE_FLAG);
			index0 = 0;
			index1 = 0;
			h_role = TCP_SERVER;
			bpf_map_update_elem(&TCP_HOST_ROLE_MAP, &index0, &h_role, BPF_ANY);
			profiling_tries = 0;
			bpf_map_update_elem(&PRFLNG_TRIES_NUM_MAP, &index1, &profiling_tries, BPF_ANY);
			break;

		case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
			h_role = TCP_CLIENT;
			__hk_record_app_info(sk_ops, h_role);
			break;

		case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
			bpf_printk("[HK-SK]: handshake complete. Unset VTL_FLAGS.\n");
			rv = bpf_sock_ops_cb_flags_set(sk_ops, 0);
			h_role = TCP_SERVER;
			__hk_record_app_info(sk_ops, h_role);
			break;

		/* Call from __tcp_transmit_skb() and tcp_current_mss()*/
		case 100: /*BPF_SOCK_OPS_VTL_CHECK_OPT_LEN_CB*/
			if(sk_ops->args[1] + sizeof(v_opt) <= 40)
				rv = sizeof(v_opt);
			else
				rv = 0;
			break;

		/* call from tcp_options_write() */
		case 200: /*BPF_SOCK_OPS_VTL_WRITE_OPT_CB:*/
	
			if(sk_ops->args[1] == 100) { // Add VTL_COMPLIANT option
				struct tcp_opt real_v_opt = {
					.kind 	= 253,
					.len 	= sizeof(v_opt),
					.data 	= bpf_htons(0x0001),
				};
				__builtin_memcpy(&opt_buff, &real_v_opt, sizeof(int));
				option_type = real_v_opt.data;
			}
			else if(sk_ops->args[1] == 200) { // Add VTL_NEGO_OPT option
				struct tcp_opt real_v_opt = {
					.kind 	= 253,
					.len 	= sizeof(v_opt),
					.data 	= bpf_htons(0x0002),
				};
				__builtin_memcpy(&opt_buff, &real_v_opt, sizeof(int));
				option_type = real_v_opt.data;
				/* We are responding to SYN. Try to classify the SYN
				   and choose the right/correct graft */
				/* sk_ops not yet set; use SYN saved in MAP by listener prog. */
				index = 0;
				struct stream_tuple *syn_info = (struct stream_tuple *)
								bpf_map_lookup_elem(&SYN_HDR_INFO_MAP, &index);
				if(syn_info != NULL) {

					local_ip4 = syn_info->src_ip;
					remote_ip4 = syn_info->dst_ip;
					local_port = syn_info->src_port; 
					remote_port = syn_info->dst_port;

					stream_cookie_id = vtl_compute_tcp_stream_cookie(
							   local_ip4, remote_ip4, local_port, remote_port);
					bpf_printk("[HK-SK]: computed stream cookie=%d.\n", stream_cookie_id);

					/* Don't use directlty *stream_cookie_id*. 
					   NO NO NO. Verifier will not like that ;-) */
					index = stream_cookie_id;
					tcp_stream = (struct vtl_tcp_stream_info *)
						     bpf_map_lookup_elem(&TCP_STREAMS_CACHE_MAP, &index);
					if(tcp_stream != NULL) {
						// TODO: use stream getted.
						bpf_printk("[HK-SK]: stream profil found = %d. Use proto graft "
							   "gid=%d.\n", tcp_stream->profil, tcp_stream->gid);
					} 
					else
						bpf_printk("[HK-SK]: stream profil NOT found. Use default proto " 
							   "graft gid=%d. Try %d.\n", 8, -1);
				}
				else
					bpf_printk("[HK-SK]: unable to get syn_info from MAP.\n");
				
			} 
			else if (sk_ops->args[1] == 300) { // Add VTL_NEGO_ACK_OPT option
				struct tcp_opt real_v_opt = {
					.kind 	= 253,
					.len 	= sizeof(v_opt),
					.data 	= bpf_htons(0x0003),
				};
				__builtin_memcpy(&opt_buff, &real_v_opt, sizeof(int));
				option_type = real_v_opt.data;
			} 

			if(option_type > 0) {
				rv = opt_buff;
				bpf_printk("[HK-SK]: VTL option set to: %s.\n",  
					   vtl_opt_to_string(bpf_ntohs(option_type)));
			}
			else
				bpf_printk("[HK-SK]: Not Ctrl/Signal packet. "
					   "No need to add option.\n");

			break;
		
		/* call after vtl_tcp_send_ack() from tcp_input.c */
		case 300: /* BPF_SOCK_OPS_HSK_END_CB */
			bpf_printk("[HK-SK]: handshake complete. Unset VTL_FLAGS.\n");
			rv = bpf_sock_ops_cb_flags_set(sk_ops, 0);
			break;	
		
		default:
			break;
	} // End of switch of

	sk_ops->reply = rv;
	return 1;
}

SEC("hooker_redirector/0")
int hooker_switch_packet_data(struct sk_msg_md *msg) {

	__u64 flags = BPF_F_INGRESS;
	int default_app_hash = 0;

	if(msg->local_port == HK_REDIR_PORT) {
		// Hooker -> app
		int index = 0, app_hash_cpy;
		int *get_app_hash = NULL;
		get_app_hash = bpf_map_lookup_elem(&APP_HASH_ID_MAP, &index);
		if(get_app_hash == NULL) {
			bpf_printk("[HK-SM]: unable to get app info to redirect " 
				   "data. Skipping (SK_PASS) ...\n");
			return SK_PASS;
		}

		app_hash_cpy = *get_app_hash;
		bpf_printk("[HK-SM]: hk_kern redirects to appli: %d bytes.\n", 
			   msg->size);
		return bpf_msg_redirect_hash(msg, &HK_SOCK_MAP, &app_hash_cpy, flags);
	}
	else {
		// app -> Hooker
		bpf_printk("[HK-SM]: hk_kern redirects to hk_user: %d bytes.\n",
			   msg->size);
		return bpf_msg_redirect_hash(msg, &HK_SOCK_MAP, &default_app_hash, flags);
	}
}

SEC("hooker_listener/0")
int hooker_get_vtl_opt(struct xdp_md *ctx) {

	__u32 local_ip4 = 0, remote_ip4 = 0;
	__u32 local_port = 0, remote_port = 0;
	unsigned int index, index0, index1;
	uint16_t stream_cookie_id;
	int *profiling_tries = NULL;
	enum tcp_host_role *h_role = NULL;
	
	void *data = (void *)(long)ctx->data;
	void *data_end = (void *)(long)ctx->data_end;

	struct ethhdr *eth = (struct ethhdr *)data;
	if(eth + 1 > data_end) {
		bpf_printk("[HK-X]: Eth malformed header.\n");
		return XDP_PASS;
	}

	struct iphdr *iph = (struct iphdr *)(eth + 1);
	if(iph + 1 > data_end) {
		bpf_printk("[HK-X]: Iph malformed header.\n");
		return XDP_PASS;
	}

	if(iph->protocol == IPPROTO_TCP) {

		struct tcphdr *tcph = (struct tcphdr *)(iph + 1);
		if(tcph + 1 > data_end) {
			bpf_printk("[HK-X]: Tcph malformed header.\n");
			return XDP_PASS;
		}

		index0 = 0;
		index1 = 0;
		
		h_role = (enum tcp_host_role *)
			 bpf_map_lookup_elem(&TCP_HOST_ROLE_MAP, &index0);
		if(h_role == NULL)
			bpf_printk("[HK-X]: unable to get TCP host role from MAP (0).\n");
		else if(*h_role == TCP_SERVER) {

			profiling_tries = (int *)
					  bpf_map_lookup_elem(&PRFLNG_TRIES_NUM_MAP, &index1);
			if(profiling_tries == NULL)
				bpf_printk("[HK-X]: unable to get TCP host role from MAP (1).\n");
			else if(*profiling_tries < MAX_PROFILING_TRIES) { 
				// We should continue to trie profiling of tcp stream
				
				(*profiling_tries)++;
				bpf_map_update_elem(&PRFLNG_TRIES_NUM_MAP, &index1, 
						    profiling_tries, BPF_ANY);

				/* Convert to HBO order */
				local_ip4 = bpf_ntohl(iph->daddr);
				remote_ip4 = bpf_ntohl(iph->saddr);
				local_port = bpf_ntohs(tcph->dest); 
				remote_port = bpf_ntohs(tcph->source);
				
				stream_cookie_id = vtl_compute_tcp_stream_cookie(local_ip4, remote_ip4, 
										 local_port, remote_port);
				bpf_printk("[HK-X]: computed stream cookie=%d.\n", stream_cookie_id);

				/* Don't use directlty *stream_cookie_id* in MAP. 
				   NO NO NO. Verifier will not like that :) */
				index = stream_cookie_id;
				struct vtl_tcp_stream_info tcp_stream;
				__builtin_memset(&tcp_stream, 0x0, sizeof(tcp_stream));
				tcp_stream.gid = -1; // Attribute default protocol graft
				/* Register the stream cookie to assist app profiler agent */
				bpf_map_update_elem(&TCP_STREAMS_CACHE_MAP, &index, &tcp_stream, BPF_ANY);
			}
		}

		if(tcph->syn && !tcph->ack) { // SYN packet. Play with it !

			index = 0;
			struct stream_tuple syn_info;
			__builtin_memset(&syn_info, 0x0, sizeof(syn_info));
			syn_info.src_ip = local_ip4;
			syn_info.dst_ip = remote_ip4;
			syn_info.src_port = local_port;
			syn_info.dst_port = remote_port;
			bpf_map_update_elem(&SYN_HDR_INFO_MAP, &index, &syn_info, BPF_ANY);

			int length = (tcph->doff * 4) - sizeof(struct tcphdr);
			const unsigned char *ptr = (const unsigned char *)(tcph + 1);
			if(ptr + 1 > data_end) {
				bpf_printk("[HK-X]: Error parsing TCP options (0).\n");
				return XDP_PASS;
			}

			int i = 0;
			//TODO: Fix bound 7
			for(i = 0; i < 7; i++) {
				int opcode = *ptr;

				if(length <= 0)
					break;
			
				if(opcode == 253 || opcode == 254) {
					struct tcp_opt *t_opt = (struct tcp_opt *)ptr;
					if(t_opt + 1 > data_end) {
						bpf_printk("[HK-X]: Error getting VTL Opt (0).\n");
						return XDP_PASS;
					}
					
					break;
				}
				else if(opcode == TCPOPT_NOP) {
					length--;
					ptr++;
					if(ptr + 1 > data_end) {
						bpf_printk("[HK-X]: Error parsing TCP options (1).\n");
						return XDP_PASS;
					}
					continue;
				}

				ptr++;
				if(ptr + 1 > data_end) {
					bpf_printk("[HK-X]: Error parsing TCP options (2).\n");
					return XDP_PASS;
				}
				int opsize = *ptr;

				ptr += opsize - 1;
				if(ptr + 1 > data_end) {
					bpf_printk("[HK-X]: Error parsing TCP options (3).\n");
					return XDP_PASS;
				}

				length -= opsize;
			}
		} // End SYN handle 
		else if(tcph->syn && tcph->ack) {
			// Retrieve NEGO_OPT option and deduce the required proto X
			int length = (tcph->doff * 4) - sizeof(struct tcphdr);
			const unsigned char *ptr = (const unsigned char *)(tcph + 1);
			if(ptr + 1 > data_end) {
				bpf_printk("[HK-X]: Error parsing TCP options (4).\n");
				return XDP_PASS;
			}

			int i = 0;
			for(i = 0; i < 7; i++) {
				int opcode = *ptr;

				if(length <= 0)
					break;
			
				if(opcode == 253 || opcode == 254) {
					struct tcp_opt *t_opt = (struct tcp_opt *)ptr;
					if(t_opt + 1 > data_end) {
						bpf_printk("[HK-X]: Error getting VTL Opt (1).\n");
						return XDP_PASS;
					}
					
					break;
				}
				else if(opcode == TCPOPT_NOP) {
					length--;
					ptr++;
					if(ptr + 1 > data_end) {
						bpf_printk("[HK-X]: Error parsing TCP options (5).\n");
						return XDP_PASS;
					}
					continue;
				}

				ptr++;
				if(ptr + 1 > data_end) {
					bpf_printk("[HK-X]: Error parsing TCP options (6).\n");
					return XDP_PASS;
				}
				int opsize = *ptr;

				ptr += opsize - 1;
				if(ptr + 1 > data_end) {
					bpf_printk("[HK-X]: Error parsing TCP options (7).\n");
					return XDP_PASS;
				}

				length -= opsize;
			}
		}
	} // End TCP handle

	return XDP_PASS;
}

char _license[] SEC("license") = "GPL";