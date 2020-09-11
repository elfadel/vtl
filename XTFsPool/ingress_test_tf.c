/* SPDX-License-Identifier: GPL-2.0 */

#include <stdint.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <linux/bpf.h>
#include "bpf/bpf_helpers.h"

#include "../include/vtl.h"

struct bpf_map_def SEC("maps") xsks_map = {
    .type = BPF_MAP_TYPE_XSKMAP,
    .key_size = sizeof(int),
    .value_size = sizeof(int),
    .max_entries = 64,
};

#define bpf_printk(fmt, ...)                       \
    ({                                             \
        char ____fmt[] = fmt;                      \
        bpf_trace_printk(____fmt, sizeof(____fmt), \
                         ##__VA_ARGS__);           \
    })

SEC("ingress_tf_sec")
int xdp_sock_prog(struct xdp_md *ctx)
{
    int index = ctx->rx_queue_index, p_time = 0;

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

    p_time = bpf_ktime_get_ns();
    bpf_printk("Received pkt at rt=%d\n", p_time);

    vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
    if(vtlh + 1 > data_end){
        bpf_printk("VTLH: malformed header.\n");
        return XDP_DROP;
    }

    p_time = bpf_ktime_get_ns();
    bpf_printk("Finish Process at rt=%d\n", p_time);
    if (bpf_map_lookup_elem(&xsks_map, &index))
      return bpf_redirect_map(&xsks_map, index, 0);

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
