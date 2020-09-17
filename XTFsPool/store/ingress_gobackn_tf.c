/* SPDX-License-Identifier: GPL-2.0 */

//INGRESS GOBACKN ARQ KTF

#include <stdint.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <linux/bpf.h>
#include "bpf/bpf_helpers.h"
#include "bpf/tc_bpf_util.h"

#include "../include/vtl.h"

struct bpf_map_def SEC("maps") xsks_map = {
  .type = BPF_MAP_TYPE_XSKMAP,
  .key_size = sizeof(int),
  .value_size = sizeof(int),
  .max_entries = 64,
};

struct bpf_map_def SEC("maps") NUM_ACK_MAP = {
  .type = BPF_MAP_TYPE_ARRAY, //TODO: replace by HASH
  .key_size = sizeof(unsigned int),
  .value_size = sizeof(uint16_t),
  .max_entries = 1,
};

static __always_inline void swap_src_dst_mac(void *data) {
    unsigned short *p = data;
    unsigned short dst[3];

    dst[0] = p[0];
    dst[1] = p[1];
    dst[2] = p[2];
    p[0] = p[3];
    p[1] = p[4];
    p[2] = p[5];
    p[3] = dst[0];
    p[4] = dst[1];
    p[5] = dst[2];
}

SEC("ingress_tf_sec")
int xdp_sock_prog(struct xdp_md *ctx) {
  int index = 0, xsk_index = ctx->rx_queue_index;
  uint16_t *ack_num = NULL;

  void *data = (void *)(long)ctx->data;
  void *data_end = (void *)(long)ctx->data_end;

  struct ethhdr *eth = (struct ethhdr *)data;
  if (eth + 1 > data_end){
      bpf_printk("ETH: malformed header.\n");
      return XDP_DROP;
  }

  struct iphdr *iph = (struct iphdr *)(eth + 1);
  if (iph + 1 > data_end){
      bpf_printk("IPH: malformed header.\n");
      return XDP_DROP;
  }

  if(iph->protocol != IPPROTO_VTL){
      return XDP_PASS;
  }

  vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
  if(vtlh + 1 > data_end){
      bpf_printk("VTLH: malformed header.\n");
      return XDP_DROP;
  }

  uint8_t *d = (uint8_t *)(vtlh + 1);
  if(d + 1 > data_end) {
    bpf_printk("VTL layer: malformed payload.\n");
    return XDP_DROP;
  }

  ack_num = (uint16_t *) bpf_map_lookup_elem(&NUM_ACK_MAP, &index);
  if(!ack_num) {
    bpf_printk("KTF: unable to find num ack\n");
    return XDP_PASS;
  }

  int len = (int)(data_end - data); // There is no skb here,
  uint16_t send_csum = vtlh->checksum;

  int pkt_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(vtl_hdr_t) + VTL_DATA_SIZE;
  int pload_s = (len == pkt_size) ? 1024 : 363;

  int y = 0;
  uint8_t sum = 0;
  for(y = 0; y < pload_s; y++) {
    uint8_t *block = (uint8_t *)(d + y - 1);
    if(block + 1 > data_end)
      return XDP_DROP;
    sum ^= *block;
  }
  uint16_t recv_csum = sum; //vtlh->checksum;

  if(recv_csum == send_csum && *ack_num == vtlh->seq_num) { //Pkt not corrupted and in sequence
    if (bpf_map_lookup_elem(&xsks_map, &xsk_index)) {

      bpf_printk("KTF: Found %d, Wait %d\n", vtlh->seq_num, *ack_num);
      (*ack_num)++;
      *ack_num %= 8;
      bpf_map_update_elem(&NUM_ACK_MAP, &index, ack_num, BPF_ANY);

      return bpf_redirect_map(&xsks_map, xsk_index, 0);
    }
  }
  else {
    bpf_printk("KTF: packet %d not in sequence.\n", vtlh->seq_num);
    swap_src_dst_mac(eth);
    __be32 temp_ip = iph->saddr;
    iph->saddr = iph->daddr;
    iph->daddr = temp_ip;

    //TODO: resize pkt before XDP_TX
    vtlh->type = NACK;

    return XDP_TX;
  }

  return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
