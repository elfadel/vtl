/**
 * @file :		sctp_server.c
 * @author :		El-Fadel Bonfoh
 * @date :		2020
 * @version :		0.1
 * @brief :
*/

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/sctp.h>

#include <arpa/inet.h>

#define MAX 				1024
#define SA 				struct sockaddr
#define FILE_NAME 			"../../../fat.txt"


int sctp_flags, val = 20;

struct sctp_sndrcvinfo snd_rcv_info;

struct sctp_initmsg initmsg = { 
    	.sinit_num_ostreams 	= 32,
    	.sinit_max_instreams 	= 32,
    	.sinit_max_attempts 	= 4,
};

struct sockaddr_in to;

void apps(int sockfd) {

	char buff[MAX];
	int n;
	static FILE *tx_file = NULL;

	tx_file = fopen(FILE_NAME, "r");
	if(tx_file == NULL)
		printf("[LEG SERVER]: fopen() failed " 
			"-- unable to open file tx file.\n");

	for(int i = 0; i < 1; i++) { // useless loop !

		memset(buff, 0, MAX);

		printf("[LEG SERVER]: sending data ...\n");
		while(!feof(tx_file)) {

			n = fread(buff, 1/* nombre d'octets par données lues*/,
				  MAX /*le nombre de données à lire*/, tx_file);

			sctp_sendmsg(sockfd, buff, n, 
				     NULL, 0, 0, 0, 0, 0, 0);
		}
	}
}

int main(){

	struct sockaddr_in graft_addr;
	const char *remote_ip = "10.0.0.5";
	int err = 0, sctp_fd;

	sctp_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP);
	if(sctp_fd < 0) {
	       return errno;
	}


	to.sin_family = AF_INET;
    	to.sin_addr.s_addr = inet_addr(remote_ip);
    	to.sin_port = htons(6666);

	graft_addr.sin_family = AF_INET;
    	graft_addr.sin_addr.s_addr = htonl(INADDR_ANY); // "10.0.0.5"
    	graft_addr.sin_port = htons(6666);
       	err = bind(sctp_fd, (struct sockaddr *)&graft_addr, sizeof(graft_addr));
        if (err < 0) {
            	return errno;
        }

        err = setsockopt(sctp_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
        if(err < 0) {
                return errno;
        }

    	err = listen(sctp_fd, 32);
        if (err < 0) {
            	return errno;
        }
        socklen_t len = sizeof(to);
        int sctp_fd_cpy = accept(sctp_fd, (SA*)&to, &len);
        if(sctp_fd_cpy < 0) {
            	return errno;
        }

	apps(sctp_fd_cpy);

	close(sctp_fd_cpy);

	return 0;
}