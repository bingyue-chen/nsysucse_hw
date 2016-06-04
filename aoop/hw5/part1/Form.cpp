#include "Form.h"

Form& Form::scientific(){
	this->format = 1;
	return *this;
}

Form& Form::fixed(){
	this->format = 2;
	return *this;
}

Form& Form::precision(int precision){
	this->n_precision = precision;
	return *this;
}

Form& Form::operator=(const Form& other){
	this->n_precision = other.n_precision;
	this->format    = other.format;

	return *this;
}

Form& Form::operator()(double d){
	this->value = d;
	return *this;
}

std::ostream& operator<<(std::ostream& o, const Form& target){

	//backup ostream state
	std::streamsize old_o_precision = o.precision();
	std::ios::fmtflags old_o_flag(o.flags());

	o << std::setprecision(target.n_precision);

	switch(target.format){
		case 0: break;
		case 1: o << std::scientific; break;
		case 2: o << std::fixed; break;
		default : break;
	}

	o << target.value;

	//restore ostream state
	o << std::setprecision(old_o_precision);
	o.flags(old_o_flag);

	return o;
}
