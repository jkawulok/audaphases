// audaphases.cpp : Defines the entry point for the console application.
//
//

#include "stdafx.h"
#include "TimeSeries.h"

using namespace std;



void usage()
{
	cout << "\nSmile phases\n";
	cout << " ver. " << audaphases_VER << " (" << audaphases_DATE << ")\n";

	cout << "Usage:\n audaphases [options] \n";


	cout << "  -IN<name>\t- path and name of the input file\n";
	cout << "  -OUT<name>\t- path and prefix name of output\n";
	cout << "  -WIN<number>\t- size of the window (default: 27) \n";

	cout << "Example:\n";
	cout << "audaphases -IN./001_deliberate_smile_2_smile_intensities.txt -OUT./001_deliberate_smile_2_smile_   \n";
	cout << "audaphases -IN./smile_time_series.txt -OUT./OUT_smile_time_series_  -WIN9 \n";

};






int main(int argc, char* argv[])
{


	CFiles myfiles;

	CTimeSeries mydata;
	CTimeSeries trend;
	CTimeSeries trend2nd;
	CTimeSeries coeff;
	CTimeSeries trendCorrected;

	int a;
	scanf_s("%d", &a);

	for (int i = 1; i < argc; ++i)
	{
		if (argv[i][0] != '-')
			break;

		if ((strncmp(argv[i], "-h", 2) == 0) || (strncmp(argv[i], "help", 4) == 0) || (strncmp(argv[i], "-help", 5) == 0))
		{
			usage();
			return 0;

		}


		else if (strncmp(argv[i], "-IN", 3) == 0)
			myfiles.file_IN = (char*)(&argv[i][3]);
		else if (strncmp(argv[i], "-OUT", 4) == 0) // path and prefix out
			myfiles.file_OUT = (char*)(&argv[i][4]);
		else if (strncmp(argv[i], "-WIN", 4) == 0)
			myfiles.SetWin((atoi(&argv[i][4])));


	}



	if (!myfiles.OpenReadData())
	{
		usage();
		return 0;
	}


	mydata.SetData(&(myfiles.getvectordata())[0], myfiles.getlength());
	mydata.AnalyzeTrend(myfiles.getwin(), trend, coeff, trendCorrected);
	trend.AnalyzeTrend(myfiles.getwin(), trend2nd, coeff, trendCorrected);
	vector<TPhase> vectPhase = CTimeSeries::DetectPhases(mydata, trend, trend2nd);


	string mysuffix;
	mysuffix = "trend_" + to_string(myfiles.getwin());
	if (!myfiles.WriteNewData(trend.AccessData(), mysuffix))
	{
		return 0;
	}


	mysuffix = "phases";
	if (!myfiles.WriteNewData(vectPhase, mysuffix))
	{
		return 0;
	}



	cout << "The output files have been saved \n";




	return 0;




}


