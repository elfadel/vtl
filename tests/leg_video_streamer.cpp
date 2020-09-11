#include <iostream>
#include <iomanip>
#include <unordered_map>

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/un.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>

#define SA 				struct sockaddr
#ifndef _MP4STREAMER_H_
#define _MP4STREAMER_H_
#include <list>

using namespace std;
void timeoutHandler(int);

class PlayList { // TODO: Don't need this class. Fix it.
public:
	PlayList();
	const char* getNextVideo();
	void addVideoToPlayList(const char* video);
	int gotoTopPlayList(void);
	void printPlayList(void);
private:
	static std::list<const char*>* playlist;
	static std::list<const char*>::iterator prevVideo;
};

#endif // _MP4STREAMER_H_
#include <fstream>

void timeoutHandler(int);

FILE* video_fd;
int sock_fd;
bool over = false;

int main(int argc, char **argv) {
	int n, file_size = 0;
	char* channel_name;
	struct sockaddr_in server_addr;

	if(argc != 5) {
		printf("[LEG V_STREAMER] usage: %s <IP address> <port no> <channel> <playlist file>\n", argv[0]);
		exit(1);
	}

	char* playlistfilename = (char *) calloc(1, strlen(argv[4]));
	strcpy(playlistfilename, argv[4]);

	FILE *plf_fd = fopen(playlistfilename, "r");
	if(plf_fd == NULL) {
		cout << "[LEG V_STREAMER] ERROR - Error opening " << playlistfilename << endl;
		cout << strerror(errno) << endl;
		exit(1);
	}

	// Create the internal playlist
	PlayList pl;
	while(!feof(plf_fd)) {
		char* video_name = (char *) calloc(1, 1024);
		char *pch = NULL;
		fgets(video_name, 1024, plf_fd);
		pch = strchr(video_name, '\n');
		if(pch != NULL)
			*pch = '\0';
		pl.addVideoToPlayList(video_name);
	}

	if((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		printf("[LEG V_STREAMER] err_sys: socket() error.\n");

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(atoi(argv[2]));
	server_addr.sin_addr.s_addr = inet_addr(argv[1]);

	if(connect(sock_fd, (SA *)&server_addr, sizeof(server_addr)) < 0) {
		printf("[LEG V_STREAMER] err_sys: connect() error.\n");
		printf("%s\n", strerror(errno));
		exit(1);
	}

	channel_name = (char *) calloc(1, strlen(argv[3])+1);
	strcpy(channel_name, argv[3]);

	if(send(sock_fd, channel_name, strlen(channel_name), 0) == -1) {
		perror("[LEG V_STREAMER]: send() channel_name.");
	}

	printf("[LEG V_STREAMER] mount point sent is :::::::::::: %s\n", channel_name);
	free(channel_name);

	printf("[LEG V_STREAMER] waiting a wee bit for the mount point to be properly recognized ...\n");
	printf("[LEG V_STREAMER] sleep(3)...\n");
	sleep(3);

	if(pl.gotoTopPlayList() < 0) {
		printf("[LEG V_STREAMER] ERROR: PlayList empty.\n");
		close(sock_fd);
		exit(1);
	}

	while(1) {
		const char *nextVideo = NULL;
		signal(SIGALRM, timeoutHandler);
		printf("[LEG V_STREAMER] next song in PlayList: %s\n", pl.getNextVideo());

		nextVideo = pl.getNextVideo();
		video_fd = fopen(pl.getNextVideo(), "r"); //TODO: replaceby original in case of bugs.
		if(video_fd == NULL) {
			printf("[LEG V_STREAMER] error opening file [%s]\n", nextVideo);
			printf("[LEG V_STREAMER] %s\n", strerror(errno));
			exit(2);
		}

		itimerval ival;
		ival.it_value.tv_sec = 0;
		ival.it_value.tv_usec = 2000;
		ival.it_interval.tv_sec = 0;
		ival.it_interval.tv_usec = 2000;
		setitimer(ITIMER_REAL, &ival, NULL);

		while(!over);
			over = false;
			fclose(video_fd);
	}
	close(sock_fd);
	exit(0);
}

void timeoutHandler(int a) {
	char buff[4096];
	int n = fread(buff, 1, 1024, video_fd); // 1024 is arbitrary value
	static int sc = 0;

	printf("#");
	fflush(stdout);

	if(n > 0) {
		if(send(sock_fd, buff, n, 0) < 0)
			printf("[LEG V_STREAMER] error writing.\n");
	}
	else
		over = true;
}