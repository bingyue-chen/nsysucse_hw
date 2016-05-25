#ifndef _WORD_H_
#define _WORD_H_

#include <string>
#include "Token.h"
#include "Tag.h"


namespace lexer{

    class Word : public Token {
        public: std::string lexeme;
        public: Word(std::string s, int tag)  : Token(tag){
            this->lexeme = s;
        };

        public: std::string toString(){

            std::string c;

            switch(this->tag){
                case Tag::AND :   c = "AND";   break;
                case Tag::BASIC : c = "BASIC"; break;
                case Tag::BREAK : c = "BREAK"; break;
                case Tag::DO :    c = "DO";    break;
                case Tag::ELSE :  c = "ELSE";  break;
                case Tag::FALSE : c = "FALSE"; break;
                case Tag::GE :    c = "GE";    break;
                case Tag::ID :    c = "ID";    break;
                case Tag::IF :    c = "IF";    break;
                case Tag::INDEX : c = "INDEX"; break;
                case Tag::LE :    c = "LE";    break;
                case Tag::MINUS : c = "MINUS"; break;
                case Tag::NE :    c = "NE";    break;
                case Tag::NUM :   c = "NUM";   break;
                case Tag::OR :    c = "OR";    break;
                case Tag::REAL :  c = "REAL";  break;
                case Tag::TEMP :  c = "TEMP";  break;
                case Tag::TRUE :  c = "TRUE";  break;
                case Tag::WHILE : c = "WHILE"; break;
                default:
                    char ch = (char)this->tag;
                    c = std::string(&ch, 1);
            }
            
            return this->lexeme + "\t(" + c + ")";
        };

        public:  friend bool operator==(const Word& x, const Word& y){
            return (x.tag == y.tag && x.lexeme == y.lexeme);
        };

        public: static const Word And, Or, eq, ne, le, ge, minus, True, False, temp;
   
    };

};

#endif