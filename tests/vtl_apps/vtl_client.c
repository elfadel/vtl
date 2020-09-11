/*
* @file :		vtl_client.c
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		11/2019
* @version :		0.2
* @brief :		VTL-aware Client: Receive data form VTL-aware server
*/

//SPDX-License-Identifier: GPL-2.0

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vtl.h>

#include "../../service_api/service_api.h"

#define SRC_IP 					"10.0.0.5"
#define DST_IP					"10.0.0.4"
#define DEV_NAME 				"enp0s3"
#define DATASIZE 				1024;

static bool global_exit;

static void exit_appli(int signal) {
	signal = signal;
	global_exit = true;
}

int main(int argc, char **argv) {

	char ifname[] = DEV_NAME;
	char src_ip[] = SRC_IP;
	//char dst_ip[] = DST_IP;

	vtl_socket_t *vtl_sock;
	char err_buf[VTL_ERRBUF_SIZE];

	signal(SIGINT, exit_appli);

	printf("VTL-aware Client starting...");
	vtl_host_role role = VTL_RECVER_ROLE;
	vtl_sock = vtl_init(role, src_ip, NULL, ifname, err_buf);
	if(vtl_sock == NULL) {
		fprintf(stderr, "%s", err_buf);
		fprintf(stderr, "ERR: vtl_init() failed.\n");
		exit(EXIT_FAILURE);
	}

	FILE *rx_file = NULL;
	rx_file = fopen("./files-receiver/out_GoT.mp4", "wb");
	if(rx_file == NULL) {
		fprintf(stderr, "ERR: failed to open test file\n");
		exit(EXIT_FAILURE);
	}

	printf("\nReceiving data...");

	global_exit = false;
	printf("\n");

	while(!global_exit) { // Continue to recv data till explicit exit (Ctrl+C)
		vtl_recv_data(vtl_sock, rx_file);
		printf("Recv pkt: %d --- Recv bytes: %d\r",
						vtl_sock->cnt_pkts, vtl_sock->cnt_bytes);
		fflush(stdout);
	}

	printf("\n");
	printf("Done\n");

	fclose(rx_file);

	return 0;
}
