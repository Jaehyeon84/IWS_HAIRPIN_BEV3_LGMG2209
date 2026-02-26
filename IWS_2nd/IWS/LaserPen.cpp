#include "stdafx.h"
#include "LaserPen.h"
#include <vector>
#include <algorithm>

int CLaserPen::m_nPen = 0;

CLaserPen::CLaserPen()
{
	m_iPenID = m_nPen;
	CLaserPen::m_nPen++;
	m_dblFrequency = 60.0;
	m_dblPulseWidth = 0.0;
	m_dblMarkSpeed = 300;
	m_dblJumpSpeed = 2500;
	m_nMarkDelay = 0;
	m_nJumpDelay = 0;
	m_nPolygonDelay = 0;
	m_nLaserOnDelay = 0;
	m_nLaserOffDelay = 0;
	m_dblPower = 0;
	m_iPWS = 11;
	m_bUseWobbel = FALSE;
	m_dWbFreq = 0;
	m_dWbLongitudinal = 0;
	m_dWbTransbersal = 0;
	m_bUsePowerShape = FALSE;
	m_nResPowerShape = 100;
	m_bUsePulseShape = FALSE;
}


CLaserPen::~CLaserPen()
{
}

CLaserPen& CLaserPen::operator=(const CLaserPen &rhs)
{
	this->m_iPenID			= rhs.m_iPenID;
	this->m_dblPower = rhs.m_dblPower;
	this->m_dblFrequency	= rhs.m_dblFrequency;
	this->m_dblPulseWidth	= rhs.m_dblPulseWidth;
	this->m_dblMarkSpeed	= rhs.m_dblMarkSpeed;
	this->m_dblJumpSpeed	= rhs.m_dblJumpSpeed;
	this->m_nMarkDelay		= rhs.m_nMarkDelay;
	this->m_nJumpDelay		= rhs.m_nJumpDelay;
	this->m_nPolygonDelay	= rhs.m_nPolygonDelay;
	this->m_nLaserOnDelay	= rhs.m_nLaserOnDelay;
	this->m_nLaserOffDelay	= rhs.m_nLaserOffDelay;
	this->m_iPWS			= rhs.m_iPWS;
	this->m_bUseWobbel = rhs.m_bUseWobbel;
	this->m_dWbLongitudinal = rhs.m_dWbLongitudinal;
	this->m_dWbTransbersal = rhs.m_dWbTransbersal;
	this->m_dWbFreq = rhs.m_dWbFreq;
	this->m_bUsePowerShape = rhs.m_bUsePowerShape;
	if (rhs.m_bUsePowerShape) {
		this->m_pwrShape.resize(rhs.m_pwrShape.size());
		std::copy(rhs.m_pwrShape.begin(), rhs.m_pwrShape.end(), this->m_pwrShape.begin());
	}
	else {
		POINTDOUBLE v;
		v.x = 0.0; v.y = rhs.m_dblPower; this->m_pwrShape.push_back(v);
		v.x = 100.0; v.y = rhs.m_dblPower; this->m_pwrShape.push_back(v);
	}
	this->m_nResPowerShape = rhs.m_nResPowerShape;
	this->m_bUsePulseShape = rhs.m_bUsePulseShape;
	if (rhs.m_bUsePulseShape) {
		this->m_pulseShape.resize(rhs.m_pulseShape.size());
		std::copy(rhs.m_pulseShape.begin(), rhs.m_pulseShape.end(), this->m_pulseShape.begin());
	}
	else {
		POINTDOUBLE v;
		v.x = 0.0; v.y = rhs.m_dblPower; this->m_pulseShape.push_back(v);
		v.x = 100.0; v.y = rhs.m_dblPower; this->m_pulseShape.push_back(v);
	}
	return *this;
}

void CLaserPen::Serialize(CArchive& ar)
{
	if (ar.IsStoring()) {
		ar<<m_iPenID;
		ar << m_dblPower;
		ar<<m_dblFrequency;
		ar<<m_dblPulseWidth;
		ar<<m_dblMarkSpeed;
		ar<<m_dblJumpSpeed;
		ar<<m_nMarkDelay;
		ar<<m_nJumpDelay;
		ar<<m_nPolygonDelay;
		ar<<m_nLaserOnDelay;
		ar<<m_nLaserOffDelay;
		ar << m_bUseWobbel;
		ar << m_dWbFreq;
		ar << m_dWbLongitudinal;
		ar << m_dWbTransbersal;
		ar << m_bUsePowerShape;
		ar << m_bUsePulseShape;
		if (m_bUsePowerShape) {
			ar << m_pwrShape.size();
			for (auto p : m_pwrShape) {
				ar << p.x;
				ar << p.y;
			}
		}
		if (m_bUsePulseShape) {
			ar << m_pulseShape.size();
			for (auto p : m_pulseShape) {
				ar << p.x;
				ar << p.y;
			}
		}
	}
	else {
		ar >> m_iPenID;
		ar >> m_dblPower;
		ar >> m_dblFrequency;
		ar >> m_dblPulseWidth;
		ar >> m_dblMarkSpeed;
		ar >> m_dblJumpSpeed;
		ar >> m_nMarkDelay;
		ar >> m_nJumpDelay;
		ar >> m_nPolygonDelay;
		ar >> m_nLaserOnDelay;
		ar >> m_nLaserOffDelay;
		ar >> m_bUseWobbel;
		ar >> m_dWbFreq;
		ar >> m_dWbLongitudinal;
		ar >> m_dWbTransbersal;

		ar >> m_bUsePowerShape;
		ar >> m_bUsePulseShape;
		if (m_bUsePowerShape) {
			size_t nShape = 0;
			ar >> nShape;
			m_pwrShape.clear();
			for (int i = 0; i < nShape;i++) {
				POINTDOUBLE p;
				ar >> p.x;
				ar >> p.y;
				m_pwrShape.push_back(p);
			}
		}
		if (m_bUsePulseShape) {
			size_t nShape = 0;
			ar >> nShape;
			m_pulseShape.clear();
			for (int i = 0; i < nShape; i++) {
				POINTDOUBLE p;
				ar >> p.x;
				ar >> p.y;
				m_pulseShape.push_back(p);
			}
		}

	}
}


