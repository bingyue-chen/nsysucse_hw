#ifndef _MYTAR_H_
#define _MYTAR_H_

#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

class Mytar {
private:

    struct TarHeader {
        char filename[100];
        char filemode[8];
        char userid[8];
        char groupid[8];
        char filesize[12];
        char mtime[12];
        char checksum[8];
        char type;
        char lname[100];
        /* USTAR Section */
        char USTAR_id[6];
        char USTAR_ver[2];
        char username[32];
        char groupname[32];
        char devmajor[8];
        char devminor[8];
        char prefix[155];
        char pad[12];
    };

    ifstream file;
    vector<TarHeader> th_collector;

    void readTarHeader(TarHeader *th);
    bool isEnd(TarHeader *th);
    bool isUSTar(TarHeader *th);
    bool isOK(TarHeader *th);
    unsigned long long getFilesize(TarHeader *th);
    struct tm* getTime(TarHeader *th);
    void move_to_next_header(TarHeader *th);
    int cal_ullong_len(unsigned long long x);
    void get_width(int *uname_width, int *gname_width, int *filesize_width);
    void displayAllTarHeader();
    void combineDev(TarHeader *th, char * buffer, size_t buffer_size);
    void displayTarHeader(TarHeader *th, int uname_width = 4, int gname_width = 4, int filesize_width = 4);

public:

    Mytar(){ this->th_collector.clear(); };

    ~Mytar(){ if(this->file.is_open()) this->file.close(); };
    
    void process(char *filename);
};

#endif
