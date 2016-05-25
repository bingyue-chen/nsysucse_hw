#include "mytar.h"

int main(int argc, char *argv[]){
	Mytar mt;
	if(argc >= 2) mt.process(argv[1]);
	else cout << "./mytar {filename}" << endl;
	return 0;
}
