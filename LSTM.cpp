/////////////////////////////////////////////////////////////////
//Project name: LSTM Simulator for Inference, Beta 1.0 Version
//File name: LSTM.cpp
//Author: Jiayu Li
//Date: 02/11/2018
//Discription: This file implements calculation steps in LSTM
//All Rights Reserved 
/////////////////////////////////////////////////////////////////////
#include "Data.h"
#include "Op.h"
#include "IO.h"
#include "Calculation.h"

#ifdef _INFERENCE
int main()
{
	//initialize
	Data lstm_data(16);
	IO lstm_io;
	Op operate;


	//test read and set data
	//read xt
	//define size
	int input_total_size_xi = 160;
	string lstm_x_path = "data/DBRNN_1-fw-custom_lstm_xi.txt";
	lstm_io.read_Training_Data(lstm_x_path, 0, lstm_data, "xt", input_total_size_xi);
	//read yt-1
	//define size
	int input_total_size_yt1 = 512;
	string lstm_yt1_path = "data/DBRNN_1-fw-custom_lstm_yt-1.txt";
	lstm_io.read_Training_Data(lstm_yt1_path, 0, lstm_data, "yt-1", input_total_size_yt1);
	//read ct-1
	//define size
	int input_total_size_ct1 = 1024;
	string lstm_ct1_path = "data/DBRNN_1-fw-custom_lstm_ct-1.txt";
	lstm_io.read_Training_Data(lstm_ct1_path, 0, lstm_data, "ct-1", input_total_size_ct1);

    //read kernel weight
	string lstm_kernel_weight_path = "data/DBRNN_1-fw-custom_lstm_cell-kernel(Weights).txt";
	lstm_io.read_Kernel_Weight_Input(lstm_kernel_weight_path, lstm_data, operate);
	//read Wym data
	string read_diag_weight_path_wym = "data/DBRNN_1-fw-custom_lstm_cell-projection-kernel(Wym).txt";
	lstm_io.read_Weight_ym(read_diag_weight_path_wym, lstm_data, operate);
	//read bias
	string lstm_bias_path_bi = "data/DBRNN_1-fw-custom_lstm_cell-bias.txt";
	lstm_io.read_Bias_Input(lstm_bias_path_bi, lstm_data);
	//read diag weight
	string read_diag_weight_path_wic = "data/DBRNN_1-fw-custom_lstm_cell-w_i_diag.txt";
	lstm_io.read_Diag_Weight_Input(read_diag_weight_path_wic, lstm_data, "Wic");

	string read_diag_weight_path_wfc = "data/DBRNN_1-fw-custom_lstm_cell-w_f_diag.txt";
	lstm_io.read_Diag_Weight_Input(read_diag_weight_path_wfc, lstm_data, "Wfc");

	string read_diag_weight_path_woc = "data/DBRNN_1-fw-custom_lstm_cell-w_o_diag.txt";
	lstm_io.read_Diag_Weight_Input(read_diag_weight_path_woc, lstm_data, "Woc");

	//initialize calculations
	Update_calculation yt1("yt-1");
	Update_calculation ct1("ct-1");
	IFOG_calculation it("It");
	IFOG_calculation ft("Ft");
	IFOG_calculation gt("Gt");
	C_calculation ct("ct");
	IFOG_calculation ot("Ot");
	M_calculation mt("Mt");
	Y_calculation yt("yt");

	//computing inference
	for (int time = 0; time < 5; time++)
	{
		cout << "\nRunning " << time << "th time loop!" << endl;
		if (time != 0) {
			yt1.update(lstm_data, "yt", time);
			ct1.update(lstm_data, "ct", time);
		}
		it.calculation(operate, lstm_data, "Wix", "Wir", "Wic", "Bi", "ct-1", false, time);
		ft.calculation(operate, lstm_data, "Wfx", "Wfr", "Wfc", "Bf", "ct-1", false, time);
		gt.calculation(operate, lstm_data, "Wcx", "Wcr", "", "Bc", "", true, time);
		ct.calculation(operate, lstm_data, "Ft", "ct-1", "Gt", "It", time);
		ot.calculation(operate, lstm_data, "Wox", "Wor", "Woc", "Bo", "ct", false, time);
		mt.calculation(operate, lstm_data, "Ot", "ct", time);
		yt.calculation(operate, lstm_data, "Wym", "Mt", time);
	}
}
#endif

