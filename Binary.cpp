///////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Binary.cpp
//Author: Pu Zhao, Jiayu Li, Hao Zhang
//Date: 02/19/2018
//Discription: This file implements funcion declarations in head file
//All Rights Reserved 
//////////////////////////////////////////////////////////////////////
#include "Binary.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <cmath>
#include <iomanip>

//////////////////////////////////////////////////////////////////////////////////////
/************************* bin class implementation *******************************/

int bin::overflow_ = 0;

//<------------------- construct a bin object from double ------------------------->
bin::bin(double value)
{
	double absvalue = fabs(value);
	int integral_part = int(absvalue);
	double fractional_part = absvalue - integral_part;
	int bits1count = INT_BITS;
	int bits2count = FRAC_BITS;

	while (bits1count > 0) {
		bits[INT_BITS - bits1count + FRAC_BITS] = integral_part % 2;
		integral_part = integral_part / 2;
		bits1count--;
	}

	while (bits2count > 0) {
		bits[bits2count - 1] = int(fractional_part * 2);
		fractional_part = fractional_part * 2 - int(fractional_part * 2);
		bits2count--;
	}

	if (value >= 0) {
		bits[INT_BITS + FRAC_BITS] = 0;
	}
	else {
		bits[INT_BITS + FRAC_BITS] = 1;
	}

	this->complement();

	/*  check whether the value is out of range
	if (absvalue >= (pow(2, INT_BITS))) {
		overflow_ = overflow_ + 1;
	}*/
}

//<------------------- construct a bin object from decimal double string ------------------------->
bin::bin(string str)
{
	if (str.length() != INT_BITS + FRAC_BITS + 1) {
		cout << " String length error! " << endl;
	}
	bitset<INT_BITS + FRAC_BITS + 1> temp(str);
	bits = temp;
	complement();
}

//<------------------- add two binary ------------------------->
bin bin::operator+(const bin& operand)
{
	bin res;
	bin opera;
	opera.bits = bits;

	res = opera / operand;

	if (bits[INT_BITS + FRAC_BITS] == operand.bits[INT_BITS + FRAC_BITS] && bits[INT_BITS + FRAC_BITS] != res.bits[INT_BITS + FRAC_BITS]) {
		overflow_++;
	}

	bitset<INT_BITS + FRAC_BITS + 1> Negtive_zero;
	Negtive_zero.set(INT_BITS + FRAC_BITS);

	if (bits[INT_BITS + FRAC_BITS] == operand.bits[INT_BITS + FRAC_BITS] && bits[INT_BITS + FRAC_BITS] == 1 && res.bits == Negtive_zero) {
		overflow_++;
	}

	return res;
}

void bin::reset_of()
{
	overflow_ = 0;
}

//<------------------- subtract two binary ------------------------->
bin bin::operator-(const bin& operand)
{/*   // implement subtract with addition
	bin temp = operand;
	bin ophanda;
	ophanda.bits = bits;
	// if operand is not all zeros.
	if ( operand.bits.count() != 0) {
		if (temp.isNeg()) {
			temp.complement();
			temp.setPos();
		}
		else {
			temp.setNeg();
			temp.complement();
		}
		bin res = ophanda + temp;
		return res;
	}
	else {   // if operand is all zeros.
		return ophanda;
	}*/

	// implement subtraction directly
	bin res;
	bin opera;
	opera.bits = bits;

	res = opera | operand;

	if (bits[INT_BITS + FRAC_BITS] != operand.bits[INT_BITS + FRAC_BITS] && bits[INT_BITS + FRAC_BITS] != res.bits[INT_BITS + FRAC_BITS]) {
		overflow_++;
	}

	bitset<INT_BITS + FRAC_BITS + 1> Negtive_zero;
	Negtive_zero.set(INT_BITS + FRAC_BITS);

	if (bits[INT_BITS + FRAC_BITS] != operand.bits[INT_BITS + FRAC_BITS] && bits[INT_BITS + FRAC_BITS] == 1 && res.bits == Negtive_zero) {
		overflow_++;
	}

	return res;
}

//<------------------- multiply two binary ------------------------->
bin bin::operator*(const bin& operand)
{
	bin result;
	bin opera;
	opera.bits = bits;

	bin operb = operand;

	bitset<INT_BITS + FRAC_BITS >  shiftout;

	int compa = 0;
	bin tempsum(0);

	// booth algorithm
	for (int i = 0; i <= INT_BITS + FRAC_BITS - 1; i++) {
		if ((compa == 0 && operb.bits[i] == 0) || (compa == 1 && operb.bits[i] == 1)) {
			shiftout[i] = tempsum.bits[0];
			tempsum.shift_right_onebit();
			compa = operb.bits[i];
		}
		else if (compa == 0 && operb.bits[i] == 1) {
			tempsum = tempsum | opera;  // minus complement
			shiftout[i] = tempsum.bits[0];
			tempsum.shift_right_onebit();
			compa = operb.bits[i];
		}
		else if (compa == 1 && operb.bits[i] == 0) {
			tempsum = tempsum / opera;   // add complement
			shiftout[i] = tempsum.bits[0];
			tempsum.shift_right_onebit();
			compa = operb.bits[i];
		}
	}

	if (compa == 0 && operb.bits[INT_BITS + FRAC_BITS] == 1) {
		tempsum = tempsum | opera;  // there is no shift in the final step
	}
	else if (compa == 1 && operb.bits[INT_BITS + FRAC_BITS] == 0) {
		tempsum = tempsum / opera;  // there is no shift in the final step
	}

	// the result is stored in tempsum and shiftout.
	// constructure the result
	string temp = tempsum.bits.to_string() + shiftout.to_string();

	bitset<INT_BITS + FRAC_BITS + 1> mul(temp, INT_BITS, INT_BITS + FRAC_BITS + 1);
	mul[INT_BITS + FRAC_BITS] = tempsum.bits[INT_BITS + FRAC_BITS];
	result.bits = mul;
	return result;
}

//<------------------- unsigned binary addition, not divide ------------------------->
// addition only used in the multiplication, don't care about overflows at all

/*
bin bin::operator/(const bin& operand)
{
	bin res;
	bitset<INT_BITS + FRAC_BITS + 1> result;

	int carry = 0;
	for (int i = 0; i <= INT_BITS + FRAC_BITS; i++) {
		int tempsum = bits[i] + operand.bits[i] + carry;
		if (tempsum > 1) {
			result[i] = tempsum % 2;
			carry = 1;
		}
		else {
			result[i] = tempsum;
			carry = 0;
		}
	}

	res.bits = result;
	return res;
}
*/

bin bin::operator/(const bin& operand)
{
	bin res;
	bitset<INT_BITS + FRAC_BITS + 1> result;

	int carry = 0;
	for (int i = 0; i <= INT_BITS + FRAC_BITS; i++) {
		// based on the truth table
		result[i] = ((bits[i] ^ operand.bits[i]) ^ carry);
		carry = ((bits[i] & operand.bits[i]) | (bits[i] & carry)) | (operand.bits[i] & carry);
	}

	res.bits = result;
	return res;
}

//<------------------- unsigned binary subtraction, not or ------------------------->
// subtraction only used in the multiplication, don't care about overflows at all
/*
bin bin::operator|(const bin& operand)
{
	bin res;
	bitset<INT_BITS + FRAC_BITS + 1> result;

	int carry = 0;
	for (int i = 0; i <= INT_BITS + FRAC_BITS; i++) {
		int tempsum = bits[i] - operand.bits[i] - carry;
		if (tempsum < 0 ) {
			result[i] = ( tempsum*(-1) ) % 2;
			carry = 1;
		}
		else {
			result[i] = tempsum;
			carry = 0;
		}
	}

	res.bits = result;
	return res;
}
*/
bin bin::operator|(const bin& operand)
{
	bin res;
	bitset<INT_BITS + FRAC_BITS + 1> result;
	bitset<INT_BITS + FRAC_BITS + 1> opera;
	opera = bits;

	int carry = 0;
	for (int i = 0; i <= INT_BITS + FRAC_BITS; i++) {
		// based on the truth table
		result[i] = ((bits[i] ^ operand.bits[i]) ^ carry);
		carry = (opera[i].flip() &(operand.bits[i] ^ carry)) | (operand.bits[i] & carry);

	}

	res.bits = result;
	return res;
}

//<------------------- take the two's complement ------------------------->
void bin::complement()
{
	if (bits[INT_BITS + FRAC_BITS] == 1) {
		bits.flip();

		int carry = 1;
		int i = 0;
		int temp;
		while (carry != 0 && i <= INT_BITS + FRAC_BITS - 1) {
			temp = bits[i] + carry;
			if (temp == 2) {
				carry = 1;
				bits[i] = 0;
			}
			else {
				carry = 0;
				bits[i] = temp;
			}
			i++;
		}
		bits.set(INT_BITS + FRAC_BITS);
		if (carry == 1) {
			bits[INT_BITS + FRAC_BITS] = 0;
		}
	}
}


//<------------------- shift bits to right by 1 index ------------------------->
// the most significatant bit is the same with the sign, not zero padding. 
void bin::shift_right_onebit()
{
	bits = bits >> 1;
	bits[INT_BITS + FRAC_BITS] = bits[INT_BITS + FRAC_BITS - 1];
}

void bin::shift_left_onebit()
{
	int temp = bits[INT_BITS + FRAC_BITS];
	bits = bits << 1;
	bits[INT_BITS + FRAC_BITS] = temp;
}
//<------------------- convert to decimal double ------------------------->
double bin::to_double()
{
	complement();  // when showing the double value, we get original code first.
	double inte = 0;
	double frac = 0;
	double whole;

	for (unsigned int i = INT_BITS + FRAC_BITS - 1; i >= FRAC_BITS; i--) {
		inte = inte * 2 + (bits[i]);
	}

	for (unsigned int i = 0; i < FRAC_BITS; i++) {
		frac = frac / 2 + (bits[i]);
	}
	frac = frac / 2;
	if (bits[INT_BITS + FRAC_BITS] == 1) {
		whole = -(inte + frac);
	}
	else {
		whole = inte + frac;
	}
	complement();  // after get the double value, change back to complement code.
	return whole;
}

//<------------------- output in decimal double string format ------------------------->
string bin::to_double_str()
{
	stringstream ss;
	double data = to_double();
	ss << setprecision(10) << data;
	return ss.str();
}

//<------------------- output in binary bits string format ------------------------->
string bin::to_string()
{
	return bits.to_string();
}

string bin::to_hex()
{
	stringstream res;
	res << setfill('0') << setw((1 + INT_BITS + FRAC_BITS) / 4) << hex << bits.to_ulong();
	return res.str();
}

int bin::overflow()
{
	return overflow_;
}

bitset<INT_BITS + FRAC_BITS + 1> bin::getBits()
{
	return bits;
}


//////////////////////////////////////////////////////////////////////////////////////
/************************* bin_c class implementation *******************************/

bin_c::bin_c(bin real) :
	real_(real)
{
	bin zero(0.0);
	imag_ = zero;
}

//<------------------- construct a bin_c from two bin object ------------------------->
bin_c::bin_c(bin real, bin imag) :
	real_(real),
	imag_(imag) {}

//<------------------- construct a binc_c from decimal double string ------------------------->
bin_c::bin_c(string real, string imag) :
	real_(bin(real)),
	imag_(bin(imag)) {}

bin_c::bin_c(double real, double imag) :
	real_(bin(real)),
	imag_(bin(imag)) {}

//<------------------- add two complex bin ------------------------->
bin_c bin_c::operator+(const bin_c& operand)
{
	bin real_result = this->real_ + operand.real_;
	bin imag_result = this->imag_ + operand.imag_;
	return bin_c(real_result, imag_result);
}

//<------------------- subtract two complex bin ------------------------->
bin_c bin_c::operator-(const bin_c& operand)
{
	bin real_result = this->real_ - operand.real_;
	bin imag_result = this->imag_ - operand.imag_;
	return bin_c(real_result, imag_result);
}

//<------------------- multiply two complex bin ------------------------->
bin_c bin_c::operator*(const bin_c& operand)
{
	bin real_result = this->real_ * operand.real_ - this->imag_ * operand.imag_;
	bin imag_result = this->real_ * operand.imag_ + this->imag_ * operand.real_;
	return bin_c(real_result, imag_result);
}

//<------------------- shift to right by n index ------------------------->
void bin_c::shift_right_onebit()
{
	real_.shift_right_onebit();
	imag_.shift_right_onebit();
}
/*
//<------------------- shift to left by n index ------------------------->
void bin_c::shift_left(int n)
{
	real_.shift_left(n);
	imag_.shift_left(n);
}
*/
//<------------------- get conjugate ------------------------->
void bin_c::conjugate()
{
	if (this->imag_.isNeg()) {
		this->imag_.complement();
		this->imag_.setPos();
	}
	else if (this->imag_.getBits().count() != 0) {
		this->imag_.setNeg();
		this->imag_.complement();
	}
}

//<------------------- output in decimal double string format ------------------------->
string bin_c::to_double_str()
{
	if (imag_.isNeg())
		return real_.to_double_str() + imag_.to_double_str() + "i";
	else
		return real_.to_double_str() + "+" + imag_.to_double_str() + "i";
}

//<------------------- output in binary string format ------------------------->
string bin_c::to_string()
{
	//return real_.to_string() + "+" + imag_.to_string() + "i";
	return real_.to_string() + imag_.to_string();
}

string bin_c::to_hex()
{
	bitset<2 * (1 + INT_BITS + FRAC_BITS)> newBits;
	for (int i = 0; i < 1 + INT_BITS + FRAC_BITS; i++)
	{
		newBits[i] = imag_.getBits()[i];
		newBits[i + 1 + INT_BITS + FRAC_BITS] = real_.getBits()[i];
	}

	stringstream res;
	//res << hex << newBits.to_ulong();
	res << setfill('0') << setw((1 + INT_BITS + FRAC_BITS) / 2) << hex << newBits.to_ulong();
	return res.str();
}
