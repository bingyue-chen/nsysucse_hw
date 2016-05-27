#ifndef CLIENT_SOCKET_H

#define CLIENT_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#ifndef INADDR_NONE
#define INADDR_NONE 0xffffffff
#endif // INADDR_NONE

static int connect_socket(const char* host, const char* service, const char* transport);

int connect_udp(const char* host, const char* service);
int connect_tcp(const char* host, const char* service);

#endif // CLIENT_SOCKET_H

