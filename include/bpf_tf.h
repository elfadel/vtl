/**
* @file : 	bpf_tf.h
* @author :	El-Fadel Bonfoh
* @date :	03/2019 - 
* @version : 	0.1
* @brief :	
*/

#ifndef BPF_TF_H
#define BPF_TF_H

/*************
* Includes...*
*************/
#include <api/net-api.h>
#include <api/user-api.h>

#include "vtl.h"

#define MAX_ACTIONS 32

typedef struct {

	// Functions...
	/**
	* @brief : 	Send packet to the next TF.
	* @return :	
	*/
	void (*push)(struct vtl_pkt pkt, int next_bpf_tf);

	/**
	* @brief :	Retrieve packet (from the previous TF).
	* @return :	The packet 'pkt' retrieve from previous TF if successful.
	*/
	struct vtl_pkt (*pull)(void);

	/**
	* @brief : 	Process the packet 'pkt'.
	* @return :	The modified packet 'm_pkt'.
	*/
	struct vtl_pkt (*actions[MAX_ACTIONS])(struct vtl_pkt pkt); // array of functions pointer, to process packet.

	// Vars...
	
} bpf_tf;

#endif // end of "define BPF_TF_H"
