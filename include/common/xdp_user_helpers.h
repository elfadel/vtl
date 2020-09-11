/* Common BPF/XDP functions used by userspace side programs */
#pragma once

#include <linux/types.h>
#include <net/if.h>
#include <stdbool.h> // error: unknown type name ‘bool’...Weird

//TODO: put it common.h
#define ERR_BUFF_SIZE     16384
//Facilite la comparaison avant strcpy
#define BPF_FILE_SIZE	512
#define BPF_PROGSEC_SIZE 32

#ifndef PATH_MAX
#define PATH_MAX	4096
#endif


//#include <bpf/libbpf.h>
//#include "common_libbpf.h"

typedef struct xdp_config xdp_cfg_t;
struct xdp_config {
	
	char filename[BPF_FILE_SIZE];
	char progsec[BPF_PROGSEC_SIZE];
	char pin_dir[512];
	bool do_unload;
	bool reuse_maps;
	__u32 xdp_flags;
	int ifindex;
	char ifname[IF_NAMESIZE];
	char ifname_buf[IF_NAMESIZE];
	char err_buf[ERR_BUFF_SIZE];
};

/**
 * Attach bpf program to XDP hook point.
 * @param ifindex - interface for attaching bpf program
 * @param xdp_flags - 
 * @param prog_fd - bpf program's file descriptor
 * @retval 0 on success
 * @retval != 0 on failure
 **/ 
int 
xdp_link_attach(int ifindex, __u32 xdp_flags, int prog_fd);

/**
 * Detach bpf program to XDP hook point.
 * @param ifindex - interface for attaching bpf program
 * @param xdp_flags - 
 * @param prog_fd - bpf program's file descriptor
 * @retval 0 on success
 * @retval != 0 on failure
 **/ 
int 
xdp_link_detach(int ifindex, __u32 xdp_flags, __u32 expected_prog_id);

/**
 * Load bpf program in Kernel space and Attach them to 
 * XDP hook point.
 * @param xdp_cfg pointer on XDP configuration structure
 * @return pointer loaded bpf object or NULL on failure
 **/ 
struct bpf_object *
load_bpf_and_xdp_attach(const char *filename, char *ifname, char *sec_name, 
						__u32 xdp_flags, bool reuse_maps);