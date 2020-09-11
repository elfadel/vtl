/**
* @file :		legacy_client.c
* @author :		El-Fadel Bonfoh
* @contribs :	-
* @date :		05/2019 -
* @version :	0.1
* @brief :
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <arpa/inet.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void apps(int sockfd) {

	char buff[MAX];
	int n;
	static FILE *rx_file = NULL;

	rx_file = fopen("../files/out_file.txt", "w");
	if(rx_file == NULL)
		printf("[LEG CLIENT]: fopen() failed -- unable to open rx file.\n");

	for(int i = 0; i < 1; i++) { // useless loop !

		bzero(buff, sizeof(buff));
		printf("[LEG CLIENT]: uploading data ...\n");

		n = read(sockfd, buff, sizeof(buff));
		while (n > 0) {

			fwrite(buff, 1, n, rx_file);
			fflush(rx_file);
			n = read(sockfd, buff, sizeof(buff));
		}
	}
}

int main() {

	int sockfd/*, connfd*/;
	struct sockaddr_in servaddr/*, cli*/;

	// socket create
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {

		printf("[LEG CLIENT]: socket creation failed.\n");
		exit(0);
	}
	else
		printf("[LEG CLIENT]: socket successfully created.\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("10.0.0.5");
	servaddr.sin_port = htons(PORT);

	//connect the client socket to the server socket
	if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {

		printf("[LEG CLIENT]: connection to the server failed.\n");
		exit(0);
	}
	else
		printf("[LEG CLIENT]: connected to the server.\n");

	apps(sockfd);

	close(sockfd);
}
