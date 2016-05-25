#ifndef _TYPE_H_
#define _TYPE_H_

#include <string>
#include "Word.h"


namespace lexer{

    class Type : public Word {

        public: int width;
        public: Type( std::string s, int tag, int w) : Word(s, tag){
            width = w;
        }
        
        public: static const Type Int, Float, Char, Bool, Null;

        public: static bool numeric(Type p){
            return (
                p == Type::Char  || 
                p == Type::Int   || 
                p == Type::Float
            );
        }

        public: static Type max(Type p1, Type p2){
            if(!Type::numeric(p1) && !Type::numeric(p2))
                return Type::Null;
            else if(p1 == Type::Float && p1 == Type::Float)
                return Type::Float;
            else if(p1 == Type::Int && p1 == Type::Int)
                return Type::Int;
            else
                return Type::Char;
        }
    };
};

#endif