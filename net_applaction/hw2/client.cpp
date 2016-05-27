#include <unistd.h>
#include <iostream>
#include <string>
#include <sstream>

#include "client_socket.h"
#include "huffmancode.h"

#define socket_buffer_size 1048576

using namespace std;

static unsigned char socket_buffer[socket_buffer_size+1];
static void action(int fd);

static string int_to_string(int n){
    string s="";
    stringstream ss;

    ss << n;
    ss >> s;

    return s;
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

    string origin_file_path, compress_file_path, file_name, file_size;
    unsigned char choice;
    int compress_length, n;
    size_t found;
    FILE * pFile;

    while(1){
        cout << "do you want to upload file? [y/n] : "; cin >> choice;
        if(choice != 'y' && choice != 'Y') break;

        cout << "input file path : "; cin >> origin_file_path;
        cout << "choice file compress 1.fixed huffman 2.variable huffman 3.none : "; cin >> choice;
        cout <<  endl;

        if(choice == '1'){
            huffmancompression *hc = new huffmancompression();
            hc->compression(origin_file_path, true);
            compress_file_path = hc->get_compression_file_path();
            file_name = hc->get_compression_file_name();
            compress_length = hc->get_compress_length();
            delete hc;
        }
        else if(choice == '2'){
            huffmancompression *hc = new huffmancompression();
            hc->compression(origin_file_path);
            compress_file_path = hc->get_compression_file_path();
            file_name = hc->get_compression_file_name();
            compress_length = hc->get_compress_length();
            delete hc;
        }
        else{
            pFile = fopen(origin_file_path.c_str(), "rb");
            if (pFile == NULL){
                cout << "Error opening file" << endl;
                exit(-1);
            }

            choice = '3';
            compress_file_path = origin_file_path;
            found = origin_file_path.find_last_of("\\");
            if(found != string::npos)
                file_name = origin_file_path.substr(found + 1);
            else
                file_name = origin_file_path;
            fseek (pFile, 0, SEEK_END);
            compress_length = (int)ftell(pFile);
            fclose (pFile);
        }

        file_size = int_to_string(compress_length) + '\0';
        file_name += '\0';

        write(fd, &choice, sizeof(choice));
        write(fd, file_name.c_str(), sizeof(char)*50);
        write(fd, file_size.c_str(), sizeof(char)*15);

        cout << "file start transfer." << endl;

        pFile = fopen (compress_file_path.c_str(), "rb");
        if (pFile==NULL){
            cout << "Error opening file" << endl;
            exit(-1);
        };

        while((n = fread(socket_buffer, sizeof(unsigned char), socket_buffer_size, pFile)) > 0){
            write(fd, socket_buffer, n);
            fsync(fd);
        }

        fclose (pFile);

        if((n=read(fd, socket_buffer, socket_buffer_size)) <= 0){
            cout << "upload file failed." << endl << endl;
            return;
        }

        cout << "upload file success." << endl << endl;
    }

}
