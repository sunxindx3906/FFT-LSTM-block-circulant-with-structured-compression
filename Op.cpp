/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Op.cpp
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file implements funcion declarations in head file
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include "Op.h"
#include <algorithm>
#include <math.h>
#include <ctgmath>
#include <random>
#include <chrono>
#include <iomanip>

/**
* Constructor for Op
*/
Op::Op() {}

/**
* write data to correspoding path
* @param path, The file path
* @param data, Data written to a file
*/
void writeDoubleToFile(string path, vector<double> data)
{
	ofstream out(path);
	if (!out.is_open())
	{
		cout << "Couldn't write to: " << path << endl;
		return;
	}

	for (int i = 0; i < data.size(); i++)
	{
		out << std::setprecision(20) << data[i] << endl;
	}
	out.close();
}

/**
* change a binary number without imaginary part to a binary number with imaginary part
* @param input, binary numbers without imaginary part
*/
vector<bin_c> Op::to_bin_c(vector<bin> input)
{
	vector<bin_c> result;
	for (auto data : input)
	{
		bin_c tmp(data);
		result.push_back(tmp);
	}
	return result;
}

/**
* change a binary number with imaginary part to a binary number without imaginary part
* @param input, binary numbers with imaginary part
*/
vector<bin> Op::to_bin(vector<bin_c> input)
{
	vector<bin> result;
	for (auto data : input)
	{
		bin tmp(data.real());
		result.push_back(tmp);
	}
	return result;
}

/**
* generate FFT results which are right shifted with sqrt( input.size() ) bits
* @param input, The number of data in each input
* @param output, The number of data in each output
* @param loop_time, The frequency of loop time 
* @param shiftflag, The level in FFT and IFFT
*/
void Op::FFT_block(vector<bin_c> input, vector<bin_c>& output, int loop_time, int shiftflag)
{
	int NumPoints = input.size();
	if (NumPoints == 1) {
		output = input;
	}
	else {
		int half = NumPoints / 2;

		//computer twiddle
		vector<bin_c> weights(half);
		complex<double> weightComplex;
		for (unsigned int i = 0; i < half; i++) {
		complex<double> constexp(0, -2 * M_PI * i / NumPoints);
		weightComplex = exp(constexp);
		bin_c weightemp(weightComplex.real(), weightComplex.imag());
		weights[i] = weightemp;
		}

		vector<bin_c> Stage1Temp(NumPoints);

		for (unsigned int i = 0; i < half; i++) {
			butterfly(input[i], input[i + half], Stage1Temp[2 * i], Stage1Temp[2 * i + 1], weights[i]);
		}

		if (shiftflag == 1 || shiftflag == 3 || shiftflag == 5 || shiftflag == 7) {
			for (unsigned int i = 0; i < NumPoints; i++) {
				Stage1Temp[i].shift_right_onebit();
			}
		}
		shiftflag++;

		vector<bin_c> Stage1_first_half(half);
		vector<bin_c> Stage1_second_half(half);

		vector<bin_c> final_Stage_half1(half);
		vector<bin_c> final_Stage_half2(half);

		for (unsigned int i = 0; i < half; i++) {
			Stage1_first_half[i] = Stage1Temp[2 * i];
			Stage1_second_half[i] = Stage1Temp[2 * i + 1];
		}

		FFT_block(Stage1_first_half, final_Stage_half1, loop_time, shiftflag);
		FFT_block(Stage1_second_half, final_Stage_half2, loop_time, shiftflag);

		for (unsigned int i = 0; i < half; i++) {
			output[2 * i] = final_Stage_half1[i];
			output[2 * i + 1] = final_Stage_half2[i];
		}
	}
	if (bin::overflow() > 0)
	{
		cout << "\n " << loop_time << "_th loop time: " << " FFT OVERFLOW: " << bin::overflow() << endl;
		bin::reset_of();
	}
}

/**
* generate IFFT results which are right shifted with sqrt( input.size() ) bits
*
* @param input, The number of data in each input
* @param output, The number of data in each output
* @param loop_time, The frequency of loop time
*/
void Op::IFFT_block(vector<bin_c> input, vector<bin_c>& output, int loop_time)
{
	conjugate(input);
	IFFT_recursive(input, output, 1);
	conjugate(output);
	if (bin::overflow() > 0)
	{
		cout << "\n" << loop_time << "_th loop time: " << " IFFT OVERFLOW: " << bin::overflow() << endl;
		bin::reset_of();
	}
}

/**
* An IFFT function will be used recursively
*
* @param input, The number of data in each input
* @param output, The number of data in each output
* @param shiftflag, The level in FFT and IFFT
*/
void Op::IFFT_recursive(vector<bin_c> input, vector<bin_c>& output, int shiftflag)
{
	int NumPoints = input.size();
	if (NumPoints == 1) {
		output = input;
	}
	else {
		int half = NumPoints / 2;

		//compute twiddle
		vector<bin_c> weights(half);

		complex<double> weightComplex;
		for (unsigned int i = 0; i < half; i++) {
		complex<double> constexp(0, -2 * M_PI * i / NumPoints);
		weightComplex = exp(constexp);
		bin_c weightemp(weightComplex.real(), weightComplex.imag());
		weights[i] = weightemp;
		}

		vector<bin_c> Stage1Temp(NumPoints);

		for (unsigned int i = 0; i < half; i++) {
			butterfly(input[i], input[i + half], Stage1Temp[2 * i], Stage1Temp[2 * i + 1], weights[i]);
		}

		if (shiftflag == 1 || shiftflag == 3 || shiftflag == 5 || shiftflag == 7) {
			for (unsigned int i = 0; i < NumPoints; i++) {
				Stage1Temp[i].shift_right_onebit();
			}
		}
		shiftflag++;

		vector<bin_c> Stage1_first_half(half);
		vector<bin_c> Stage1_second_half(half);

		vector<bin_c> final_Stage_half1(half);
		vector<bin_c> final_Stage_half2(half);

		for (unsigned int i = 0; i < half; i++) {
			Stage1_first_half[i] = Stage1Temp[2 * i];
			Stage1_second_half[i] = Stage1Temp[2 * i + 1];
		}

		IFFT_recursive(Stage1_first_half, final_Stage_half1, shiftflag);
		IFFT_recursive(Stage1_second_half, final_Stage_half2, shiftflag);

		for (unsigned int i = 0; i < half; i++) {
			output[2 * i] = final_Stage_half1[i];
			output[2 * i + 1] = final_Stage_half2[i];
		}
	}
}

/**
* butterfly, a feature of the calculations in FFT and IFFT
*/
void Op::butterfly(bin_c input1, bin_c input2, bin_c & output1, bin_c & output2, bin_c twiddle)
{
	output1 = input1 + input2;
	output2 = (input1 - input2) * twiddle;
}

void Op::conjugate(vector<bin_c>& input)
{
	for (int i = 0; i < input.size(); i++)
	{
		input[i].conjugate();
	}
}


/**
* sigmoid function
*/
void Op::sigmoid(vector<vector<bin>> in, vector<vector<bin>>& out) {
	for (int i = 0; i < in.size(); i++)
	{
		vector<bin> temp;
		for (int j = 0; j < in[i].size(); j++)
		{
			bin cur_in = in[i][j];
			double in_double = cur_in.to_double();
			if (in_double >= 5.5){
				temp.push_back(bin(1));
			}
			else if(in_double <= -5.5)
			{
				temp.push_back(bin(0));
			}
			else
			{
				cur_in = cur_in / bin(sigmoid_step_size);
				int index = (int)cur_in.to_double() + SIGMOID_DRIFT;
				temp.push_back(bin(k_sigmoid[index]) * in[i][j] + bin(b_sigmoid[index]));
			}
		}
		out.push_back(temp);
	}
	//int index = (int)(in / sigmoid_step_size) + SIGMOID_DRIFT;
	//if (index < 0) {
	//	index -= 1;
	//}
	//out = k_sigmoid[index] * in + b_sigmoid[index];
}


/**
* tahn function
*/
void Op::h(vector<vector<bin>> in, vector<vector<bin>>& out) {
	for (int i = 0; i < in.size(); i++)
	{
		vector<bin> temp;
		for (int j = 0; j < in[i].size(); j++)
		{
			bin cur_in = in[i][j];
			double in_double = cur_in.to_double();
			if (in_double >= 4.4) {
				temp.push_back(bin(1));
			}
			else if (in_double <= -4.4)
			{
				temp.push_back(bin(-1));
			}
			else
			{
				cur_in = cur_in / bin(tanh_step_size);
				int index = (int)cur_in.to_double() + TANH_DRIFT;
				temp.push_back(bin(k_tanh[index]) * in[i][j] + bin(b_tanh[index]));
			}
		}
		out.push_back(temp);
	}
}

//module test Op for sigmoid and h(tahn). we don't test other function because we tested them before
#ifdef _TEST_OP
#include "IO.h"
int main()
{
	//initialize
	Data lstm_data(16);
	IO lstm_io;
	Op operate;

	//define size
	int input_total_size = 160;

	//test read and set data
	string lstm_x_path = "data/DBRNN_1-fw-custom_lstm_test_sigmoid_h.txt";
	lstm_io.read_Training_Data(lstm_x_path, 0, lstm_data, "xt", input_total_size);
	//get data
	vector<vector<bin>> xt = lstm_data.getNormalData("xt");

	//computing sigmoid
	vector<vector<bin>> sigmoid_output;
	operate.sigmoid(xt, sigmoid_output);
	string path_g = "data/test_sigmoid_output.txt";
	vector<bin> writeOutput_sigmoid;
	for (int i = 0; i < sigmoid_output.size(); i++)
	{
		writeOutput_sigmoid.insert(writeOutput_sigmoid.end(), sigmoid_output[i].begin(), sigmoid_output[i].end());
	}
	operate.writeToFile(path_g, writeOutput_sigmoid, "dec");

	//computing tahn
	vector<vector<bin>> tahn_output;
	operate.h(xt, tahn_output);
	string path_h = "data/test_tahn_output.txt";
	vector<bin> writeOutput_tahn_output;
	for (int i = 0; i < sigmoid_output.size(); i++)
	{
		writeOutput_tahn_output.insert(writeOutput_tahn_output.end(), tahn_output[i].begin(), tahn_output[i].end());
	}
	operate.writeToFile(path_h, writeOutput_tahn_output, "dec");
}
#endif
