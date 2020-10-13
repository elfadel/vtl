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
#define SOL_DCCP 			269
#define DCCP_SOCKOPT_SERVICE 		2

int pkt_size = 256;

struct sockaddr_in to;

void apps(int sockfd) {

	char buff[MAX];
	int n, ret;
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
			do {
                		ret = send(sockfd, buff, n, 0);
            		} while((ret < 0) && (errno == EAGAIN));
		}
	}
}

int main(){

	struct sockaddr_in graft_addr;
	const char *remote_ip = "10.0.0.5";
	int err, dccp_fd;

	dccp_fd = socket(AF_INET, SOCK_DCCP, IPPROTO_DCCP);
	if(dccp_fd < 0) {
	       return errno;
	}
	setsockopt(dccp_fd, SOL_DCCP, DCCP_SOCKOPT_SERVICE, 
			(char*)&pkt_size, sizeof(pkt_size));

	to.sin_family = AF_INET;
    	to.sin_addr.s_addr = inet_addr(remote_ip);
    	to.sin_port = htons(6666);

	graft_addr.sin_family = AF_INET;
    	graft_addr.sin_addr.s_addr = htonl(INADDR_ANY); // "10.0.0.5"
    	graft_addr.sin_port = htons(6666);
       	err = bind(dccp_fd, (struct sockaddr *)&graft_addr, sizeof(graft_addr));
        if (err < 0) {
            	return errno;
        }

    	err = listen(dccp_fd, 32);
        if (err < 0) {
            	return errno;
        }
        socklen_t len = sizeof(to);
        int dccp_fd_cpy = accept(dccp_fd, (SA*)&to, &len);
        if(dccp_fd_cpy < 0) {
            	return errno;
        }

	apps(dccp_fd_cpy);

	close(dccp_fd_cpy);

	return 0;
}