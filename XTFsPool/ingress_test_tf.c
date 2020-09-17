// INGRESS CANO GRAFT

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

struct bpf_map_def SEC("maps") QOS_NEGO_MAP = {
    .type = BPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(unsigned int),
    .value_size = sizeof(negotiation_state),
    .max_entries = 1,
};

SEC("ingress_tf_sec")
int xdp_sock_prog(struct xdp_md *ctx)
{
    int index = ctx->rx_queue_index, id = 0/*, p_time = 0*/;
    negotiation_state *nego_state = NULL;

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
    
    if(vtlh->gid > 0) {
        // Just pass the pkt to user program and wait a while
        bpf_vtl_start_timer(10); // 10 ms

    nego_state = (negotiation_state *) bpf_map_lookup_elem(&QOS_NEGO_MAP, &id);
    if(!nego_state) {
        bpf_printk("Can not lookup QOS_NEGO_MAP. Skipping.\n");
        return XDP_DROP;
    }

    if(*nego_state == N_ACCEPT) {
        vtlh->pkt_type = NEGO_ACK;
    }
    else if(*nego_state == N_REFUSE){
        vtlh->pkt_type = NEGO_NACK;
    }

    if(bpf_map_lookup_elem(&xsks_map, &index))
        return bpf_redirect_map(&xsks_map, index, 0); // TODO replace
    }
    else if(vtlh->gid == 0) {
        // Just continueto send with cano. Pass data to appli and send ack
        if (bpf_map_lookup_elem(&xsks_map, &index))
            return bpf_redirect_map(&xsks_map, index, 0);
    }

    return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
