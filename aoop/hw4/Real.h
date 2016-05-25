#ifndef _REAL_H_
#define _REAL_H_

#include <string>
#include <sstream>
#include "Tag.h"
#include "Token.h"

namespace lexer{ 

	class Real : public Token {
		public: float value;
		public: Real(float v) : Token(Tag::REAL){
			this->value = v;
		};

		public:  std::string toString(){
			char ch = (char)this->tag;
			std::string c(&ch, 1);
			std::stringstream ss;
			std::string s;
			ss << this->value;
			ss >> s;
			return s + "\t(REAL)";
		}
	};
};

#endif