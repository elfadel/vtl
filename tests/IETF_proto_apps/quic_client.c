/**
 * @file :		quic_client.c
 * @author :		El-Fadel Bonfoh
 * @contribs :
 * @date :		2020
 * @version :		0.1
 * @brief :
*/
#include <quicly/examples/vtl_quic.h>

#define MAX_PKT_SIZE            1024*1000

int main() 
{
    const char *local_ip = "10.0.0.5", *remote_ip = "10.0.0.4";  
    const char *local_port = "4433", *remote_port = "6655";
    int ret;
    int buff_len;
    char buff[MAX_PKT_SIZE];

    static FILE *in_file = NULL;
    in_file = fopen("../../files/out_file.txt", "w");
    if(in_file == NULL) {
        printf("fopen() failed. errno=%d\n", errno);
        exit(1);
    } 

    static vtl_quic_socket_t *vtl_quic_sock = NULL;

    vtl_quic_sock = vtl_quic_init(2, local_ip, local_port, remote_ip, remote_port);
    if(vtl_quic_sock == NULL) {
        printf("vtl_quic_init() failed. errno=%d\n", errno);
        exit(1);
    }

    ret = vtl_quic_connect(&vtl_quic_sock, remote_ip);
    if(ret != 0) {
        printf("vtl_quic_accept() failed. errno=%d\n", errno);
        exit(1);
    } 

    int total_bytes = 0;
    while(1) {
        vtl_quic_recv(vtl_quic_sock, (uint8_t *)buff, (size_t *) &buff_len);
        
        total_bytes += (int)buff_len;

        fwrite(buff, 1, buff_len, in_file);
        fflush(in_file);
        
        printf("TOTAL Bytes = %d\n", total_bytes);
    };

    return 0;
}