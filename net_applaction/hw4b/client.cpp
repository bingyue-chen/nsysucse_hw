#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstdio>

#include "client_socket.h"

#define socket_buffer_size 1024

using namespace std;

static unsigned char socket_buffer[socket_buffer_size+1];
static void action(int fd);

static int uchar_to_int(unsigned char* p){
    int n = 0;
    while(*p >= '0' && *p <= '9'){
        n = n*10 + *p - '0';
        p++;
    }
    return n;
}

int main(){

    string host, service, s;
    int socket;

    cout << "Input host address : "; cin >> host;
    cout << "Input service/port : "; cin >> service;

    socket = connect_tcp(host.c_str(), service.c_str());

    cout << "connect " << host << ":" << service << " success." << endl << endl;

    action(socket);
    close(socket);

    cout << "disconnect" << endl << endl;

    return 0;
}

static void action(int fd){

    string choice, filename;
    FILE* pfile;
    int rwsize;

    while(1){

        cout << endl << "(1)upload (2)download (3)exit : "; cin >> choice;
        if(choice[0] != '1' && choice[0] != '2' && choice[0] != '3'){
            cout << "Invaild choice!!" << endl;
            continue;
        }

        if(choice[0] == '3'){
            write(fd, &choice[0], sizeof(char));
            break;
        }

        cout << "File name : "; cin >> filename;


        if(choice[0] == '1'){

            if((pfile = fopen(filename.c_str(), "rb")) == NULL){
                cout << "file can't open" << endl;
                continue;
            }

            fseek(pfile, 0, SEEK_END);
            int length = (int)ftell(pfile);
            rewind(pfile);

            char len[socket_buffer_size+1] = {0};
            sprintf(len, "%d", length);

            write(fd, &choice[0], sizeof(char));
            write(fd, filename.c_str(), socket_buffer_size);

            char isUploadable;
            read(fd, &isUploadable, sizeof(char));

            write(fd, &len, socket_buffer_size);

            if(isUploadable == '1'){
                cout << "server can't create this file" << endl;
                fclose(pfile);
                continue;
            }

            while((rwsize = fread(socket_buffer, sizeof(unsigned char), socket_buffer_size, pfile)) > 0){
                write(fd, socket_buffer, rwsize);
            }

            fclose(pfile);

            cout << filename << " upload" << endl;
        }
        else{
            if((pfile = fopen(filename.c_str(), "wb")) == NULL){
                cout << "file can't create" << endl;
                continue;
            }

            write(fd, &choice[0], sizeof(char));
            write(fd, filename.c_str(), socket_buffer_size);

            char isDownloadable;
            read(fd, &isDownloadable, sizeof(char));

            if(isDownloadable == '1'){
                cout << "server can't find this file" << endl;
                fclose(pfile);
                continue;
            }

            read(fd, socket_buffer, socket_buffer_size);

            int filesize = uchar_to_int(socket_buffer);

            cout << "socket : " << socket_buffer << endl;
            cout << "filesize : " << filesize << endl;

            choice = '0';
            write(fd, &choice, sizeof(char));

            while(filesize > 0 && (rwsize = read(fd, socket_buffer, socket_buffer_size)) > 0){
                fwrite(socket_buffer, sizeof(unsigned char), rwsize, pfile);
                fflush(pfile);
                filesize -= rwsize;
            }

            fclose(pfile);

            cout << filename << " download" << endl;
        }
    }

}
