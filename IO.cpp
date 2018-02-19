/////////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: IO.h
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file implements funcion declarations in head file
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include "IO.h"

void IO::read_Training_Data(string folder, int time, Data& data, string dataTypes,int totalSize)
{
	ifstream fin(folder);
	if (!fin.is_open())
	{
		cout << "Error opening file"; exit(1);
	}

	string rowStr;
	vector<bin> dataVec;
	while (getline(fin, rowStr))
	{
		stringstream ss(rowStr);
		double buf;
		while (ss >> buf)
			dataVec.push_back(bin(buf));
	}

	//add 0 up to 160 (total size 160 * 1)
	for (int i = dataVec.size(); i < totalSize; i++)
	{
		double tmp = 0;
		dataVec.push_back(bin(0));
	}

	vector<vector<bin>> inputPic = partition(dataVec, data.getBlockSize());
	data.setNormalData(inputPic, dataTypes);
	fin.close();
}

void IO::read_Kernel_Weight_Input(string path, Data& data, Op& op)
{
	ifstream fin(path);
	if (!fin.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	int blockSize = data.getBlockSize();
	string rowStr;
	vector<bin> row;

	while (getline(fin, rowStr))
	{
		stringReplace(rowStr);
		stringstream ss(rowStr);
		double buf;
		while (ss >> buf)
			row.push_back(bin(buf));
	}
	vector<vector<bin>> start =  partition(row, blockSize);
	vector<vector<bin_c>> inputWeight;

	Weight_FFT(start, inputWeight, blockSize, op);

	set_Input_Weight(inputWeight, "Wix", data, 0, 639, 10);
	set_Input_Weight(inputWeight, "Wir", data, 640, 2687, 32);

	set_Input_Weight(inputWeight, "Wfx", data, 2688, 3327, 10);
	set_Input_Weight(inputWeight, "Wfr", data, 3328, 5375, 32);

	set_Input_Weight(inputWeight, "Wcx", data, 5376, 6015, 10);
	set_Input_Weight(inputWeight, "Wcr", data, 6016, 8063, 32);

	set_Input_Weight(inputWeight, "Wox", data, 8064, 8703, 10);
	set_Input_Weight(inputWeight, "Wor", data, 8704, 10751, 32);

	fin.close();
}

void IO::read_Bias_Input(string path, Data& data)
{
	vector<bin> bias_;
	ifstream fin(path);
	if (!fin.is_open())
	{
		cout << "Error opening file: " << path << endl;
		exit(0);
	}
	int blockSize = data.getBlockSize();
	string row;
	while (getline(fin, row)) {
		stringstream ss(row);
		double val;
		while (ss >> val) {
			bias_.push_back(bin(val));
		}
	}
	vector<vector<bin>> bias = partition(bias_, blockSize);
	set_Input_Bias(bias, "Bi", data, 0, 63);
	set_Input_Bias(bias, "Bf", data, 64, 127);
	set_Input_Bias(bias, "Bc", data, 128, 191);
	set_Input_Bias(bias, "Bo", data, 192, 255);
	fin.close();
}

void IO::set_Input_Bias(vector<vector<bin>>& sourceData, string dataType, Data& data, int start_index, int end_index)
{
	vector<vector<bin>> res;
	for (size_t i = start_index; i <= end_index; i++)
	{
		vector<bin> tmp;
		tmp.assign(sourceData[i].begin(), sourceData[i].end());
		res.push_back(tmp);
	}
	data.setNormalData(res, dataType);
}

void IO::set_Input_Weight(vector<vector<bin_c>>& sourceData, string dataType, Data& data, int start_index, int end_index, int number_in_row) {
	vector<vector<vector<bin_c>>> res;
	vector<vector<bin_c>> tempo;
	int k = 0;
	for (size_t i = start_index; i <= end_index; i++)
	{
		vector<bin_c> tmp;
		tmp.assign(sourceData[i].begin(), sourceData[i].end());
		tempo.push_back(tmp);
		k++;
		if (k == number_in_row)
		{
			res.push_back(tempo);
			tempo.clear();
			k = 0;
		}
	}
	data.setWeightData(res, dataType);
}

void IO::read_Diag_Weight_Input(string path, Data& data, string dataType)
{
	vector<bin> diag_;
	ifstream fin(path);
	if (!fin.is_open())
	{
		cout << "Error opening file: " << path << endl;
		exit(0);
	}
	int blockSize = data.getBlockSize();
	string row;
	while (getline(fin, row)) {
		stringstream ss(row);
		double val;
		while (ss >> val) {
			diag_.push_back(bin(val));
		}
	}
	vector<vector<bin>> diag_weight = partition(diag_, blockSize);
	data.setNormalData(diag_weight, dataType);
	fin.close();
}

void IO::read_Weight_ym(string path, Data& data, Op& op)
{
	ifstream fin(path);
	if (!fin.is_open())
	{
		cout << "Error opening file"; exit(1);
	}
	int blockSize = data.getBlockSize();
	string rowStr;
	vector<bin> row;
	while (getline(fin, rowStr))
	{
		stringReplace(rowStr);
		stringstream ss(rowStr);
		double buf;
		while (ss >> buf)
			row.push_back(bin(buf));
	}
	vector<vector<bin_c>> inputWeight;
	Weight_FFT(partition(row, blockSize), inputWeight, blockSize, op);
	set_Input_Weight(inputWeight, "Wym", data, 0, 2047, 64);
	fin.close();
}

void IO::Weight_FFT(vector<vector<bin>>& input, vector<vector<bin_c>>& output, int blockSize_, Op& op)
{
	for (int i = 0; i < input.size(); i++) {
		vector<bin_c> fftBlockOutput(blockSize_);

		op.FFT_block(op.to_bin_c(input[i]), fftBlockOutput, -1);

		output.push_back(fftBlockOutput);
	}
}

void IO::stringReplace(string& input) {
	while (true) {
		string::size_type pos(0);
		if ((pos = input.find(',')) != string::npos)
			input.replace(pos, 1, " ");
		else   
			break;
	}
}

//IO module test
#ifdef _TEST_IO
int main()
{
	//initialize
	Data lstm_data(16);
	IO lstm_io;
	Op operate;
	//define size
	int input_total_size = 160;

	//test read and set data
	string lstm_x_path = "data/DBRNN_1-fw-custom_lstm_xi.txt";
	lstm_io.read_Training_Data(lstm_x_path, 0, lstm_data, "xt", input_total_size);
	
	string lstm_kernel_weight_path = "data/DBRNN_1-fw-custom_lstm_cell-kernel(Weights).txt";
	lstm_io.read_Kernel_Weight_Input(lstm_kernel_weight_path, lstm_data, operate);

	string lstm_bias_path_bi = "data/DBRNN_1-fw-custom_lstm_cell-bias.txt";
	lstm_io.read_Bias_Input(lstm_bias_path_bi, lstm_data);

	string read_diag_weight_path_wic = "data/DBRNN_1-fw-custom_lstm_cell-w_i_diag.txt";
	lstm_io.read_Diag_Weight_Input(read_diag_weight_path_wic, lstm_data, "Wic");

	string read_diag_weight_path_wym = "data/DBRNN_1-fw-custom_lstm_cell-projection-kernel(Wym).txt";
	lstm_io.read_Weight_ym(read_diag_weight_path_wym, lstm_data, operate);

	//test output
	vector<vector<bin>> xt = lstm_data.getNormalData("xt");
	cout << xt[0][0].to_string() << endl;
	vector<vector<vector<bin_c>>> wix = lstm_data.getWeightData("Wix");
	cout << wix[0][0][0].to_double_str() << endl;
	vector<vector<vector<bin_c>>> wir = lstm_data.getWeightData("Wir");
	cout << wir[0][0][0].to_double_str() << endl;
	vector<vector<vector<bin_c>>> wym = lstm_data.getWeightData("Wym");
	cout << wym[0][0][0].to_string() << endl;
	vector<vector<bin>> bi = lstm_data.getNormalData("Bi");
	cout << bi[0][0].to_string() << endl;
	vector<vector<bin>> wic = lstm_data.getNormalData("Wic");
	cout << wic[0][0].to_string() << endl;

}
#endif

