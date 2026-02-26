#pragma once
#include "./ChartDirect/ChartViewer.h"
class CChartPowerShape :
	public CChartViewer
{
public:
	CChartPowerShape();
	~CChartPowerShape();
	void DrawChart();
	void trackLineLabel(XYChart *c, int mouseX, int mouseY);
	virtual void MouseMovePlotArea();
	virtual void ViewPortChanged();
	double *m_pdata;
	double *m_px;
	int m_nData;
	CStringA m_strTitleX;
	void SetData(double *px, double *py, int nLen);
protected:

};

