// ### EGRESS CANO GRAFT

#include <stdbool.h>
#include <stdint.h>
#include <linux/if_ether.h>
#include <netinet/ip.h>
#include <linux/pkt_cls.h> //TODO: add it to headers repo ?
#include <linux/bpf.h>

#include "bpf/bpf_helpers.h"
#include "bpf/tc_bpf_util.h"

#include "../include/vtl.h"



SEC("egress_tf_sec")
int _tf_tc_egress(struct __sk_buff *skb) {
	void *data = (void *)(long)skb->data;
  void *data_end = (void *)(long)skb->data_end;

	struct ethhdr *eth = (struct ethhdr *)data;
  if(eth + 1 > data_end){
    bpf_printk("ETH layer: malformed header.\n");
    return TC_ACT_OK;
  }

  struct iphdr *iph = (struct iphdr *)(eth + 1);
  if(iph + 1 > data_end){
    bpf_printk("IP layer: malformed header.\n");
    return TC_ACT_OK;
  }

	if(iph->protocol != IPPROTO_VTL)
	         return TC_ACT_OK;

	vtl_hdr_t *vtlh = (vtl_hdr_t *)(iph + 1);
	if(vtlh + 1 > data_end){
    bpf_printk("VTL layer: malformed header.\n");
    return TC_ACT_OK;
  }

int i = 0;
do {
  //bpf_printk("redirect to nic = %d\n", skb->ifindex + 1);
  bpf_vtl_nic_tx(skb, skb->ifindex + 1, 0, 0);
  i++;
} while(i < 1);

  return TC_ACT_SHOT;
}

SEC("listener_tf_sec")
int _listener_tf(struct xdp_md *ctx) {
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

  return XDP_PASS; //Let it for the kern network stack???
}

char _license[] SEC("license") = "GPL";
