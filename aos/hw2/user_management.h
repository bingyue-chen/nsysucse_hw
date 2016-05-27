
#include <string>
#include <map>
#include <vector>

using namespace std;

class user {
public:
	user(string uname, string pwd, string gname);
	string name;
	string groupname;
	bool   check_pwd(string pwd);
	string   update_capability(string filename, int capability);
	int    check_capability(string filename, int action);
	int    del_capability(string filename);
private:
	unsigned long password;
	unsigned long hash(const char *str);
	map<string, int> capability_list;
	string show_capability_list();
};

class user_management {
public:
	user_management();
	string update_capability(string filename, string ownername, string groupname, int owner_capability, int group_capability, int other_capability);
	user* login(string name, string pwd);
private:
	map<string, vector<user> > users;
	
};
