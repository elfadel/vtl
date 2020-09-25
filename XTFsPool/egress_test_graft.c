//EGRESS GOBACKN ARQ GRAFT

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

#define RETX_MAX      		3
#define WND_SIZE 		4

struct bpf_elf_map SEC("maps") ACK_WND_MAP = {
  	.type =       BPF_MAP_TYPE_ARRAY,
  	.size_key =   sizeof(unsigned int),
  	.size_value = sizeof(struct sock_state_t),
  	.pinning =    PIN_GLOBAL_NS,
  	.max_elem =   WND_SIZE,
};

struct bpf_elf_map SEC("maps") NUM_SEQ_MAP = {
  	.type = BPF_MAP_TYPE_ARRAY,
  	.size_key = sizeof(unsigned int),
  	.size_value = sizeof(uint16_t),
  	.pinning = PIN_GLOBAL_NS,
  	.max_elem = 1,
};

struct bpf_elf_map SEC("maps") BUF_WND_MAP = {
  	.type =       BPF_MAP_TYPE_ARRAY,
  	.size_key =   sizeof(unsigned int),
  	.size_value = sizeof(vtl_pkt_t),
  	.pinning =    PIN_GLOBAL_NS,
  	.max_elem =   WND_SIZE,
};

SEC("egress_tf_sec")
int _tf_tc_egress(struct __sk_buff *skb) {

  	void *data = (void *)(long)skb->data;
  	void *data_end = (void *)(long)skb->data_end;

  	struct ethhdr *eth = (struct ethhdr *)data;
  	if(eth + 1 > data_end) {
    		bpf_printk("ETH layer: malformed header.\n");
    		return TC_ACT_OK;
  	}

  	struct iphdr *iph = (struct iphdr *)(eth + 1);
  	if(iph + 1 > data_end) {
    		bpf_printk("IP layer: malformed header.\n");
    		return TC_ACT_OK;
  	}

  	if(iph->protocol != IPPROTO_VTL) {
    		return TC_ACT_OK;
  	}

  	vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
  	if(vtlh + 1 > data_end) {
    		bpf_printk("VTL layer: malformed header.\n");
    		return TC_ACT_OK;
  	}

  	struct sock_state_t *sk_state = NULL;
  	vtl_pkt_t *save_skb = NULL;
  	
  	int id_seq = 0, id_ack_buf = 0;
  	const uint16_t last_len = (const uint16_t)skb->len;
  	uint16_t *num_seq = NULL;

  	// Filling VTL packet header: start
  	vtlh->pkt_type = DATA;
  	
  	uint8_t* d = (uint8_t *)(vtlh + 1);
  	if(d + 1 > data_end) {
    		bpf_printk("VTL layer: malformed payload.\n");
    		return TC_ACT_OK;
  	}

  	int y = 0;
  	uint8_t sum = 0;
  	for(y = 0; y < vtlh->payload_len; y++) {

  		if(y > VTL_DATA_SIZE)
  			break;

    		uint8_t *block = (uint8_t *)(d + y - 1);
    		if(block + 1 > data_end) {
      			return TC_ACT_OK;
    		}
    		sum ^= *block;
  	}
  	vtlh->checksum = sum;

  	num_seq = (uint16_t *) bpf_map_lookup_elem(&NUM_SEQ_MAP, &id_seq);
  	if(!num_seq){
    		bpf_printk("Unable to read NUM_SEQ_MAP.\n");
    		return TC_ACT_OK;
  	}
  	vtlh->seq_num = *num_seq;

  	// Filling VTL packet header: end

  	id_ack_buf = (unsigned int) *num_seq;
  	sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&ACK_WND_MAP, &id_ack_buf);
  	if(!sk_state) {
    		bpf_printk("Unable to read ACK_WND_MAP.\n");
    		return TC_ACT_OK;
  	}
  	sk_state->event = IDLE;
  	bpf_map_update_elem(&ACK_WND_MAP, &id_ack_buf, sk_state, BPF_ANY);

  	uint32_t cur_pload_len = (uint32_t) vtlh->payload_len;
  	if(cur_pload_len !=  VTL_DATA_SIZE) {

    		if(bpf_skb_change_tail(skb, ON_WIRE_SIZE, 0))
                	bpf_printk("KTF WARN: unable to resize buffer for pkt\n");
		
		data = (void *)(long)skb->data;
		data_end = (void *)(long)skb->data_end;
	        save_skb = (vtl_pkt_t *)data;
		if(save_skb + 1 > data_end) {
			bpf_printk("Unable to save last pkt %d.\n", id_ack_buf);
			return TC_ACT_OK;
		}
		bpf_map_update_elem(&BUF_WND_MAP, &id_ack_buf, save_skb, BPF_ANY);

		if(bpf_skb_change_tail(skb, last_len, 0))
                	bpf_printk("KTF WARN: unable to resize buffer for pkt\n");
	}
	else {
		data = (void *)(long)skb->data;
		data_end = (void *)(long)skb->data_end;
		save_skb = (vtl_pkt_t *)data;
		if(save_skb + 1 > data_end) {
			bpf_printk("Unable to save pkt %d.\n", id_ack_buf);
			return TC_ACT_OK;
		}
		bpf_map_update_elem(&BUF_WND_MAP, &id_ack_buf, save_skb, BPF_ANY);
	}
	
  	// Update seq num for the next packet
  	(*num_seq)++;
  	*num_seq %= WND_SIZE;
  	bpf_map_update_elem(&NUM_SEQ_MAP, &id_seq, num_seq, BPF_ANY);

  	bpf_clone_redirect(skb, skb->ifindex+1, 0);

  	int tx_num = 0;
  	if(id_ack_buf == WND_SIZE - 1) {

     		bpf_printk("Last packet, purging window.\n");

     		unsigned int cursor = 0, i = 0, j = 0, temoin = 0;
     		do {
       			bpf_vtl_start_timer(10); // ms
       			
       			tx_num++;
       			if(tx_num > RETX_MAX) {
         			bpf_printk("Max Retx = %d reached, end loop.\n", tx_num);
         			break;
       			}

       			for(i = cursor; i < WND_SIZE; i++) { // ACKs checking loop
         			
         			id_ack_buf = i;
         			sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&ACK_WND_MAP, &id_ack_buf);
         			if(!sk_state) {
           				bpf_printk("Unable to check pkt %d ACK status.\n", i);
           				break;
         			}

         			if(sk_state->event != RECV_ACK) {
           				bpf_printk("Lost or corrupted pkt %d.\n", i);
           				temoin = 1; // In the case, the lost happen on last packet
           				break;
         			}
         			else {
           				bpf_printk("Pkt %d acked, tx_num = %d\n", i, tx_num);
         			}
       			}

       			cursor = i;
       			if(cursor != WND_SIZE || temoin == 1) { // There are lost/corrupted pkts
       				
         			for(j = cursor; j < WND_SIZE; j++) { // Retx loop
           				
			        	id_ack_buf = j;

			        	vtl_pkt_t *get_skb = (vtl_pkt_t *) bpf_map_lookup_elem(&BUF_WND_MAP, &id_ack_buf);
     					if(!get_skb) {
         					bpf_printk("Unable to get pkt %d for retx.\n", j);
         					break;
     					}

			        	if(j != WND_SIZE - 1 || cur_pload_len == VTL_DATA_SIZE) {

	     					if(j == cursor)
	               					bpf_skb_change_tail(skb, ON_WIRE_SIZE, 0);

	        	     			bpf_vtl_store_bytes(skb, 0, get_skb, ON_WIRE_SIZE, 0);
			        	}
			        	else {
           					
	                			bpf_vtl_store_bytes(skb, 0, get_skb, ON_WIRE_SIZE, 0);
	                			bpf_skb_change_tail(skb, last_len, 0);
			        	}

           				bpf_clone_redirect(skb, skb->ifindex+1, 0);
	             			bpf_printk("Retx pkt %d.\n", j);
         			}
         			temoin = 0;
       			}
       			else //All pkts are ACKs
         			break; 
     		} while(true);
  	}

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
    		bpf_printk("listener ETH layer: malformed header.\n");
    		return XDP_DROP;
  	}

  	struct iphdr *iph = (struct iphdr *)(eth + 1);
  	if(iph + 1 > data_end) {
    		bpf_printk("listener IP layer: malformed header.\n");
    		return XDP_DROP;
  	}

  	if(iph->protocol != IPPROTO_VTL)
    		return XDP_PASS;

  	vtl_hdr_t *vtlh  = (vtl_hdr_t *)(iph + 1);
  	if(vtlh + 1 > data_end) {
    		bpf_printk("listener VTL layer: malformed header.\n");
    		return XDP_DROP;
  	}

  	index = (int) vtlh->seq_num;
  	sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&ACK_WND_MAP, &index);
  	if(!sk_state) {
    		bpf_printk("listener VTL layer: unable to read ACK_WND_MAP.\n");
    		return XDP_PASS;
  	}

  	if(vtlh->pkt_type == NACK) {
    		//bpf_printk("listener VTL layer: processing NACK.\n");
    		sk_state->event = IDLE; // TIMEOUT ???
  	}
  	else {
    		sk_state->event = RECV_ACK;
  	}

  	long ret = bpf_map_update_elem(&ACK_WND_MAP, &index, sk_state, BPF_ANY);
  	if(ret != 0){
    		bpf_printk("listener VTL layer: unable to update ACK_WND_MAP.\n");
    		return XDP_DROP;
  	}

  	return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
