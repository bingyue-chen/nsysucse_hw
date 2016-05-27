#include <iostream>
#include <unistd.h>
#include <sstream>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "client_socket.h"

#define SOCKET_BUFFER_SIZE 10240

using namespace std;

static char socket_buffer[SOCKET_BUFFER_SIZE+1];

static int dologin(int fd, string username, string password);
static void do_command(int fd);
static void do_new(int fd, string filename);

int main(int argc, char const *argv[]){
	
	int fd, res_code;
	string username, password, info;

	fd = connect_tcp("localhost", "8080");

	//do login
	cout << "username: "; cin >> username;
	cout << "password: "; cin >> password;
	res_code = dologin(fd, username, password);

	if(res_code != 200){
		cout << "Invaild username or password." << endl;
		close(fd);
		return 0;
	}
	
	cout << "success login" << endl;
	
	do_command(fd);

	close(fd);
	return 0;
}

int dologin(int fd, string username, string password){

	int res_code;

	sprintf(&socket_buffer[0], "%s %s", username.c_str(), password.c_str());
	write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
	read(fd, socket_buffer, SOCKET_BUFFER_SIZE);

	sscanf(&socket_buffer[0], "%d", &res_code);

	return res_code;
};

void do_command(int fd){

	string in_command, command, filename, option;
	int res_code, filed, read_size;

	getline(cin, in_command);
	while(1){
		command = filename = option = "";
		memset(socket_buffer, 0, SOCKET_BUFFER_SIZE);
		cout << "command: "; getline(cin, in_command);
		stringstream ss;
		ss << in_command; ss >> command >> filename >> option;

		//cout << command << "," << filename << "," << option << endl;

		if(command == "new" || command == "write"){
			if(access(filename.c_str(), R_OK) == -1){
				cout << filename << " not exist or read permission denied." << endl;
				continue;
			}
		}
		else if(command == "read"){
			if(access(filename.c_str(), F_OK) != -1){
				string isOverwrite;
				cout << filename << " exist in local. Overwrite? [y/n] :"; getline(cin, isOverwrite);

				if(isOverwrite[0] != 'y' && isOverwrite[0] != 'Y')
					continue;

				if(access(filename.c_str(), W_OK) == -1){
					cout << filename << " write permission denied." << endl;
					continue;
				}
			}
		}

		sprintf(&socket_buffer[0], "%s", in_command.c_str());
		write(fd, socket_buffer, SOCKET_BUFFER_SIZE);
		read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
		sscanf(&socket_buffer[0], "%d", &res_code);

		if(res_code == 400){
			cout << "bad command" << endl;
			continue;
		}

		if(command == "new"){
			if(res_code == 403){
				cout << "file exist in the server." << endl;
			}
			else{
				struct stat st;
				stat(filename.c_str(), &st);
				sprintf(&socket_buffer[0], "%lld", st.st_size);
				write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

				int filed = open(filename.c_str(), O_RDWR, 0666);
				while((read_size = read(filed, socket_buffer, SOCKET_BUFFER_SIZE))){
					write(fd, socket_buffer, read_size);
				}

				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				sscanf(&socket_buffer[0], "%d", &res_code);

				cout << command << " " << filename;

				if(res_code == 200){
					cout << " success" << endl;
				}
				else{
					cout << " failed" << endl;
				}
			}
		}
		else if(command == "read"){
			if(res_code == 404){
				cout << filename << " not exist in the server" << endl;
			}
			else if(res_code == 401){
				cout << "permission denied from server" << endl;
			}
			else if(res_code == 403){
				cout << "someone change " << filename << " on the server" << endl;
				cout << "Please try again later" << endl;
			}
			else{
				int filed, file_size, receive_size;
				filed = open(filename.c_str(), O_RDWR, 0666);
				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				sscanf(&socket_buffer[0], "%d", &file_size);
				while(file_size){
					receive_size = read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
					write(filed, socket_buffer, receive_size);
					file_size -= receive_size;
				}
				close(filed);

				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				sscanf(&socket_buffer[0], "%d", &res_code);

				cout << command << " " << filename;

				if(res_code == 200){
					cout << " success" << endl;
				}
				else{
					cout << " failed" << endl;
				}
			}
		}
		else if(command == "write"){
			if(res_code == 404){
				cout << filename << " not exist in the server" << endl;
			}
			else if(res_code == 401){
				cout << "permission denied from server" << endl;
			}
			else if(res_code == 403){
				cout << "someone change/read " << filename << " on the server" << endl;
				cout << "Please try again later" << endl;
			}
			else{
				struct stat st;
				stat(filename.c_str(), &st);
				sprintf(&socket_buffer[0], "%lld", st.st_size);
				write(fd, socket_buffer, SOCKET_BUFFER_SIZE);

				int filed = open(filename.c_str(), O_RDWR, 0666);
				while((read_size = read(filed, socket_buffer, SOCKET_BUFFER_SIZE))){
					write(fd, socket_buffer, read_size);
				}

				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				sscanf(&socket_buffer[0], "%d", &res_code);

				cout << command << " " << filename;

				if(res_code == 200){
					cout << " success" << endl;
				}
				else{
					cout << " failed" << endl;
				}
			}
		}
		else if(command == "change"){
			if(res_code == 404){
				cout << filename << " not exist in the server" << endl;
			}
			else if(res_code == 401){
				cout << "permission denied from server" << endl;
			}
			else if(res_code == 403){
				cout << "someone change/read " << filename << " on the server" << endl;
				cout << "Please try again later" << endl;
			}
			else{
				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				sscanf(&socket_buffer[0], "%d", &res_code);

				cout << command << " " << filename;

				if(res_code == 200){
					cout << " success" << endl;
				}
				else{
					cout << " failed" << endl;
				}
			}
		}
		else if(command == "information"){
			if(res_code == 404){
				cout << filename << " not exist in the server" << endl;
			}
			else if(res_code == 401){
				cout << "permission denied from server" << endl;
			}
			else if(res_code == 403){
				cout << "someone change " << filename << " on the server" << endl;
				cout << "Please try again later" << endl;
			}
			else{
				
				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				cout << socket_buffer << endl;

				read(fd, socket_buffer, SOCKET_BUFFER_SIZE);
				sscanf(&socket_buffer[0], "%d", &res_code);

			}
		}
		else if(command == "exit"){
			cout << "disconnect" << endl;
			return;
		}
		
	}

}
