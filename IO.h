#pragma once
/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: IO.h
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file defines IO operations. IO operations are
//responsible for reading and reorganizing(reshaping) data from outside 
//of the LSTM simulator system. We also do some preprocess data in 
//IO operation. For example, we do FFT(w) when getting the data from
//outside. After finishing IO operation, data will be stored in Data
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include <iostream>
#include "Layer.h"
#include "Data.h"
#include "Op.h"

#ifndef IO_H
#define IO_H
class IO : Layer
{
public:
	void read_Training_Data(string folder, int index, Data& data, string dataTypes, int totalSize);
	void read_Kernel_Weight_Input(string path, Data& data, Op& op);
	void read_Bias_Input(string path, Data& data);
	void set_Input_Weight(vector<vector<bin_c>>& sourceData, string dataType, Data& data, int start_index, int end_index, int number_in_row);
	void read_Diag_Weight_Input(string path, Data& data, string dataType);
	void read_Weight_ym(string path, Data& data, Op& op);
	void set_Input_Bias(vector<vector<bin>>& sourceData, string dataType, Data& data, int start_index, int end_index);
	void Weight_FFT(vector<vector<bin>>&, vector<vector<bin_c>>&, int, Op& op);
	void stringReplace(string&);
};
#endif


