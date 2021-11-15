/* 
 * @file :          hooker.c
 * @author :        El-Fadel Bonfoh, Cédric Tape
 * @date :          07/2020
 * @version:        1.0
 * @versions :      0.1 (05/2019); 1.0 (07/2020) 
 * @brief :         hooker daemon component
*/

#include "../include/vtl.h"
#include <quicly/examples/vtl_quic.h>

#include "hooker.h"

#define 	clean_errno() (errno == 0 ? "None" : strerror(errno))
#define 	log_err(MSG, ...) fprintf(stderr, "(%s:%d: errno: %s) " MSG "\n", \
    						__FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)


/* globals */
const char *local_ip = "10.0.0.4", *remote_ip = "10.0.0.5";
const char *local_port = "6666", *remote_port = "6666";
int hk_redir_fd, hk_phony_fd, hk_l4_graft_fd, selected_l4_proto_graft = 0;
int hk_l4_graft_fd_cpy = -1;
int sctp_flags, pkt_size = 256, val = 20;
/*static*/ vtl_quic_socket_t *vtl_quic_sock = NULL;
struct sockaddr_in to;

struct sctp_sndrcvinfo snd_rcv_info;

struct sctp_initmsg initmsg = { 
    .sinit_num_ostreams = 32,
    .sinit_max_instreams = 32,
    .sinit_max_attempts = 4,
};

static const char* path_to_sockmap = "/sys/fs/bpf/skmap";

int hk_get_map_fd(const char* path_name) {
    return bpf_obj_get(path_name);
}

int hk_init_sockets(int IPPROTO_X) {
    
    int err, one;
    struct sockaddr_in addr, graft_addr;

    /* START: Create and config HK's sockets */
    if((hk_redir_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[HK-USER]: socket() redir sk failed.\n");
        return errno;
    }
    printf("[HK-USER]: redirector socket init=%d.\n", hk_redir_fd);

    if((hk_phony_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[HK-USER]: socket() phony sk failed.\n");
        return errno;
    }
    printf("[HK-USER]: phony socket init=%d.\n", hk_phony_fd);

    // Allow reuse
    err = setsockopt(hk_phony_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    if(err) {
        perror("[HK-USER]: setsockopt() phony sk failed.\n");
        return errno;
    }

    // Non-blocking socket
    err = ioctl(hk_phony_fd, FIONBIO, (char*)&one); 
    if (err < 0) {
        perror("[HK-USER]: ioctl() phony sk failed.\n");
        return errno;
    }

    // Bind and passive connect phony sk
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = htons(HK_PHONY_PORT);
    err = bind(hk_phony_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        perror("[HK-USER]: bind() phony sk failed.\n");
        return errno;
    }
    
    err = listen(hk_phony_fd, 32);
    if (err < 0) {
        perror("[HK-USER]: listen() phony sk failed.\n");
        return errno;
    }   
    printf("[HK-USER]: phony sk listening...\n");

    // Bind and active connect redir sk
    struct sockaddr_in caddr;
    memset(&caddr, 0, sizeof(struct sockaddr_in));
    caddr.sin_family = AF_INET;
    caddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    caddr.sin_port = htons(HK_REDIR_PORT);

    err = setsockopt(hk_redir_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    if(err) {
        perror("[HK-USER]: setsockopt() redir sk failed.\n");
        return errno;
    }

    err = bind(hk_redir_fd, (struct sockaddr *)&caddr, sizeof(caddr));
    if (err < 0) {
        perror("[HK-USER]: bind() redirect sk failed.\n");
        return errno;
    }
    // Connect to @phony_sk
    err = connect(hk_redir_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0 && errno != EINPROGRESS) {
        perror("[HK-USER]: connect() redir sk failed.\n");
        return errno;
    }
    printf("[HK-USER]: redirector sk connecting...\n");

    if (accept(hk_phony_fd, NULL, NULL) < 0) {
        perror("[HK-USER]: accept() redir_sk to phony_sk failed.\n");
        return errno;
    }
    printf("[HK-USER]: phony TCP connection established !\n");
    /* END: Create and config HK's sockets */

    /* 
     * START: config vtl socket/channel and bind to selected l4 graft to send()/recv()
     *        hooked and redirected packets data.
    */ 
    switch(IPPROTO_X) {
        case IPPROTO_UDP:
            printf("[HK-USER]: configuring vtl UDP socket ...\n");
            hk_l4_graft_fd = socket(AF_INET, SOCK_DGRAM, selected_l4_proto_graft);
            if(hk_l4_graft_fd < 0) {
                perror("[HK-USER]: creation vtl socket failed.\n");
                return errno;
            }
            break;

        case IPPROTO_UDPLITE:
            printf("[HK-USER]: configuring vtl UDPLite socket ...\n");
            selected_l4_proto_graft = IPPROTO_UDPLITE;
            hk_l4_graft_fd = socket(AF_INET, SOCK_DGRAM, selected_l4_proto_graft);
            if(hk_l4_graft_fd < 0) {
                perror("[HK-USER]: creation vtl socket failed.\n");
                return errno;
            }
            setsockopt(hk_l4_graft_fd, SOL_UDPLITE, UDPLITE_SEND_CSCOV, &val, 
                                        sizeof(int));
            break;

        case IPPROTO_DCCP:
            printf("[HK-USER]: configuring vtl DCCP socket ...\n");
            selected_l4_proto_graft = IPPROTO_DCCP;
            hk_l4_graft_fd = socket(AF_INET, SOCK_DCCP, selected_l4_proto_graft);
            if(hk_l4_graft_fd < 0) {
                perror("[HK-USER]: creation vtl socket failed.\n");
                return errno;
            }
            setsockopt(hk_l4_graft_fd, SOL_DCCP, DCCP_SOCKOPT_SERVICE,
                            (char*)&pkt_size, sizeof(pkt_size));
            break;

        case IPPROTO_SCTP:
            printf("[HK-USER]: configuring vtl SCTP socket ...\n");
            selected_l4_proto_graft = IPPROTO_SCTP;
            hk_l4_graft_fd = socket(AF_INET, SOCK_STREAM, selected_l4_proto_graft);
            if(hk_l4_graft_fd < 0) {
                perror("[HK-USER]: creation vtl socket failed.\n");
                return errno;
            }
            break;

        case IPPROTO_QUIC:
            selected_l4_proto_graft = IPPROTO_QUIC;
            printf("[HK-USER]: configuring vtl QUIC socket ...\n");
            vtl_quic_sock = vtl_quic_init(1, local_ip, local_port, remote_ip, remote_port);
            if(vtl_quic_sock == NULL) {
                perror("[HK-USER]: creation vtl socket failed.\n");
                return errno;
            }
            break;

        default:
            printf("Something went wrong. Fallback to TCP !\n");
            break;
    }

    setsockopt(hk_l4_graft_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));

    graft_addr.sin_family = AF_INET;
    graft_addr.sin_addr.s_addr = htonl(INADDR_ANY); // "10.0.0.5"
    graft_addr.sin_port = htons(L4_GRAFT_PORT);
    if(selected_l4_proto_graft != IPPROTO_QUIC) {
        err = bind(hk_l4_graft_fd, (struct sockaddr *)&graft_addr, sizeof(graft_addr));
        if (err < 0) {
            perror("[HK-USER]: bind() l4 graft sk failed.\n");
            return errno;
        }
    }

    // Fill destination information (app legacy)
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = inet_addr(remote_ip);
    to.sin_port = htons(L4_GRAFT_PORT);

    if(IPPROTO_X == IPPROTO_DCCP || IPPROTO_X == IPPROTO_SCTP) {

        if(IPPROTO_X == IPPROTO_SCTP) {
            err = setsockopt(hk_l4_graft_fd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof(initmsg));
            if(err < 0) {
                perror("[HK-USER]: setsockopt() vtl SCTP sk failed.\n");
                return errno;
            }
        }

        err = listen(hk_l4_graft_fd, 32);
        if (err < 0) {
            perror("[HK-USER]: listen() graft sk failed.\n");
            return errno;
        }
        socklen_t len = sizeof(to);
        hk_l4_graft_fd_cpy = accept(hk_l4_graft_fd, (SA*)&to, &len);
        if(hk_l4_graft_fd_cpy < 0) {
            perror("[HK-USER]: accept graft sk failed(1).\n");
            return errno;
        }
        printf("[HK-USER]: redirection connection established.\n");
    }
    else if(IPPROTO_X == IPPROTO_QUIC) {
        err = vtl_quic_accept(&vtl_quic_sock);
        if(err != 0) {
            perror("[HK-USER]: accept graft sk failed(2).\n");
            return errno;
        }
        printf("[HK-USER]: redirection connection established.\n");
    }
    else {
        printf("[HK-USER]: vtl socket configure !\n");
    }

    return 0;
}

int hk_listen_app(void) {
    
    char buffer[MAXDATASIZE];
    int ret;
    while(1) {
        memset(buffer, 0, sizeof(buffer));
        /* Local appli --> Hooker */
        if((ret = recv(hk_redir_fd, buffer, MAXDATASIZE, 0)) == -1) {
            perror("[HK-USER]: recv() from local app failed.\n");
            return errno;
        }
           
        /* Hooker -> remote appli */
        if(selected_l4_proto_graft == IPPROTO_DCCP) {
            do {
                ret = send(hk_l4_graft_fd_cpy, buffer, strlen(buffer), 0);
            } while((ret < 0) && (errno == EAGAIN));
        }
        else if(selected_l4_proto_graft == IPPROTO_SCTP) {
            ret = sctp_sendmsg(hk_l4_graft_fd_cpy, buffer, strlen(buffer), NULL, 0, 0, 0, 0, 0, 0);
            if(ret < 0) {
                perror("[HK-USER]: sctp_sendmsg() to remote app (remote hooker in fact).\n");
                return errno;
            }
        }
        else if(selected_l4_proto_graft == IPPROTO_QUIC) {
            size_t buff_len = (size_t) strlen(buffer);
            ret = vtl_quic_send(vtl_quic_sock, (uint8_t *)buffer, buff_len);
            if(ret != 0) {
                perror("[HK-USER]: vtl_quic_send() to remote app (remote hooker in fact).\n");
                printf("errno=%d\n", errno);
                return errno;
            } 
        }
        else {
            ret = sendto(hk_l4_graft_fd, buffer, strlen(buffer), 0, (const struct sockaddr *)&to, sizeof(to));
            if(ret < 0) { 
                perror("[HK-USER]: sendto() remote app (remote hooker in fact).\n");
                return errno;
            }
        }
    }
           
    return 0;
}

int hk_sendto_app(void) {
        
	char buffer[MAXDATASIZE];
    	int ret; 
    	int tosize = sizeof(to);
    	while(1) {
	    	
	    	memset(buffer, 0, sizeof(buffer));

	    	/* remote appli -> Hooker */
	    	if(selected_l4_proto_graft == IPPROTO_DCCP) {
	        	ret = recv(hk_l4_graft_fd_cpy, buffer, MAXDATASIZE, 0);
	        	if(ret < 0) {
	            		perror("[HK-USER]: dccp recv() from remote app (local hooker in fact).\n");
	            		return errno;
	        	}
	    	}
	    	else if(selected_l4_proto_graft == IPPROTO_SCTP) {
	        	ret = sctp_recvmsg(hk_l4_graft_fd_cpy, buffer, MAXDATASIZE, NULL, 0, &snd_rcv_info, &sctp_flags);
	    	}
	    	else if(selected_l4_proto_graft == IPPROTO_QUIC) {
	        	size_t buff_len = 0;
	        	do {
	            		ret = vtl_quic_recv(vtl_quic_sock, (uint8_t *)buffer, &buff_len);
	        	} while(buff_len == 0);
	    	}
	    	else {
	        	ret = recvfrom(hk_l4_graft_fd, buffer, MAXDATASIZE, 0, 
	                            (struct sockaddr *)&to, (socklen_t *)&tosize); 
	        	if(ret < 0) {
	            		perror("[HK-USER]: recvfrom () from remote app (local hooker in fact).\n");
	            		return errno;
	        	}
	    	}

	    	/* Hooker -> local appli */
	    	if (send(hk_redir_fd, buffer, strlen(buffer), 0) == -1) {
	        	perror("[HK-USER]: send() to local appli.");
	        	return errno;
	    	}

	    	printf("[HK-USER]: local redirected %ld bytes.\n", strlen(buffer));
    	}

    return 0;
}

void *thread_listen(void *arg) {
    
    (void) arg;
    hk_listen_app();
    pthread_exit(NULL);
}

void *thread_sendto(void *arg) {
    
    (void) arg;
    hk_sendto_app();
    pthread_exit(NULL);
}

int main(int argc, char*argv[]) {
    
    int sockmap_check_count = 0, status = 0, ret;
    int hk_sock_map_fd = hk_get_map_fd(path_to_sockmap);
    while (/*sockmap_check_count < MAX_SOCKMAP_CHECK &&*/ hk_sock_map_fd < 0) { // kern part not yet deployed. Just retry. Be strong bro :)
        fprintf(stderr, "[HK-USER]: Try n° %d. Sockmap not found: %s. Sleeping...\n", sockmap_check_count++, strerror(errno));
        sleep(1);
        hk_sock_map_fd = hk_get_map_fd(path_to_sockmap);
    }
    
    if(argc != 2) {
        printf("[HK-USER]: Syntax error.\nUsage: %s <IPPROTO>\n", argv[0]);
        exit(0);
    }

    if(hk_init_sockets(atoi(argv[1])))
        return -1;

    /* Fill MAPS: 
     * - first entry of hk_sock_map
    */
    int app_index = 0;
    if(bpf_map_update_elem(hk_sock_map_fd, &app_index, &hk_redir_fd, BPF_ANY) != 0) {
        printf("[HK-USER]: bpf_map_update() hk_sock_map failed\n");
        status = -1;
        return status;
    } 

    pthread_t hk_listen_thread;
    pthread_t hk_send_thread;

    ret = pthread_create(&hk_listen_thread, NULL, thread_listen, NULL);
    if(ret) {
        perror("[HK-USER]: error on create listen thread\n");
        status = -1;
        return status;
    }
    
    ret = pthread_create(&hk_send_thread, NULL, thread_sendto, NULL);
    if(ret) {
        perror("[HK-USER]: error on create of send thread\n");
        status = -1;
        return status;
    }

    ret = pthread_join(hk_listen_thread, NULL);
    if(ret) {
        perror("[HK-USER]: join() listen thread failed\n");
        status = -1;
        return status;
    }

    ret = pthread_join(hk_send_thread, NULL);
    if(ret) {
        perror("[HK-USER]: join() sendto thread failed\n");
        status = -1;
        return status;
    }

    close(hk_phony_fd);
    close(hk_redir_fd);
    close(hk_l4_graft_fd);
    if(hk_l4_graft_fd_cpy != -1)
        close(hk_l4_graft_fd_cpy);

    return status;
}
