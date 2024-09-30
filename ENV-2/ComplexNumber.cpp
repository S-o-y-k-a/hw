#include "ComplexNumber.hpp"
#include <ostream>
#include <math.h>
 
ComplexNumber::ComplexNumber() : a(0), b(0){}
 
ComplexNumber::ComplexNumber(double num) : a(num), b(0){}
 
ComplexNumber::ComplexNumber(double numa, double numb) : a(numa), b(numb){}
 
ComplexNumber::~ComplexNumber() {}
 
ComplexNumber ComplexNumber::operator+ (const ComplexNumber& other) {
	ComplexNumber temp;
	temp.a = this->a + other.a;
	temp.b = this->b + other.b;
	return temp;
}
 
ComplexNumber ComplexNumber::operator- (const ComplexNumber& other) {
	ComplexNumber temp;
	temp.a = this->a - other.a;
	temp.b = this->b - other.b;
	return temp;
}
 
ComplexNumber ComplexNumber::operator* (double c) {
	ComplexNumber temp;
	this->a *= c;
	this->b *= c;
	return temp;
}
 
ComplexNumber& ComplexNumber::operator= (const ComplexNumber& other) {
	this->a = other.a;
	this->b = other.b;
	return *this;
}
 
double ComplexNumber::abs_value() const{
	return sqrt(this->a * this->a + this->b * this->b);
}
 
 
std::ostream& operator<< (std::ostream& os, const ComplexNumber& other) {
		return os << other.a << " + " << other.b << "i";
}
