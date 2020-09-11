//SPDX-License-Identifier: GPL-2.0

/*
 * @file :		launcher.c
 * @authors :	El-Fadel Bonfoh, Cedric Tape
 * @date :		12/2019
 * @version :	0.1
 * @brief :
*/

#include <bpf/libbpf.h>
#include <linux/if_link.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "launcher.h"

static char license[128];
static int kern_version; // TODO: it is __u32 under libbpf
static bool processed_sec[128];
static const char *pinned_skops_file = "/sys/fs/bpf/skops";
static const char *pinned_skmap_file = "/sys/fs/bpf/skmap";
static const char *pinned_appli_file = "/sys/fs/bpf/appli";
char bpf_log_buff[ERR_BUFF_SIZE];

int progs_fd[MAX_TF_PROGS];
int events_fd[MAX_TF_PROGS];
int progs_count;
int prog_array_fd = -1; // For tail calls

int maps_fd[MAX_MAPS];
struct bpf_map_data maps_data[MAX_MAPS];
int maps_data_count;

int cg_fd = -1;

static int __populate_prog_array(const char *sec_name, int prog_fd) {
	int ind = atoi(sec_name), err;

	err = bpf_map_update_elem(prog_array_fd, &ind, &prog_fd, BPF_ANY);
	if(err < 0) {
		printf("[LAUNCHER]: WARN -- __populate_prog_array() -- failed to store prog_fd in prog_array\n");
		return -1;
	}

	printf("[LAUNCHER]: Hooker activated !\n");
	return 0;
}

char *__find_cg_root(void) {
	FILE *f;
	struct mntent *mnt;

	f = fopen("/proc/mounts", "r");
	if(!f)
		return NULL;

	while((mnt = getmntent(f))) {
		if(strcmp(mnt->mnt_type, "cgroup2") == 0) {
			fclose(f);
			return strdup(mnt->mnt_dir);
		}
	}

	fclose(f);
	return NULL;
}

/**
 * @brief: deploy transport functions on tc hook
 *         to process outgoing data.
 * @param: struct tc_config *cfg - configure tc attachment
 * @param: char *tf_file - bpf file to load in kernel
 * @param: char *interface - NIC where to load tf
 * @param: int flags -
 * @return:
 *
*/
int launcher_load_egress_tf(const char *path, char *interface, char* sec_name) {

	printf("[LAUNCHER]: Loading egress KTF ...\n");

	if(tc_egress_attach_bpf(interface, path, sec_name)) {
		return -1;
	}

	// TODO: Add prog_fd to progs_fd[]
	printf("[LAUNCHER]: egress KTF loaded !\n");

	return 0;
}
/**
  * @brief: remove transport functions on tc hook
  * @param: struct tc_config *cfg -
  * @param: int flags -
  * @return:
  *
*/
int launcher_unload_egress_tf(struct tc_config *cfg, char *interface, int flags) {

	int ifindex = if_nametoindex(interface);

	if(!(ifindex)) {
		snprintf(cfg->err_buf, ERR_BUFF_SIZE, "ERR: --egress \"%s\" not real or virt dev\n",
		 					interface);
		return -1;
	}

	strcpy(cfg->dev, interface);
	printf("Unloading egress KTF on device %s...\n", cfg->dev);
	switch(flags) {
		case TC_INGRESS_ATTACH:
			return 0;
			break;
		case TC_EGRESS_ATTACH:
			tc_remove_egress(cfg);
			break;
		default:
			snprintf(cfg->err_buf, ERR_BUFF_SIZE,
								"ERR: launcher_remove_egress_tf() failed, unknown flags.\n");
			return -1;
			break;
	}

	return 0;
}

/**
  * @brief: deploy transport functions on xdp hook
  	   to process incoming data.
  * @param: struct xdp_config *cfg - configure xdp attachment
  * @param: char *tf_file - bpf file to load in kernel
  * @param: char *interface - NIC where to load tf
  * @return:
  *
*/
int launcher_load_ingress_tf(const char *path, char *interface, char *sec_name, __u32 xdp_flags) 
{
	int prog_fd = -1;
	struct bpf_object *bpf_obj = NULL;
	struct bpf_program *bpf_prog = NULL;

	bool reuse_maps = false;
	printf("[LAUNCHER] Loading ingress KTF ...\n");
	bpf_obj = load_bpf_and_xdp_attach(path, interface, sec_name, xdp_flags, reuse_maps);
	if(!bpf_obj) {
		printf("[LAUNCHER]: Ingress KTF loading failed.\n");
		return -1; // TODO: modify return number to 1.
	}

	bpf_prog = bpf_object__find_program_by_title(bpf_obj, sec_name);
	if(!bpf_prog) {
		printf("[LAUNCHER]: Ingress KTF loading failed -- unable to find prog by title.\n");
		return -1;
	}

	prog_fd = bpf_program__fd(bpf_prog);
	if(prog_fd <= 0) {
		printf("[LAUNCHER]: Ingress KTF loading failed -- unable to find prog fd.\n");
		return -1;
	}

	progs_fd[progs_count++] = prog_fd;
	printf("[LAUNCHER]: ingress KTF loaded !\n");

	return 0;
}

/**
  * @brief: remove transport functions on xdp hook
  * @param: struct xdp_config *cfg -
  * @param: char *interface - NIC where to remove tf
  * @param: flags -
  * @return:
  *
*/
int launcher_unload_ingress_tf(char *interface, __u32 xdp_flags) {

	int ret, ifindex = if_nametoindex(interface);

	ret = xdp_link_detach(ifindex, xdp_flags, 0);

	if(ret != 0) {
		return -1;
	}

	return 0;
}

int launcher_load_listener_tf(const char *sec_name, struct bpf_insn *prog, int size, char *ifname, int xdp_flags) {
	bool is_xdp_listener = (strncmp(sec_name, "listener_tf_sec", 15) == 0) || (strncmp(sec_name, "hooker_listener", 15) == 0);
	if(!is_xdp_listener) {
		printf("[LAUNCHER]: launcher_load_listener_tf() -- the prog \"%s\" is not a listener. Loading failed.\n", sec_name);
		return -1;
	}

	size_t insns_cnt = size / sizeof(struct bpf_insn);
	enum bpf_prog_type prog_type = BPF_PROG_TYPE_XDP;
	int fd;

	if(progs_count == MAX_TF_PROGS) {
		printf("[LAUNCHER]: launcher_load_listener_tf() -- MAX progs reached. Loading failed.\n");
		return -1; // TODO: aligned with the others loading funcs
	}

	printf("[LAUNCHER]: Loading listener KTF ...\n");

	fd = bpf_load_program(prog_type, prog, insns_cnt, license, kern_version, bpf_log_buff, ERR_BUFF_SIZE);
	if(fd < 0) {
		printf("[LAUNCHER]: bpf_load_program() failed. -- err=%d\n%s\n", errno, bpf_log_buff);
		return -1;
	}

	progs_fd[progs_count++] = fd;

	int ifindex = if_nametoindex(ifname);
	if(!(ifindex)) {
		printf("[LAUNCHER]: launcher_load_listener_tf() failed: \"%s\" not real or virt dev\n", ifname);
		return -1;
	}
	if(xdp_link_attach(ifindex, xdp_flags, fd)) {
		printf("[LAUNCHER]: launcher_load_listener_tf() failed -- Unable to attach prog to iface.\n");
		return -1;
	}

	printf("[LAUNCHER]: listener KTF loaded !\n");

	return 0;
}

int launcher_unload_listerner_tf(char *interface, __u32 xdp_flags) {
	printf("[LAUNCHER]: unloading listener xdp\n");
	return launcher_unload_ingress_tf(interface, xdp_flags);
}

int launcher_load_hooker_progs(const char *sec_name, struct bpf_insn *prog, int prog_size) {
	bool is_sockops = strncmp(sec_name, "hooker_sockops", 14) == 0;
	bool is_rdctor = strncmp(sec_name, "hooker_redirector", 17) == 0;

	if((!is_sockops && !is_rdctor) || (is_sockops && is_rdctor)) {
		printf("[LAUNCHER]: launcher_load_hooker_progs() -- check the prog \"%s\" type. Loading failed.\n", sec_name);
		return -1;
	}

	size_t insns_cnt = prog_size  / sizeof(struct bpf_insn);
	enum bpf_prog_type prog_type = BPF_PROG_TYPE_SOCK_OPS;

	// Now check if there is a need for tail calls or get the num of redirector.
	if(is_sockops)
		sec_name += 14;
	else
		sec_name += 17;
	if(*sec_name != '/') // No need for tail call
		return 0;
	sec_name++;
	if(!isdigit(*sec_name)) {
		printf("[LAUNCHER]: invalid prog number.\n"); // User doesn't follow naming convention
		return -1;
	}

	if(is_rdctor){
		switch(*sec_name){
			case '0':
				prog_type = BPF_PROG_TYPE_SK_MSG;
			break;
			
			case '1':
			case '2':
				prog_type = BPF_PROG_TYPE_SK_SKB;
				break;

			default:
				/* Should never happen */
				break;
		}
	}
	int fd;

	if(progs_count == MAX_TF_PROGS) {
		printf("[LAUNCHER]: launcher_load_hooker_progs() -- MAX progs reached. Loading failed.\n");
		return -1; // TODO: aligned with the others loading funcs
	}

	fd = bpf_load_program(prog_type, prog, insns_cnt, license, kern_version, bpf_log_buff, ERR_BUFF_SIZE);
	if(fd < 0) {
		printf("[LAUNCHER]: bpf_load_program() failed. -- err=%d\n%s\n", errno, bpf_log_buff);
		return -1;
	}

	progs_fd[progs_count++] = fd;

	// Attaching ...
	if(is_sockops) {
		/*** Attach to cgroup2 ***/
		char *cg_root_path = NULL;
		cg_root_path = __find_cg_root();
		if(cg_root_path == NULL) {
			printf("[LAUNCHER]: can't find cgroup2 root.\n");
			return -1;
		}
		cg_fd = open(cg_root_path, O_RDONLY);
		if(cg_fd < 0) {
			printf("[LAUNCHER]: failed to open cgroup root file\n");
			return -1;
		}

		printf("[LAUNCHER]: attaching Hooker skops prog ...\n");
		int ret = bpf_prog_attach(fd, cg_fd, BPF_CGROUP_SOCK_OPS, BPF_F_ALLOW_MULTI);
		if(ret) {
			printf("[LAUNCHER]: failed to attach Hooker skops prog to cgroup root\n");
			return -1;
		}
		printf("[LAUNCHER]: sock_ops attached !\n");

		printf("[LAUNCHER]: pinning Hooker skops prog to %s\n", pinned_skops_file);
		ret = bpf_obj_pin(fd, pinned_skops_file);
		if(ret) {
			printf("[LAUNCHER]: WARN - failed to pin Hooker skops prog.\n");
		}
		else
			printf("[LAUNCHER]: sock_ops pinned ! \n");
	}
	else {
		/*** Attach to sockmap ***/
		int ret = -1;
		switch(*sec_name) {
			case '0':
				printf("[LAUNCHER]: attaching Hooker skmsg prog ...\n");
				ret = bpf_prog_attach(fd, maps_fd[0] /* <==> HK_SOCK_MAP */, BPF_SK_MSG_VERDICT, 0);
				if(ret) {
					printf("[LAUNCHER]: failed to attach Hooker skmsg prog to sockmap\n");
					return -1;
				}
				printf("[LAUNCHER]: sk_msg attached to sockmap %d!\n", maps_fd[0]);
				break;

			case '1':
				printf("[LAUNCHER]: attaching Hooker skskb parser prog ...\n");
				ret = bpf_prog_attach(fd, maps_fd[0] /* <==> HK_SOCK_MAP */, BPF_SK_SKB_STREAM_PARSER, 0);
				if(ret) {
					printf("[LAUNCHER]: failed to attach Hooker skskb parser prog to sockmap\n");
					return -1;
				}
				printf("[LAUNCHER]: skskb parser attached to sockmap %d!\n", maps_fd[0]);
				break;

			case '2':
				printf("[LAUNCHER]: attaching Hooker skskb verdict prog ...\n");
				ret = bpf_prog_attach(fd, maps_fd[0] /* <==> HK_SOCK_MAP */, BPF_SK_SKB_STREAM_VERDICT, 0);
				if(ret) {
					printf("[LAUNCHER]: failed to attach Hooker skskb verdict prog to sockmap\n");
					return -1;
				}
				printf("[LAUNCHER]: skskb verdict attached to sockmap %d!\n", maps_fd[0]);
				break;

			default:
				/* Should never happen */
				break;
		}
	}

	// There is not tail call currently
	//return __populate_prog_array(sec_name, fd);
	return 0;
}

int launcher_unload_hooker_progs() {

	// TODO: Fix. progs_fd[] and maps_fd[] always return 0
	/*if(progs_fd[1] && maps_fd[0]) { // sk_msg && sk_map  
		printf("[LAUNCHER]: detaching sk_msg...\n");

		if(system("bpftool prog detach id progs_fd[1] msg_verdict pinned \"/sys/fs/bpf/skmap\"") == -1) {
			printf("[LAUNCHER]: failed to detach sk_msg\n");
			perror("launcher_unload_hooker_progs");
			return -1;
		}
	} */

	//if(progs_fd[0] && cg_fd != -1) {
		printf("[LAUNCHER]: detaching sk_ops...\n");

		if(system("bpftool cgroup detach \"/sys/fs/cgroup/unified/\" sock_ops pinned \"/sys/fs/bpf/skops\"") == -1) {
			printf("[LAUNCHER]: failed to detach sk_ops\n");
			perror("launcher_unload_hooker_progs");
			return -1;
		}

		if(system("rm \"/sys/fs/bpf/skops\"") == -1) {
			printf("[LAUNCHER]: WARN - failed to remove pinned skops\n");
		}
	//}

	if(system("rm \"/sys/fs/bpf/appli\"") == -1) {
		printf("[LAUNCHER]: WARN - failed to remove pinned appli map\n");
	}
	
	if(system("rm \"/sys/fs/bpf/skmap\"") == -1) {
		printf("[LAUNCHER]: WARN - failed to remove pinned skmap\n");
	}

	printf("[LAUNCHER]: sk_ops and sk_msg detached !\n");
	return 0;
}

static int __load_maps(struct bpf_map_data *maps, int nr_maps,
		     fixup_map_cb fixup_map) 
{
	int i, numa_node;

	for (i = 0; i < nr_maps; i++) {
		if (fixup_map) {
			fixup_map(&maps[i], i);
			/* Allow userspace to assign map FD prior to creation */
			if (maps[i].fd != -1) {
				maps_fd[i] = maps[i].fd;
				continue;
			}
		}

		numa_node = maps[i].def.map_flags & BPF_F_NUMA_NODE ?
			maps[i].def.numa_node : -1;

		if (maps[i].def.type == BPF_MAP_TYPE_ARRAY_OF_MAPS ||
		    maps[i].def.type == BPF_MAP_TYPE_HASH_OF_MAPS) {
			int inner_map_fd = maps_fd[maps[i].def.inner_map_idx];

			maps_fd[i] = bpf_create_map_in_map_node(maps[i].def.type,
							maps[i].name,
							maps[i].def.key_size,
							inner_map_fd,
							maps[i].def.max_entries,
							maps[i].def.map_flags,
							numa_node);
		} else {
			maps_fd[i] = bpf_create_map_node(maps[i].def.type,
							maps[i].name,
							maps[i].def.key_size,
							maps[i].def.value_size,
							maps[i].def.max_entries,
							maps[i].def.map_flags,
							numa_node);
		}
		if (maps_fd[i] < 0) {
			printf("failed to create map %d (%s): %d %s\n",
			       i, maps[i].name, errno, strerror(errno));
			return 1;
		}
		maps[i].fd = maps_fd[i];

		if (maps[i].def.type == BPF_MAP_TYPE_PROG_ARRAY)
			prog_array_fd = maps_fd[i];
		if(maps[i].def.type == BPF_MAP_TYPE_SOCKHASH) {
			printf("[LAUNCHER]: pinning SOCKMAP fd = %d to %s\n", maps_fd[i], pinned_skmap_file);
			int ret = bpf_obj_pin(maps_fd[i], pinned_skmap_file);
			if(ret)
				printf("[LAUNCHER]: WARN - failed to pin sockmap.\n");
			else
				printf("[LAUNCHER]: sockmap pinned !\n");
		}
		if(strcmp(maps[i].name, "APP_HASH_ID_MAP") == 0) {
			printf("[LAUNCHER]: pinning app_info_map fd = %d to %s\n", maps_fd[i], pinned_appli_file);
			int ret = bpf_obj_pin(maps_fd[i], pinned_appli_file);
			if(ret)
				printf("[LAUNCHER]: WARN - failed to pin applimap.\n");
			else
				printf("[LAUNCHER]: applimap pinned !\n"); 
		}
	}
	return 0;
}

static int __get_sec(Elf *elf, int i, GElf_Ehdr *ehdr, char **shname,
					GElf_Shdr *shdr, Elf_Data **data)
{
	Elf_Scn *scn;
	scn = elf_getscn(elf, i);
	if(!scn)
		return 1;

	if(gelf_getshdr(scn, shdr) != shdr)
		return 2;

	*shname = elf_strptr(elf, ehdr->e_shstrndx, shdr->sh_name);
	if(!*shname || !shdr->sh_size)
		return 3;

	*data = elf_getdata(scn, 0);
	if(!*data || elf_getdata(scn, *data) != NULL)
		return 4;

	return 0;
}

static int __parse_relo_and_apply(Elf_Data *data, Elf_Data *symbols,
				GElf_Shdr *shdr, struct bpf_insn *insn,
				struct bpf_map_data *maps, int nr_maps)
{
	int i, nrels;

	nrels = shdr->sh_size / shdr->sh_entsize;

	for (i = 0; i < nrels; i++) {
		GElf_Sym sym;
		GElf_Rel rel;
		unsigned int insn_idx;
		bool match = false;
		int map_idx;

		gelf_getrel(data, i, &rel);

		insn_idx = rel.r_offset / sizeof(struct bpf_insn);

		gelf_getsym(symbols, GELF_R_SYM(rel.r_info), &sym);

		if (insn[insn_idx].code != (BPF_LD | BPF_IMM | BPF_DW)) {
			printf("invalid relo for insn[%d].code 0x%x\n",
			       insn_idx, insn[insn_idx].code);
			return 1;
		}
		insn[insn_idx].src_reg = BPF_PSEUDO_MAP_FD;

		/* Match FD relocation against recorded map_data[] offset */
		for (map_idx = 0; map_idx < nr_maps; map_idx++) {
			if (maps[map_idx].elf_offset == sym.st_value) {
				match = true;
				break;
			}
		}
		if (match) {
			insn[insn_idx].imm = maps[map_idx].fd;
		} else {
			printf("invalid relo for insn[%d] no map_data match\n",
			       insn_idx);
			return 1;
		}
	}

	return 0;
}

static int __cmp_symbols(const void *l, const void *r)
{
	const GElf_Sym *lsym = (const GElf_Sym *)l;
	const GElf_Sym *rsym = (const GElf_Sym *)r;

	if (lsym->st_value < rsym->st_value)
		return -1;
	else if (lsym->st_value > rsym->st_value)
		return 1;
	else
		return 0;
}

static int __load_elf_maps_section(struct bpf_map_data *maps, int maps_shndx,
									Elf *elf, Elf_Data *symbols, int strtabidx)
{
	int map_sz_elf, map_sz_copy;
	bool validate_zero = false;
	Elf_Data *data_maps;
	int i, nr_maps;
	GElf_Sym *sym;
	Elf_Scn *scn;

	if(maps_shndx < 0)
		return -EINVAL;

	// Get Data for maps section via elf index
	scn = elf_getscn(elf, maps_shndx);
	if(scn)
		data_maps = elf_getdata(scn, NULL);
	if(!scn || !data_maps) {
		printf("Failed to get Elf_Data from maps section %d\n", maps_shndx);
		return -EINVAL;
	}

	// For each map get corresponding symbol table entry
	sym = calloc(MAX_MAPS+1, sizeof(GElf_Sym));
	for(i = 0, nr_maps = 0; i < symbols->d_size / sizeof(GElf_Sym); i++) {
		assert(nr_maps < MAX_MAPS+1);
		if(!gelf_getsym(symbols, i, &sym[nr_maps]))
			continue;
		if(sym[nr_maps].st_shndx != maps_shndx)
			continue;

		// Only increment iff maps section
		nr_maps++;
	}

	/* Align to map_fd[] order, via sort on offset in sym.st_value */
	qsort(sym, nr_maps, sizeof(GElf_Sym), __cmp_symbols);

	/* Keeping compatible with ELF maps section changes
	 * ------------------------------------------------
	 * The program size of struct bpf_load_map_def is known by loader
	 * code, but struct stored in ELF file can be different.
	 *
	 * Unfortunately sym[i].st_size is zero.  To calculate the
	 * struct size stored in the ELF file, assume all struct have
	 * the same size, and simply divide with number of map
	 * symbols.
	 */
	map_sz_elf = data_maps->d_size / nr_maps;
	map_sz_copy = sizeof(struct bpf_load_map_def);
	if (map_sz_elf < map_sz_copy) {
		/*
		 * Backward compat, loading older ELF file with
		 * smaller struct, keeping remaining bytes zero.
		 */
		map_sz_copy = map_sz_elf;
	} else if (map_sz_elf > map_sz_copy) {
		/*
		 * Forward compat, loading newer ELF file with larger
		 * struct with unknown features. Assume zero means
		 * feature not used.  Thus, validate rest of struct
		 * data is zero.
		 */
		validate_zero = true;
	}

	/* Memcpy relevant part of ELF maps data to loader maps */
	for (i = 0; i < nr_maps; i++) {
		struct bpf_load_map_def *def;
		unsigned char *addr, *end;
		const char *map_name;
		size_t offset;

		map_name = elf_strptr(elf, strtabidx, sym[i].st_name);
		maps[i].name = strdup(map_name);
		if (!maps[i].name) {
			printf("strdup(%s): %s(%d)\n", map_name,
			       strerror(errno), errno);
			free(sym);
			return -errno;
		}

		/* Symbol value is offset into ELF maps section data area */
		offset = sym[i].st_value;
		def = (struct bpf_load_map_def *)(data_maps->d_buf + offset);
		maps[i].elf_offset = offset;
		memset(&maps[i].def, 0, sizeof(struct bpf_load_map_def));
		memcpy(&maps[i].def, def, map_sz_copy);

		/* Verify no newer features were requested */
		if (validate_zero) {
			addr = (unsigned char *) def + map_sz_copy;
			end  = (unsigned char *) def + map_sz_elf;
			for (; addr < end; addr++) {
				if (*addr != 0) {
					free(sym);
					return -EFBIG;
				}
			}
		}
	}

	free(sym);
	return nr_maps;
}

/***
 * @brief: 	This function will initialize TFs and Maps 
 *		  	composing the grafts.
 * @param: 	char *path - the path to object file containing TFs and Maps
 * @return: 	int - zero on success
*/
int launcher_load_graft_file(const char *path, char *ifname) {
	int fd, i, maps_shndx = -1, nr_maps = 0, ret, strtabidx = -1;
	char *shname, *shname_prog;
	Elf *elf;
	Elf_Data *data, *data_prog, *data_maps = NULL, *symbols = NULL;
	GElf_Ehdr ehdr;
	GElf_Shdr shdr, shdr_prog;

	// reset global vars
	kern_version = 0;
	memset(license, 0, sizeof(license));
	memset(processed_sec, 0, sizeof(processed_sec));

	if(elf_version(EV_CURRENT) == EV_NONE)
		return 1;

	fd = open(path, O_RDONLY, 0);
	if(fd < 0)
		return 1;

	// TODO: open in ELF_C_RDWR mode for VTL composition compilation
	elf = elf_begin(fd, ELF_C_READ, NULL);
	if(!elf)
		return 1;

	if(gelf_getehdr(elf, &ehdr) != &ehdr)
		return 1;

	// Scan over all elf sections to get license and map info
	for(i = 1; i < ehdr.e_shnum; i++) {

		if(__get_sec(elf, i, &ehdr, &shname, &shdr, &data))
			continue;

		if(0) /* helpful for llvm debugging */
			printf("section %d:%s data %p size %zd link %d flags %d\n", 
					i, shname, data->d_buf, data->d_size, 
					shdr.sh_link, (int) shdr.sh_flags);

		if(strcmp(shname, "license") == 0) {
			processed_sec[i] = true;
			memcpy(license, data->d_buf, data->d_size);
		}
		else if(strcmp(shname, "version") == 0) {
			processed_sec[i] = true;
			if(data->d_size != sizeof(int)) {
				printf("invalid size of version section %zd\n\n", data->d_size);
				return 1;
			}
		}
		else if(strcmp(shname, "maps") == 0) {
			int j;

			maps_shndx = i;
			data_maps = data;
			for(j = 0; j < MAX_MAPS; j++)
				maps_data[j].fd = -1;
		}
		else if(shdr.sh_type == SHT_SYMTAB) {
			strtabidx = shdr.sh_link;
			symbols = data;
		}
	}

	ret = 1;

	if(!symbols) {
		printf("missing SHT_SYMTAB section\n");
		goto done;
	}

	if(data_maps) {
		nr_maps = __load_elf_maps_section(maps_data, maps_shndx, elf, 
											symbols, strtabidx);
		if(nr_maps < 0) {
			printf("Error: Failed loading ELF maps (errno:%d):%s\n", 
					nr_maps, strerror(-nr_maps));
			goto done;
		}
		if(__load_maps(maps_data, nr_maps, NULL)) // NOTE: modified line
			goto done;
		maps_data_count = nr_maps;

		processed_sec[maps_shndx] = true;
	}

	// Process all relo sections, and rewrite bpf insns for maps
	for(i = 1; i < ehdr.e_shnum; i++) {
		if(processed_sec[i])
			continue;

		if(__get_sec(elf, i, &ehdr, &shname, &shdr, &data))
			continue;

		if(shdr.sh_type == SHT_REL) {
			struct bpf_insn *insns;

			// Locate prog sec that need map fixup (relocations)
			if(__get_sec(elf, shdr.sh_info, &ehdr, &shname_prog, 
						&shdr_prog, &data_prog))
				continue;

			if(shdr_prog.sh_type != SHT_PROGBITS || 
				!(shdr_prog.sh_flags & SHF_EXECINSTR))
				continue;

			insns = (struct bpf_insn *) data_prog->d_buf;
			processed_sec[i] = true; // relo section

			if(__parse_relo_and_apply(data, symbols, &shdr, insns, 
									maps_data, nr_maps))
				continue;
		}
	}

	// Load programs
	for(i = 1; i < ehdr.e_shnum; i++) {

		if(processed_sec[i])
			continue;

		if(__get_sec(elf, i, &ehdr, &shname, &shdr, &data))
			continue;

		if(memcmp(shname, "egress_tf_sec", 13) == 0) {
			// Call TC loader
			printf("[LAUNCHER]: Trying to load egress KTF %d !\n", i);
			if(launcher_load_egress_tf(path, ifname, shname) < 0) {
				goto done;
			}
			else
				ret = 0;
		}
		else if (memcmp(shname, "ingress_tf_sec", 14) == 0) {
			// Call XDP loader
			printf("[LAUNCHER]: Trying to load ingress KTF %d !\n", i);
			int xdp_flags = 0;
			xdp_flags &= ~XDP_FLAGS_MODES;
			xdp_flags |= XDP_FLAGS_SKB_MODE;
			if(launcher_load_ingress_tf(path, ifname, shname, xdp_flags) < 0) {
				goto done;
			}
			else
				ret = 0;
		}
		else if(memcmp(shname, "listener_tf_sec", 15) == 0) {
			// Call XDP loader 2
			printf("[LAUNCHER]: Trying to load listener KTF %d !\n", i);
			
			if(launcher_load_listener_tf(shname, data->d_buf, data->d_size, ifname, 0) != 0) {
				goto done;
			}
			else 
				ret = 0;
		}
		else if((memcmp(shname, "hooker_sockops", 14) == 0) || (memcmp(shname, "hooker_redirector", 17) == 0)) {

			if(launcher_load_hooker_progs(shname, data->d_buf, data->d_size) != 0) {
				goto done;
			}
			else
				ret = 0;
		}
		else if(memcmp(shname, "hooker_listener", 15) == 0) {
			if(launcher_load_listener_tf(shname, data->d_buf, data->d_size, ifname, 0) != 0) {
				goto done;
			}
			else
				ret = 0;
		}
	}
done:
	close(fd);
	return ret;
}