#include <quicly/examples/vtl_quic.h>

#define MAX_PKT_SIZE            1024
 
int main()
{
    const char *local_ip = "10.0.0.4", *remote_ip = "10.0.0.5";
    const char *local_port = "6655", *remote_port = "4433";
    int ret;
    int buff_len;
    char buff[MAX_PKT_SIZE];

	FILE *out_file = fopen("../files/file8K.txt", "r");
    if(out_file == NULL) {
        printf("fopen() failed. errno=%d\n", errno);
        exit(1);
    }

    static vtl_quic_socket_t *vtl_quic_sock = NULL;

    vtl_quic_sock = vtl_quic_init(1, local_ip, local_port, remote_ip, remote_port);
    if(vtl_quic_sock == NULL) {
        printf("vtl_quic_init() failed. errno=%d\n", errno);
        exit(1);
    }

    ret = vtl_quic_accept(&vtl_quic_sock);
    if(ret != 0) {
        printf("vtl_quic_accept() failed. errno=%d\n", errno);
        exit(1);
    }
    //sleep(10);
    memset(buff, 0, MAX_PKT_SIZE);
    printf("start file streaming !\n");
    while(!feof(out_file)) {
        buff_len = fread(buff, 1, MAX_PKT_SIZE, out_file);

        vtl_quic_send(vtl_quic_sock, (uint8_t *)buff, (size_t)buff_len);
    }
    printf("end file streaming. closing file !\n");

    fclose(out_file);

    return 0;
}