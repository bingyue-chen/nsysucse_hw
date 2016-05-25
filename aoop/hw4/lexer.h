#ifndef _LEXER_H_
#define _LEXER_H_

#include <iostream>
#include <string>
#include <map>
#include <locale>
#include <vector>
#include "Token.h"
#include "Word.h"
#include "Type.h"
#include "Num.h"
#include "Real.h"

namespace lexer { 

	class Lexer {

		public: int line;
		char peek;
		std::map<std::string, Word> words;
		std::vector<Token> v_token;
		std::vector<Num> v_num;
		std::vector<Real> v_real;

		void reserve(Word w);

		public: Lexer();

		void readch() throw(int);

		bool readch_is_c(char c) throw(int);

		Token* scan_operator(char is_c, const Word* default_token) throw(int);

		public: Token* scan() throw(int);

	};

};

#endif