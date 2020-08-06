#include "StdAfx.h"

#include "TimeSeries.h"
#include "linear.h"
#include <list>


#include <ctime>

CTimeSeries::CTimeSeries()
	: m_data(NULL) // input data
	, m_length(0)
{
}

//CTimeSeries::CTimeSeries(int length, int fps, double initValue)
//	: m_length(length)
//	, m_fps(fps)
//{
//	m_data = new double[m_length];
//	for(int i = 0; i < m_length; i++)
//		m_data[i] = initValue;
//}
//
//CTimeSeries::CTimeSeries(const CTimeSeries& ts)
//	: m_data(NULL)
//	, m_length(0)
//	, m_fps(0)
//{
//	*this = ts;
//}


CTimeSeries::~CTimeSeries(void)
{
	if(m_data)
		delete [] m_data;
	m_data = NULL;
	m_length = 0;

	auto it = m_trends.begin();
	for(; it != m_trends.end(); ++it)
	{
		if((*it).second._trend)
			delete (*it).second._trend;
		if((*it).second._coeffs)
			delete (*it).second._coeffs;
		if((*it).second._corrected)
			delete (*it).second._corrected;
	}
}



bool CTimeSeries::SetData(double* data, int length)
{
	if(length <= 0)
		return false;

	if(m_data)
		delete [] m_data;
	m_data = new double[length];
	m_length = length;
	if(data == NULL)
	{
		for(int i = 0; i < m_length; i++)
			m_data[i] = -1;
	}
	else
	{
		memcpy(m_data, data, length * sizeof(double));

		
	}

	return true;
}


bool CTimeSeries::AnalyzeTrend(int window, CTimeSeries& trend, CTimeSeries& coeff, CTimeSeries& trendCorrected)
{
	trend.m_length = m_length;
	coeff.m_length = m_length;
	trendCorrected.m_length = m_length;
	trend.m_data = new double[m_length];
	coeff.m_data = new double[m_length];
	trendCorrected.m_data = new double[m_length];

	double* x = new double[window];
	for (int i = 0; i < window; i++)
		x[i] = i;


	double* _dataEnhanced = new double[m_length + 2 * (window / 2)];
	for(int i = 0; i < window / 2; ++i)
		_dataEnhanced[i] = m_data[0];
	::memcpy(_dataEnhanced + window / 2, m_data, m_length * sizeof(double));
	for(int i = m_length; i < m_length + 2 * (window / 2); ++i)
		_dataEnhanced[i] = m_data[m_length - 1];

	int a = 0;
	for(int i = 0; i < m_length; i++, a++)
	{
		double* y = _dataEnhanced;
		int n = window + a;

		if(a > 0)
		{
			y = _dataEnhanced + a;
			n = window;
		}


		Maths::Regression::Linear A(n, x, y);
		trend.m_data[i] = A.getSlope();
		coeff.m_data[i] = A.getCoefficient();
		trendCorrected.m_data[i] = trend.m_data[i] * fabs(coeff.m_data[i]);
	}

	delete [] x;
	delete [] _dataEnhanced;

	return true;
}





std::vector<TPhase> CTimeSeries::DetectPhases(CTimeSeries& values, CTimeSeries& deltas, CTimeSeries& deltasDeriv)
{
	const int stability_check = 3;


	std::vector<TPhase> vectPhase;
	vectPhase.resize(values.GetLength(), TPhase::none);
	std::vector<double> vectVal;
	vectVal.resize(values.GetLength());
	std::vector<double> vectDelta;
	vectDelta.resize(values.GetLength());

	int i_start = 0;

	double _valOnset_acum = 0;

	while (i_start < values.GetLength())
	{
		int _countPos = 0;
		for (int i = i_start; i < values.GetLength(); ++i, ++i_start)
		{
			double _delta = deltas.AccessData()[i];
			if (_delta > 0)
			{
				_countPos++;
			}
			else
			{
				_countPos = 0;
				vectPhase[i] = TPhase::none;
			}
			if (_countPos == stability_check)
			{
				i_start -= (_countPos - 1);
				break;
			}
		}
		if (i_start >= values.GetLength())
			break;
		double _val_start = values.AccessData()[i_start];

		bool _correct = true;
		double _deltaDerPos_min = 0;
		double _deltaDerNeg_min = 0;
		double _delta_max = 0;
		double _delta_min = 0;

		int i_deltaDerPos_min = -1;
		int i_deltaDerNeg_min = -1;
		int i_delta_max = -1;
		int i_delta_min = -1;

		int i_endPos = -1;
		int i_endSearch = -1;
		int i_endOffset = values.GetLength();

		double _val_ref = values.AccessData()[i_start];

		int _countFinal = 0;
		double _val_max = values.AccessData()[i_start];
		bool _finalSearch = false;
		i_deltaDerPos_min = i_start;
		for (int i = i_start; i < values.GetLength(); ++i)
		{
			double _val = values.AccessData()[i];
			double _delta = deltas.AccessData()[i];

			if (_delta > _delta_max)
			{
				_delta_max = _delta;
				i_delta_max = i;
			}

			if (_delta < _delta_min)
			{
				_delta_min = _delta;
				i_delta_min = i;
			}

			if (_val > _val_max)
			{
				_val_max = _val;
				_val_ref = (values.AccessData()[i_start] + _val_max) / 2;
			}
			if (_val < _val_ref && _delta < 0)
			{
				_finalSearch = true;
			}
			if (_finalSearch)
				if (_delta > 0)
				{
					_countFinal++;
				}
				else
					_countFinal = 0;

			if (_finalSearch && _countFinal >= stability_check)
			{
				i_endOffset = i - _countFinal;
				break;
			}
		}
		if (_delta_max <= _delta_min)
			_correct = false;
		if (i_endOffset < 0)
			i_endSearch = i_delta_min;
		else
			i_endSearch = i_endOffset;

		double _deltaDer_prev = deltasDeriv.AccessData()[i_delta_max];;
		for (int i = i_delta_max; i < i_delta_min; ++i)
		{
			double _val = values.AccessData()[i];
			double _delta = deltas.AccessData()[i];
			double _deltaDer = deltasDeriv.AccessData()[i];

			if (i > i_delta_max && _deltaDer_prev < _deltaDer)
			{
				_deltaDerPos_min = _deltaDer_prev;
				i_deltaDerPos_min = i - 1;
				break;
			}
			_deltaDer_prev = _deltaDer;
		}

		_deltaDer_prev = deltasDeriv.AccessData()[i_delta_min - 1];;
		for (int i = i_delta_min - 1; i > i_delta_max; --i)
		{
			double _val = values.AccessData()[i];
			double _delta = deltas.AccessData()[i];
			double _deltaDer = deltasDeriv.AccessData()[i];

			if (i < i_delta_min - 1 && _deltaDer_prev < _deltaDer)
			{
				_deltaDerNeg_min = _deltaDer_prev;
				i_deltaDerNeg_min = i + 1;
				break;
			}
			_deltaDer_prev = _deltaDer;
		}

		int i_startApex = i_deltaDerPos_min;
		int i_startOffset = i_deltaDerNeg_min;
		if (i_startApex == i_startOffset)
		{
			if (i_startApex - i_delta_max > i_delta_min - i_startApex)
				i_startApex = i_delta_max;
			else
				i_startOffset = i_delta_min;
		}

		if (i_start >= i_startApex || i_startApex >= i_startOffset || i_startOffset >= i_endOffset)
		{
			_correct = false;
		}
		if (i_endOffset - i_start < BASELINE_FPS)
		{
			i_endSearch = i_endOffset;
			_correct = false;
		}

		if (_correct)
		{
			for (int i = i_start; i < i_startApex; ++i)
			{
				vectPhase[i] = TPhase::onset;
			}
			for (int i = i_startApex; i < i_startOffset; ++i)
			{
				vectPhase[i] = TPhase::apex;
			}
			for (int i = i_startOffset; i < i_endOffset; ++i)
			{
				vectPhase[i] = TPhase::offset;
			}
			i_endSearch = i_endOffset;
		}
		else
			for (int i = i_start; i < i_endSearch; ++i)
			{
				vectPhase[i] = TPhase::none;
			}

		i_start = i_endSearch;
	}



	return vectPhase;
}