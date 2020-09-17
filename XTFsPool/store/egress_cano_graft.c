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

struct bpf_elf_map SEC("maps") QOS_NEGO_MAP = {
  .type = BPF_MAP_TYPE_ARRAY,
  .size_key = sizeof(unsigned int),
  .size_value = sizeof(negotiation_state),
  .pinning = PIN_GLOBAL_NS,
  .max_elem = 1, // Should be > 1 to handle multi applis
};

#define RETX_MAX      10

/* TODO: Move to vtl.h 
 * This function move pointer to the first field of the VTL header pkt
 * If the pkt is not VTL, it will return NULL
*/
static __always_inline vtl_hdr_t* egress_move_to_vtl_hdr(struct __sk_buff *skb) {
  void *data = (void *)(long)skb->data;
  void *data_end = (void *)(long)skb->data_end;

  struct ethhdr *eth = (struct ethhdr *)data;
  if(eth + 1 > data_end){
    bpf_printk("ETH layer: malformed header.\n");
    return NULL;
  }

  struct iphdr *iph = (struct iphdr *)(eth + 1);
  if(iph + 1 > data_end){
    bpf_printk("IP layer: malformed header.\n");
    return NULL;
  }

  if(iph->protocol != IPPROTO_VTL)
    return NULL;

  vtl_hdr_t *hdr = (vtl_hdr_t *)(iph + 1);
  if(hdr + 1 > data_end){
    bpf_printk("VTL layer: malformed header.\n");
    return NULL;
  }

  return hdr;
}

SEC("egress_tf_sec")
int _tf_tc_egress(struct __sk_buff *skb) {
  
  vtl_hdr_t *vtlh = egress_move_to_vtl_hdr(skb);
  if(!vtlh)
    return TC_ACT_OK;
  
  if(vtlh->gid == -1) {

    bpf_printk("gid = %d ==> this is a DATA.\n", vtlh->gid);
    bpf_printk("pkt send to nic = %d\n", skb->ifindex+1);
    vtlh->pkt_type = DATA;
    bpf_vtl_nic_tx(skb, skb->ifindex+1, 0, 0);
  
  } 
  else { // Assumption: VTL present only two types of packets at egress path till now
    bpf_printk("gid = %d ==> this is a NEGO.\n", vtlh->gid);
    vtlh->pkt_type = NEGO;
    bpf_vtl_nic_tx(skb, skb->ifindex+1, 0, 0);
  }

  return TC_ACT_SHOT;
}

SEC("listener_tf_sec")
int _listener_tf(struct xdp_md *ctx) {
  int index = 0;
  negotiation_state nego_state = N_IDLE;

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

  if(vtlh->pkt_type == NEGO_ACK) {
    nego_state = N_ACCEPT;
    bpf_printk("NEGO_ACK received !\n");
  }
  else if(vtlh->pkt_type == NEGO_NACK) {
    nego_state = N_REFUSE;
    bpf_printk("NEGO_NACK received !\n");
  }
  // No need "else". The receiver will not send ACK unless for negotiation.

  long ret = bpf_map_update_elem(&QOS_NEGO_MAP, &index, &nego_state, BPF_ANY);
  if(ret != 0) {
    bpf_printk("Unable to update negotiation map.\n");
    return XDP_DROP;
  }

  return XDP_DROP; // Let it for the kern network stack???
}

char _license[] SEC("license") = "GPL";
