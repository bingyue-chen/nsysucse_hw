#include <unistd.h>
#include <iostream>
#include <arpa/inet.h>
#include <sstream>

#include "server_socket.h"
#include "huffmancode.h"

#define QLEN 32
#define socket_buffer_size 1048576

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


    msock = passive_TCP("8080", QLEN);

    while(1){
        ssock = accept(msock, (struct sockaddr*)&fsin, &alen);
        if(ssock < 0){
            cout << "accept failed." << endl;
            exit(5);
        }

        cout << "user connect" << endl;
        cout << "user ip        : " << inet_ntop(AF_INET, &fsin.sin_addr, ip, INET_ADDRSTRLEN) << endl;
        cout << "user port      : " << ntohs(fsin.sin_port) << endl;
        //cout << "user transport : " <<
        cout << endl;

        action(ssock);
        close(ssock);

        cout << "user disconnect" << endl << endl;
    }

    return 0;
}

static void action(int fd){

    int n, file_size;
    string file_name;
    unsigned char choice;
    FILE* pfile;

    while(1){

        if((n = read(fd, socket_buffer, sizeof(unsigned char))) <= 0) return;
        choice = socket_buffer[0];
        //cout << "choice : " << choice << endl;

        if((n = read(fd, socket_buffer, sizeof(char)*50)) <= 0) return;
        socket_buffer[n] = 0;
        file_name = uchar_to_string(socket_buffer);
        //cout << "file_name : " << file_name << endl;

        if((n = read(fd, socket_buffer, sizeof(char)*15)) <= 0) return;
        socket_buffer[n] = 0;
        file_size = uchar_to_int(socket_buffer);
        //cout << "file_size : " << file_size << endl;

        cout << "file start receive." << endl;

        if((pfile = fopen(file_name.c_str(), "wb")) == NULL){
            cout << "open " << file_name << " failed." << endl;
            return;
        };

        while(file_size > 0 && (n = read(fd, socket_buffer, socket_buffer_size)) > 0){
            fwrite(socket_buffer, sizeof(unsigned char), n, pfile);
            fflush(pfile);
            file_size -= n;
        }

        fclose(pfile);

        if(choice == '1' || choice == '2'){
            huffmandecompression *hdc = new huffmandecompression();
            hdc->decompression(file_name);
            delete hdc;
        }

        cout << "receive file success." << endl << endl;

        write(fd, "ok", sizeof(char)*2);

    }

    return;
}
