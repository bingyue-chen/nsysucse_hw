#include "user_management.h"

user::user(string uname, string pwd, string gname){
	this->name      = uname;
	this->password  = this->hash(pwd.c_str());
	this->groupname = gname;
}

unsigned long user::hash(const char *str){
	unsigned long hash = 6666;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    return hash;
};

bool user::check_pwd(string pwd){
	return (this->password == this->hash(pwd.c_str()));
};

string user::update_capability(string filename, int capability){
	map<string, int>::iterator it;

	it = this->capability_list.find(filename);

	if(it == this->capability_list.end()){
		if(capability  > 0)
			this->capability_list.insert(pair<string, int>(filename, capability));
	}
	else{
		if(capability > 0)
			it->second = capability;
		else
			this->capability_list.erase(it);
	}

	string show = this->show_capability_list();

	return show;

};

int user::check_capability(string filename, int action){
	map<string, int>::iterator it;
	int res_code = 401;

	it = this->capability_list.find(filename);

	if(it != this->capability_list.end() && it->second & action){
		res_code =  200;
	}

	return res_code;
};

int user::del_capability(string filename){
	map<string, int>::iterator it;
	int res_code = 403;

	it = this->capability_list.find(filename);

	if(it == this->capability_list.end()){
		res_code = 404;
	}
	else{
		this->capability_list.erase(it);
		res_code = 200;
	}

	return res_code;
};

string user::show_capability_list(){
	string s = "[" + this->name + " capability list]:";
	for(map<string, int>::iterator it = this->capability_list.begin(); it != this->capability_list.end(); ++it){
		s += "\t" + it->first + "(" ;
		s += (it->second & 4) ? "r" : "-";
		s += (it->second & 2) ? "w" : "-";
		s += (it->second & 1) ? "x" : "-";
		s += ")";
	}

	s += "\n";

	return s;
};

user_management::user_management(){
	string groupname[3] = {"AOS_students", "CSE_students", "other_students"};
	string username[3][2] = {{"as1", "as2"}, {"cs1", "cs2"}, {"os1", "os2"}};
	string password[3][2] = {{"as1_pwd", "as2_pwd"}, {"cs1_pwd", "cs2_pwd"}, {"os1_pwd", "os2_pwd"}};
	int i, j;

	for(i = 0; i < 3; i++){
		vector<user> vu;
		for(j = 0; j < 2; j++){
			user u(username[i][j], password[i][j], groupname[i]);
			vu.push_back(u);
		}
		this->users.insert(pair<string, vector<user> >(groupname[i], vu));
	}
};

user* user_management::login(string name, string pwd){

	for(map<string, vector<user> >::iterator it = this->users.begin(); it != this->users.end(); ++it){
		for(unsigned int i = 0; i < it->second.size(); i++){
			if(it->second[i].name == name && it->second[i].check_pwd(pwd)){
				return &it->second[i];
			}
		}
	};

	return NULL;
};

string user_management::update_capability(string filename, string ownername, string groupname, int owner_capability, int group_capability, int other_capability){
	
	string s;
	for(map<string, vector<user> >::iterator it = this->users.begin(); it != this->users.end(); ++it){
		for(unsigned int i = 0; i < it->second.size(); i++){
			if(it->second[i].name == ownername){
				s += it->second[i].update_capability(filename, owner_capability);
			}
			else if(it->first == groupname){
				s += it->second[i].update_capability(filename, group_capability);
			}
			else {
				s += it->second[i].update_capability(filename, other_capability);
			}
		}
	};

	return s;

};
