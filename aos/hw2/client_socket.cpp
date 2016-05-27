#include "client_socket.h"

int connect_tcp(const char* host, const char* service){
    return connect_socket(host, service, "tcp");
}

int connect_udp(const char* host, const char* service){
    return connect_socket(host, service, "udp");
}

static int connect_socket(const char* host, const char* service, const char* transport){

    struct hostent *phe;
    struct servent *pse;
    struct protoent *ppe;
    struct sockaddr_in sin;
    int s, type;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    if((pse = getservbyname(service, transport)))
        sin.sin_port = pse->s_port;
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0){
        printf("Invaild service name or port : %s.\n", service);
        exit(1);
    }

    if((phe = gethostbyname(host)))
        memcpy(&sin.sin_addr, phe->h_addr, phe->h_length);
    else if((sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE){
        printf("Invaild host : %s\n.", host);
        exit(2);
    }

    if((ppe = getprotobyname(transport)) == 0){
        printf("Invaild transport : %s\n.", transport);
        exit(3);
    }

    type = (strcmp(transport, "udp") == 0) ? SOCK_DGRAM : SOCK_STREAM;

    if((s = socket(PF_INET, type, ppe->p_proto)) < 0){
        printf("create socket failed.\n");
        exit(4);
    }

    if(connect(s, (struct sockaddr*)&sin, sizeof(sin)) < 0){
        printf("connect %s : %s failed.\n", host, service);
        exit(5);
    }

    return s;

}
