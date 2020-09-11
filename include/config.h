/*
 * Fichiers de configuration
 * 
*/

#ifndef __HK_CONFIG_H
#define __HK_CONFIG_H

/* globals...change later */
#include <stdio.h>
//#include <stdlib.h>
#include <linux/types.h>

#define MAX_DATA_SIZE           1024 // TODO : Find the good size...

#define PORT_SERVER_HOOKER     10000 // TODO : change name later...
#define PORT_SOCK_REDIR          10002

#define PORT_SERVER_TCP          9091
#define PORT_CLIENT_TCP          9092
         


typedef struct sock_key sock_key_t; // TODO : find a better file ...
struct sock_key{

    __u32 sip4;
    __u32 dip4;
    __u32 sport;
    __u32 dport;

};

#endif