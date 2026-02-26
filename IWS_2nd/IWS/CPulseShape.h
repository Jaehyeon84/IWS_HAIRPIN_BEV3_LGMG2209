#pragma once
#include <vector>

class CPulseShape
{
public:
	CPulseShape();
	~CPulseShape();

	void SetPulseShape(double dFreq, double dDuty, double power);
	vector_point_dbl m_shape_tp; // time vs power
	vector_point_dbl m_shape_dt; // distance power
};

