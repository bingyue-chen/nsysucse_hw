#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sstream>
#include <unistd.h>

#include "server_socket.h"


#define QLEN 32
#define socket_buffer_size 1024

using namespace std;

static unsigned char socket_buffer[socket_buffer_size+1];
static void action(int fd);

static string uchar_to_string(unsigned char* p){
    string s="";
    while(*p){
        s += *p;
        p++;
    }
    return s;
}

static int uchar_to_int(unsigned char* p){
    int n = 0;
    while(*p >= '0' && *p <= '9'){
        n = n*10 + *p - '0';
        p++;
    }
    return n;
}

int main(){

    struct sockaddr_in fsin;
    int msock, ssock;
    unsigned int alen = sizeof(fsin);
    char ip[INET_ADDRSTRLEN];
    pid_t pid;


    msock = passive_TCP("8080", QLEN);

    while(1){
        ssock = accept(msock, (struct sockaddr*)&fsin, &alen);
        if(ssock < 0){
            cout << "accept failed." << endl;
            exit(5);
        }

        pid = fork();

        if(pid == 0){
            action(ssock);
            close(ssock);
        }
        else if(pid < 0){
            printf("error fork\n");
            exit(1);
        }

    }

    return 0;
}

static void action(int fd){

    char choice;
    FILE* pfile;
    string filename;
    int rwsize;

    while(1){

        read(fd, &choice, sizeof(char));

        //client exit
        if(choice == '3'){
            break;
        }

        rwsize = read(fd, socket_buffer, socket_buffer_size);
        socket_buffer[rwsize] = '\0';
        filename = uchar_to_string(socket_buffer);

        //client upload
        if(choice == '1'){

            if((pfile = fopen(filename.c_str(), "wb")) == NULL){
                choice = '1';
                write(fd, &choice, sizeof(char));
                continue;
            }

            choice = '0';
            write(fd, &choice, sizeof(char));

            read(fd, socket_buffer, socket_buffer_size);

            int filesize = uchar_to_int(socket_buffer);

            while(filesize > 0 && (rwsize = read(fd, socket_buffer, socket_buffer_size)) > 0){
                fwrite(socket_buffer, sizeof(unsigned char), rwsize, pfile);
                fflush(pfile);
                filesize -= rwsize;
            }

            fclose(pfile);

            cout << filename << " download from client" << endl;
        }
        else{

            if((pfile = fopen(filename.c_str(), "rb")) == NULL){
                choice = '1';
                write(fd, &choice, sizeof(char));
                continue;
            }

            fseek(pfile, 0, SEEK_END);
            int length = (int)ftell(pfile);
            rewind(pfile);

            char len[20] = {0};
            sprintf(len, "%d", length);

            choice = '0';
            write(fd, &choice, sizeof(char));

            write(fd, &len, socket_buffer_size);

            read(fd, &choice, sizeof(char));

            while((rwsize = fread(socket_buffer, sizeof(unsigned char), socket_buffer_size, pfile)) > 0){
                write(fd, socket_buffer, rwsize);
            }

            fclose(pfile);

            cout << filename << " upload to client" << endl;

        }
    }
}
