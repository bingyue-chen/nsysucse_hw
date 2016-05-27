#include "file_management.h"

file::file(string file_name, string owner_name, string group_name, int owner_capability, int group_capability, int other_capability, unsigned long file_size){
	this->file_name  = file_name;
	this->owner_name = owner_name;
	this->group_name = group_name;

	this->owner_capability = owner_capability;
	this->group_capability = group_capability;
	this->other_capability = other_capability;

	this->file_size  = file_size;
	time(&this->created_at);

	pthread_mutex_init(&this->f_mutex, NULL);

	this->isRead  = 0;
	this->isWrite = 1;

}

string file::info(){

	string s = "-";

	s += (this->owner_capability & 4) ? "r" : "-";
	s += (this->owner_capability & 2) ? "w" : "-";
	s += (this->owner_capability & 1) ? "x" : "-";

	s += (this->group_capability & 4) ? "r" : "-";
	s += (this->group_capability & 2) ? "w" : "-";
	s += (this->group_capability & 1) ? "x" : "-";

	s += (this->other_capability & 4) ? "r" : "-";
	s += (this->other_capability & 2) ? "w" : "-";
	s += (this->other_capability & 1) ? "x" : "-";

	s += "\t";
	s += this->owner_name;

	s += "\t";
	s += this->group_name;

	s += "\t";
	s += this->file_size;

	struct tm * timeinfo;
	char timebuf[50];
	timeinfo = localtime (&this->created_at);
	strftime(timebuf, 50, "%b %d %Y", timeinfo);

	string timestring(timebuf);
	s += timestring;

	s += "\t";
	s += this->file_name;
	s += "\n";

	return s;
}

file* file_management::insert_file(file f){

	map<string, file>::iterator it = this->files.find(f.file_name); 

	if(it == this->files.end()){
		this->files.insert(pair<string, file>(f.file_name, f));
		it = this->files.find(f.file_name);
	}

	return &it->second;
};


file* file_management::getfile(string file_name){

	map<string, file>::iterator it = this->files.find(file_name); 

	if(it == this->files.end()){
		return NULL;
	}
	else{
		return &it->second;
	}
};

int file_management::del_file(string file_name){

	map<string, file>::iterator it = this->files.find(file_name); 

	if(it == this->files.end()){
		return 404;
	}
	else{
		this->files.erase(it);
		return 200;
	}

	return 404;
};
