
#ifndef __RAW_SOCKET_H
#define __RAW_SOCKET_H

#include <netinet/ip.h>       // struct ip and IP_MAXPACKET (which is 65535)
//#include <linux/ip.h>
#include <net/if.h>           // struct ifreq
#include <sys/ioctl.h>        // macro ioctl is defined
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t

#include "../../include/vtl.h"


//TODO: Find later a better name
struct inject_config {

        uint8_t *packet;
        uint8_t *data;
        size_t datalen;      
        vtl_hdr vtlh;
        struct ip iphdr;

        int *ip_flags;
        char *src_ip;
        char *dst_ip;
        char *interface;
        char *target; // Why ???
  
        struct ifreq ifr;// TODO: is it necessary ?

};

char *allocate_strmem (int len);
uint8_t *allocate_ustrmem (int len);
int *allocate_intmem (int len);
int *allocate_intmem (int len);

int create_raw_sock(void);
int enable_ip4_hdr_gen(int sock_fd);
int bind_raw_sock_to_interface(char *interface, int sock_fd);
void fill_sockaddr_in(struct sockaddr_in *to, struct inject_config *inject_cfg);

int create_ip4_hdr(struct inject_config *cfg);
void ip4_pkt_assemble(struct inject_config *cfg);

int send_packet(int sock_fd, struct inject_config *inject_cfg , struct sockaddr_in *to);




#endif /* __RAW_SOCKET_H */
