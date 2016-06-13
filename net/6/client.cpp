#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#include "client_socket.h"
#include "config.h"
#define socket_buffer_size 10240

static unsigned char socket_buffer[socket_buffer_size];
static char *host, *port;
static int client_socket;
static unsigned short d_port, s_port;
static struct sockaddr sin;
static socklen_t addrlen = sizeof(sin);
static unsigned int sequence_number;
static unsigned int ack_number;
static size_t TCP_Header_size = sizeof(TCP_Header_Format);
static const int data_size = 10240;
static unsigned char data[data_size], record_ack[data_size+1];

static void TCP_Header_init (struct TCP_Header_Format &TH);
static void TCP_connect();
static void TCP_disconnect();
static void receive_data();

int main(int argc, char* argv[]){

    if(argc < 3){
        printf("no ip or port");
        exit(-1);
    }

    host = argv[1];
    port = argv[2];
    client_socket = connect_udp(host, port);

    getpeername(client_socket, &sin, &addrlen);
    struct sockaddr_in* temp;
    temp = (struct sockaddr_in*)&sin;
    s_port = temp->sin_port;

    sscanf(port, "%hu", &d_port);
    srand(time(NULL));
    sequence_number = rand()%1024 + 1;
    ack_number = 0;

    //connect
    TCP_connect();

    //receive data
    receive_data();

    //disconnect
    TCP_disconnect();

    return 0;
}

static void TCP_Header_init (struct TCP_Header_Format &TH){
    memset(&TH, 0, TCP_Header_size);
    TH.source_port     = s_port;
    TH.dest_port       = d_port;
    TH.sequence_number = sequence_number++;
    TH.ack_number      = (ack_number == 0) ? 0 : ack_number;
}

static void TCP_connect(){

    struct TCP_Header_Format TCP_Header_sender, TCP_Header_receiver;

    //send SYN
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.SYN = 1;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(client_socket, socket_buffer, socket_buffer_size, 0, (const struct sockaddr *)0, 0);
    printf("Send a SYN packet to %s : %s\n", host, port);

    //receive SYN/ACK
    recvfrom(client_socket, &socket_buffer, socket_buffer_size, 0, (struct sockaddr *)0, (socklen_t *)0);
    printf("Received a SYN/ACK packet from %s : %s\n", host, port);
    memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
    show_TCP_Header(TCP_Header_receiver);
    ack_number = ++TCP_Header_receiver.sequence_number;

    //send ACK
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.ACK = 1;
    TCP_Header_sender.window = data_size;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(client_socket, socket_buffer, socket_buffer_size, 0, (const struct sockaddr *)0, 0);
    printf("Send an ACK packet to %s : %s\n", host, port);

    printf("Complete three way handshake\n");

}

static void TCP_disconnect(){

    struct TCP_Header_Format TCP_Header_sender, TCP_Header_receiver;

    //send ACK
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.ACK = 1;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(client_socket, socket_buffer, socket_buffer_size, 0, (const struct sockaddr *)0, 0);
    printf("Send an ACK packet to %s : %s\n", host, port);

    //send FIN
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.FIN = 1;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(client_socket, socket_buffer, socket_buffer_size, 0, (const struct sockaddr *)0, 0);
    printf("Send a FIN packet to %s : %s\n", host, port);

    //receive ACK
    recvfrom(client_socket, &socket_buffer, socket_buffer_size, 0, (struct sockaddr *)0, (socklen_t *)0);
    printf("Received an ACK packet from %s : %s\n", host, port);
    memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
    show_TCP_Header(TCP_Header_receiver);
    ++ack_number;

    printf("Complete four way handshake\n");
}

static void receive_data(){

    struct TCP_Header_Format TCP_Header_sender, TCP_Header_receiver;
    int n, rcv_data_size, t_ack, *lose_number, lose_amount, i;
    struct timeval timeout;

    memset(&TCP_Header_sender, 0, TCP_Header_size);
    TCP_Header_sender.source_port   = s_port;
    TCP_Header_sender.dest_port     = d_port;
    t_ack = 1;

    timeout.tv_sec = 0;
    timeout.tv_usec = 200;
    lose_amount = 3;

    lose_number = new int[lose_amount];
    lose_number[0] = 5120, lose_number[1] = 6144, lose_number[2] = 7168;


    memset(record_ack, 0, sizeof(record_ack));

    printf("%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t%-7s\t\n", "ACK", "1 Left", "1 Right", "2 Left", "2 Right", "3 Left", "3 Right");
    while(1){

        timeout.tv_sec = 0;
        timeout.tv_usec = 200;
        setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
        //receive
        n = recvfrom(client_socket, &socket_buffer, socket_buffer_size, 0, (struct sockaddr *)0, (socklen_t *)0);
        if(n > 0 ){

            memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
            if(TCP_Header_receiver.FIN == 1){
                printf("Received a FIN packet from %s : %s\n", host, port);
                show_TCP_Header(TCP_Header_receiver);
                return;
            }
            //show_TCP_Header(TCP_Header_receiver);

            //special lose
            for(i = 0; i < lose_amount; i++){
                if(TCP_Header_receiver.sequence_number == lose_number[i]){
                    lose_number[i] = 0;
                    break;
                }
            }
            if(i < lose_amount) continue;

            //processing data
            n -= TCP_Header_size;
            if(TCP_Header_receiver.sequence_number-1+n <= data_size){
                //printf("receive size = %d\n", n);
                rcv_data_size -= n;
                memcpy(data+TCP_Header_receiver.sequence_number-1, socket_buffer+TCP_Header_size, n);
                memset(record_ack+TCP_Header_receiver.sequence_number, 1, n);
            }
            continue;
        }

        //send ACK
        while(record_ack[t_ack] == 1) t_ack++;
        printf("%-7d\t", t_ack);
        for(i = t_ack + 1; i < data_size+1; i++){
            if(record_ack[i] != record_ack[i-1]){
                printf("%-7d\t", i);
            }
        }
        printf("\n");

        TCP_Header_sender.sequence_number   = sequence_number++;
        TCP_Header_sender.ack_number        = t_ack;
        TCP_Header_sender.window            = data_size - rcv_data_size;
        memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
        sendto(client_socket, socket_buffer, socket_buffer_size, 0, (const struct sockaddr *)0, 0);

    }

    timeout.tv_sec = 1;
    timeout.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));

    return;
}
