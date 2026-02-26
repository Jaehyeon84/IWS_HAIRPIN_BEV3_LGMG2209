#pragma once
typedef struct _POINTDOUBLE {
	double x;
	double y;
} POINTDOUBLE, *PPOINTDOUBLE;

typedef std::vector<POINTDOUBLE> vector_point_dbl; 

class CLaserPen
{
public:
	CLaserPen();
	~CLaserPen();
	CLaserPen& operator=(const CLaserPen &rhs);
	static int m_nPen;
	int m_iPenID;
	double m_dblPower;
	double m_dblFrequency;
	double m_dblPulseWidth;
	double m_dblMarkSpeed;
	double m_dblJumpSpeed;
	short m_nMarkDelay;
	short m_nJumpDelay;
	short m_nPolygonDelay;
	short m_nLaserOnDelay;
	short m_nLaserOffDelay;
	BOOL m_bUseWobbel;
	double m_dWbTransbersal;
	double m_dWbLongitudinal;
	double m_dWbFreq;
	BOOL m_bUsePowerShape;
	BOOL m_bUsePulseShape;
	vector_point_dbl m_pwrShape;
	vector_point_dbl m_pulseShape;
	BYTE  m_iPWS;// pulse wave table; for spi������
	int	  m_nResPowerShape;
public:
	virtual void Serialize(CArchive& ar);
};

