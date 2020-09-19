// INGRESS STOP-WAIT ARQ KTF

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
        if(eth + 1 > data_end) {
                bpf_printk("ETH: malformed header.\n");
                return XDP_DROP;
        }

        struct iphdr *iph = (struct iphdr *)(eth + 1);
        if(iph + 1 > data_end) {
                bpf_printk("IPH: malformed header.\n");
                return XDP_DROP;
        }

        if(iph->protocol != IPPROTO_VTL){
                return XDP_PASS;
        }

        vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
        if(vtlh + 1 > data_end) {
                bpf_printk("VTLH: malformed header.\n");
                return XDP_DROP;
        }

        uint16_t recv_csum = vtlh->checksum;

        uint8_t *d = (uint8_t *)(vtlh + 1);
        if(d + 1 > data_end) {
                bpf_printk("VTL layer: malformed payload.\n");
                return XDP_DROP;
        }

        int y = 0;
        uint8_t sum = 0;
        for(y = 0; y < vtlh->payload_len; y++) {

                if(y > VTL_DATA_SIZE)
                        break;

                uint8_t *block = (uint8_t *)(d + y - 1);
                if(block + 1 > data_end) {
                        return XDP_DROP;
                }
                sum ^= *block;
        }

        uint16_t compute_csum = sum;
        if (compute_csum == recv_csum) { // NO corrupted pkt

                bpf_printk("send ACK !\n");
                if (bpf_map_lookup_elem(&xsks_map, &index))
                        return bpf_redirect_map(&xsks_map, index, 0);
        }
        else {
                bpf_printk("send NACK \n");
                swap_src_dst_mac(eth);
                __be32 temp_ip = iph->saddr;
                iph->saddr = iph->daddr;
                iph->daddr = temp_ip;
                vtlh->pkt_type = NACK;

                return XDP_TX;
        }

        return XDP_PASS;
}

char _license[] SEC("license") = "GPL";