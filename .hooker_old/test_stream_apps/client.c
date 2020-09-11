/**
* @file :		client.c
* @author :		El-Fadel Bonfoh, Cédric Tape 
* @date :		05/2019 - 
* @version :		0.1
* @brief :	
*/

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

void apps(int sockfd) {

	char buff[MAX];
	int n;
	static FILE *rx_file = NULL;

	rx_file = fopen("../files/out_GoT_2.mp4", "w");

	for(int i = 0; i < 1; i++) { // boucle inutile fadel !

		bzero(buff, sizeof(buff));
		printf("Uploading data ...\n");
		
		while (read(sockfd, buff, sizeof(buff)) > 0) {

			fwrite(buff, 1, MAX, rx_file);
			fflush(rx_file); // don't asked why this instruction 
		}
	}
}

int main() {

	int sockfd, connfd;
	struct sockaddr_in servaddr, cli;

	// socket create
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {

		printf("socket creation failed\n");
		exit(0);
	}
	else
		printf("socket successfully created\n");
	bzero(&servaddr, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
	servaddr.sin_port = htons(PORT);

	//connect the client socket to the server socket
	if(connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
		
		printf("connection with the server failed\n");
		exit(0);
	}
	else
		printf("connected to the server\n");

	apps(sockfd);

	close(sockfd);
}
