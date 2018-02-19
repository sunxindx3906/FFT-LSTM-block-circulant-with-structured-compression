#pragma once
/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Layer.h
//Author: Hao Zhang, Jiayu Li
//Date: 02/11/2018
//Discription: This file defines some operations for data processing
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include <vector>
#include "Binary.h"
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

using namespace std;

class Layer
{
public:
	vector<bin> readFile(string path);
	vector<bin> parseData(string rowStr);

	template<typename T>
	void writeToFile(string path, vector<T> data, string mode = "dec");

	template<typename T>
	vector<vector<T>> partition(vector<T> data, int PartitionSize);
};

template<typename T>
inline void Layer::writeToFile(string path, vector<T> data, string mode)
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
inline vector<vector<T>> Layer::partition(vector<T> data, int PartitionSize)
{
	vector<vector<T>> result;
	for (vector<T>::iterator it = data.begin(); it != data.end(); it += PartitionSize)
	{
		vector<T> cell_16;
		cell_16.assign(it, it + PartitionSize);
		result.push_back(cell_16);
	}
	return result;
}
