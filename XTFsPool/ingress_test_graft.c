/* SPDX-License-Identifier: GPL-2.0 */

//INGRESS GOBACKN ARQ GRAFT

#include <stdint.h>
#include <netinet/ip.h>
#include <linux/bpf.h>

#include "bpf/bpf_helpers.h"
#include "bpf/tc_bpf_util.h"

#include "../include/vtl.h"

#define WND_SIZE                16

struct bpf_map_def SEC("maps") xsks_map = {
        .type           = BPF_MAP_TYPE_XSKMAP,
        .key_size       = sizeof(int),
        .value_size     = sizeof(int),
        .max_entries    = 64,
};

struct bpf_map_def SEC("maps") NUM_ACK_MAP = {
        .type           = BPF_MAP_TYPE_ARRAY,
        .key_size       = sizeof(unsigned int),
        .value_size     = sizeof(uint16_t),
        .max_entries    = 1,
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
        if (eth + 1 > data_end)
                return XDP_DROP;

        struct iphdr *iph = (struct iphdr *)(eth + 1);
        if (iph + 1 > data_end)
                return XDP_DROP;

        if(iph->protocol != IPPROTO_VTL) {
                return XDP_PASS;
        }

        vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
        if(vtlh + 1 > data_end)
                return XDP_DROP;

        ack_num = (uint16_t *) bpf_map_lookup_elem(&NUM_ACK_MAP, &index);
        if(!ack_num)
                return XDP_PASS;

        uint16_t recv_csum = vtlh->checksum;

        uint16_t compute_csum = 0;
        if(vtl_csum(data, data_end, &compute_csum) == -1)
                return XDP_PASS;

        if(recv_csum == compute_csum && 
                *ack_num == vtlh->seq_num) {

                if (bpf_map_lookup_elem(&xsks_map, &xsk_index)) {

                        (*ack_num)++;
                        *ack_num %= WND_SIZE;
                        bpf_map_update_elem(&NUM_ACK_MAP, &index, ack_num, BPF_ANY);

                        return bpf_redirect_map(&xsks_map, xsk_index, 0);
                }
        }
        else {
                swap_src_dst_mac(eth);
                __be32 temp_ip = iph->saddr;
                iph->saddr = iph->daddr;
                iph->daddr = temp_ip;

                vtlh->pkt_type = NACK;

                // Trim NACK packet. TODO: add to ACK and update ip.len
                bpf_xdp_adjust_tail(ctx, -vtlh->payload_len);

                return XDP_TX;
        }

        return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
