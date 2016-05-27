#include "server_socket.h"

// for non-root services
unsigned short port_base = 0;

int passive_UDP(const char* service){
    return passive_socket(service, "udp", 0);
}

int passive_TCP(const char* service, int qlen){
    return passive_socket(service, "tcp", qlen);
}

static int passive_socket(const char* service, const char* transport, int qlen){

    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;

    int s, type;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = INADDR_ANY;

    if((pse = getservbyname(service, transport)))
        sin.sin_port = htons(ntohs((unsigned short)pse->s_port) + port_base);
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0){
        printf("Invaild service name or port : %s.\n", service);
        exit(1);
    }

    if((ppe = getprotobyname(transport)) == 0){
        printf("Invaild transport : %s\n.", transport);
        exit(2);
    }

    type = (strcmp(transport, "udp") == 0) ? SOCK_DGRAM : SOCK_STREAM;


    if((s = socket(PF_INET, type, ppe->p_proto)) < 0){
        printf("create socket failed.\n");
        exit(3);
    }

    if(bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0){
        printf("bind  to %s port failed.\n", service);
        exit(4);
    }

    if(type == SOCK_STREAM && listen(s, qlen) < 0){
        printf("listen on %s port failed.\n", service);
        exit(5);
    }

    return s;

}

