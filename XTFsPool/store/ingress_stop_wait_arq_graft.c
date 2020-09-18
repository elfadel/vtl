//INGRESS STOP-WAIT ARQ KTF

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
    int index = ctx->rx_queue_index;
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

    uint16_t send_csum = vtlh->checksum;
    int len = (int)(data_end - data);
    int pkt_size = sizeof(struct ethhdr) + sizeof(struct iphdr) + sizeof(vtl_hdr_t) + VTL_DATA_SIZE;
    int pload_s = (len == pkt_size) ? 1024 : 696;

    uint8_t *d = (uint8_t *)(vtlh + 1);
    if(d + 1 > data_end) {
      bpf_printk("VTL layer: malformed payload.\n");
      return XDP_DROP;
    }

    //uint16_t sender_csum = vtlh->checksum;
    int y = 0;
    uint8_t sum = 0;
    for(y = 0; y < pload_s; y++){
      uint8_t *block = (uint8_t *)(d + y - 1);
      if(block + 1 > data_end) {
        return XDP_DROP;
      }
      sum ^= *block;
    }
    uint16_t recv_csum = sum;/*vtlh->checksum;*/
    if (recv_csum == send_csum) // NO corrupted pkt
    {
      if (bpf_map_lookup_elem(&xsks_map, &index))
          return bpf_redirect_map(&xsks_map, index, 0);
    }
    else {
      swap_src_dst_mac(eth);
      __be32 temp_ip = iph->saddr;
      iph->saddr = iph->daddr;
      iph->daddr = temp_ip;
      vtlh->type = NACK;

      return XDP_TX;
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";