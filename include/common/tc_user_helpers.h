
#pragma once

//TODO: put it common.h
#define ERR_BUFF_SIZE     16384
struct tc_config {

        char filename[512];
        char dev[20];
        char err_buf[ERR_BUFF_SIZE];

};

/**
 * Attach bpf programs on a tc hook point.
 * @param cfg pointer to a tc configuration object
 * @retval  >= 0 on success
 * @retval -1 on failure
 **/ 
int 
tc_egress_attach_bpf(const char* ifname, const char* bpf_obj, const char* sec_name);

/**
 * Remove bpf programs on a tc hook point. 
 * @param cfg pointer to a tc configuration object
 * @retval  >= 0 on success
 * @retval -1 on failure
 **/ 
int 
tc_remove_egress(struct tc_config *cfg);