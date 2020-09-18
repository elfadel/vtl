// EGRESS STOP-WAIT ARQ GRAFT

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <linux/pkt_cls.h>
#include <linux/bpf.h>

#include "bpf/bpf_helpers.h"
#include "bpf/tc_bpf_util.h"
#include "../include/vtl.h"

#define RETX_MAX      10

struct bpf_elf_map SEC("maps") EGRESS_SOCKS_MAP = {
  	.type = BPF_MAP_TYPE_ARRAY,
  	.size_key = sizeof(unsigned int),
  	.size_value = sizeof(struct sock_state_t),
  	.pinning = PIN_GLOBAL_NS,
  	.max_elem = 1,
};

static __always_inline vtl_hdr_t* egress_move_to_vtl_hdr(struct __sk_buff *skb) {
    	void *data = (void *)(long)skb->data;
    	void *data_end = (void *)(long)skb->data_end;

  	struct ethhdr *eth = (struct ethhdr *)data;
    	if(eth + 1 > data_end){
        	bpf_printk("ETH layer: malformed header.\n");
        	return NULL;
    	}

    	struct iphdr *iph = (struct iphdr *)(eth + 1);
    	if(iph + 1 > data_end){
        	bpf_printk("IP layer: malformed header.\n");
        	return NULL;
    	}

    	if(iph->protocol != IPPROTO_VTL)
        	return NULL;

    	vtl_hdr_t *hdr = (vtl_hdr_t *)(iph + 1);
    	if(hdr + 1 > data_end){
        	bpf_printk("VTL layer: malformed header.\n");
        	return NULL;
    	}

    	return hdr;
}

SEC("egress_tf_sec")
int _tf_tc_egress(struct __sk_buff *skb) {
  	struct sock_state_t *sk_state = NULL;
  	int index = 0, tx_num = 0;
  	void *data_end = (void *)(long)skb->data_end;

	vtl_hdr_t *vtlh = egress_move_to_vtl_hdr(skb);
	if(!vtlh) {
    		bpf_printk("VTL layer: malformed header.\n");
    		return TC_ACT_OK;
  	}

  	int pkt_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(vtl_hdr_t) + VTL_DATA_SIZE;
  	int pload_s = (skb->len == pkt_size) ? 1024 : 696;

  	uint8_t* d = (uint8_t *)(vtlh + 1);
  	if(d + 1 > data_end) {
    		bpf_printk("VTL data: malformed payload.\n");
    		return TC_ACT_OK;
  	}

 	/*TODO: perform calculation with pseudo hdr =
  		{ip_src@ + ip_dst@ + L4 proto field + L4 pkt len + Reserved 8bits}*/
  	int y = 0;
  	uint8_t sum = 0;
  	for(y = 0; y < pload_s; y++) {
    		uint8_t *block = (uint8_t *)(d + y - 1);
    		if(block + 1 > data_end) {
      			return TC_ACT_OK;
    		}
    		sum ^= *block;
  	}
  	vtlh->pkt_type = DATA;
  	vtlh->checksum = sum;

  	do { // Tx loop
    		bpf_clone_redirect(skb, skb->ifindex+1, 0/*egress*/);
    		bpf_vtl_start_timer(10); //ms

    		tx_num++;
    		if(tx_num > RETX_MAX) { // Reach max of retransmission
      			bpf_printk("tx=%d | Max Retx, end loop.\n", tx_num);
      			break;
    		}

    		sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&EGRESS_SOCKS_MAP, &index);
    		if(!sk_state) // Mandatory. Otherwise, the verifier will reject the KTF.
      			continue; // Don't stop Tx loop, just retry. Be strong bro :)

    		if(sk_state->event == RECV_ACK) {
      			sk_state->event = IDLE;
      			bpf_map_update_elem(&EGRESS_SOCKS_MAP, &index, sk_state, BPF_ANY);
      			break;
    		}
    		else if(sk_state->event == IDLE) {
      			bpf_printk("tx=%d | Packet loss or corrupted\n", tx_num-1);
      			continue;
    		}
  	} while(true);

  	return TC_ACT_SHOT;
}

SEC("listener_tf_sec")
int _listener_tf(struct xdp_md *ctx) {
  	int index = 0;
  	struct sock_state_t *sk_state = NULL;
  	void *data = (void *)(long)ctx->data;
  	void *data_end = (void *)(long)ctx->data_end;

  	struct ethhdr *eth = (struct ethhdr *)data;
  	if(eth + 1 > data_end) {
    		bpf_printk("listener sec (ETH layer): malformed header.\n");
    		return XDP_DROP;
  	}

  	struct iphdr *iph = (struct iphdr *)(eth + 1);
  	if(iph + 1 > data_end) {
    		bpf_printk("listener sec (IP layer): malformed header.\n");
    		return XDP_DROP;
  	}

  	if(iph->protocol != IPPROTO_VTL)
    		return XDP_PASS;

  	vtl_hdr_t *vtlh  = (vtl_hdr_t *)(iph + 1);
  	if(vtlh + 1 > data_end) {
    		bpf_printk("listener sec (VTL layer): malformed header.\n");
    		return XDP_DROP;
  	}

  	sk_state = (struct sock_state_t *)bpf_map_lookup_elem(&EGRESS_SOCKS_MAP, &index);
  	if(!sk_state)
    		return XDP_PASS;

  	if(vtlh->pkt_type == NACK) {
    	sk_state->event = IDLE;
		bpf_printk("Processing VTL NACK\n");
	}
  	else {
  		bpf_printk("Processing VTL ACK !\n");
    		sk_state->event = RECV_ACK;
	}
  	long ret = bpf_map_update_elem(&EGRESS_SOCKS_MAP, &index, sk_state, BPF_ANY);
  	if(ret != 0) {
    		return XDP_DROP;
  	}

  	return XDP_PASS; //Let it for the kern network stack???
}

char _license[] SEC("license") = "GPL";
