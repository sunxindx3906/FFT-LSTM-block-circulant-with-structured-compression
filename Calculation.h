#pragma once
/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Calculation.h
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file defines four kinds of operations
//All Rights Reserved 
///////////////////////////////////////////////////////////////////
#include "Op.h"
#include "Data.h"


//calculate the following formulas:
//It = sigmoid(IFFT(FFT(Wix) x FFT(Xt)) + IFFT(FFT(Wir) x FFT(Yt-1)) + Wic*Ct-1 + Bi)
//Ft = sigmoid(IFFT(FFT(Wfx) x FFT(Xt)) + IFFT(FFT(Wfr) x FFT(Yt-1)) + Wfc*Ct-1 + Bf)
//Ot = sigmoid(IFFT(FFT(Wox) x FFT(Xt)) + IFFT(FFT(Wor) x FFT(Yt-1)) + Woc*Ct-1 + Bo)
//Gt = sigmoid(IFFT(FFT(Wcx) x FFT(Xt)) + IFFT(FFT(Wcr) x FFT(Yt-1)) + Bc)
class IFOG_calculation
{
public:
	IFOG_calculation(string storeData);
	void calculation(Op& op_cal, Data& data, string W_x, string W_r, string W_c, string Bias, string data_c, bool cal_g, int &time);
private:
	string store_data;
};


//calculate the following formulas:
//Ct = Ft * Ct-1 + Gt * It
class C_calculation
{
public:
	C_calculation(string storeData);
	void calculation(Op& op, Data& data, string ft, string ct1, string gt, string it, int &time);
private:
	string store_data;
};


//calculate the following formulas:
//Mt = Ot * h(Ct)
class M_calculation
{
public:
	M_calculation(string storeData);
	void calculation(Op& op, Data& data, string ot, string ct, int &time);
private:
	string store_data;
};


//calculate the following formulas:
//Yt = Wym * Mt
class Y_calculation
{
public:
	Y_calculation(string storeData);
	void calculation(Op& op, Data& data, string W_ym, string mt, int &time);
private:
	string store_data;
};


//Update Yt-1 and Ct-1 from Yt and Ct
class Update_calculation
{
public:
	Update_calculation(string store_data);
	void update(Data& data, string cur_data, int &time);

private:
	string store_data;
};