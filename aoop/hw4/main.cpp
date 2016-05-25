#include "lexer.h"

int main(){

	lexer::Lexer lexer;

	try{
		while(true){
			lexer::Token* t = lexer.scan();
			std::cout << "Token : " << t->toString() << std::endl;
		}
	}
	catch(int i){
		if(i == -1)
			std::cout << "End of file reached" << std::endl;
		else
			std::cout << "error : " << i << std::endl;
	}
	catch(std::exception& e){
    	std::cout << e.what() << std::endl;
 	}

}
