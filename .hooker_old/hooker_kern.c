/* 
 * @file :		hooker_kern.c
 * @author :		Cédric Tape, El-Fadel Bonfoh
 * @date :		05/2019
 * @version :		0.1
 * @brief : 		hooker kernel part
*/ 

//#include <uapi/linux/bpf.h>
#include <linux/bpf.h>
#include "bpf_helpers.h"
#include "bpf_endian.h"

#define SOCKOPS_MAP_SIZE            20
#define H_PORT                      10002
#define S_PORT                      9090

#ifndef memcpy
# define memcpy(dest, src, n)   __builtin_memcpy((dest), (src), (n))
#endif

struct sock_key {
	
	__u32 src_ip4;
    	__u32 dst_ip4;	
    	__u32 src_port;	
    	__u32 dst_port;	

}__attribute__((packed));

struct bpf_map_def SEC("maps") tx = {
	.type = BPF_MAP_TYPE_HASH,
    	.key_size = sizeof(int),
    	.value_size = sizeof(struct sock_key),
    	.max_entries = 1,
};

struct bpf_map_def SEC("maps") hmap = {
	.type = BPF_MAP_TYPE_SOCKHASH,
	.key_size = sizeof(struct sock_key),
	.value_size = sizeof(int),
	.max_entries = 20,
};

static __always_inline 
void h_add_hmap(struct bpf_sock_ops *skops) {
	
	struct sock_key key = {}; 
    	struct sock_key skey = {};  
    	int tx_key = 0;

    	key.dst_ip4 = skops->remote_ip4; //TODO : name changing...
    	key.src_ip4 = skops->local_ip4;
    	key.dst_port = skops->remote_port ; 
    	key.src_port = bpf_ntohl(skops->local_port) ;
    
    	if(key.src_port == S_PORT) {
		skey = key;
        	bpf_map_update_elem(&tx, &tx_key,&skey, BPF_ANY);
    	}
        
    	bpf_sock_hash_update(skops, &hmap, &key, BPF_NOEXIST);
}

SEC("sockops")
int msg_redirector_prog1(struct bpf_sock_ops *skops) {
	
	int key = 0;
    	int *value;
    	__u32 family, op = skops->op;

    	switch(op) {
		
		// Established connection
		case BPF_SOCK_OPS_PASSIVE_ESTABLISHED_CB:
        	case BPF_SOCK_OPS_ACTIVE_ESTABLISHED_CB:
			h_add_hmap(skops);

            		break;
        	default:
            		break;
    	} 

    return 0;
}

SEC("sk_msg")
int msg_redirector_prog2(struct sk_msg_md *msg) {
	
	__u64 flags = BPF_F_INGRESS;
    	__u32 lport, rport;
    	struct sock_key hsock_key = {};
    
    	lport = msg->local_port;
    	if(lport == H_PORT) {
		int tx_key = 0;
        	struct sock_key *value = NULL;
        	struct sock_key skey = {};
        	value = bpf_map_lookup_elem(&tx, &tx_key);
        	skey = *value;
        	bpf_msg_redirect_hash(msg, &hmap, &skey, flags);
    	}
    	else {
        	bpf_msg_redirect_hash(msg, &hmap, &hsock_key, flags);
    	}
    
    	return SK_PASS;
} 

char _license[] SEC("license") = "GPL";
