/*
* @file :		vtl_server.c
* @authors :		El-Fadel Bonfoh, Cedric Tape
* @date :		11/2019
* @version :		0.2
* @brief :		VTL-aware Server streaming data to VTL-aware client
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../service_api/service_api.h"

#include <vtl.h>

#define SRC_IP 						"10.0.0.4"
#define DST_IP 						"10.0.0.5"
#define DEV_NAME 					"enp0s3"
#define DATASIZE 					1024 // in vtl.h too

int main(int argc, char **argv) {

	int ret, cnt_pkt = 0, cnt_bytes = 0;
	FILE *tx_file = NULL;
	uint8_t *send_data;
	size_t send_data_s;

	char ifname[] = DEV_NAME;
	char src_ip[] = SRC_IP;
	char dst_ip[] = DST_IP;

	vtl_socket_t *vtl_sock;
	char err_buf[VTL_ERRBUF_SIZE];

	printf("VTL-aware Server starting...\n");

	vtl_host_role role = VTL_SENDER_ROLE;
	vtl_sock = vtl_init(role, src_ip, dst_ip, ifname, err_buf);
	if(vtl_sock == NULL) {
		fprintf(stderr, "%s", err_buf);
		fprintf(stderr, "ERR: vtl_init() failed.\n");
		exit(EXIT_FAILURE);
	}

	tx_file = fopen("../../files/video16M.mp4", "rb");
	if(tx_file == NULL) {
		fprintf(stderr, "ERR: failed to open test file\n");
		exit(EXIT_FAILURE);
	}

	printf("\nSending data ...");
	send_data = (uint8_t *) malloc(DATASIZE*sizeof(uint8_t));
	if(send_data == NULL) {
		fprintf(stderr, "ERR: Cannot allocate memory for Tx Buff.\n");
		exit(EXIT_FAILURE);
	}
	memset(send_data, 0, DATASIZE*sizeof(uint8_t));

	while(!feof(tx_file)) { //Learn till end of files
		send_data_s = fread(send_data, 1, DATASIZE, tx_file);
		ret = vtl_send_data(vtl_sock, send_data, send_data_s, err_buf);
		if(ret < 0) {
			fprintf(stderr, "%s\n", err_buf);
			fprintf(stderr, "vtl_send_data() failed.\n");
			exit(EXIT_FAILURE);
		}
		cnt_pkt++;
		cnt_bytes += send_data_s;
	}

	printf("Done\n\n");

	printf("Nbrs of sent packets: %d pkts\n", cnt_pkt);
	printf("Nbrs of sent bytes: %d bytes\n", cnt_bytes);

	fclose(tx_file);

	return 0;
}
