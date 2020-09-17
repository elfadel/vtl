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

struct bpf_map_def SEC("maps") GRAFT_ID_MAP = {
    .type = BPF_MAP_TYPE_ARRAY,
    .key_size = sizeof(unsigned int),
    .value_size = sizeof(int8_t),
    .max_entries = 256, 
};

SEC("ingress_tf_sec")
int xdp_sock_prog(struct xdp_md *ctx)
{
    int index = ctx->rx_queue_index, id = 0/*, p_time = 0*/;
    int ret;

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

        // Pass the required graft id to the cbr component
        if(vtlh->gid > 0) {
            ret = bpf_map_update_elem(&GRAFT_ID_MAP, &id, &(vtlh->gid), BPF_ANY);
            if(ret != 0) {
                bpf_printk("Could not update GRAFT_ID_MAP.\n");
            }

            return XDP_DROP;
        }
    
        // Else, there is no need for nego. Just pass data to appli
        if (bpf_map_lookup_elem(&xsks_map, &index))
            return bpf_redirect_map(&xsks_map, index, 0);

        return XDP_PASS;
}

char _license[] SEC("license") = "GPL";
