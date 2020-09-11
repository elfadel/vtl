/**
* @file :		server.c
* @author :		El-Fadel Bonfoh, Cédric Tape 
* @date :		05/2019 - 
* @version :		0.1
* @brief :	
*/

#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <stdio.h>			// Add by me. Why ? I don't know :(, Kidding !!!
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

#define MAX 80
#define PORT 8080
#define SA struct sockaddr

// Function designed for what ???
void apps(int sockfd) {

	char buff[MAX];
	int n;
	static FILE *tx_file = NULL;

	tx_file = fopen("../files/GoT_2.mp4", "r");

	// infinite loop for ???
	for(int i = 0; i < 1; i++) { // useless loop ?

		bzero(buff, MAX); // deprecated and not recommended function ==> see memset() instaed

		// Début des ennuies ...
		while(!feof(tx_file)) { // tant que je ne suis pas à la fin du fichier ?

			// lire une partie de (80 octets ?)
			fread(buff, 1/* nombre d'octets par données lues*/, MAX /*le nombre de données à lire*/, tx_file);

			// Envoyer au *putain* de client
			write(sockfd, buff, sizeof(buff));
		}
	}
}

int main() {

	int sockfd, connfd, len;
	struct sockaddr_in servaddr, cli;

	// socket create and verification
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
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(PORT);

	// binding newly created socket to given IP and verification
	if((bind(sockfd, (SA*)&servaddr, sizeof(servaddr))) != 0) {

		printf("socket bind failed\n");
		exit(0);
	}
	else
		printf("socket successfully binded\n");

	// Now server is ready to listen
	if((listen(sockfd, 3)) != 0) {
		
		printf("listen failed\n");
		exit(0);
	}
	else
		printf("server listening\n");
	len = sizeof(cli);

	// accept the data packet from client 
	connfd = accept(sockfd, (SA*)&cli, &len);
	if(connfd < 0) {

		printf("server accept failed\n");
		exit(0);
	}
	else
		printf("server accept the client\n");

	apps(connfd);

	close(sockfd);
}
