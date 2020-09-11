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
#include <pthread.h>
#include <list>

namespace std { using namespace __gnu_cxx; }

#define MAXLINE 		1024
#define MAX_MNT_LEN 	64
#define CLIENT_PORT 	4443
#define STREAMER_PORT 	4444
#define SA 				struct sockaddr
#define LISTENQ 		10
using namespace std;

struct eqstr {
	bool operator() (const char* s1, const char* s2) const
	{
		return (strcmp (s1, s2) == 0);
	}
};

// Typedefs for iterators
typedef unordered_map<int, const char*>::iterator lisMpIter;
typedef unordered_map<int, const char*>::iterator strmMpIter;
typedef unordered_map<const char*, unordered_map<int, int>*, hash<const char*>, eqstr>::iterator masterIter;
typedef unordered_map<int, int>::iterator clientsIter;

//Function prototypes
bool addClient (int, const char*);
void addStreamer (int, const char*);
void delClient (int);
void delStreamer(int);
void printAllTables ();
void printMasterTable ();
void printStreamerTable ();
void printListenerTable ();
int max2 (int s1, int s2);

//Thread function prototypes
void media_streamer (void*);
void accept_connections (void*);
void sigpipeHandler (int a);

/* ----------------------------------------------------------------------------
 * Global Variables
 * ---------------------------------------------------------------------------- */
int listen_client;	// listen for mp4 clients on this socket.
int listen_streamer;// listen for mp4 streamers on this socket.

pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

//hash maps to maintain sources and listeners
unordered_map<int, const char*> streamers_2_mountpt;
unordered_map<int, const char*> listeners_2_mountpt;
unordered_map<const char*, unordered_map<int,int>*, hash<const char*>, eqstr> master_table;


/* ----------------------------------------------------------------------------
 *  Main:
 * 	Init the support tables.
 * 	Create streamer side and client side server sockets.
 * 	Create threads for accepting connections and streaming.
 * ---------------------------------------------------------------------------- */

int main(int argc, char *argv[]) {
	signal(SIGPIPE, sigpipeHandler);

	// Clear all the hash maps
	streamers_2_mountpt.clear();
	listeners_2_mountpt.clear();
	master_table.clear();

	// Create TCP server socket to listen to incoming streamer connection
	listen_streamer = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_streamer < 0) {
		printf("[LEG V_GW] ERROR: streamer-socket() creation error.\n");
		exit(1);
	}

	struct sockaddr_in server_addr_streamer;
	memset(&server_addr_streamer, 0, sizeof(server_addr_streamer));
	server_addr_streamer.sin_family = AF_INET;
	server_addr_streamer.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr_streamer.sin_port = htons(STREAMER_PORT);

	if(bind(listen_streamer, (SA*)&server_addr_streamer, sizeof(server_addr_streamer)) < 0) {
		printf("[LEG V_GW] ERROR: streamer-socket bind() error.\n");
		exit(2);
	}

	// Create TCP server socket to listen to incoming client/player connection
	listen_client = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_client < 0) {
		printf("[LEG V_GW] ERROR: client-socket() creation error.\n");
		exit(1);
	}

	struct sockaddr_in server_addr_client;
	memset(&server_addr_client, 0, sizeof(server_addr_client));
	server_addr_client.sin_family = AF_INET;
	server_addr_client.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr_client.sin_port = htons(CLIENT_PORT);

	if(bind(listen_client, (SA*)&server_addr_client, sizeof(server_addr_client)) < 0) {
		printf("[LEG V_GW] ERROR: client-socket bind() error.\n");
		exit(2);
	}

	listen(listen_streamer, LISTENQ);
	listen(listen_client, LISTENQ);

	pthread_t connAcceptorTid;
	pthread_t mediaStreamerTid;

	pthread_create(&connAcceptorTid, NULL, (void *(*) (void *))accept_connections, NULL);
	pthread_create(&mediaStreamerTid, NULL, (void *(*) (void *))media_streamer, NULL);

	// Wait for both the threads to finish.
	pthread_join(mediaStreamerTid, NULL);
	pthread_join(connAcceptorTid, NULL);
}

/* ----------------------------------------------------------------------------
 * Function	: accept_connections (void* arg) 
 * Description	: Thread
 * 	a. Accepts connects from both stremers and clients
 * 	b. Updates the support tables
 * ---------------------------------------------------------------------------- */

void accept_connections(void *arg) {
	printf("Waiting for connections ...\n");

	while(1) {
		fd_set rset;
		FD_ZERO(&rset);
		FD_SET(listen_streamer, &rset);
		FD_SET(listen_client, &rset);

		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		select(max2(listen_streamer, listen_client)+1, &rset, NULL, NULL, &tv);
		if(FD_ISSET(listen_streamer, &rset)) {
			struct sockaddr_in client_addr_streamer;
			socklen_t len = sizeof(client_addr_streamer);
			int conn_streamer = accept(listen_streamer, (SA*)&client_addr_streamer, &len);
			if(conn_streamer < 0) {
				printf("%s\n", strerror(errno));
				printf("[LEG V_GW] ERROR: accept() connection from streamer.\n");
			}
			else {
				/* 
				 * Currently, the mount point is the only thing the
				 * streamer will send. Hence what we receive is the
				 * mount point.
				*/
				char mount_pt_from_streamer[MAX_MNT_LEN];
				int recv_len = recv(conn_streamer, mount_pt_from_streamer, MAX_MNT_LEN, 0);

				if(recv_len < 0) {
					printf("[LEG V_GW]: %s\n", strerror(errno));
					exit(1);
				}
				else {
					mount_pt_from_streamer[recv_len] = '\0';

					printf("[LEG V_GW] Read %d bytes from socket\n", recv_len);
					printf("%s: mount point received is: %s\n", __PRETTY_FUNCTION__, mount_pt_from_streamer);

					int pml = pthread_mutex_lock(&db_mutex);
					addStreamer(conn_streamer, mount_pt_from_streamer);
					pml = pthread_mutex_unlock(&db_mutex);
				} // end recv_len < 0
			} // end conn_streamer < 0
		}
		else if(FD_ISSET(listen_client, &rset)) {
			char client_hdr[MAXLINE];
			char mount_pt[MAX_MNT_LEN];
			int i = 0;
			struct sockaddr_in client_addr_client;
			socklen_t len = sizeof(struct sockaddr_in);
			int conn_client = accept(listen_client, (SA*)&client_addr_client, &len);
			if(conn_client < 0) {
				printf("[LEG V_GW] ERROR: accept() connection from client.\n");
			}
			else {
				int bytes_read = recv(conn_client, client_hdr, MAXLINE, 0);
				if(bytes_read < 0) {
					printf("[LEG V_GW] mount point not received from client.\n");
					exit(1);
				}
				printf("[LEG V_GW] Client header: \n%s", client_hdr);
				if(strncmp(client_hdr, "GET", 4)) {
					char *ptr1 = strchr(client_hdr, '/');
					printf("ptr1 = %c\n", *ptr1);
					int i = 0;
					while(*ptr1 != ' ')
						mount_pt[i++] = *ptr1++;
					mount_pt[i] = '\0';
					printf("[LEG V_GW] mount point = %s.\n", mount_pt);
				}
				else
					printf("[LEG V_GW] received corrupted header.\n");
				
				printf("[LEG V_GW] Client num = %d.\n", conn_client);
				char video_buff[MAXLINE];
				printf("[LEG V_GW] accepted connection from client !\n");

				int pml = pthread_mutex_lock(&db_mutex);
				bool addClientSuccess = addClient(conn_client, mount_pt);
				pml = pthread_mutex_unlock(&db_mutex);

				if(addClientSuccess) {
					strcpy(video_buff, "HTTP/1.1 200 OK\r\n");
					strcat(video_buff, "Content-Type: video/mp4\r\n");
				}
				else
					strcat(video_buff, "HTTP/1.1 404 Not Found\r\n");
				strcat(video_buff, "\r\n");

				// Send HTTP Ack
				if(send(conn_client, video_buff, strlen(video_buff), 0) < 0)
					perror("[LEG V_GW] send HTTP Ack failed.");
			} // end conn_client < 0
		}

		FD_SET(listen_streamer, &rset);
		FD_SET(listen_client, &rset);
	} // end while(1)
}

/* ----------------------------------------------------------------------------
 * Function	: media_streamer 
 * Description	: Thread
 * 	a. Receives the streamed media from the streamers
 * 	b. Streams the media to the registered clients
 * ---------------------------------------------------------------------------- */

void media_streamer (void* arg) {
	char buff[MAXLINE];
	int oldmax = 0;

	while(1) {
		int x = pthread_mutex_lock(&db_mutex);

		fd_set rset;
		FD_ZERO(&rset);			

		int max_sock_fd = 0;
		for(strmMpIter i = streamers_2_mountpt.begin(); i != streamers_2_mountpt.end(); ++i) {
			// Add sockfd to the set of sockfds to be listened to.
			FD_SET(i->first, &rset);
			// Find the maxsockfd also.
			if(i->first > max_sock_fd)
				max_sock_fd = i->first;
		}
		if(max_sock_fd > oldmax) {
			cout << "[LEG V_GW] new streamer added with sockfd " << max_sock_fd << endl;
			oldmax = max_sock_fd;
		}

		struct timeval tv;
		tv.tv_sec = 0;	
		tv.tv_usec = 0;	
		select(max_sock_fd + 1, &rset, NULL, NULL, &tv);

		list<int> removeStreamerList;
		removeStreamerList.clear();

		for(strmMpIter i = streamers_2_mountpt.begin(); i != streamers_2_mountpt.end(); ++i) {
			if(FD_ISSET(i->first, &rset)) {
				list<int> removeListenerList;
				removeListenerList.clear();
				int recv_len = recv(i->first, buff, MAXLINE, 0);

				if(recv_len <= 0) {
					printf("[LEG V_GW] Read error from streamer.\n");
					removeStreamerList.push_back(i->first);
				} 
				else {
					// First get the "channel" onto which the socket 
					// is feeding. (basically mountpoint).
					const char* mp = i->second;

					// Now get the table of listeners for that mountPoint.
					unordered_map<int, int>* phmii = master_table[mp];

					// For all the listeners, feed the song. If an error
					// is found for a listener, add that socket 
					// to removeList so that we can remove later.
					for(clientsIter j = phmii->begin(); j != phmii->end(); ++j) {
						int sendRetVal = send(j->first, buff, recv_len, 0);
						//printf("[LEG V_GW] %d bytes of data streamed from the server\n", sendRetVal);
						if (sendRetVal < 0) {
							removeListenerList.push_back(j->first);
						}
					}

					// remove listeners who have had problems earlier.
					for (list<int>::iterator k = removeListenerList.begin(); k != removeListenerList.end (); ++k) {
							delClient (*k);
					}
				} // else (if recvLen <= 0)
			} // if (FD_ISSET (i->first, &rset)) 
		}// end of for loop to locate the streamer accepted

		// Remove the streamers who were misbehaving.
		for (list<int>::iterator rsli = removeStreamerList.begin(); rsli != removeStreamerList.end(); ++rsli)
		{
			cout << "[LEG V_GW] Deleting streamer " << *rsli << endl;
			delStreamer (*rsli);
		}
		// We are done with this list. Clear all.
		removeStreamerList.clear();
		// Also if the streamers list is empty, clear rset also.
		if (streamers_2_mountpt.empty())
		{
			FD_ZERO(&rset);
		}
		int y = pthread_mutex_unlock (&db_mutex);
	}//end of while loop
}

/* ----------------------------------------------------------------------------
 * Function	: addClient
 * Description	: adds clients sockfd & mountpoint provided in hash maps -
 * 		a) listeners_2_mountpt
 * 		b) master_table 
 * Inputs	:connected socket descriptor & requested MountPoint
 * Return Value	:a flag indicating whether hash maps are upgraded or not
 * Notes	:
 * ---------------------------------------------------------------------------- */

bool addClient (int sockfd, const char* mp) {
	printf("[LEG V_GW] %s: sockfd = (%d), mp = (%s)\n", __PRETTY_FUNCTION__, sockfd, mp);

	masterIter iter = master_table.find(mp);
	if (iter != master_table.end()) {
		printf("[LEG V_GW] %s: Added a client to the list.", __PRETTY_FUNCTION__);
		lisMpIter i = listeners_2_mountpt.find(sockfd); 
		if(i == listeners_2_mountpt.end()) {
			char *s = (char*) calloc (1, strlen(mp) + 1);
			strcpy (s, mp);
			listeners_2_mountpt[sockfd] = s;
			unordered_map<int, int>* plhm = iter->second;
			(*(plhm))[sockfd] = sockfd;
		} 
		else {
			printf ("[LEG V_GW] Client :(%d) exists\n",sockfd);
			return false; // Though this case is extremely (~100%) unlikely !
		}
	}
	else {
		printf("[LEG V_GW] Client cannot be added as source streaming %s songs is not streaming\n", mp);		   
		//printAllTables();
		return false;
	}

	printAllTables ();
	return true;
}

/* ----------------------------------------------------------------------------
 * Function	: addStreamer
 * Description	: adds clients sockfd & mountpoint provided in hash maps -
 * 		a) streamers_2_mountpt
 * 		b) master_table 
 * Inputs	:connected socket descriptor & requested MountPoint
 * Return Value	:a flag indicating whether hash maps are upgraded or not
 * Notes	:
 * ----------------------------------------------------------------------------*/
void addStreamer (int sockfd, const char* mp) {
	printf("%s: sockfd = (%d), mountPoint = (%s)\n", __PRETTY_FUNCTION__, sockfd, mp);
	printAllTables();

	// Check if mount point present in Master Table
	masterIter iter = master_table.find(mp);
	if (iter == master_table.end()) {
		// This mountpoint not in Master Table. Go ahead and add it.
		cout << "[LEG V_GW] no entry in Master Table for " << mp << endl;
		cout << "Adding " << mp  << " to Master table and streamers_2_mountpt table" << endl;

		strmMpIter i = streamers_2_mountpt.find(sockfd);	
		if(i == streamers_2_mountpt.end()) {
			// streamers_2_mountpt does not contain sockfd
			char* s = (char*) calloc (1, strlen(mp) + 1);
			strcpy(s, mp);
			streamers_2_mountpt[sockfd] = s;
			master_table[s] = new unordered_map<int, int>;
		} 
		else {
			// streamers_2_mountpt already contains sockfd
			printf ("[LEG V_GW] %s: Streamer with sockfd(%d) is already in use\n", __PRETTY_FUNCTION__, sockfd);
		}
	} else {
		// This mountpoint already present in Master Table. Ignore this
		// streamer. Also close the socket connection to the streamer.
	    cout << "[LEG V_GW] mount point " << mp << " already in Master Table. Ignoring..." << endl;
	    close(sockfd);
	}

	printAllTables ();
}

/* ----------------------------------------------------------------------
 * Function delClient
 * ---------------------------------------------------------------------- */
void delClient(int sockfd) {
	printf ("[LEG V_GW] %s: sockfd = (%d)\n", __PRETTY_FUNCTION__, sockfd);

	// Remove clients' entry in the dynamic client table.
	clientsIter ci = (*(master_table[listeners_2_mountpt[sockfd]])).find(sockfd);
	(*(master_table[listeners_2_mountpt[sockfd]])).erase(ci);

	lisMpIter lmi = listeners_2_mountpt.find(sockfd);
	// Free the memory allocated to store the mount point
	free((char*)lmi->second);
	// Remove clients' entry from listeners_2_mountpt table	
	listeners_2_mountpt.erase(lmi);
	// Also close the clients' conn socket
	close (sockfd);

	printAllTables();
}

void delStreamer(int sockfd) {
	printf ("[LEG V_GW] %s: sockfd = (%d)\n", __PRETTY_FUNCTION__, sockfd);

	unordered_map<int, int>* plistbl = master_table[streamers_2_mountpt[sockfd]];
	
	// Close all the client conn sockets listening to this streamer
	// Also erase off the client's entry in the listeners table.
	for (clientsIter si = plistbl->begin(); si != plistbl->end(); ++si) {
		cout << "[LEG V_GW] Closing socket" << si->first << endl;
		close(si->first);

		// erase client's entry in the listeners table.
		listeners_2_mountpt.erase(si->first);
	}

	// Delete the listeners table
	delete plistbl;

	// Erase the streamers entry in the master table.
	masterIter mi = master_table.find(streamers_2_mountpt[sockfd]);
	master_table.erase(mi);

	strmMpIter si = streamers_2_mountpt.find(sockfd);
	// Free the memory allocated to store the mount point
	free((char*)si->second);
	// Erase the streamer from the streamers list.
	streamers_2_mountpt.erase(si);

	printAllTables();
}


void sigpipeHandler (int a)
{
	 printf ("[LEG V_GW] %s called.", __PRETTY_FUNCTION__);
}

void printAllTables ()
{
	printf ("\n-----------------------------------------------------------\n");	
	printMasterTable ();
	printStreamerTable ();
	printListenerTable ();
	printf ("\n-----------------------------------------------------------\n");	
}

void printMasterTable ()
{
	printf ("----------------- Master Table ----------------\n");
	for (masterIter i = master_table.begin(); i != master_table.end(); ++i)
	{
		printf ("mp = (%s)\n", i->first);
		unordered_map<int, int>* plt = i->second;
		for (clientsIter j = plt->begin(); j != plt->end(); ++j)
		{
			printf ("\tsockfd = (%d)\n", j->first);
		}
	}
	printf ("\n");
}

void printStreamerTable ()
{
	printf ("----------------- Streamer Table ----------------\n");
	for (strmMpIter i = streamers_2_mountpt.begin(); i != streamers_2_mountpt.end(); ++i)
	{
		printf ("sockfd = (%d), mp = (%s)\n", i->first, i->second);
	}
	printf ("\n");
}

void printListenerTable ()
{
	printf ("----------------- Listeners Table ----------------\n");
	for (lisMpIter i = listeners_2_mountpt.begin(); i != listeners_2_mountpt.end(); ++i)
	{
		printf ("sockfd = (%d), mp = (%s)\n", i->first, i->second);
	}
	printf ("\n");
}

int max2 (int s1, int s2)
{
	return (s1 > s2 ? s1 : s2);
}
