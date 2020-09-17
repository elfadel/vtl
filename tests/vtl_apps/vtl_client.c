/*
 * @file:		vtl_client.c
 * @authors:		El-Fadel Bonfoh, Cedric Tape
 * @date:		11/2019
 * @version:		0.2
 * @brief:		VTL-aware Client: Receive data form VTL-aware server
*/

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <vtl.h>

#include "../../service_api/service_api.h"

#define SRC_IP 					"10.0.0.5"
#define DST_IP					"10.0.0.4"
#define DEV_NAME 				"enp0s3"
#define DATASIZE 				1024
#define MAX_DATA_SIZE 				1024*10

#define SYSTEM(CMD) 							\
	do {								\
		if(system(CMD)) 					\
			printf("Shell cmd failed !\n");			\
		else printf("Shell cmd OK !\n");			\
	} while(0);

static bool global_exit;

static void exit_appli(int signal) {
	signal = signal;
	global_exit = true;
}

int main(int argc, char **argv) {

	char ifname[] = DEV_NAME;
	char src_ip[] = SRC_IP;

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

	static FILE *rx_file = NULL;
	rx_file = fopen("../../files/out_file.txt", "wb");
	if(rx_file == NULL) {
		fprintf(stderr, "ERR: failed to open test file\n");
		exit(EXIT_FAILURE);
	}

	printf("\nReceiving data...");

	global_exit = false;
	printf("\n");

	uint8_t *rx_data = NULL;
	rx_data = calloc(1, MAX_DATA_SIZE);
	if(rx_data == NULL) {
		printf("Error: unable to calloc()\n");
		return -1;
	}

	size_t rx_data_len = 0;
	while(!global_exit) { // Continue to recv data till explicit exit (Ctrl+c)
		vtl_recv_data(vtl_sock, rx_data, &rx_data_len, err_buf);
		
		fflush(stdout);
		if(rx_data_len != 0) {
			printf("Receive and write %ld bytes\n", rx_data_len);
			fwrite(rx_data, 1, rx_data_len, rx_file);
			fflush(rx_file);
		}
	}

	printf("\n");
	printf("Done !\n");

	fclose(rx_file);

	return 0;
}