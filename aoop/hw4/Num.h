#ifndef _NUM_H_
#define _NUM_H_

#include <string>
#include <sstream>
#include "Tag.h"
#include "Token.h"

namespace lexer{ 

	class Num : public Token {
		public: int value;
		public: Num(int v) : Token(Tag::NUM){
			this->value = v;
		};

		public:  std::string toString(){
			char ch = (char)this->tag;
			std::string c(&ch, 1);
			std::stringstream ss;
			std::string s;
			ss << this->value;
			ss >> s;
			return s + "\t(NUM)";
		}
	};
};

#endif