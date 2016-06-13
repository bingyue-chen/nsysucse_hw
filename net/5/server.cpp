#include <unistd.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <algorithm>

#include "server_socket.h"
#include "config.h"
#define socket_buffer_size 10240

static unsigned char socket_buffer[socket_buffer_size];
static unsigned int msock, RTT, threshold, MSS, sequence_number, ack_number, cwnd, rwnd, s_port, state;
static char* host;
static unsigned short d_port;
static struct sockaddr client_socket;
static socklen_t addrlen = sizeof(client_socket);
static size_t TCP_Header_size = sizeof(TCP_Header_Format);
static const int data_size = 10240;
static unsigned char data[data_size];

static void init();
static void TCP_Header_init (struct TCP_Header_Format &TH);
static void TCP_connect();
static void TCP_disconnect();
static void TCP_disconnect();
static void send_data();
static void recv_ACK(int times);

int main(){


    msock = passive_UDP("10220");

    while(1){
        init();
        TCP_connect();
        send_data();
        TCP_disconnect();

    }

    return 0;
}

static void init(){
    s_port    = 10220;
    RTT       = 200;
    threshold = 4096;
    MSS       = 512;

    printf("==========\n");
    printf("Set RTT = %d ms\n", RTT);
    printf("Set threshold = %d bytes\n", threshold);
    printf("Set MSS = %d bytes\n", MSS);
    printf("Buffer size = %d bytes\n", socket_buffer_size);
    printf("server's ip is 127.0.0.1\n");
    printf("Server listening on port 10220\n");
    printf("==========\n");
    srand(time(NULL));
    sequence_number = rand()%1024 + 1;
    ack_number = 0;
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
    struct sockaddr_in* client;

    printf("Listening......\n");

    //receive SYN
    recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
    client =(struct sockaddr_in*)&client_socket;
    host = inet_ntoa(client->sin_addr);
    d_port = ntohs(client->sin_port);
    printf("Received a SYN packet from %s : %d\n", host, d_port);
    memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
    show_TCP_Header(TCP_Header_receiver);
    ack_number = ++TCP_Header_receiver.sequence_number;

    //send SYN/ACK
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.ACK = 1;
    TCP_Header_sender.SYN = 1;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(msock, &socket_buffer, socket_buffer_size, 0, &client_socket, addrlen);
    printf("Send a SYN/ACK packet to %s : %d\n", host, d_port);

    //receive ACK
    recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
    printf("Received an ACK packet from %s : %d\n", host, d_port);
    memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
    rwnd = TCP_Header_receiver.window;
    show_TCP_Header(TCP_Header_receiver);
    ++ack_number;

    printf("Complete three way handshake\n");
    printf("==========\n");
}

static void TCP_disconnect(){

    struct TCP_Header_Format TCP_Header_sender, TCP_Header_receiver;

    //send FIN
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.FIN = 1;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(msock, &socket_buffer, socket_buffer_size, 0, &client_socket, addrlen);
    printf("Send a FIN packet to %s : %d\n", host, d_port);

    //receive ACK
    recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
    printf("Received an ACK packet from %s : %d\n", host, d_port);
    memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
    show_TCP_Header(TCP_Header_receiver);
    ++ack_number;

    //receive FIN
    recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
    printf("Received an FIN packet from %s : %d\n", host, d_port);
    memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
    show_TCP_Header(TCP_Header_receiver);
    ++ack_number;

    //send ACK
    TCP_Header_init(TCP_Header_sender);
    TCP_Header_sender.ACK = 1;
    memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
    sendto(msock, &socket_buffer, socket_buffer_size, 0, &client_socket, addrlen);
    printf("Send an ACK packet to %s : %d\n", host, d_port);

    printf("Complete four way handshake\n");
}

static void send_data(){

    int t_seqnumber, swnd, send_data_size;
    struct TCP_Header_Format TCP_Header_sender;

    cwnd = 1;
    t_seqnumber = send_data_size = 0;

    printf("Begin to send file, %d bytes\n", data_size);

    state = 0;
    //0 slow start
    //1 congestion avoid
    //2 fast recovery
    memset(&TCP_Header_sender, 0, TCP_Header_size);
    TCP_Header_sender.source_port   = s_port;
    TCP_Header_sender.dest_port     = d_port;

    while(send_data_size < data_size){

        //send data
        swnd = (cwnd < rwnd) ? cwnd : rwnd;
        swnd = (swnd > data_size-send_data_size) ? data_size-send_data_size : swnd;

        while(swnd > MSS){
            t_seqnumber = send_data_size+1;
            TCP_Header_sender.sequence_number = t_seqnumber;
            TCP_Header_sender.ack_number      = ack_number;
            printf("(Cwnd = %d, Rwnd = %d, threshold = %d) Send Packet at: %d byte\n", cwnd, rwnd, threshold, t_seqnumber);

            memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
            memcpy(socket_buffer+TCP_Header_size, data+send_data_size, MSS);
            sendto(msock, &socket_buffer, TCP_Header_size+MSS, 0, &client_socket, addrlen);
            send_data_size += MSS;
            swnd -= MSS;
        }

        t_seqnumber = send_data_size+1;
        TCP_Header_sender.sequence_number = t_seqnumber;
        TCP_Header_sender.ack_number      = ack_number;
        printf("(Cwnd = %d, Rwnd = %d, threshold = %d) Send Packet at: %d byte\n", cwnd, rwnd, threshold, t_seqnumber);

        memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
        memcpy(socket_buffer+TCP_Header_size, data+send_data_size, swnd);
        sendto(msock, &socket_buffer, TCP_Header_size+swnd, 0, &client_socket, addrlen);
        send_data_size += swnd;

        //receive ack
        recv_ACK(send_data_size+1);

    }
}

static void recv_ACK(int wait_ack_number){

    struct TCP_Header_Format TCP_Header_receiver, TCP_Header_sender;
    int rcv_ACK, duplicate_ack_times, send_size;

    rcv_ACK = duplicate_ack_times = 0;
    while(rcv_ACK != wait_ack_number){
        //receive ack
        recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
        memcpy(&TCP_Header_receiver, socket_buffer, TCP_Header_size);
        show_TCP_Header(TCP_Header_receiver);


        if(rcv_ACK == TCP_Header_receiver.ack_number){
            //duplicate ack
            duplicate_ack_times++;
            if(duplicate_ack_times >= 2){
                printf("Received three ACK with same number\n");
                printf("***Fast retransmit.***\n");
                printf(" ***Get into Fast Recovery.***\n");

                threshold = cwnd/2;
                cwnd = threshold + 3;
                duplicate_ack_times = 0;
                TCP_Header_sender.sequence_number = rcv_ACK;
                TCP_Header_sender.ack_number      = ack_number;
                printf("(Cwnd = %d, Rwnd = %d, threshold = %d) Send Packet at: %d byte\n", cwnd, rwnd, threshold, rcv_ACK);
                send_size = (MSS < rwnd) ? MSS : rwnd;
                memcpy(socket_buffer, &TCP_Header_sender, TCP_Header_size);
                memcpy(socket_buffer+TCP_Header_size, data+rcv_ACK-1, send_size);
                sendto(msock, &socket_buffer, TCP_Header_size+send_size, 0, &client_socket, addrlen);
            }
        }
        else{
            rcv_ACK = TCP_Header_receiver.ack_number;
            rwnd = TCP_Header_receiver.window;
            if(state == 0 && cwnd >= threshold){
                state = 1;
                printf("***Come into congestion avoid***\n");
            }
            cwnd = (state == 0) ? cwnd<<1 : cwnd+1;
        }

        ++ack_number;
    }

}
