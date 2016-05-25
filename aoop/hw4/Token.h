#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <string>
#include <sstream>

namespace lexer {

	class Token {
		public: int tag;
		public: Token(int t){ this->tag = t; }

		public: virtual std::string toString(){

			char ch = (char)this->tag;
			std::string c(&ch, 1);
			std::string s = "";
			s += c;
			s += "\t";
			s += "(" + c + ")";
			return s;
		}
	};
};

#endif