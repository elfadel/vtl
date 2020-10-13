/**
 * @file :		legacy_server.c
 * @author :		El-Fadel Bonfoh
 * @contribs :	
 * @date :		05/2019 -
 * @version :		0.1
 * @brief :
*/

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h> 
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX 					1024 /* Adapt to the MTU of net link */
#define PORT 					2222 		
#define SA 						struct sockaddr
#define FILE_NAME 				"../../fat.txt"

void apps(int sockfd) {
	char buff[MAX];
	int n;
	static FILE *tx_file = NULL;

	tx_file = fopen(FILE_NAME, "r");
	if(tx_file == NULL)
		printf("[LEG SERVER]: fopen() failed -- unable to open file tx file.\n");

	for(int i = 0; i < 1; i++) { // useless loop !

		memset(buff, 0, MAX);

		printf("[LEG SERVER]: sending data ...\n");
		while(!feof(tx_file)) {

			n = fread(buff, 1/* nombre d'octets par données lues*/, MAX /*le nombre de données à lire*/, tx_file);

			// Envoyer au *putain* de client :)
			send(sockfd, buff, n, 0);
		    usleep(500); //0.5ms
		}
	}

	fclose(tx_file);
}

int main() {

	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1) {
		printf("[LEG SERVER]: socket creation failed.\n");
		exit(0);
	}
	else
		printf("[LEG SERVER]: socket successfully created.\n");
	memset(&servaddr, 0, sizeof(servaddr));

	// assign IP, PORT
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	int one = 1;
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));

	if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {

		printf("[LEG SERVER]: socket bind failed.\n");
		exit(0);
	}
	else
		printf("[LEG SERVER]: socket successfully binded.\n");

	if((listen(sockfd, 32)) != 0) {

		printf("[LEG SERVER]: listen() failed.\n");
		exit(0);
	}
	else
		printf("[LEG SERVER]: server start listening at fd=%d.\n", sockfd);
	len = sizeof(cli);

	// accept the data packet from client
	connfd = accept(sockfd, (SA*)&cli, (socklen_t *)&len);
	if(connfd < 0) {

		printf("[LEG SERVER]: server accept failed\n");
		exit(0);
	}
	else
		printf("[LEG SERVER]: server accepts the client connect() request.\n");

	apps(connfd);

	sleep(4);

	close(sockfd);

	printf("[LEG SERVER]: server stoped listening and closed connection.\n");

	return 0;
}
