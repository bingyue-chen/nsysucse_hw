#include "Complex.h"

Complex::Complex(const double re, const double im){
	this->real = re;
	this->imag = im;
}

Complex::Complex(const Complex& c){
	this->real = c.Real();
	this->imag = c.Imag();
};

Complex& Complex::operator=(const Complex& c){
	this->real = c.Real();
	this->imag = c.Imag();
	return *this;
};

Complex::~Complex(){

};

Complex Complex::Polar(const double leng, const double arg){
	this->real = leng*cos(arg);
	this->imag = leng*sin(arg);
	return *this;
};

double Complex::Real() const{
	return this->real;
};

double Complex::Imag() const{
	return this->imag;
};

double Complex::Norm() const{
	return this->real*this->real + this->imag*this->imag;
};

double Complex::Abs() const{
	return sqrt(this->Norm());
};

double Complex::Arg() const{
	return acos(this->real/this->Abs());
};

Complex Complex::operator++(){
	this->real++;
	this->imag++;
	return *this;
};

Complex Complex::operator++(int){
	Complex t = Complex(*this);
	this->real++;
	this->imag++;
	return t;
};

Complex Complex::operator--(){
	this->real++;
	this->imag++;
	return *this;
};

Complex Complex::operator--(int){
	Complex t = Complex(*this);
	this->real++;
	this->imag++;
	return t;
};

Complex Polar(const double leng, const double arg){
	Complex c = Complex(0,0);
	return c.Polar(leng, arg);
};

double Norm(const Complex& x){
	return x.Norm();
};

double Abs(const Complex& x){
	return x.Abs();
};

double Arg(const Complex& x){
	return x.Arg();
};

Complex operator+(const Complex& x, const Complex& y){
	return Complex(x.Real()+y.Real(), x.Imag()+y.Imag());
};

Complex operator-(const Complex& x, const Complex& y){
	return Complex(x.Real()-y.Real(), x.Imag()-y.Imag());
};

Complex operator*(const Complex& x, const Complex& y){
	return Complex(x.Real()*y.Real()-x.Imag()*y.Imag(), x.Real()*y.Imag()+x.Imag()*y.Real());
};

Complex operator/(const Complex& x, const Complex& y){
	double d = y.Real()*y.Real()+y.Imag()*y.Imag();
	return Complex((x.Real()*y.Real()+x.Imag()*y.Imag())/d, (-x.Real()*y.Imag()+x.Imag()*y.Real())/d);
};

Complex operator+=(Complex& x, const Complex& y){
	x = x+y;
	return x;
};

Complex operator-=(Complex& x, const Complex& y){
	x = x-y;
	return x;
};

Complex operator*=(Complex& x, const Complex& y){
	x = x*y;
	return x;
};

Complex operator/=(Complex& x, const Complex& y){
	x = x/y;
	return x;
};

bool operator==(const Complex& x, const Complex& y){
	return (x.Real() == y.Real() && x.Imag() == y.Imag());
};

bool operator!=(const Complex& x, const Complex& y){
	return !(x==y);
};

ostream& operator<<(ostream& o, const Complex& x){
	return o << "(" << x.Real() << ',' << x.Imag() << ')';
};

