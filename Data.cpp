///////////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Data.cpp
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file implements funcion declarations in head file.
//All Rights Reserved 
//////////////////////////////////////////////////////////////////////
#include "Data.h"

/**
* constructor
*
* @param blocksize, each block size
*/
Data::Data(int blocksize) {
	initializer(blocksize);
}

/**
* initialize map to store data
*
* @param block_size, each block size
*/
void Data::initializer(int block_size)
{
	blockSize_ = block_size;
	//initialize hashtable
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wix", Wix));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wir", Wir));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wfx", Wfx));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wfr", Wfr));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wcx", Wcx));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wcr", Wcr));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wox", Wox));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wor", Wor));
	HashTableWeight.insert(pair<string, vector<vector<vector<bin_c>>>&>("Wym", Wym));

	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Wic", Wic));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Wfc", Wfc));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Woc", Woc));

	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Bi", Bi));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Bf", Bf));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Bc", Bc));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Bo", Bo));

	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("xt", xt));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("yt-1", yt1));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("ct", ct));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("ct-1", ct1));

	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("yt", yt));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("It", It));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Ft", Ft));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Gt", Gt));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Ot", Ot));
	HashTableNormal.insert(pair<string, vector<vector<bin>>&>("Mt", Mt));
}

/**
* set data normal, data type is vector<vector<bin>>
*
* @param data, data written to store in Data package
* @param dataName, find the position in Data package
*/
void Data::setNormalData(vector<vector<bin>>& data, string dataName)
{
	map<string, vector<vector<bin>>&>::iterator it = HashTableNormal.find(dataName);
	if (it != HashTableNormal.end())
	{
		it->second.clear();
		for (size_t i = 0; i < data.size(); i++)
		{
			vector<bin> temp;
			temp.assign(data[i].begin(), data[i].end());
			it->second.push_back(temp);
		}
	}
	else
	{
		cout << "cannot find the data name" << endl;
		exit(0);
	}
}

/**
* set data weight, data type is vector<vector<vector<bin>>>
*
* @param data, data written to store in Data package
* @param dataName, find the position in Data package
*/
void Data::setWeightData(vector<vector<vector<bin_c>>>& data, string dataName)
{
	map<string, vector<vector<vector<bin_c>>>&>::iterator it = HashTableWeight.find(dataName);
	if (it != HashTableWeight.end())
	{
		it->second.clear();
		for (size_t i = 0; i < data.size(); i++)
		{
			vector<vector<bin_c>> tmp;
			for (size_t j = 0; j < data[i].size(); j++)
			{
				vector<bin_c> temp;
				temp.assign(data[i][j].begin(), data[i][j].end());
				tmp.push_back(temp);
			}
			it->second.push_back(tmp);
		}
	}
	else
	{
		cout << "cannot find the data name" << endl;
		exit(0);
	}
}

/**
* get weight data, returning data type is vector<vector<vector<bin>>>
*
* @param dataName, find the position in Data package
*/
vector<vector<vector<bin_c>>> Data::getWeightData(string dataName)
{
	return _hashWeight(dataName);
}

/**
* get data, returning data type is vector<vector<bin>>
*
* @param dataName, find the position in Data package
*/
vector<vector<bin>> Data::getNormalData(string dataName)
{
	return _hash_normal(dataName);
}

/**
* find data in map
*
* @param dataName, find the position in Data package
*/
vector<vector<bin>>& Data::_hash_normal(string dataName)
{
	map<string, vector<vector<bin>>&>::iterator it = HashTableNormal.find(dataName);
	if (it != HashTableNormal.end())
	{
		return it->second;
	}
	else
	{
		cout << "cannot find the data name" << endl;
		exit(0);
	}
}

/**
* find data in map
*
* @param dataName, find the position in Data package
*/
vector<vector<vector<bin_c>>>& Data::_hashWeight(string dataName)
{
	map<string, vector<vector<vector<bin_c>>>&>::iterator it = HashTableWeight.find(dataName);
	if (it != HashTableWeight.end())
		return it->second;
	else
	{
		cout << "cannot find the data name" << endl;
		exit(0);
	}
}

/**
* get block size
*
*/
int Data::getBlockSize() {
	return blockSize_;
}

