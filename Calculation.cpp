////////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: Calculation.cpp
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file implements funcion declarations in head file
//All Rights Reserved 
//////////////////////////////////////////////////////////////////////
#include "Calculation.h"


/**
* Constructor IFOG_calculation
* @param storeData, The name of a variable will be updated after calculaltion
*/
IFOG_calculation::IFOG_calculation(string storeData):
	store_data(storeData) {}


/**
* calculation function for IFOG_calculation class
* @param op, Operator objective used to call operation function defined by Op
* @param data, Data package used to get data and set the data
* @param W_x, choose W_x data from Data package
* @param W_r, choose W_r data from Data package
* @param W_c, choose W_c data from Data package
* @param Bias, choose Bias data from Data package
* @param data_c, choose ct or ct-1 data from Data package
* @param cal_g, if doing Gt calculation
* @param time, loop times
*/
void IFOG_calculation::calculation(Op& op, Data& data, string W_x, string W_r, string W_c, string Bias, string data_c, bool cal_g, int &time) {
	cout << "Forwarding " << store_data << " Calculation, loop time: " << time << endl;
	//variables used to calculate
	vector<vector<vector<bin_c>>> Weight_x = data._hashWeight(W_x);
	vector<vector<vector<bin_c>>> Weight_r = data._hashWeight(W_r);
	vector<vector<bin>> Bias_ = data._hash_normal(Bias);
	vector<vector<bin>> x_t = data._hash_normal("xt");
	vector<vector<bin>> y_t1 = data._hash_normal("yt-1");
	int block_size = data.getBlockSize();
	vector<vector<bin>> Weight_c;
	vector<vector<bin>> c_t;
	if (W_c.size() != 0)
		Weight_c = data._hash_normal(W_c);
	if (data_c.size() != 0)
		c_t = data._hash_normal(data_c);

	//computer w_x * x_t
	// compute FFT for x_t
	vector<vector<bin_c>> fft_x_t;
	for (int i = 0; i < x_t.size(); i++) {
		vector<bin_c> fftBlockOutput_xt(block_size);
		op.FFT_block(op.to_bin_c(x_t[i]), fftBlockOutput_xt, time);
		fft_x_t.push_back(fftBlockOutput_xt);
	}

	//cout << "IFOG_calculation_inner time: " << time << endl;
	//compute dot product and IFFT for w_x and x_t
	vector<vector<bin>> wx_xt;
	for (int i = 0; i < Weight_x.size(); i++) {
		vector<bin> blockAddResult(block_size);
		for (int j = 0; j < fft_x_t.size(); j++)
		{
			vector<bin_c> dotProductResult = op.dotProduct(fft_x_t[j], Weight_x[i][j], time);
			vector<bin_c> ifftOutput(block_size);
			op.IFFT_block(dotProductResult, ifftOutput, time);
			blockAddResult = op.addVector(blockAddResult, op.to_bin(ifftOutput), time);
		}
		wx_xt.push_back(blockAddResult);
	}
	//cout << "IFOG_calculation_inner time: " << time << endl;
	//computer w_r * y_t1
	// compute FFT for y_t1
	vector<vector<bin_c>> fft_y_t1;
	for (int i = 0; i < y_t1.size(); i++) {
		vector<bin_c> fftBlockOutput_yt1(block_size);
		op.FFT_block(op.to_bin_c(y_t1[i]), fftBlockOutput_yt1, time);
		fft_y_t1.push_back(fftBlockOutput_yt1);
	}
	//cout << "IFOG_calculation_inner time: " << time << endl;
	//compute dot product and IFFT for w_r and y_t1
	vector<vector<bin>> wr_yt1;
	for (int i = 0; i < Weight_r.size(); i++) {
		vector<bin> blockAddResult(block_size);
		for (int j = 0; j < fft_y_t1.size(); j++)
		{
			vector<bin_c> dotProductResult = op.dotProduct(fft_y_t1[j], Weight_r[i][j], time);
			vector<bin_c> ifftOutput(block_size);
			op.IFFT_block(dotProductResult, ifftOutput, time);
			blockAddResult = op.addVector(blockAddResult, op.to_bin(ifftOutput), time);
		}
		wr_yt1.push_back(blockAddResult);
	}
	//cout << "IFOG_calculation_inner time: " << time << endl;
	// compute FFT for w * c_t1 or set w_ct1 to zero
	vector<vector<bin>> w_ct1;
	if (cal_g)
	{
		for (int i = 0; i < 64; i++)
		{
			vector<bin> temp_zero;
			for (int j = 0; j < 16; j++)
			{
				double number = 0.0;
				bin zero(number);
				temp_zero.push_back(zero);
			}
			w_ct1.push_back(temp_zero);
		}
	}
	else
	{
		for (int i = 0; i < Weight_c.size(); i++)
		{
			vector<bin> dot_res = op.dotProduct(c_t[i], Weight_c[i], time);
			w_ct1.push_back(dot_res);
		}
	}
	//cout << "IFOG_calculation_inner time: " << time << endl;
	//add all vector<vector<bin>>
	vector<vector<bin>> res;
	for (size_t i = 0; i < wx_xt.size(); i++)
	{
		vector<bin> add_Output_1 = op.addVector(wx_xt[i], wr_yt1[i], time);
		vector<bin> add_Output_2 = op.addVector(add_Output_1, w_ct1[i], time);
		vector<bin> add_Output_3 = op.addVector(add_Output_2, Bias_[i], time);
		res.push_back(add_Output_3);
	}
	vector<vector<bin>> final_res;
	op.sigmoid(res, final_res);
	data.setNormalData(final_res, store_data);
}


/**
* Constructor C_calculation
* @param storeData, The name of a variable will be updated after calculaltion
*/
C_calculation::C_calculation(string storeData) : store_data(storeData){}


/**
* calculation function for C_calculation class
* @param op, Operator objective used to call operation function defined by Op
* @param data, Data package used to get data and set the data
* @param ft, choose ft data from Data package
* @param ct1, choose ct-1 data from Data package
* @param gt, choose gt data from Data package
* @param it, choose it data from Data package
* @param time, loop times
*/
void C_calculation::calculation(Op& op, Data& data, string ft, string ct1, string gt, string it, int &time) {
	cout << "Forwarding " << store_data << " Calculation, loop time: " << time << endl;
	//variables used to calculate
	vector<vector<bin>> cur_ft = data._hash_normal(ft);
	vector<vector<bin>> cur_ct1 = data._hash_normal(ct1);
	vector<vector<bin>> cur_gt = data._hash_normal(gt);
	vector<vector<bin>> cur_it = data._hash_normal(it);
	int block_size = data.getBlockSize();
	//computing ft * ct-1
	vector<vector<bin>> fc_dotproduct;
	for (int i = 0; i < cur_ft.size(); i++)
	{
		vector<bin> dot_product = op.dotProduct(cur_ft[i], cur_ct1[i], time);
		fc_dotproduct.push_back(dot_product);
	}

	//computing gt * it
	vector<vector<bin>> gi_dotproduct;
	for (int i = 0; i < cur_gt.size(); i++)
	{
		vector<bin> dot_product = op.dotProduct(cur_gt[i], cur_it[i], time);
		gi_dotproduct.push_back(dot_product);
	}

	//computing final result
	vector<vector<bin>> final_res;
	for (int i = 0; i < fc_dotproduct.size(); i++)
	{
		vector<bin> res = op.addVector(fc_dotproduct[i], gi_dotproduct[i], time);
		final_res.push_back(res);
	}

	data.setNormalData(final_res, store_data);
}


/**
* constructor M_calculation
* @param storeData, The name of a variable will be updated after calculaltion
*/
M_calculation::M_calculation(string storeData):store_data(storeData){}


/**
* calculation function for M_calculation class
* @param op, Operator objective used to call operation function defined by Op
* @param data, Data package used to get data and set the data
* @param ot, choose ot data from Data package
* @param ct, choose ct data from Data package
* @param time, loop times
*/
void M_calculation::calculation(Op& op, Data& data, string ot, string ct, int &time) {
	cout << "Forwarding " << store_data << " Calculation, loop time: " << time << endl;
	//variables used to calculate
	vector<vector<bin>> cur_ot = data._hash_normal(ot);
	vector<vector<bin>> cur_ct = data._hash_normal(ct);
	int block_size = data.getBlockSize();

	vector<vector<bin>> cur_h_ct;
	op.h(cur_ct, cur_h_ct);

	vector<vector<bin>> final_res;
	for (int i = 0; i < cur_ot.size(); i++)
	{
		vector<bin> dot_res = op.dotProduct(cur_ot[i], cur_h_ct[i], time);
		final_res.push_back(dot_res);
	}
	data.setNormalData(final_res, store_data);
}


/**
* constructor Y_calculation
* @param storeData, The name of a variable will be updated after calculaltion
*/
Y_calculation::Y_calculation(string storeData): store_data(storeData){}


/**
* calculation function for M_calculation class
* @param op, Operator objective used to call operation function defined by Op
* @param data, Data package used to get data and set the data
* @param W_ym, choose W_ym data from Data package
* @param mt, choose mt data from Data package
* @param time, loop times
*/
void Y_calculation::calculation(Op& op, Data& data, string W_ym, string mt, int &time) {
	cout << "Forwarding " << store_data << " Calculation, loop time: " << time << endl;
	//variables used to calculate
	vector<vector<vector<bin_c>>> Weight_ym = data._hashWeight(W_ym);
	vector<vector<bin>> cur_mt = data._hash_normal(mt);
	int block_size = data.getBlockSize();

	//computer w_ym * m_t
	// compute FFT for m_t
	vector<vector<bin_c>> fft_m_t;
	for (int i = 0; i < cur_mt.size(); i++) {
		vector<bin_c> fftBlockOutput_mt(block_size);
		op.FFT_block(op.to_bin_c(cur_mt[i]), fftBlockOutput_mt, time);
		fft_m_t.push_back(fftBlockOutput_mt);
	}
	//compute dot product and IFFT for w_x and x_t
	vector<vector<bin>> wym_mt;
	for (int i = 0; i < Weight_ym.size(); i++) {
		vector<bin> blockAddResult(block_size);
		for (int j = 0; j < fft_m_t.size(); j++)
		{
			vector<bin_c> dotProductResult = op.dotProduct(fft_m_t[j], Weight_ym[i][j], time);
			vector<bin_c> ifftOutput(block_size);
			op.IFFT_block(dotProductResult, ifftOutput, time);
			blockAddResult = op.addVector(blockAddResult, op.to_bin(ifftOutput), time);
		}
		wym_mt.push_back(blockAddResult);
	}

	data.setNormalData(wym_mt, store_data);
}


/**
* constructor Update_calculation
* @param storeData, The name of a variable will be updated after calculaltion
*/
Update_calculation::Update_calculation(string storeData): store_data(storeData){}


/**
* calculation function for M_calculation class
* @param data, Data package used to get data and set the data
* @param cur_data, choose cur_data data from Data package
* @param time, loop times
*/
void Update_calculation::update(Data& data, string cur_data, int &time){
	cout << "Forwarding " << store_data << " Update, loop time: " << time << endl;
	vector<vector<bin>> cur_yt = data._hash_normal(cur_data);
	data.setNormalData(cur_yt, store_data);
}