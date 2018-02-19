#pragma once
/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Data.h
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file defines Data data type contains the data 
//used to calulate in all formulas. It also includes some operators 
//for Data.
//All Rights Reserved 
///////////////////////////////////////////////////////////////////
#include "Binary.h"
#include <map>


#ifndef DATA_H
#define DATA_H
class Data
{
public:
	Data(int);
	void initializer(int);
	//search data
	vector<vector<vector<bin_c>>>& _hashWeight(string);
	vector<vector<bin>>& _hash_normal(string);
	//set and get data
	void setNormalData(vector<vector<bin>>&, string);
	void setWeightData(vector<vector<vector<bin_c>>>&, string);
	vector<vector<bin>> getNormalData(string);
	vector<vector<vector<bin_c>>> getWeightData(string);
	//get block size
	int getBlockSize();
	~Data() {}
private:
	int blockSize_;
	//map data structure for stoing data
	map<string, vector<vector<bin>>&> HashTableNormal;
	map<string, vector<vector<vector<bin_c>>>&> HashTableWeight;
	//matrix weight
	vector<vector<vector<bin_c>>> Wix;
	vector<vector<vector<bin_c>>> Wir;
	vector<vector<vector<bin_c>>> Wfx;
	vector<vector<vector<bin_c>>> Wfr;
	vector<vector<vector<bin_c>>> Wcx;
	vector<vector<vector<bin_c>>> Wcr;
	vector<vector<vector<bin_c>>> Wox;
	vector<vector<vector<bin_c>>> Wor;

	//diagonal matrix weight
	vector<vector<bin>> Wic;
	vector<vector<bin>> Wfc;
	vector<vector<bin>> Woc;
	
	//Wym weight
	vector<vector<vector<bin_c>>> Wym;
    
	//Bias
	vector<vector<bin>> Bi;
	vector<vector<bin>> Bf;
	vector<vector<bin>> Bc;
	vector<vector<bin>> Bo;

	//x variable
	vector<vector<bin>> xt;
	vector<vector<bin>> yt1;
	vector<vector<bin>> ct;
	vector<vector<bin>> ct1;

	//calculate result
	vector<vector<bin>> It;
	vector<vector<bin>> Ft;
	vector<vector<bin>> Gt;
	vector<vector<bin>> Ot;
	vector<vector<bin>> Mt;
	vector<vector<bin>> yt;
};
#endif