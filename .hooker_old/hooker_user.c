/* 
 * @file :          hooker_user.c
 * @author :        El-Fadel Bonfoh, Cédric Tape
 * @date :          07/2020
 * @version:        1.0
 * @versions :      0.1 (05/2019); 1.0 (07/2020) 
 * @brief :         hooker userspace component
*/ 

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//#include <mntent.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#include <semaphore.h>
#include <pthread.h>
#include <mqueue.h>

#include <bpf/libbpf.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <sys/ioctl.h>
#include <sys/resource.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include <linux/bpf.h>
//#include "bpf_load.h"
//#include "libbpf.h"
#include "../launcher/launcher.h"

//#define HOOKER_BPF_FILENAME        "hooker-kern.o"

//#define  CGROUP_PATH            "/test_cgroup"
#define MAXDATASIZE         100
#define SA                  struct sockaddr
//#define PORT              8080
#define HK_REDIR_PORT       10000
#define HK_FAKE_PORT        10001
#define L4_GRAFT_PORT       6666
#define UDPLITE             136
#define MAX_SOCKMAP_CHECK   100

#define clean_errno() (errno == 0 ? "None" : strerror(errno))
#define log_err(MSG, ...) fprintf(stderr, "(%s:%d: errno: %s) " MSG "\n", \
    __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)


/* globals */
char *serv_addr = "10.0.0.4";
int cg_fd/*, hacpt*/, hk_redir_fd, hk_fake_fd, hk_l4_graft_fd, selected_l4_proto_graft = 0;
struct sockaddr_in to;

/*struct sock_key {     
    __u32 src_ip4;      
    __u32 dst_ip4;
    __u32 src_port;
    __u32 dst_port;
};*/

struct stream_tuple { // TODO: include vtl.h and remove this struct.
    __u32 src_ip;
    __u32 dst_ip;
    __u32 src_port;
    __u32 dst_port;
};

int hk_init_sockets(void) {
    
    int err, one;
    struct sockaddr_in addr;

    /* START: Create and config HK's sockets */

    if((hk_redir_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[HK-USER]: socket() redir sk failed.\n");
        return errno;
    }

    if((hk_fake_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("[HK-USER]: socket() fake sk failed.\n");
        return errno;
    }

    // Allow reuse
    err = setsockopt(hk_fake_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&one, sizeof(one));
    if(err) {
        perror("[HK-USER]: setsockopt() fake sk failed.\n");
        return errno;
    }

    // Non-blocking socket
    err = ioctl(hk_fake_fd, FIONBIO, (char*)&one); 
    if (err < 0) {
        perror("[HK-USER]: ioctl() fake sk failed.\n");
        return errno;
    }

    // Bind and passive connect fake sk
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // TODO: Consider the use of var *serv_addr*
    addr.sin_port = htons(HK_FAKE_PORT);
    err = bind(hk_fake_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        perror("[HK-USER]: bind() fake sk failed.\n");
        return errno;
    }
    //addr.sin_port = htons(S1_PORT);
    err = listen(hk_fake_fd, 32);
    if (err < 0) {
        perror("[HK-USER]: listen() fake sk failed.\n");
        return errno;
    }   

    // Bind and active connect redir sk
    struct sockaddr_in caddr;
    memset(&caddr, 0, sizeof(struct sockaddr_in));
    caddr.sin_family = AF_INET;
    caddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // TODO: Consider the use of var *serv_addr*
    caddr.sin_port = htons(HK_REDIR_PORT);
    err = bind(hk_redir_fd, (struct sockaddr *)&caddr, sizeof(caddr));
    if (err < 0) {
        perror("[HK-USER]: bind() redirect sk failed.\n");
        return errno;
    }
    //addr.sin_port = htons(S1_PORT);
    // Connect to @fake_sk
    err = connect(hk_redir_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0 && errno != EINPROGRESS) {
        perror("[HK-USER]: connect() redir sk failed.\n");
        return errno;
    }

    // Establish fake tcp connection: redir_sk <--> fake_sk 
    if (accept(hk_fake_fd, NULL, NULL) < 0) {
        perror("[HK-USER]: accept() redir_sk to fake_sk failed.\n");
        return errno;
    }

    /* END: Create and config HK's sockets */

    /* 
     * START: config vtl socket/channel and bind to selected l4 graft to send()/recv()
     *        hooked and redirected packets data.
    */ 
    /* 
     * srand(time(0));
     * selected_l4_proto_graft = rand()%2;
    */
    hk_l4_graft_fd = socket(AF_INET, SOCK_DGRAM, selected_l4_proto_graft);
    if(hk_l4_graft_fd < 0) {
        perror("[HK-USER]: creation vtl socket failed.\n");
        return errno;
    }

    addr.sin_addr.s_addr = INADDR_ANY; // Consider the use of "10.0.0.5"
    addr.sin_port = htons(L4_GRAFT_PORT);
    err = bind(hk_l4_graft_fd, (struct sockaddr *)&addr, sizeof(addr));
    if (err < 0) {
        perror("[HK-USER]: bind() l3 graft sk failed.\n");
        return errno;
    }

    // Fill destination information (app legacy)
    to.sin_family = AF_INET;
    to.sin_addr.s_addr = inet_addr(serv_addr); // TODO: replace by server ip = 10.0.0.4??
    to.sin_port = htons(L4_GRAFT_PORT);

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
            return -1; // TODO: Consider the use of errno
        }
        printf("[HK-USER]: Packet redirected: %d -> %d", -1, hk_redir_fd);
        
        /* Hooker -> remote appli */
        if((ret = sendto(hk_l4_graft_fd, buffer, strlen(buffer), 0, 
                    (const struct sockaddr *)&to, sizeof(to))) < 0 ) {
            perror("[HK-USER]: sendto() remote app (remote hooker in fact).\n");
            return errno;
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
        ret = recvfrom(hk_l4_graft_fd, buffer, MAXDATASIZE, 0, 
                                (struct sockaddr *)&to, (socklen_t *)&tosize);
        if(ret < 0) {
            perror("[HK-USER]: recvfrom () from remote app (local hooker in fact).\n");
            return errno;
        }

        /* Hooker -> local appli */
        if (send(hk_redir_fd, buffer, strlen(buffer), 0) == -1) {
            perror("[HK-USER]: send() to local appli.");
            return errno;
        }
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
/*
char *find_cgroup_root(void) {
    
    FILE *f;
    struct mntent *mnt;
    
    f = fopen("/proc/mounts", "r");
    if(f == NULL)
        return NULL;
    while ((mnt = getmntent(f))) {
        if(strcmp(mnt->mnt_type, "cgroup2") == 0) {
            fclose(f);
            return strdup(mnt-> mnt_dir);
        }
    }

    fclose(f);
    return NULL;
} 
*/

/*void detach_cgroup_root(int sig) {
    
    fprintf(stderr,"Removing bpf program...\nExit\n");
        int ret = bpf_prog_detach2(prog_fd[0], cg_fd, BPF_CGROUP_SOCK_OPS);
        if(ret) {
                printf("Failed to detach bpf program\tret = %d\n", ret);
                perror("bpf_prog_attach");          
        }
        close(cg_fd);
        exit(0); 
}*/


int main(int argc, char*argv[]) {
    
    /*int status = 0;

    printf("Loading bpf program in kernel...\n");
    if (load_bpf_file(HOOKER_BPF_FILENAME)) {
        printf("erreur!");
        fprintf(stderr, "ERR in load_bpf_file(): %s\n", bpf_log_buf);
        status = -1;
        goto out;
    }*/

    int sockmap_check_count = 0, status = 0;
    while (sockmap_check_count < MAX_SOCKMAP_CHECK && !maps_fd[0]) { // kern part not yet deployed.
        fprintf(stderr, "[HK-USER]: Try n° %d. Sockmap not found: %s\n", sockmap_check_count++, strerror(errno));
    }
    if (sockmap_check_count == MAX_SOCKMAP_CHECK)  { // Give up. No sockmap loaded by launcher component.
        fprintf(stderr, "[HK-USER]: Exit. No sockmap found: %s\n", strerror(errno));
        status = -1;
        goto out;
    } 
    
    if(hk_init_sockets())
        goto out;

    /* get cgroup root descriptor */
    /*char *cgroup_root_path = find_cgroup_root();
    cg_fd = open(cgroup_root_path, O_RDONLY);
    if (cg_fd < 0) {
        log_err("Opening cgroup root");
                status = -1;
            goto out;
    } 
    */

    /* Fill MAPS: 
     * first entry of hk_sock_map and,  
     * the unique entry of hk_app_info_map 
    */
    struct stream_tuple app_index = {0};
    int hk_sock_map_fd = maps_fd[0];
    if(bpf_map_update_elem(hk_sock_map_fd, &app_index, &hk_redir_fd, BPF_ANY) != 0) {
        printf("[HK-USER]: bpf_map_update() hk_sock_map failed\n");
        //perror("bpf_map_update");
        status = -1;
        //goto close;
    } 
    // TODO: Check if the below init is necessary or not.
    __u32 sk_index = 0;
    long value = 0;
    int hk_app_info_map_fd = maps_fd[1];
    if(bpf_map_update_elem(hk_app_info_map_fd, &sk_index, &value, BPF_ANY) != 0){
        printf("[HK-USER]: update cnt_map failed\n");
        //bpf_map_update() hk_sock_map failed
        status = -1;
        goto out;
    }

    /* Attach ebpf programs to... */

    /*  printf("Attaching bpf program...\n");
        int bpf_sockops = prog_fd[0];
        int bpf_redir = prog_fd[1];
        int ret;

        ret = bpf_prog_attach(bpf_sockops, cg_fd, BPF_CGROUP_SOCK_OPS, 
                  BPF_F_ALLOW_MULTI);
        if(ret) {
            printf("Failed to attach bpf_sockops to cgroup root program\tret = %d\n", ret);
                perror("bpf_prog_attach");
                status = -1;
                goto err_sockops;
        } 
    
        ret = bpf_prog_attach(bpf_redir, hmap, BPF_SK_MSG_VERDICT,0);
        if(ret) {
                printf("Failed to attach bpf_redir to sockhash\tret = %d\n", ret);
                perror("bpf_prog_attach");
                status = -1;
                goto err_skmsg;
        } */
        int ret;

        pthread_t hk_listen_thread;
        pthread_t hk_send_thread;

        ret = pthread_create(&hk_listen_thread, NULL, thread_listen, NULL);
        if(ret) {
                perror("[HK-USER]: error on create listen thread\n");
                status = -1;
                goto out;
        }
    
        ret = pthread_create(&hk_send_thread, NULL, thread_sendto, NULL);
        if(ret) {
                perror("[HK-USER]: error on create of send thread\n");
                status = -1;
                goto out;
        }

        ret = pthread_join(hk_listen_thread, NULL);
        if(ret) {
                perror("[HK-USER]: join() listen thread failed\n");
                status = -1;
                goto out;
        }

        ret = pthread_join(hk_send_thread, NULL);
        if(ret) {
                perror("[HK-USER]: join() sendto thread failed\n");
                status = -1;
                goto out;
        }

out: 
        return status;
}
