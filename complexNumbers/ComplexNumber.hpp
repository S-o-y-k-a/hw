#pragma once
#include <ostream>
 
class ComplexNumber {
 
public:
 
	ComplexNumber();
 
	ComplexNumber(double num);
 
	ComplexNumber(double numa, double numb);
 
	~ComplexNumber();
 
	ComplexNumber operator+ (const ComplexNumber& other);
 
	ComplexNumber operator- (const ComplexNumber& other);
 
	ComplexNumber operator* (double c);
 
	ComplexNumber& operator= (const ComplexNumber& other);
 
	double abs_value() const;
 
	friend std::ostream& operator<< (std::ostream& os, const ComplexNumber& other);
 
 
 
private:
	double a;
	double b;
 
};
