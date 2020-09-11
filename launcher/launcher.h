//SPDX-License-Identifier: GPL-2.0

/*
* @file :		launcher.h
* @authors :	El-Fadel Bonfoh, Cedric Tape
* @date :		12/2019
* @version :	1.0
* @brief :
*/
#pragma once

#include <assert.h>
//#include <bpf/bpf.h> TODO: fix in vtl_ui.c, conflict with pcap.h. bpf.h contains bpf_map_update_elem()
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctype.h>
#include <mntent.h>

#include "../include/common/tc_user_helpers.h"
#include "../include/common/xdp_user_helpers.h"

#define BPF_FILE_NAME_SIZE 			512
#define MAX_TF_PROGS 				32 // one way max ???
#define MAX_MAPS					32 // per TF or overall VTL ?
#define TC_INGRESS_ATTACH 			0x0
#define TC_EGRESS_ATTACH 			0x1

struct bpf_load_map_def {
	unsigned int type;
	unsigned int key_size;
	unsigned int value_size;
	unsigned int max_entries;
	unsigned int map_flags;
	unsigned int inner_map_idx;
	unsigned int numa_node;
};

struct bpf_map_data {
	int fd;
	char *name;
	size_t elf_offset;
	struct bpf_load_map_def def;
};

extern int progs_fd[MAX_TF_PROGS];
extern int events_fd[MAX_TF_PROGS];
extern char bpf_log_buff[ERR_BUFF_SIZE];
extern int progs_count;

extern int maps_fd[MAX_MAPS];
extern struct bpf_map_data maps_data[MAX_MAPS]; // one-to-one mapping with maps_fd[]
extern int maps_data_count;

typedef void (*fixup_map_cb)(struct bpf_map_data *map, int idx);

// NOTE: load = (load + attach)
/* egress TF loader */
int launcher_load_egress_tf(const char *tf_file, char *interface, char* sec_name);
int launcher_unload_egress_tf(struct tc_config *cfg, char *interface, int flags);

/* ingress TF loader */
int launcher_load_ingress_tf(const char *tf_file, char *interface, char *sec_name, __u32 xdp_flags);
int launcher_unload_ingress_tf(char *interface, __u32 xdp_flags);

/* listener TF loader 
 *
 * Note: Serve also to load xdp part of hooker prog !
 *
*/
int launcher_load_listener_tf(const char *sec_name, struct bpf_insn *prog, int size, char *ifname, int xdp_flags);
int launcher_unload_listerner_tf(char *interface, __u32 xdp_flags);

int launcher_load_hooker_progs(const char *sec_name, struct bpf_insn *prog, int prog_size);
int launcher_unload_hooker_progs(void);

int launcher_load_graft_file(const char *path_to_file, char *ifname);

