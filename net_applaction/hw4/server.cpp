#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>

#include "server_socket.h"
#define socket_buffer_size 1024
#define small_socket_buffer_size 128

#ifndef netThreshold
#define netThreshold 0.1
#endif // netThreshold

#ifndef defaultWaitseconds
#define defaultWaitseconds 5
#endif // defaultWaitseconds

using namespace std;

static unsigned char socket_buffer[socket_buffer_size+1];

int main(){

    struct sockaddr_in fsin;
    int msock, n, recvfile, nfds, filesize, hasfilename;
    float p;
    FILE *out;
    struct sockaddr client_socket;
    socklen_t addrlen = sizeof(client_socket);
    unsigned char seqnumber;
    fd_set rfds;
    struct timeval tv;
    char filename[socket_buffer_size+1] = {0};
    unsigned char c;

    nfds = getdtablesize();
    FD_ZERO(&rfds);

    msock = passive_UDP("8080");

    srand (time(NULL));
    cout << "network threshold : " << netThreshold << endl;

    hasfilename = 0;
    while(1){

        cout << endl;

        //recv filename
        if(hasfilename == 0){
            n = recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
        }
        socket_buffer[n] = '\0';

        n = strlen((char *)socket_buffer);
        strncpy(filename, (char *)socket_buffer+1, n-1);
        filename[n-1] ='\0';
        cout << "file name : " << filename << endl;
        hasfilename = 0;

        if((out = fopen(filename, "wb")) == NULL){
            cout << "open file error" << endl;
            c = '1';
            sendto(msock, &c, sizeof(c), 0, &client_socket, addrlen);
            continue;
        }
        c = '0';
        sendto(msock, &c, sizeof(c), 0, &client_socket, addrlen);

        //recv filesize
        FD_SET(msock, &rfds);

        tv.tv_sec = defaultWaitseconds;
        tv.tv_usec = 0;
        if(select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv) < 0){
            printf("select error\n");
            exit(-1);
        }

        if(!FD_ISSET(msock, &rfds)){
            fclose(out);
            cout << "client network someting wrong!!" << endl;
            remove(filename);
            continue;
        }

        n = recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
        socket_buffer[n] = '\0';

        sscanf((char *)socket_buffer,"%d", &filesize);
        cout << "file size : " << filesize << endl;
        c = '0';
        sendto(msock, &c, sizeof(c), 0, &client_socket, addrlen);

        //recv file
        seqnumber ='0';
        while(1){

            FD_SET(msock, &rfds);

            tv.tv_sec = defaultWaitseconds;
            tv.tv_usec = 0;
            if(select(nfds, &rfds, (fd_set *)0, (fd_set *)0, &tv) < 0){
                printf("select error\n");
                exit(-1);
            }

            if(!FD_ISSET(msock, &rfds)){
                break;
            }

            n = recvfrom(msock, socket_buffer, socket_buffer_size, 0, &client_socket, &addrlen);
            socket_buffer[n] = '\0';

            cout << endl;
            p = (rand() % 100) / 100.0;
            cout << "p : " << p << endl;

            if(p >= netThreshold){

                cout << "seq number : " << socket_buffer[0] << endl;

                if(socket_buffer[0] == '2'){
                    hasfilename = 1;
                    break;
                }
                else if(socket_buffer[0] != seqnumber){
                    cout << "duplicate file track" << endl;
                    c = socket_buffer[0];

                    p = (rand() % 100) / 100.0;
                    cout << "p : " << p << endl;

                    if(p >= netThreshold){
                        cout << "send seq number : " << c << endl;
                        sendto(msock, &c, sizeof(c), 0, &client_socket, addrlen);
                    }
                    else{
                        cout << "lost seq number : " << c << endl;
                    }
                }
                else{
                    cout << "file accept" << endl;
                    fwrite(socket_buffer+1, sizeof(unsigned char), n-1, out);
                    fflush(out);
                    filesize -= (n-1);

                    p = (rand() % 100) / 100.0;
                    cout << "p : " << p << endl;

                    if(p >= netThreshold){
                        cout << "send seq number : " << seqnumber << endl;
                        sendto(msock, &seqnumber, sizeof(seqnumber), 0, &client_socket, addrlen);
                    }
                    else{
                        cout << "lost seq number : " << seqnumber << endl;
                    }
                    seqnumber = (seqnumber == '0') ? '1' : '0';
                }
            }
            else {
               cout << "lost file" << endl;
            }
        }

        fclose(out);
        if(filesize > 0){
            cout << "client network someting wrong!!" << endl;
            remove(filename);
        }
        else
            cout << "recvive file " << filename << " finish." << endl;
    }

    return 0;
}
