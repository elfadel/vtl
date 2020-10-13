/**
 * @file :		sctp_client.c
 * @author :		El-Fadel Bonfoh
 * @date :		2020
 * @version :		0.1
 * @brief :
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>

#define MAX 				1024
#define SA 				struct sockaddr
#define FILE_NAME 			"../../files/out_file.txt"


int sctp_flags, pkt_size = 256, val = 20;

struct sctp_sndrcvinfo snd_rcv_info;

struct sockaddr_in to;

void apps(int sockfd) {

	char buff[MAX*1024];
	static FILE *rx_file = NULL;

	rx_file = fopen(FILE_NAME, "w");
	if(rx_file == NULL)
		printf("[LEG CLIENT]: fopen() failed -- unable to open rx file.\n");

	for(int i = 0; i < 1; i++) { // useless loop !

		memset(buff, 0, sizeof(buff));
		printf("[LEG CLIENT]: downloading data ...\n");

		sctp_recvmsg(sockfd, buff, MAX, NULL, 0, 
			     &snd_rcv_info, &sctp_flags);
		while (strlen(buff) > 0) {

			fwrite(buff, 1, n, rx_file);
			fflush(rx_file);
			sctp_recvmsg(sockfd, buff, MAX, NULL, 0, 
			             &snd_rcv_info, &sctp_flags);
		}
	}

	fclose(rx_file);
}

int main() {

	const char *remote_ip = "10.0.0.4";
	int err = 0, sctp_fd;

	sctp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(sctp_fd < 0) {
	       return errno;
	}

	to.sin_family = AF_INET;
    	to.sin_addr.s_addr = inet_addr(remote_ip);
    	to.sin_port = htons(6666);

        if(connect(sctp_fd, (SA*)&to, sizeof(to)) != 0) {
	    	return errno;
	}

	apps(sctp_fd);

	close(sctp_fd);

	return 0;
}
