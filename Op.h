#pragma once
/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Op.h
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file defines Op operations. Op operation will
//be used in each calculation step. It includes some core calculations
//like FFT, IFFT and dotproduct.
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include <vector>
#include "Binary.h"
#include "Layer.h"
#include "Data.h"
using namespace std;

// sigmoid
#define SIGMOID_STEP_NUM  22
#define SIGMOID_DRIFT (SIGMOID_STEP_NUM/2)

// tanh
#define TANH_STEP_NUM  22
#define TANH_DRIFT (TANH_STEP_NUM/2)

/////////////////////////////////////////////////////
//sigmoid k and b
//if x <= -5.5, value will be equal to 0, if x >= 5.5, value will be set to 1
//if -5.5 < x < 5.5, we will use calculate the approximation by using the following k and b
const int k_sigmoid[SIGMOID_STEP_NUM] = { 0.0133857, 0.00858818, 0.0139985, 0.022652, 0.0362273, 0.0568646, 0.0866895, 0.126445, 0.173032,
0.217198, 0.244919, 0.244919, 0.217198, 0.173032, 0.126445, 0.0866895, 0.0568646, 0.0362273, 0.022652, 0.0139985, 0.00858818, 0.0133857};

const int b_sigmoid[SIGMOID_STEP_NUM] = { 0.0736214, 0.0496338, 0.0739803, 0.108594, 0.156108, 0.21802, 0.292582, 0.372093, 0.441973, 0.48614,
0.5, 0.5, 0.51386, 0.558027, 0.627907, 0.707418, 0.78198, 0.843892, 0.891406, 0.92602, 0.950366, 0.926379 };
const int sigmoid_step_size = 0.5;

/////////////////////////////////////////////////////
//tanh k and b 
//if x <= -4.4, value will be equal to -1, if x >=4.4, value will be set to 1
//if -4.4 < x < 4.4, we will use calculate the approximation by using the following k and b
const int k_tanh[TANH_STEP_NUM] = { 0.00167675, 0.00205339, 0.00456386, 0.0101272, 0.0223917, 0.0491182, 0.105898, 0.220035,
0.424045, 0.71022, 0.949872, 0.949872, 0.71022, 0.424045, 0.220035, 0.105898, 0.0491182, 0.0223917, 0.0101272, 0.00456386, 0.00205339, 0.00167675 };

const int b_tanh[TANH_STEP_NUM] = {-0.992622, -0.991116, -0.982078, -0.964275, -0.929935, -0.865791, -0.752232, -0.569613, -0.324801, -0.0958612,
0, 0, 0.0958612, 0.324801, 0.569613, 0.752232, 0.865791, 0.929935, 0.964275, 0.982078, 0.991116, 0.992622 };
const int tanh_step_size = 0.4;


#ifndef OP_H
#define op_H
class Op
{
public:
	//Basic operation
	Op();
	vector<bin_c> to_bin_c(vector<bin> input);
	vector<bin> to_bin(vector<bin_c> input);

	template<typename T>
	vector<T> dotProduct(vector<T> data, vector<T> weight, int loop_time);
	template<typename T>
	vector<T> addVector(vector<T> data, vector<T> bias, int loop_time);

	//For IFFT and FFT operations
	static void FFT_block(vector<bin_c> input, vector<bin_c>& output, int loop_time, int shiftflag = 1);
	static void IFFT_block(vector<bin_c> input, vector<bin_c>& output, int loop_time);

	//For output operations
	template<typename T>
	static void writeToFile(string path, vector<T> data, string mode = "dec");
	template<typename T>
	static void writeToFile2(string path, vector<T> data, string mode = "dec");

	//Activation functions
	void sigmoid(vector<vector<bin>> in, vector<vector<bin>>& out);
	void h(vector<vector<bin>> in, vector<vector<bin>>& out);

private:
	//For FFT and IFFT operations
	static void IFFT_recursive(vector<bin_c> input, vector<bin_c>& output, int shiftflag);
	static void butterfly(bin_c input1, bin_c input2, bin_c& output1, bin_c& output2, bin_c twiddle);
	static void conjugate(vector<bin_c>& input);
};
#endif


/**
* Compute the dot product of two complex vectors
*
* @param data The result of FFT operation
* @param weight The weight vector
* @return The result of dot product
*/
template<typename T>
inline vector<T> Op::dotProduct(vector<T> data1, vector<T> data2, int loop_time)
{
	vector<T> output;
	for (int i = 0; i < data1.size(); i++) {
		output.push_back(data1[i] * data2[i]);
	}

	if (bin::overflow() > 0)
	{
		cout << "\n" << loop_time << "_th loop time: " << " DOT PRODUCT OVERFLOW: " << bin::overflow() << endl;
		bin::reset_of();
	}

	return output;
}


/**
* Add two vectors
*
* @param data The result of IFFT operation
* @param bias The bias vector
* @return The result of bias addition
*/
template<typename T>
inline vector<T> Op::addVector(vector<T> data1, vector<T> data2, int loop_time)
{
	vector<T> output;

	for (int i = 0; i < data1.size(); i++) {
		output.push_back(data1[i] + data2[i]);
	}

	if (bin::overflow() > 0)
	{
		cout << "\n " << loop_time << "_th loop time: " << " BLOCK ADDITION OVERFLOW: " << bin::overflow() << endl;
		bin::reset_of();
	}

	return output;
}


template<typename T>
inline static void Op::writeToFile2(string path, vector<T> data, string mode)
{
	ofstream out(path);
	if (!out.is_open())
	{
		cout << "Couldn't write to: " << path << endl;
		return;
	}
	for (int i = 0; i < data.size(); i++)
	{
		if (mode == "hex")
			out << data[i].to_hex() << endl;
		if (mode == "dec")
			out << data[i].to_double_str() << endl;
		if (mode == "bin")
			out << data[i].to_string() << endl;
	}
	out.close();
}


template<typename T>
inline static void Op::writeToFile(string path, vector<T> data, string mode)
{
	ofstream out(path);
	if (!out.is_open())
	{
		cout << "Couldn't write to: " << path << endl;
		return;
	}
	for (int i = 0; i < data.size(); i++)
	{
		if (mode == "hex")
			out << data[i].to_hex() << endl;
		if (mode == "dec")
			out << data[i].to_double() << endl;
		if (mode == "bin")
			out << data[i].to_string() << endl;
	}
	out.close();
}