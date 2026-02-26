#pragma once
#include "Color.h"
#include "ChartViewer.h"

class CChartHistogram :
	public CChartViewer
{
public:
	CChartHistogram();
	~CChartHistogram();

	void DrawChart();
	void trackLineLabel(XYChart *c, int mouseX, int mouseY);
	virtual void MouseMovePlotArea();
	virtual void ViewPortChanged();

	void SetData(double *px, double histoWidth, int nLen);
	void SetLogScale(BOOL bLog) { m_bScaleLog = bLog; DrawChart(); };
protected:
	double *m_px;
	double m_width;
	int m_nData;
	BOOL m_bScaleLog;
public:
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

