#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>           // close()
#include <sys/types.h>        // needed for socket(), uint8_t, uint16_t, uint32_t

#include <netdb.h>
#include <sys/socket.h>       // needed for socket()
#include <arpa/inet.h>        // inet_pton() and inet_ntop()


#include <string.h>           // strcpy, memset(), and memcpy()
#include <errno.h>            // errno, perror()

#include "raw_sock.h"


// Computing the internet checksum (RFC 1071).
static uint16_t
checksum (uint16_t *addr, int len)
{
        int count = len;
        register uint32_t sum = 0;
        uint16_t answer = 0;

        // Sum up 2-byte values until none or only one byte left.
        while (count > 1) {
                sum += *(addr++);
                count -= 2;
        }

        // Add left-over byte, if any.
        if (count > 0) {
              sum += *(uint8_t *) addr;  
        }

        // Fold 32-bit sum into 16 bits; we lose information by doing this,
        // increasing the chances of a collision.
        // sum = (lower 16 bits) + (upper 16 bits shifted right 16 bits)
        while (sum >> 16) {
        sum = (sum & 0xffff) + (sum >> 16);
        }

        // Checksum is one's compliment of sum.
        answer = ~sum;

        return (answer);
}


// Allocate memory for an array of chars.
char *
allocate_strmem (int len)
{
        void *tmp;
        if (len <= 0) {
                fprintf (stderr, "ERR: Cannot allocate memory because len = %i in allocate_strmem().\n", len);
                exit (EXIT_FAILURE);
        }

        tmp = (char *) malloc (len * sizeof (char));
        if (tmp != NULL) {
                memset (tmp, 0, len * sizeof (char));
                return (tmp);
        } else {
                fprintf (stderr, "ERR: Cannot allocate memory for array allocate_strmem().\n");
                exit (EXIT_FAILURE);
        }
}


// Allocate memory for an array of unsigned chars.
uint8_t *
allocate_ustrmem (int len)
{
        void *tmp;

        if (len <= 0) {
                fprintf (stderr, 
                        "ERR: Cannot allocate memory because len = %i in allocate_ustrmem().\n", 
                        len);
                exit (EXIT_FAILURE);
        }

        tmp = (uint8_t *) malloc (len * sizeof (uint8_t));

        if (tmp == NULL) {
                fprintf (stderr, 
                        "ERR: Cannot allocate memory for array allocate_ustrmem().\n");
                exit (EXIT_FAILURE);
        }
        memset (tmp, 0, len * sizeof (uint8_t));
        
        return tmp;     
}


// Allocate memory for an array of ints.
int *
allocate_intmem (int len)
{
        void *tmp;

        if (len <= 0) {
                fprintf (stderr, 
                         "ERR: Cannot allocate memory because len = %i in allocate_intmem().\n", 
                        len);
                exit (EXIT_FAILURE);
        }

        tmp = (int *) malloc (len * sizeof (int));

        if (tmp == NULL) {
                fprintf (stderr, 
                         "ERR: Cannot allocate memory for array allocate_intmem().\n");
                exit (EXIT_FAILURE);
        } 

        memset (tmp, 0, len * sizeof (int));
        return tmp;      
}

int
create_raw_sock(void)
{
        int sock_fd;
        
        /* Submit request for a raw socket descriptor */
        if ((sock_fd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
                perror ("ERR: socket() failed ");
                exit (EXIT_FAILURE);
        }

        return sock_fd;

}

/* Set flag so socket expects us to provide IPv4 header */
int 
enable_ip4_hdr_gen(int sock_fd)
{
        const int on = 1;
        if (setsockopt (sock_fd, IPPROTO_IP, IP_HDRINCL, &on, sizeof (on)) < 0) {
                perror ("ERR: setsockopt() failed to set IP_HDRINCL ");
                exit (EXIT_FAILURE);
        }

        return 0;
}


int
bind_raw_sock_to_interface(char *interface, int sock_fd)
{
        int sd;
        struct ifreq ifr;

        /* Submit request for a socket descriptor to look up interface. */
        if ((sd = socket (AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) {
                perror ("socket() failed to get socket descriptor for using ioctl() ");
                exit (EXIT_FAILURE); //TODO: change exit() into return 
        }

        // TODO: Is it necessary ?
        /* 
         * Use ioctl() to look up interface index which we will use to
         * bind socket descriptor sd to specified interface with setsockopt() since
         * none of the other arguments of sendto() specify which interface to use.
         */

        memset (&ifr, 0, sizeof (ifr));
        snprintf (ifr.ifr_name, sizeof (ifr.ifr_name), "%s", interface);
        if (ioctl (sd, SIOCGIFINDEX, &ifr) < 0) {
                perror ("ioctl() failed to find interface ");
                return (EXIT_FAILURE);
        }
        close (sd); //Pourquoi ?
        printf ("Index for interface %s is %i\n", 
                        interface, ifr.ifr_ifindex); // Pourquoi ? Est-ce nécessaire ?

        /* Bind socket to interface index */
        //TODO : use bind  ?
        if (setsockopt (sock_fd, SOL_SOCKET, SO_BINDTODEVICE, &ifr, sizeof (ifr)) < 0) {
                perror ("setsockopt() failed to bind to interface ");
                exit (EXIT_FAILURE);
        }

        return 0;   

}

void
fill_sockaddr_in(struct sockaddr_in *to, struct inject_config *inject_cfg)
{
      memset (to, 0, sizeof (struct sockaddr_in));
      to->sin_family = AF_INET;
      to->sin_addr.s_addr = inject_cfg->iphdr.ip_dst.s_addr; 
}


int
send_packet(int sock_fd, struct inject_config *inject_cfg , struct sockaddr_in *to)
{

        if (sendto (sock_fd, inject_cfg->packet, IP4_HDR_LEN + sizeof(vtl_hdr) + inject_cfg->datalen, 
                                0, (struct sockaddr *) to, sizeof (struct sockaddr)) < 0)  {
                        perror ("ERR: sendto() failed ");
                        exit (EXIT_FAILURE);
        }

        return 0;
}

int 
create_ip4_hdr(struct inject_config *cfg)
{       

        int status; 
        void *tmp;

        struct addrinfo hints, *res;
        struct sockaddr_in *ipv4;
             
        // Pour l'instant, pas très important
        // TODO: revoir cette partie...
        /* Fill out hints for getaddrinfo(). */
        memset (&hints, 0, sizeof (struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_flags = hints.ai_flags | AI_CANONNAME;

        /* Resolve target using getaddrinfo(). */
        if ((status = getaddrinfo (cfg->target, NULL, &hints, &res)) != 0) {
                fprintf (stderr, "ERR: getaddrinfo() failed: %s\n", gai_strerror (status));
                exit (EXIT_FAILURE);
        }
        
        // struct sockaddr_in *ipv4
        ipv4 = (struct sockaddr_in *) res->ai_addr;
        tmp = &(ipv4->sin_addr);
        if (inet_ntop (AF_INET, tmp, cfg->dst_ip, INET_ADDRSTRLEN) == NULL) {
                status = errno;
                fprintf (stderr, 
                         "ERR: inet_ntop() failed.\nError message: %s", 
                        strerror (status));
                exit (EXIT_FAILURE);
        }
        freeaddrinfo (res);

        /** IPv4 header **/

        /* IPv4 header length (4 bits): Number of 32-bit words in header = 5 */
        cfg->iphdr.ip_hl = IP4_HDR_LEN / sizeof (uint32_t);

        /* Internet Protocol version (4 bits): IPv4 */
        cfg->iphdr.ip_v = 4;

        /* Type of service (8 bits) */
        cfg->iphdr.ip_tos = 0;

        /* Total length of datagram (16 bits): IP header + VTL header + VTL data */
        cfg->iphdr.ip_len = htons (IP4_HDR_LEN + sizeof(vtl_hdr) + cfg->datalen);

        /* ID sequence number (16 bits): unused, since single datagram */
        cfg->iphdr.ip_id = htons (0);

        /* Flags, and Fragmentation offset (3, 13 bits): 0 since single datagram */

        /* Zero (1 bit) */
        cfg->ip_flags[0] = 0;

        /* Do not fragment flag (1 bit) */
        cfg->ip_flags[1] = 0;

        /* More fragments following flag (1 bit) */
        cfg->ip_flags[2] = 0;

        /* Fragmentation offset (13 bits) */
        cfg->ip_flags[3] = 0;

        cfg->iphdr.ip_off = htons ((cfg->ip_flags[0] << 15)
                                + (cfg->ip_flags[1] << 14)
                                + (cfg->ip_flags[2] << 13)
                                +  cfg->ip_flags[3]);

        /* Time-to-Live (8 bits): default to maximum value */
        cfg->iphdr.ip_ttl = 255;


        /* Transport layer protocol (8 bits) */
        cfg->iphdr.ip_p = IPPROTO_VTL;

        /* Source IPv4 address (32 bits) */
        if ((status = inet_pton (AF_INET, cfg->src_ip, &(cfg->iphdr.ip_src))) != 1) {
                fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));
                exit (EXIT_FAILURE);
        }

        /* Destination IPv4 address (32 bits) */
        if ((status = inet_pton (AF_INET, cfg->dst_ip, &(cfg->iphdr.ip_dst))) != 1) {
                fprintf (stderr, "inet_pton() failed.\nError message: %s", strerror (status));
                exit (EXIT_FAILURE);
        }

        /* IPv4 header checksum (16 bits): set to 0 when calculating checksum */
        cfg->iphdr.ip_sum = 0;
        cfg->iphdr.ip_sum = checksum ((uint16_t *) &cfg->iphdr, IP4_HDR_LEN);


        return 0;
}

//TODO: add return codes
void 
ip4_pkt_assemble(struct inject_config *cfg)
{
        /* First part is an IPv4 header */
        memcpy (cfg->packet, &cfg->iphdr, IP4_HDR_LEN);

        /* Next part of packet is upper layer protocol header : VTL header */
        memcpy ((cfg->packet + IP4_HDR_LEN), &cfg->vtlh, sizeof(vtl_hdr));

        /* Finally, add the VTL data = app payload */
        memcpy (cfg->packet + IP4_HDR_LEN + sizeof(vtl_hdr), 
                cfg->data, cfg->datalen);

}
