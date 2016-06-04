#include <iostream>
#include "Form.h"

using std::cout;
using std::endl;

Form gen4(4);

void f(double d){
	Form sci8 = gen4;
	sci8.scientific().precision(8);
	Form fix8 = gen4;
	fix8.fixed().precision(8);
	cout << "deafault = " << d << endl
         << "gen4     = " << gen4(d) << endl
         << "sci8     = " << sci8(d) << endl
         << "fix8     = " << fix8(d) << endl
         << "deafault = " << d << endl;
}

int main(){
	f(1234.56789);
	f(12.3456789);

	return 0;
}