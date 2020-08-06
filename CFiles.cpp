#include "stdafx.h"
#include "CFiles.h"


CFiles::CFiles(): in_length(0) {};
CFiles::~CFiles(){}


bool CFiles::OpenReadData()
{
	FILE* pFile_in = NULL;
	errno_t err = fopen_s(&pFile_in, file_IN.c_str(), "rt");

	if (pFile_in == NULL)
	{
		perror("fopen");
		printf("%s \n", file_IN.c_str());
		return 0;
	}


	
	double singdata;
	while (fscanf_s(pFile_in, "%lg", &singdata) != EOF)
	{
		in_length++;
		vectordata.push_back(singdata);

	}

	fclose(pFile_in);

	return 1;
}

FILE* CFiles::OpenOutputFile(string suffix)
{
	FILE* pFile_ou = NULL;
	string OUTname = file_OUT + suffix + ".txt";

	errno_t err = fopen_s(&pFile_ou, OUTname.c_str(), "wt");

	if (pFile_ou == NULL)
	{
		perror("fopen");
		printf("%s \n", OUTname.c_str());
	}

	return pFile_ou;
}

bool CFiles::WriteNewData(double* dataout, string suffix)
{
	FILE* pFile_ou = OpenOutputFile(suffix);

	if (!pFile_ou)
		return false;

	for (int i = 0; i < in_length; i++)
		fprintf(pFile_ou, "%f\n", dataout[i]);

	fclose(pFile_ou);

	return true;
}

bool CFiles::WriteNewData(vector<TPhase>& dataout, string suffix)
{
	FILE* pFile_ou = OpenOutputFile(suffix);

	if (!pFile_ou)
		return false;

	for (int i = 0; i < in_length; i++)
		fprintf(pFile_ou, "%d\n", dataout[i]);

	fclose(pFile_ou);

	return true;
}

vector<double> CFiles::getvectordata()
{
	return vectordata;
}


int CFiles::getlength()
{
	return in_length;

}


void CFiles::SetWin(int w)
{
	n_wind = w;

};
