//EGRESS GOBACKN ARQ KTF

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

#define RETX_MAX      3

struct bpf_elf_map SEC("maps") EGRESS_SOCKS_WND_MAP = {
  .type =       BPF_MAP_TYPE_ARRAY,
  .size_key =   sizeof(unsigned int),
  .size_value = sizeof(struct sock_state_t),
  .pinning =    PIN_GLOBAL_NS,
  .max_elem =   8,
};

struct bpf_elf_map SEC("maps") EGRESS_PKT_WND_MAP = {
  .type =       BPF_MAP_TYPE_ARRAY,
  .size_key =   sizeof(unsigned int),
  .size_value = sizeof(vtl_pkt_t),
  .pinning =    PIN_GLOBAL_NS,
  .max_elem =   8,
};

struct bpf_elf_map SEC("maps") EGRESS_LAST_PKT_MAP = {
  .type =       BPF_MAP_TYPE_ARRAY,
  .size_key =   sizeof(unsigned int),
  .size_value = sizeof(ugly_t),
  .pinning = PIN_GLOBAL_NS,
  .max_elem = 1,
};

struct bpf_elf_map SEC("maps") NUM_SEQ_MAP = {
  .type = BPF_MAP_TYPE_ARRAY, //TODO: repalce by HASH
  .size_key = sizeof(unsigned int),
  .size_value = sizeof(uint16_t),
  .pinning = PIN_GLOBAL_NS,
  .max_elem = 1,
};

SEC("egress_tf_sec")
int _tf_tc_egress(struct __sk_buff *skb) {

  void *data = (void *)(long)skb->data;
  void *data_end = (void *)(long)skb->data_end;
  struct sock_state_t *sk_state = NULL;
  vtl_pkt_t *save_skb = NULL;
  ugly_t *save_ugly = NULL;
  int index = 0, tx_num = 0, acks_index = 0, wnd_size = 7;
  uint16_t *num_seq = NULL;

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

  // Filling VTL packet header
  int pkt_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(vtl_hdr_t) + VTL_DATA_SIZE;
  int pload_s = (skb->len == pkt_size) ? 1024 : 363;

  uint8_t* d = (uint8_t *)(vtlh + 1);
  if(d + 1 > data_end) {
    bpf_printk("VTL layer: malformed payload.\n");
    return TC_ACT_OK;
  }

  vtlh->type = DATA;

  int y = 0;
  uint8_t sum = 0;
  for(y = 0; y < pload_s; y++) {
    uint8_t *block = (uint8_t *)(d + y - 1);
    if(block + 1 > data_end) {
      return TC_ACT_OK;
    }
    sum ^= *block;
  }
  vtlh->checksum = sum;

  num_seq = (uint16_t *) bpf_map_lookup_elem(&NUM_SEQ_MAP, &index);
  if(!num_seq){
    bpf_printk("KTF: unable to find num seq.\n");
    return TC_ACT_OK;
  }
  vtlh->seq_num = *num_seq;

  acks_index = (unsigned int) *num_seq;
  sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&EGRESS_SOCKS_WND_MAP, &acks_index);
  if(!sk_state) {
    bpf_printk("KTF: unable to read EGRESS_SOCKS_WND_MAP.\n");
    return TC_ACT_OK;
  }
  sk_state->event = IDLE;
  bpf_map_update_elem(&EGRESS_SOCKS_WND_MAP, &acks_index, sk_state, BPF_ANY);

  if(skb->len == pkt_size) {
    save_skb = (vtl_pkt_t *)data;
    if(save_skb + 1 > data_end) {
      bpf_printk("KTF FATAL ERR: unable to save pkt %d.\n", acks_index);
      return TC_ACT_OK;
    }
    bpf_map_update_elem(&EGRESS_PKT_WND_MAP, &acks_index, save_skb, BPF_ANY);
  }
  else { // last packet of last wnd
    save_ugly = (ugly_t *)data;
    if(save_ugly + 1 > data_end) {
      bpf_printk("KTF FATAL ERR: unable to save pkt %d.\n", acks_index);
      return TC_ACT_OK;
    }
    bpf_map_update_elem(&EGRESS_LAST_PKT_MAP, &index, save_ugly, BPF_ANY);
    wnd_size = *num_seq;
  }

  (*num_seq)++;
  *num_seq %= 8;
  bpf_map_update_elem(&NUM_SEQ_MAP, &index, num_seq, BPF_ANY);

  bpf_printk("Sending VTL pkt %d\n", vtlh->seq_num);
  bpf_clone_redirect(skb, skb->ifindex+1, 0);

  if(acks_index==wnd_size/*acks_index==7 || skb->len != pkt_size*/) {
     bpf_printk("Last packet, purging window.\n");

     unsigned int cursor = 0, i = 0, j = 0, temoin = 0;
     do {
       bpf_vtl_start_timer(10); // ms
       tx_num++;
       if(tx_num > RETX_MAX) {
         bpf_printk("tx=%d | Max Retx reached, end loop.\n", tx_num);
         break;
       }

       for(i = cursor; /*i<=wnd_size*/i < 8; i++) { // ACKs checking loop
         if(i>wnd_size){
           i--;
           break;
         }

         acks_index = i; //bpf_map_lookup_elem() reject code if var i is directly used !
         sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&EGRESS_SOCKS_WND_MAP, &acks_index);
         if(!sk_state) {
           bpf_printk("KTF: unable to check pkt %d ACK status.\n", i);
           break;
         }
         if(sk_state->event != RECV_ACK) { // Lost or corrupted packet
           bpf_printk("KTF: lost or corrupted pkt %d.\n", i);
           temoin = 1; // In the case, the lost happen on last packet
           break; // Stop checking
         }
         else {
           bpf_printk("KTF: pkt %d acknowledged., tx = %d\n", i, tx_num);
         }
       } //end checking loop

       cursor = i;
       if(cursor!=wnd_size || temoin == 1) { // There are lost/corrupted pkts
         for(j = cursor; j < 8; j++) { // Retx of lost or corrupted packet loop
           /*if(j>wnd_size){ // Rejected by verifier, code to complex
             break;
           }*/

           acks_index = j;
           int pad_len = 0;
           if(j!=wnd_size) { // It is not the last packet of the wnd
             vtl_pkt_t *get_skb = (vtl_pkt_t *) bpf_map_lookup_elem(&EGRESS_PKT_WND_MAP, &acks_index);
             if(!get_skb) {
                 bpf_printk("KTF: unable to get pkt %d for retx.\n", j);
                 break;
             }

             if(j==cursor) { // First pkt to retx, need to resize it
               pad_len = VTL_DATA_SIZE - pload_s;
               if(bpf_skb_adjust_room(skb, pad_len, BPF_ADJ_ROOM_NET, 0))
                bpf_printk("KTF WARN: unable to resize buffer for pkt %d, with %d.\n", j, pad_len);
             }

             bpf_skb_store_bytes(skb, 0, get_skb, sizeof(vtl_pkt_t), 0);
             bpf_clone_redirect(skb, skb->ifindex+1, 0);
             bpf_printk("KTF: Retx pkt %d.\n", j);
           }
           else { // Last pkt of wnd
             pad_len = pload_s - VTL_DATA_SIZE;
             if(pad_len == 0) { // It is the last pkt, yes. But certainly not the last wnd.
                 vtl_pkt_t *get_skb = (vtl_pkt_t *) bpf_map_lookup_elem(&EGRESS_PKT_WND_MAP, &acks_index);
                 if(!get_skb) {
                     bpf_printk("KTF: unable to get pkt %d for retx.\n", j);
                     break;
                 }
                //No need to call bpf_skb_adjust_room() here since pad_len = 0
                bpf_skb_store_bytes(skb, 0, get_skb, sizeof(vtl_pkt_t), 0);
                bpf_clone_redirect(skb, skb->ifindex+1, 0);
                bpf_printk("KTF: Retx pkt %d.\n", j);
             }
             else { // Last packet of last wnd
                 ugly_t *get_ugly = (ugly_t *) bpf_map_lookup_elem(&EGRESS_LAST_PKT_MAP, &index);
                 if(!get_ugly) {
                     bpf_printk("KTF: unable to get pkt %d for retx.\n", j);
                     break;
                 }

                 if(bpf_skb_adjust_room(skb, pad_len, BPF_ADJ_ROOM_NET, 0)) {
                     bpf_printk("KTF WARN: unable to resize buffer for pkt %d, with %d.\n", j, pad_len);
                 }
                 bpf_skb_store_bytes(skb, 0, get_ugly, sizeof(ugly_t), 0);
                 bpf_clone_redirect(skb, skb->ifindex+1, 0);
                 bpf_printk("KTF: Retx pkt %d.\n", j);
             }
           } // end else / if j!=wnd_size
         } // end retx loop

       } // end if cursor!=wnd_size
       else { //All pkts are ACKs
         break; // End of do loop, each pkt of window is ACK
       }

     } while(true); // end do

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
  if(eth + 1 > data_end){
    bpf_printk("listener ETH layer: malformed header.\n");
    return XDP_DROP;
  }

  struct iphdr *iph = (struct iphdr *)(eth + 1);
  if(iph + 1 > data_end){
    bpf_printk("listener IP layer: malformed header.\n");
    return XDP_DROP;
  }

  if(iph->protocol != IPPROTO_VTL)
    return XDP_PASS;

  vtl_hdr_t *vtlh  = (vtl_hdr_t *)(iph + 1);
  if(vtlh + 1 > data_end){
    bpf_printk("listener VTL layer: malformed header.\n");
    return XDP_DROP;
  }

  index = (int) vtlh->seq_num;
  sk_state = (struct sock_state_t *) bpf_map_lookup_elem(&EGRESS_SOCKS_WND_MAP, &index);
  if(!sk_state) {
    bpf_printk("listener VTL layer: unable to read EGRESS_SOCKS_WND_MAP.\n");
    return XDP_PASS;
  }

  if(vtlh->type == NACK) {
    //bpf_printk("listener VTL layer: processing NACK.\n");
    sk_state->event = IDLE; // TIMEOUT ???
  }
  else {
    sk_state->event = RECV_ACK;
  }

  long ret = bpf_map_update_elem(&EGRESS_SOCKS_WND_MAP, &index, sk_state, BPF_ANY);
  if(ret!=0){
    bpf_printk("listener VTL layer: unable to update EGRESS_SOCKS_WND_MAP.\n");
    return XDP_DROP;
  }

  return XDP_PASS; //Let it for the kern network stack???
}

char _license[] SEC("license") = "GPL";
