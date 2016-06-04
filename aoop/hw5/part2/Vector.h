#ifndef __VECTOR_H__
#define __VECTOR_H__

#include "Trace.h"

#include <iostream>

template<class T> class vector {

public: 

	vector(int n) : num(n), count(0){
		TRACE(dummy, string(__PRETTY_FUNCTION__));
		this->valueArray = new T[this->num];
	}

	~vector(){
		TRACE(dummy, string(__PRETTY_FUNCTION__));
		delete this->valueArray;
	}

	T& operator[](int n){
		return *(this->valueArray + n);
	}

	T& elem(int n){
		return this->operator[](n);
	}

private:

	T* valueArray;
	int num;
	int count;
};

#endif