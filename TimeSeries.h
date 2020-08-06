#pragma once
#include <vector>
#include <math.h>       /* log10 */
#include <map>
typedef enum { none, onset, apex, offset, ignored } TPhase;

// #include <opencv2/highgui/highgui.hpp>

#define BASELINE_FPS 50

class CTimeSeries
{
public:

	CTimeSeries();
//	CTimeSeries(const CTimeSeries& ts);
//	CTimeSeries(int length, int fps, double initValue = -1);

	virtual ~CTimeSeries(void);

	bool SetData(double* data, int length);
	double* AccessData() { return m_data; };
	int GetLength() { return m_length; };
//	int GetFPS() {return m_fps;}
	bool AnalyzeTrend(int window, CTimeSeries& trend, CTimeSeries& coeff, CTimeSeries& trendCorrected); 	

	struct trend
	{
		CTimeSeries* _trend;
		CTimeSeries* _coeffs;
		CTimeSeries* _corrected;
	};

	static std::vector<TPhase> DetectPhases(CTimeSeries& values, CTimeSeries& deltas, CTimeSeries& deltasDeriv);

protected:
	double* m_data; // input data

	int m_length;
//	int m_fps;
	std::map<int, trend> m_trends;


};

