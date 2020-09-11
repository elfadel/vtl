#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "defines.h"
#include "tc_user_helpers.h"

/* paramétrage commmande tc */
#define CMD_MAX 	2048
#define CMD_MAX_TC	256
static char tc_cmd[CMD_MAX_TC] = "tc";


/**
* @return: -1 in case of error
*/
int tc_egress_attach_bpf(const char* ifname, const char* bpf_obj, const char* sec_name)
{
	char cmd[CMD_MAX], err_buff[ERR_BUFF_SIZE];
	int ret = 0;

	/* Step-1: Delete clsact, which also remove filters */
	memset(&cmd, 0, CMD_MAX);

	snprintf(cmd, CMD_MAX,
		 "%s qdisc del dev %s clsact 2> /dev/null",
		 tc_cmd, ifname);

	ret = system(cmd);
	if (!WIFEXITED(ret)) {
		snprintf(err_buff, ERR_BUFF_SIZE,
			"ERR(%d): Cannot exec tc cmd\n Cmdline:%s\n",
			 WEXITSTATUS(ret), cmd);
		return -1;

	}
	
	/* Step-2: Attach a new clsact qdisc */
	memset(&cmd, 0, CMD_MAX);
	snprintf(cmd, CMD_MAX,
		 "%s qdisc add dev %s clsact",
		 tc_cmd, ifname);

	ret = system(cmd);
	if (ret) {
		snprintf(err_buff, ERR_BUFF_SIZE,
			"ERR(%d): tc cannot attach qdisc hook\n Cmdline:%s\n",
			 WEXITSTATUS(ret), cmd);
		return -1;
	}

	/* Step-3: Attach BPF program/object as egress filter */
	memset(&cmd, 0, CMD_MAX);
	snprintf(cmd, CMD_MAX,
		 "%s filter add dev %s "
		 "egress prio 1 handle 1 bpf da obj %s sec %s",
		 tc_cmd, ifname, bpf_obj, sec_name); // TODO: adapt that line for our use cases

	ret = system(cmd);
	if (ret) {
		snprintf(err_buff, ERR_BUFF_SIZE,
			"ERR(%d): tc cannot attach filter\n Cmdline:%s\n",
			 WEXITSTATUS(ret), cmd);
		return -1;
	}

	return ret;
}

/* Remove bpf program on tc egress path  */
int tc_remove_egress(struct tc_config *cfg)
{
	char cmd[CMD_MAX];
	int ret = 0;

	memset(&cmd, 0, CMD_MAX);
	snprintf(cmd, CMD_MAX,
		 /* Remove all ingress filters on dev */
		 "%s filter delete dev %s egress",
		 /* Alternatively could remove specific filter handle:
		 "%s filter delete dev %s ingress prio 1 handle 1 bpf",
		 */
		 tc_cmd, cfg->dev);

	// if (verbose) printf(" - Run: %s\n", cmd);

	ret = system(cmd);
	if (ret) {
		snprintf(cfg->err_buf, ERR_BUFF_SIZE,
			"ERR(%d): tc cannot remove filters\n Cmdline:%s\n",
			 ret, cmd);
		return -1;
	}
	return ret;
}
