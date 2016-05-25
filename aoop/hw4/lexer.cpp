#include "lexer.h"

namespace lexer {

	void Lexer::reserve(Word w){
		words.insert(std::pair<std::string, Word>(w.lexeme, w));
	}

	Lexer::Lexer(){
		this->reserve(Word("if",    Tag::IF));
		this->reserve(Word("else",  Tag::ELSE));
		this->reserve(Word("while", Tag::WHILE));
		this->reserve(Word("do",    Tag::DO));
		this->reserve(Word("break", Tag::BREAK));
		this->reserve(Word::True);
		this->reserve(Word::False);
		this->reserve(Type::Int);
		this->reserve(Type::Char);
		this->reserve(Type::Bool);
		this->reserve(Type::Float);
		this->line = 1;
		this->peek = '\0';
	}

	void Lexer::readch() throw(int){
		int i = std::cin.get();
		if(!std::cin.eof()){
			this->peek = (char)i;
		}
		else{
			throw -1;
		}
	}

	bool Lexer::readch_is_c(char c) throw(int){
		this->readch();
		if(this->peek != c)
			return false;
		this->peek = '\0';
		return true;
	}

	Token* Lexer::scan_operator(char is_c, const Word* default_token) throw(int){
		char c = this->peek;
		if(this->readch_is_c(is_c))
			return (Token *)default_token;
		else{
			Token t(c);
			this->v_token.push_back(t);
			return &this->v_token.back();
		}
	}

	Token* Lexer::scan() throw(int){
		if(this->peek == '\0' || this->peek == ' ' || this->peek == '\t' || this->peek == '\n'){
			while(true){
				this->readch();
				if(this->peek == ' ' || this->peek == '\t') continue;
				else if(this->peek == '\n') this->line++;
				else break;
			}
		}

		switch(this->peek){
			case '&':
				return this->scan_operator('&', &Word::And);
			case '|':
				return this->scan_operator('|', &Word::Or);
			case '=':
				return this->scan_operator('=', &Word::eq);
			case '!':
				return this->scan_operator('=', &Word::ne);
			case '<':
				return this->scan_operator('=', &Word::le);
			case '>':
				return this->scan_operator('=', &Word::ge);
		}

		if(std::isdigit(this->peek)){
			int v = 0;
			do{
				v = 10 * v + this->peek - '0';
				this->readch();
			}while(std::isdigit(this->peek));

			if(this->peek != '.'){
				Num n(v);
				this->v_num.push_back(n);
				return &this->v_num.back();
			}

			float x = v;
			float d = 10;

			for(;;){
				this->readch();
				if(!std::isdigit(this->peek))
					break;
				x = x + (this->peek - '0') / d;
				d = d * 10;
			}
			
			Real r(x);
			this->v_real.push_back(r);
			return &this->v_real.back();
		}

		if(std::isalpha(this->peek)){
			std::string s = "";
			do{
				s += this->peek;
				this->readch();
			}while(std::isalpha(this->peek));
			std::map<std::string, Word>::iterator it = this->words.find(s);
			if(it == this->words.end()){
				Word nw(s, Tag::ID);
				this->words.insert(std::pair<std::string, Word>(s, nw));
			}
			
			it = this->words.find(s);
			return &(it->second);
		}

		Token t(this->peek);
		this->v_token.push_back(t);
		this->peek = '\0';
		return &this->v_token.back();
	}

};


using namespace lexer;

const Word Word::And("&&",    Tag::AND);
const Word Word::Or("or",    Tag::OR);
const Word Word::eq("==",    Tag::EQ);
const Word Word::ne("!=",    Tag::NE);
const Word Word::le("<=",    Tag::LE);
const Word Word::ge(">=",    Tag::GE);
const Word Word::minus("minus",    Tag::MINUS);
const Word Word::True("true",    Tag::TRUE);
const Word Word::False("false",    Tag::FALSE);
const Word Word::temp("t",    Tag::TEMP);

const Type Type::Int("int",   Tag::BASIC, 4);
const Type Type::Float("float", Tag::BASIC, 8);
const Type Type::Char("char",  Tag::BASIC, 1);
const Type Type::Bool("bool",  Tag::BASIC, 1);
const Type Type::Null("null", Tag::BASIC, 8);
