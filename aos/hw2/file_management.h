#include <string>
#include <map>
#include <ctime>
#include <pthread.h>

using namespace std; 

class file {
public:
	file(string file_name, string owner_name, string group_name, int owner_capability, int group_capability, int other_capability, unsigned long file_size);
	string file_name, owner_name, group_name;
	int owner_capability, group_capability, other_capability, isRead, isWrite;
	unsigned long file_size;
	time_t created_at;
	pthread_mutex_t f_mutex;
	string info();
};

class file_management {
public:
	file* insert_file(file f);
	file* getfile(string file_name);
	int del_file(string file_name);
private:
	map<string, file> files;	
};
