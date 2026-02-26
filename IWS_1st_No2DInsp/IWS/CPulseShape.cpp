#include "stdafx.h"
#include "CPulseShape.h"


CPulseShape::CPulseShape()
{
}


CPulseShape::~CPulseShape()
{
}

void CPulseShape::SetPulseShape(double dFreq, double dDuty, double power)
{
	m_shape_tp.clear();
	double dperiod = 1.0f / dFreq * 1000;// sec ==> usec
	double n = dperiod / PULSE_RESOLUTION;
	int n1 = int(n * dDuty + 0.5);
	int n2 = int(n) - n1;
	POINTDOUBLE dval;
	for (int i = 0; i < n2; i++) {
		dval.x = double(i* PULSE_RESOLUTION);
		dval.y = 0.0;
		m_shape_tp.push_back(dval);
	}
	for (int i = 0; i < n1; i++) {
		dval.x = double(i* PULSE_RESOLUTION);
		dval.y = power;
		m_shape_tp.push_back(dval);
	}
}
