#include "mytar.h"


#include <cstring>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cerrno>

void Mytar::process(char *filename){
	
	this->file.open(filename);
	TarHeader th;
	
	while(1){
		this->readTarHeader(&th);
		if(!this->isOK(&th)) break;
		this->th_collector.push_back(th);
		this->move_to_next_header(&th);
	}

	if(this->file.fail()){ 
		cout << strerror(errno) << endl;
	}
	else if(!isEnd(&th) && !this->isUSTar(&th)){
		cout << "Not a ustar file." << endl;
		cout << "File opening error." << endl;
	}
	else{
		this->displayAllTarHeader();
	}
	
	this->th_collector.clear();
	if(this->file.is_open()) this->file.close();

}

void Mytar::readTarHeader(TarHeader *th){
	this->file.read((char *)th, sizeof(TarHeader));
}

bool Mytar::isEnd(TarHeader *th){
	TarHeader empty_th = {{0}};
	return (memcmp(&empty_th, th, sizeof(TarHeader)) == 0) ? true : false;
};

bool Mytar::isUSTar(TarHeader *th){
	return (strncmp("ustar", th->USTAR_id, 5) == 0);
}

bool Mytar::isOK(TarHeader *th){
	return !(this->file.eof() || this->file.fail() || isEnd(th) || !this->isUSTar(th));
}

unsigned long long Mytar::getFilesize(TarHeader *th){

	unsigned long long filesize = 0;
	unsigned long long base = 1;
	
	for(int i = sizeof(th->filesize) - 2; i >=0 ; i--, base *= 8){
		int x = th->filesize[i] - '0';
		filesize += x * base;
	}

	return filesize;
}

struct tm* Mytar::getTime(TarHeader *th){
	
	time_t t = 0;
	unsigned long long base = 1;

	for(int i = sizeof(th->mtime) - 2; i >=0 ; i--, base *= 8){
		int x = th->mtime[i] - '0';
		t += x * base;
	}

	return localtime(&t);
}

void Mytar::move_to_next_header(TarHeader *th){
	unsigned long long filesize = this->getFilesize(th);
	this->file.seekg((filesize/512 + (filesize%512 != 0)) * 512, ios::cur);
};

int Mytar::cal_ullong_len(unsigned long long x){
	
	int i = 0;

	while(x/10 > 0){
		i++;
		x = x%10;
	}

	return i + 1;
}

void Mytar::get_width(int *uname_width, int *gname_width, int *filesize_width){
	
	*uname_width = *gname_width = *filesize_width = 4;
	int username_len, groupname_len, filesize_len;

	for(vector<TarHeader>::iterator it = this->th_collector.begin(); it != this->th_collector.end(); ++it ){
		username_len  = strlen(it->username);
		groupname_len = strlen(it->groupname);
		filesize_len  = this->cal_ullong_len(this->getFilesize(&(*it)));
		*uname_width    = max(*uname_width,    (username_len/4  + (username_len%4  != 0)) * 4);
		*gname_width    = max(*gname_width,    (groupname_len/4 + (groupname_len%4 != 0)) * 4);
		*filesize_width = max(*filesize_width, (filesize_len/4  + (filesize_len%4  != 0)) * 4);
	}

}

void Mytar::displayAllTarHeader(){

	int uname_width, gname_width , filesize_width;
	
	this->get_width(&uname_width, &gname_width , &filesize_width);
	
	for(vector<TarHeader>::iterator it = this->th_collector.begin(); it != this->th_collector.end(); ++it ){
		this->displayTarHeader(&(*it), uname_width, gname_width, filesize_width);
	}

}

void Mytar::combineDev(TarHeader *th, char *buffer, size_t buffer_size){

	buffer[buffer_size-1] = '\0';

	unsigned int i, j;
	bool flag;

	for(i = j = 0, flag = false; j < (sizeof(th->devmajor) - 2) && i < buffer_size-1; j++){
		if(th->devmajor[j] > '0' && th->devmajor[j] <= '9'){
			buffer[i++] = th->devmajor[j];
			flag = true;
		}
		else if(th->devmajor[j] == '0' && (flag || (!flag && j == 5)))
			buffer[i++] = '0';
	}

	if(i < buffer_size-1) buffer[i++] = ',';

	for(j = 0, flag = false; j < (sizeof(th->devminor) - 2) && i < buffer_size-1; j++){
		if(th->devminor[j] > '0' && th->devminor[j] <= '9'){
			buffer[i++] = th->devminor[j];
			flag = true;
		}
		else if(th->devminor[j] == '0' && (flag || (!flag && j == 5)))
			buffer[i++] = '0';
	}

};

void Mytar::displayTarHeader(TarHeader *th, int uname_width, int gname_width, int filesize_width){

	switch(th->type){
		case '0': cout << '-'; break;
		case '1': cout << 'h'; break;
		case '2': cout << 'l'; break;
		case '3': cout << 'c'; break;
		case '4': cout << 'b'; break;
		case '5': cout << 'd'; break;
		case '6': cout << 'p'; break;
		default : cout << '-';
	}

	int i = sizeof(th->filemode);
	while(i>=3 && (th->filemode[--i] < '0' || th->filemode[i] > '9'));
	for(int j = i-2; j <= i; j++){
		int x = th->filemode[j] - '0';
		if((x & 4) > 0) cout << 'r'; else cout << '-';
		if((x & 2) > 0) cout << 'w'; else cout << '-';
		if((x & 1) > 0) cout << 'x'; else cout << '-';
	}

	cout << " " << left  << setw(uname_width) << th->username;
	cout << " " << left  << setw(gname_width) << th->groupname;

	if(th->type == '3' || th->type == '4'){
		char dev[6] = {0};
		this->combineDev(th, dev, 6);
		cout << " " << right << setw(filesize_width) << dev;
	}
	else
		cout << " " << right << setw(filesize_width) << this->getFilesize(th);

	char mtime[12] = {0};
	strftime (mtime, 15, "%m %d  %Y", this->getTime(th));
	cout << " " << left  << setw(sizeof(mtime)) << mtime;
	
	cout << " " << th->filename;

	if(th->type == '1')
		cout << " link to " << th->lname; 
	if(th->type == '2')
		cout << " -> " << th->lname;

	cout << endl;
}
