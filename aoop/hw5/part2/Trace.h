#ifndef __TRACE_H_INCLUDED__
#define __TRACE_H_INCLUDED__

#include <iostream>
#include <string>

using std::cerr;
using std::endl;
using std::string;

class Trace {

public:
	Trace(const string n) : name(n){
		Trace::depth++;
		this->print_name("Entering");
	}
	~Trace(){
		this->print_name("Leaving");
		Trace::depth--;
	}
private:
	void print_name(const string prefix){
		for(int i = 0; i < Trace::depth; i++)
			cerr << " ";
		cerr << prefix << " " << this->name << " (" << Trace::depth << ")" << endl;
	}
	static int depth;
	const string name;
};

#ifdef __TRACE__
#define TRACE(v,n) Trace v(n)
#else
#define TRACE(v,n)
#endif

#endif