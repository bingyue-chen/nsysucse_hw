#ifndef SERVER_SOCKET_H

#define SERVER_SOCKET_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

static int passive_socket(const char* service, const char* transport, int qlen);

int passive_UDP(const char* service);
int passive_TCP(const char* service, int qlen);

#endif // SERVER_SOCKET_H
