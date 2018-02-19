/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Layer.cpp
//Author: Hao Zhang, Jiayu Li
//Date: 02/11/2018
//Discription: This file implements funcion declarations in head file
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include "Layer.h"
#include <cmath>

vector<bin> Layer::readFile(string path)
{
	vector<bin> result;

	ifstream fin(path);
	if (!fin.is_open())
	{
		cout << "Error opening file"; exit(1);
	}

	string rowStr;
	while (getline(fin, rowStr))
	{
		stringstream ss(rowStr);
		double data_d;
		ss >> data_d;
		bin data(data_d);
		result.push_back(data);
	}
	fin.close();
	return result;
}


vector<bin> Layer::parseData(string rowStr)
{
	stringstream ss(rowStr);
	vector<bin> tokens;

	double buf;
	while (ss >> buf)
	{
		bin data(buf);
		tokens.push_back(data);
	}
	return tokens;
}


#ifdef TEST_LAYER
int main()
{
	/*Layer layer;
	cout << "\n  Average error between two files is: " << layer.computeError(
		"data/output/2_ReLU_Decimal_Real/ReLU_output_Decimal_real_0.txt",
		"data/output/Inputfc2_m2.txt") << endl;*/
	/*string path = "data/output/Inputfc2.txt";
	ifstream fin(path);
	if (!fin.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	string rowStr;
	vector<string> dataVec;
	while (getline(fin, rowStr))
	{
		double buf;
		stringstream ss(rowStr);
		ss >> buf;
		if (buf < 0)
		dataVec.push_back(rowStr);
	}
	writeDecimalData_Real("data/output/Inputfc2_m4.txt", dataVec);*/
}
#endif

