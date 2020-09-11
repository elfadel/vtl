
#ifndef __TC_UTIL_H
#define __TC_UTIL_H

/* BPF_FUNC_skb_store_bytes flags. */
#define BPF_F_RECOMPUTE_CSUM		(1ULL << 0)

#define PIN_NONE                0
#define PIN_OBJECT_NS		1
#define PIN_GLOBAL_NS		2

/* 
 * ELF map definition used by iproute2.
 * Cannot figure out how to get bpf_elf.h installed on system, so we've copied it here.
 * iproute2 claims this struct will remain backwards compatible
 * https://github.com/kinvolk/iproute2/blob/be55416addf76e76836af6a4dd94b19c4186e1b2/include/bpf_elf.h
 */
struct bpf_elf_map {
	/*
	 * The various BPF MAP types supported (see enum bpf_map_type)
	 * https://elixir.bootlin.com/linux/latest/source/include/uapi/linux/bpf.h
	 */
	__u32 type;
	__u32 size_key;
	__u32 size_value;
	__u32 max_elem;
	/*
	 * Various flags you can place such as `BPF_F_NO_COMMON_LRU`
	 */
	__u32 flags;
	__u32 id;
	/*
	 * Pinning is how the map are shared across process boundary.
	 * Cillium has a good explanation of them: http://docs.cilium.io/en/v1.3/bpf/#llvm
	 * PIN_GLOBAL_NS - will get pinned to `/sys/fs/bpf/tc/globals/${variable-name}`
	 * PIN_OBJECT_NS - will get pinned to a directory that is unique to this object
	 * PIN_NONE - the map is not placed into the BPF file system as a node,
	 			  and as a result will not be accessible from user space
	 */
	__u32 pinning;
};

//TODO: supprimer le non-essentiel

/**
 * Aside from BPF helper calls and BPF tail calls, the BPF instruction did not arbitrary 
 * support functions -- as a result all functions need the inline macro.
 * Starting with Linux kernel 4.16 and LLVM 6.0 this restriction got lifted.
 * The typical inline keyword is only a hint whereas this is definitive.
 */
#define forced_inline __attribute__((always_inline))

/* 
 * helper macro to place programs, maps, license in
 * different sections in elf_bpf file. Section names
 * are interpreted by elf_bpf loader
 */
#define SEC(NAME) __attribute__((section(NAME), used))

/*
 * helper macro to make it simpler to print trace messages to
 * bpf_trace_printk.
 * ex. bpf_printk("BPF command: %d\n", op);
 * you can find the output in /sys/kernel/debug/tracing/trace_pipe
 * however it will collide with any other running process.
 */
#define bpf_printk(fmt, ...)							\
({														\
	       char ____fmt[] = fmt;						\
	       bpf_trace_printk(____fmt, sizeof(____fmt),	\
				##__VA_ARGS__);							\
})

/*
 * The __builtin_expect macros are GCC specific macros that use the branch prediction;
 * they tell the processor whether a condition is likely to be true,
 * so that the processor can prefetch instructions on the correct "side" of the branch.
 */
#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)


#endif /*__TC_UTIL_H */