#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <ctime>

#include "client_socket.h"
#define socket_buffer_size 1024
#define small_socket_buffer_size 128
#define defaultWaitseconds 3
#define defaultRetransmitTimes 5


using namespace std;

static unsigned char socket_buffer[socket_buffer_size+1];

int main(){

    string host, services, filename, want;
    int client_socket, rwsize, nfds, notack, retransmitTimes;
    FILE *in;
    fd_set rfds;
    struct timeval tv;
    unsigned char seqnumber, recvnumber;

    cout << "Input host     : "; cin >> host;
    cout << "Input services : "; cin >> services;

    client_socket = connect_udp(host.c_str(), services.c_str());

    nfds = getdtablesize();
    FD_ZERO(&rfds);

    while(1){

        cout << "Do you want upload file to server? (y/n) : "; cin >> want;
        if(want[0] != 'Y' && want[0] != 'y') break;

        cout << "Which file do you want upload to server: "; cin >> filename;

        in = fopen(filename.c_str(), "rb");
        if(in == NULL){
            cout << filename << "can't open, please check !!\n" << endl;
            break;
        }

        // send filename
        sendto(client_socket, ("2"+filename).c_str(), socket_buffer_size, 0, (const struct sockaddr *)0, 0);

        FD_SET(client_socket, &rfds);

        tv.tv_sec = defaultWaitseconds;
        tv.tv_usec = 0;
        if(select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv) < 0){
            printf("select error\n");
            exit(-1);
        }

        if(FD_ISSET(client_socket, &rfds)){
            recvfrom(client_socket, &recvnumber, sizeof(recvnumber), 0, (struct sockaddr *)0, (socklen_t *)0);
            cout << "recvnumber : " << recvnumber << endl;
        }
        else{
            cout << "server not accept" << endl;
            fclose(in);
            continue;
        }

        if(recvnumber != '0'){
            cout << "server not accept" << endl;
            fclose(in);
            continue;
        }

        //send filesize
        fseek(in, 0, SEEK_END);
        int length = (int)ftell(in);
        fclose(in);
        in = fopen(filename.c_str(), "rb");
        if(in == NULL){
            cout << filename << "can't open, please check !!\n" << endl;
            break;
        }

        char len[socket_buffer_size+1] = {0};
        sprintf(len, "%d", length);

        sendto(client_socket, len, socket_buffer_size, 0, (const struct sockaddr *)0, 0);

        FD_SET(client_socket, &rfds);

        tv.tv_sec = defaultWaitseconds;
        tv.tv_usec = 0;
        if(select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv) < 0){
            printf("select error\n");
            exit(-1);
        }

        if(FD_ISSET(client_socket, &rfds)){
            recvfrom(client_socket, &recvnumber, sizeof(recvnumber), 0, (struct sockaddr *)0, (socklen_t *)0);
            cout << "recvnumber : " << recvnumber << endl;
        }
        else{
            cout << "server not accept" << endl;
            break;
        }

        // start file transfer
        seqnumber = '0';
        while((rwsize = fread(socket_buffer+1, sizeof(unsigned char), socket_buffer_size-1, in)) > 0){
            cout << endl;
            socket_buffer[0] = seqnumber;
            sendto(client_socket, socket_buffer, rwsize+1, 0, (const struct sockaddr *)0, 0);
            notack = 1;
            retransmitTimes = 0;
            while(notack == 1){
                FD_SET(client_socket, &rfds);

                tv.tv_sec = defaultWaitseconds;
                tv.tv_usec = 0;
                if(select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv) < 0){
                    printf("select error\n");
                    exit(-1);
                }

                if(FD_ISSET(client_socket, &rfds)){
                    recvfrom(client_socket, &recvnumber, sizeof(recvnumber), 0, (struct sockaddr *)0, (socklen_t *)0);
                    cout << "recvive seqnumber : " << recvnumber << endl;
                    if(recvnumber == seqnumber){
                        seqnumber = (seqnumber == '0') ? '1' : '0';
                        notack = 0;
                        cout << "send correct" << endl;
                        break;
                    }
                }

                if(retransmitTimes >= defaultRetransmitTimes){
                    cout << "retransmit times is up to limit times." << endl;
                    break;
                }

                if(notack) {
                    cout << "send incorrect, start retransmit" << endl;
                    retransmitTimes++;
                    sendto(client_socket, socket_buffer, rwsize+1, 0, (const struct sockaddr *)0, 0);
                }
            }

            if(notack){
                cout << "network has someting wrong." << endl;
                break;
            }
        }

        fclose(in);
    }

    return 0;
}
