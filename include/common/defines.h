#ifndef __DEFINES_H
#define __DEFINES_H

#include <net/if.h>
#include <linux/types.h>
#include <stdbool.h>

//TODO: delete later. Make attention to dependencies
//TODO: move it later to include

/* Defined in common_params.o */
extern int verbose;

/* Exit return codes */
#define EXIT_OK 		 0 /* == EXIT_SUCCESS (stdlib.h) man exit(3) */
#define EXIT_FAIL		 1 /* == EXIT_FAILURE (stdlib.h) man exit(3) */
#define EXIT_FAIL_OPTION	 2
#define EXIT_FAIL_XDP		30
#define EXIT_FAIL_BPF		40

#endif /*__DEFINES_H */
