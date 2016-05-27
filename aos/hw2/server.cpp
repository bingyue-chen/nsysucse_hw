#include <iostream>
#include <string>
#include <map>
#include <ctime>
#include <pthread.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <fcntl.h>

#include "server_socket.h"
#include "user_management.h"
#include "file_management.h"

#define QLEN 32
#define SOCKET_BUFFER_SIZE 10240

using namespace std;

static user_management um;
static file_management fm;
static pthread_mutex_t um_mutex;
static pthread_mutex_t fm_mutex;

void* run(void* argv);
static int  receive_login(int fd, char* socket_buffer, user** u);
static void do_command(int fd, char* socket_buffer, user* u);
static void do_new(int fd, char* socket_buffer, user* u, string filename, string option);
static int  do_check(int fd, char* socket_buffer, user* u, string filename, file** fp, int action);
static void do_read(int fd, char* socket_buffer, user* u, string filename);
static void do_write(int fd, char* socket_buffer, user* u, string filename, string option);
static void do_change(int fd, char* socket_buffer, user* u, string filename, string option);
static void do_info(int fd, char* socket_buffer, user* u, string filename);

int main(int argc, char const *argv[]){
    
    struct sockaddr_in fsin;
    int msock, ssock;
    unsigned int alen = sizeof(fsin);

    msock = passive_TCP("8080", QLEN);
    pthread_mutex_init(&um_mutex, NULL);
    pthread_mutex_init(&fm_mutex, NULL);

    while(1){
        ssock = accept(msock, (struct sockaddr*)&fsin, &alen);
        pthread_t p;
        pthread_create(&p, NULL, &run, &ssock);
    }

    return 0;
}

void *run(void* argv){
    int fd = *((int *)argv);
    int res_code;
    user *u = NULL;

    char socket_buffer[SOCKET_BUFFER_SIZE+1];

    //receive login info [name pwd]
    res_code = receive_login(fd, socket_buffer, &u);
    if(res_code == 404){
        close(fd);
        return NULL;
    }

    cout << u->name << " : login" << endl;

    //do command actoin
    do_command(fd, socket_buffer, u);
    
    cout << u->name << " : logout" << endl;

    close(fd);
    return NULL;

}

int receive_login(int fd, char* socket_buffer, user** u){

    int receive_size, res_code;
    string name, pwd;
    stringstream ss;

    receive_size = read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
    socket_buffer[receive_size] = '\0';

    name = pwd = "";
    ss << socket_buffer;
    ss >> name >> pwd;

    if((*u = um.login(name, pwd)) == NULL){
        res_code = 404;
    }
    else{
        res_code = 200;
    };

    sprintf(&socket_buffer[0], "%d", res_code);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

    return res_code;
};

void do_command(int fd, char* socket_buffer, user* u){

    int receive_size;
    string command, filename, option;

    while(1){
        receive_size = read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        socket_buffer[SOCKET_BUFFER_SIZE+1] = '\0';

        command = filename = option = "";
        stringstream ss;
        ss << socket_buffer;
        ss >> command >> filename >> option;

        if(command == "new" && filename != "" && option != ""){
            cout << u->name << " : " << socket_buffer << endl;
            do_new(fd, socket_buffer, u, filename, option);
        }
        else if(command == "read" && filename != ""){
            cout << u->name << " : " << socket_buffer << endl;
            do_read(fd, socket_buffer, u, filename);
        }
        else if(command == "write" && filename != "" && (option[0] == 'a' || option[0] == 'o')){
            cout << u->name << " : " << socket_buffer << endl;
            do_write(fd, socket_buffer, u, filename, option);
        }
        else if(command == "change" && filename != "" && option != ""){
            cout << u->name << " : " << socket_buffer << endl;
            do_change(fd, socket_buffer, u, filename, option);
        }
        else if(command == "information" && filename != ""){
            cout << u->name << " : " << socket_buffer << endl;
            do_info(fd, socket_buffer, u, filename);
        }
        else if(command == "exit"){
            sprintf(&socket_buffer[0], "%d", 200);
            write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
            return;
        }
        else{
            sprintf(&socket_buffer[0], "%d", 400);
            write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        }

    }

}

void do_new(int fd, char* socket_buffer, user* u, string filename, string option){

    file* fp;
    int receive_size;
    int capability[3] = {0};
    unsigned long file_size;
    stringstream ss;

    for(int i = 0; i < option.length()/2 && i < 3; i++){
        if(option[i<<1]   == 'r') capability[i] += 4;
        if(option[(i<<1)+1] == 'w') capability[i] += 2;
    }

    pthread_mutex_lock(&fm_mutex);

    if((fp = fm.getfile(filename)) != NULL){
        pthread_mutex_unlock(&fm_mutex);
        sprintf(&socket_buffer[0], "%d", 403);
        write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        return;
    }

    file f(filename, u->name, u->groupname, capability[0], capability[1], capability[2], 0);

    fp = fm.insert_file(f);

    pthread_mutex_unlock(&fm_mutex);

    sprintf(&socket_buffer[0], "%d", 200);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

    receive_size = read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
    socket_buffer[receive_size+1] = '\0';

    ss << socket_buffer;
    ss >> file_size;

    int filed = open(filename.c_str(), O_RDWR | O_CREAT, 0666);
    while(file_size){
        receive_size = read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        write(filed, socket_buffer, receive_size);
        file_size -= receive_size;
        fp->file_size += receive_size;
    }
    close(filed);

    pthread_mutex_lock(&um_mutex);

    string s = um.update_capability(f.file_name, u->name, u->groupname, capability[0], capability[1], capability[2]);

    pthread_mutex_unlock(&um_mutex);

    pthread_mutex_lock(&fp->f_mutex);

    fp->isWrite = 0;

    pthread_mutex_unlock(&fp->f_mutex);

    cout << s << endl;

    sprintf(&socket_buffer[0], "%d", 200);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
}

int do_check(int fd, char* socket_buffer, user* u, string filename, file** fp, int action){
    int res_code;

    pthread_mutex_lock(&fm_mutex);

    *fp = fm.getfile(filename);
    if(*fp != NULL)
        pthread_mutex_lock(&(*fp)->f_mutex);

    pthread_mutex_unlock(&fm_mutex);

    if(*fp == NULL){
        sprintf(&socket_buffer[0], "%d", 404);
        write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        return 404;
    }

    pthread_mutex_lock(&um_mutex);

    res_code =  u->check_capability(filename, action);

    pthread_mutex_unlock(&um_mutex);

    if(res_code != 200){
        pthread_mutex_unlock(&(*fp)->f_mutex);
        sprintf(&socket_buffer[0], "%d", 401);
        write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        return 401;
    }

    if((*fp)->isWrite || (action == 2 && (*fp)->isRead)){
        res_code = 403;
    }
    else{
        res_code = 200;
        if(action == 4)
            (*fp)->isRead++;
        else if(action == 2)
            (*fp)->isWrite++;
    }

    pthread_mutex_unlock(&(*fp)->f_mutex);
    sprintf(&socket_buffer[0], "%d", res_code);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

    return res_code;
}

void do_read(int fd, char* socket_buffer, user* u, string filename){

    file* fp;
    int res_code = do_check(fd, socket_buffer, u, filename, &fp, 4);

    if(res_code != 200) return;
    
    sprintf(&socket_buffer[0], "%lu", fp->file_size);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

    int filed, read_size; 
    filed = open(fp->file_name.c_str(), O_RDWR, 0666);

    while((read_size = read(filed, socket_buffer, SOCKET_BUFFER_SIZE))){
        write(fd, socket_buffer, read_size);
    }

    close(filed);

    pthread_mutex_lock(&fp->f_mutex);

    fp->isRead--;

    pthread_mutex_unlock(&fp->f_mutex);

    sprintf(&socket_buffer[0], "%d", 200);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

};

void do_write(int fd, char* socket_buffer, user* u, string filename, string option){

    file* fp;
    int res_code = do_check(fd, socket_buffer, u, filename, &fp, 2);

    if(res_code != 200) return;

    int filed, receive_size;
    unsigned long file_size;

    read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
    sscanf(&socket_buffer[0], "%lu", &file_size);

    if(option[0] == 'a'){
        filed = open(fp->file_name.c_str(), O_RDWR | O_APPEND, 0666);
    }
    else{
        filed = open(fp->file_name.c_str(), O_RDWR, 0666);
        fp->file_size = 0;
    }

    while(file_size){
        receive_size = read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
        write(filed, socket_buffer, receive_size);
        file_size -= receive_size;
        fp->file_size += receive_size;
    }

    close(filed);

    pthread_mutex_lock(&fp->f_mutex);

    fp->isWrite--;

    pthread_mutex_unlock(&fp->f_mutex);

    sprintf(&socket_buffer[0], "%d", 200);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
}

void do_change(int fd, char* socket_buffer, user* u, string filename, string option){
    
    file* fp;
    int res_code = do_check(fd, socket_buffer, u, filename, &fp, 2);

    if(res_code != 200) return;

    int capability[3] = {0};

    for(int i = 0; i < option.length()/2 && i < 3; i++){
        capability[i] = 0;
        if(option[i<<1]   == 'r') capability[i] += 4;
        if(option[(i<<1)+1] == 'w') capability[i] += 2;
    }

    fp->owner_capability = capability[0];
    fp->group_capability = capability[1];
    fp->other_capability = capability[2];

    pthread_mutex_lock(&um_mutex);

    string s = um.update_capability(filename, u->name, u->groupname, capability[0], capability[1], capability[2]);

    pthread_mutex_unlock(&um_mutex);

    pthread_mutex_lock(&fp->f_mutex);

    fp->isWrite--;

    pthread_mutex_unlock(&fp->f_mutex);

    sprintf(&socket_buffer[0], "%d", 200);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

    cout << s << endl;
}

void do_info(int fd, char* socket_buffer, user* u, string filename){
    
    file* fp;
    int res_code = do_check(fd, socket_buffer, u, filename, &fp, 4);

    if(res_code != 200) return;

    sprintf(&socket_buffer[0], "%s", fp->info().c_str());
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

    pthread_mutex_lock(&fp->f_mutex);

    fp->isRead--;

    pthread_mutex_unlock(&fp->f_mutex);

    sprintf(&socket_buffer[0], "%d", 200);
    write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

}
