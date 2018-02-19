#pragma once
/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Binary.h
//Author: Pu Zhao, Jiayu Li, Hao Zhang
//Date: 02/19/2018
//Discription: This file defines two data type, bin and bin_c
//bin can represent one binary number, if we need to represent
//complex number we can use bin_c consist of two bins. Also, 
//this file includes some operators for these two data types.
//All Rights Reserved 
///////////////////////////////////////////////////////////////////
#include <bitset>
#include <vector>
#include <complex>
#include "math.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//define total number of bits for each number
//e.g: Default sign bit = 1 and if INI_BITS = 5, FRAC_BITS 10, we can use 16 bits(1 + 5 + 10) to represent a number.
#define INT_BITS 5
#define FRAC_BITS 10
#define M_PI  3.14159265358979323846

using namespace std;


//////////////////////////////////////////////////////////
// bin class defines the customized binary representation
// and operations
class bin
{
public:
	bin() { bits[INT_BITS + FRAC_BITS] = 0; }
	bin(double value);
	bin(string str);
	~bin() {}

	bool isNeg() { return bits[INT_BITS + FRAC_BITS] == 1; }
	bool isPos() { return bits[INT_BITS + FRAC_BITS] == 0; }
	void setNeg() { bits[INT_BITS + FRAC_BITS] = 1; }
	void setPos() { bits[INT_BITS + FRAC_BITS] = 0; }

	bin operator+(const bin& op);
	bin operator-(const bin& op);
	bin operator*(const bin& op);
	bin operator/(const bin& op);
	bin operator|(const bin& op);

	void complement();
	void shift_right_onebit();
	void shift_left_onebit();
	bitset<INT_BITS + FRAC_BITS+1> getBits();

	double to_double();
	string to_double_str();
	string to_string();
	string to_hex();
	static void reset_of();
	static int overflow();
private:

	bitset<1 + INT_BITS + FRAC_BITS> bits;

	static int overflow_;
};


//////////////////////////////////////////////////////////////
// bin_c class defines the complex binary, which is comprised
// of a real bin part and imaginary bin part
class bin_c
{
public:
	bin_c() {}
	bin_c(bin real);
	bin_c(bin real, bin imag);
	bin_c(string real, string imag);
	bin_c(double real, double imag);

	bin real() { return real_; }
	bin imag() { return imag_; }

	bin_c operator+(const bin_c& op);
	bin_c operator-(const bin_c& op);
	bin_c operator*(const bin_c& op);

	void shift_right_onebit();
//	void shift_left(int num);
	void conjugate();

	string to_double_str();
	string to_string();
	string to_hex();
private:
	bin real_;
	bin imag_;
};


