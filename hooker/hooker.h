#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <mntent.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <linux/bpf.h>
#include <bpf/bpf.h>

#define MAXDATASIZE         			1024
#define SA                  			struct sockaddr
#define HK_REDIR_PORT       			10000
#define HK_PHONY_PORT       			10001
#define L4_GRAFT_PORT       			6666
#define SOL_UDPLITE         			136
#define UDPLITE_SEND_CSCOV  			10
#define UDPLITE_RECV_CSCOV				11
#define SOL_DCCP 						269
#define DCCP_SOCKOPT_SERVICE 			2
#define MAX_SOCKMAP_CHECK   			100
#define IPPROTO_QUIC 					142