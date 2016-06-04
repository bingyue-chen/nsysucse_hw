#ifndef __FORM_H_
#define __FORM_H_

#include <iomanip>
#include <iostream>

class Form {
public:
	Form(int precision){ 
		this->n_precision = precision; 
		this->format    = 0;
	};

	Form& scientific();

	Form& fixed();

	Form& precision(int precision);

	Form& operator=(const Form& other);

	Form& operator()(double d);

	friend std::ostream& operator<<(std::ostream& o, const Form& target);

	int n_precision;
	
	/*
	 * 0 => general, 1 => scientific, 2 => fixed 
	 */
	int format;
	double value;
};

#endif