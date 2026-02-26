#pragma once


// CStaticChart

class CStaticChart : public CStatic
{
	DECLARE_DYNAMIC(CStaticChart)
	void* m_pChart; //niVison <-> ChartViewer : Redefine error!!
public:
	CStaticChart();
	virtual ~CStaticChart();
	void SetData(double *px, double *py, int nData);
	void SetAxisTitle(CString sX, CString sY, CString sLegend);

	void DrawChart();
	double *m_py;
	double *m_px;
	int m_nData;

	CStringA m_strTitleX;
	CStringA m_strTitleY;
	CStringA m_strLegend;
protected:
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
};


