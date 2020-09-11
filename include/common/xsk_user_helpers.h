#ifndef __XSK_USER_HELPERS_H
#define __XSK_USER_HELPERS_H

#include <bpf/xsk.h>

// Faire attention
//TODO: remove defines.h
#include "defines.h"

//TODO: change later
// Afin de réduire le code à inclure
//#include "xdp_user_helpers.h"

#define NUM_FRAMES         16384
#define FRAME_SIZE         XSK_UMEM__DEFAULT_FRAME_SIZE
#define INVALID_UMEM_FRAME UINT64_MAX

struct xsk_umem_info {
	struct xsk_ring_prod fq;
	struct xsk_ring_cons cq;
	struct xsk_umem *umem;
	void *buffer;
};

struct xsk_socket_info {
	struct xsk_ring_cons rx;
	struct xsk_ring_prod tx;
	struct xsk_umem_info *umem;
	struct xsk_socket *xsk;

	uint64_t umem_frame_addr[NUM_FRAMES];
	uint32_t umem_frame_free;

	uint32_t outstanding_tx;
};

struct xsk_umem_info *configure_xsk_umem(void *buffer, uint64_t size);
uint64_t xsk_alloc_umem_frame(struct xsk_socket_info *xsk);
void xsk_free_umem_frame(struct xsk_socket_info *xsk, uint64_t frame);
uint64_t xsk_umem_free_frames(struct xsk_socket_info *xsk);

/**
 * @desc create and configure an af_xdp socket
 * @param ifname - interface
 * @param xdp_flags - xdp injection flags
 * @param xsk_bind_flags - precise creation mode of af_xdp socket
 * @param xsk_if_queue - ??
 * @param umen - ??
 * @return an af_xdp socket struct or NULL on failure
 *
 */
// TODO: errno
struct xsk_socket_info *
xsk_configure_socket(char *ifname, __u32 xdp_flags, __u16 xsk_bind_flags,
			int xsk_if_queue, struct xsk_umem_info *umem);


#endif  /*__XSK_USER_HELPERS_H */
