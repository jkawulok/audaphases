#pragma once
#include "stdafx.h"
#include <string>
#include <list>

using namespace std;



#define audaphases_VER		"1.0"
#define audaphases_DATE	    "2020-08-06"


class CFiles
{
protected:

	vector<double> vectordata;	// input data
	int in_length;
	int n_wind = 27;

	FILE* OpenOutputFile(string suffix);

public:

	string file_IN="";
	string file_OUT="";

	CFiles();
	bool OpenReadData();

	bool WriteNewData(vector<TPhase>& dataout, string suffix);
	bool WriteNewData(double* dataout, string suffix);



	vector<double> getvectordata();	// input data
	int getlength();
	int getwin() {	return n_wind;	};
	void SetWin(int w);

	~CFiles();



};

