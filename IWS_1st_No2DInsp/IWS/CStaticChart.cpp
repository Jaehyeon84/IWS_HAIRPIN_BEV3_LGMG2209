// CStaticChart.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CStaticChart.h"

#include "./ChartDirect/ChartViewer.h"

#include <sstream>
#include <vector>
using namespace std;
// CStaticChart

IMPLEMENT_DYNAMIC(CStaticChart, CStatic)

CStaticChart::CStaticChart()
{
	m_pChart = NULL;


	m_px = new double[2];
	m_py = new double[2];
	m_nData = 2;

	m_px[0] = 0.0;
	m_px[1] = 0.0;

	m_py[0] = 0.0;
	m_py[1] = 10.0;

	m_strTitleX = "AxisX";
	m_strTitleY = "AxisY";
	m_strLegend = "Data";
}

CStaticChart::~CStaticChart()
{
	if (m_pChart != NULL)
	{
		delete ((CChartViewer*)m_pChart)->getChart();
		delete ((CChartViewer*)m_pChart);
		m_pChart = NULL;
	}
	if (m_py) {delete[] m_py; m_py = NULL;}
	if (m_px) {delete[] m_px; m_px = NULL;}
}


BEGIN_MESSAGE_MAP(CStaticChart, CStatic)
END_MESSAGE_MAP()



// CStaticChart 메시지 처리기


void CStaticChart::SetAxisTitle(CString sX, CString sY, CString sLegend)
{
	m_strTitleX = CStringA(sX);
	m_strTitleY = CStringA(sY);
	m_strLegend = CStringA(sLegend);
	DrawChart();
}

void CStaticChart::SetData(double *px, double *py, int nData)
{
	m_nData = 0;
	if (m_py) { delete[] m_py; m_py = NULL; }
	if (m_px) { delete[] m_px; m_px = NULL; }
	m_px = new double[nData];
	m_py = new double[nData];
	memcpy(m_px, px, sizeof(double)*nData);
	memcpy(m_py, py, sizeof(double)*nData);
	m_nData = nData;
	DrawChart();
}
void CStaticChart::DrawChart()
{
	CChartViewer *viewer = (CChartViewer *)m_pChart;
	CRect rect;
	GetClientRect(rect);
	// Create a XYChart object of size 640 x 400 pixels
	XYChart *c = new XYChart(rect.Width(), rect.Height());

	// Add a title to the chart using 18 pts Times New Roman Bold Italic font
	//c->addTitle("Spectrum", "timesbi.ttf", 18);

	// Set the plotarea at (50, 55) with width 70 pixels less than chart width, and height 90 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	PlotArea *plotArea = c->setPlotArea(70, 30, c->getWidth() - 90, c->getHeight() - 80, COLOR_WHITE, -1, COLOR_GREY41,
		c->dashLineColor(COLOR_GREY41, DotLine), c->dashLineColor(COLOR_GREY41, DotLine));
	c->setBackground(COLOR_WHITE);
	c->setBorder(COLOR_GREY41);

	ostringstream legend;
	legend << "<*size=10*><*img=@Square,color=0xff3333,width=10,height=10,edgeColor=0x000000*>" << m_strLegend << "    ";

	DrawArea *d = c->getDrawArea();
	TTFText *t = d->text(legend.str().c_str(), "arial.ttf", 10);
	t->draw((plotArea->getLeftX() + plotArea->getRightX()) / 2, plotArea->getTopY() - 5, COLOR_DARKSLATEBLUE, Chart::BottomCenter);
	t->destroy();
	// Enable clipping mode to clip the part of the data that is outside the plot area.
	c->setClipping();

	// Set axis label style to 8pts Arial Bold
	c->xAxis()->setLabelStyle("arialbd.ttf", 10);
	c->yAxis()->setLabelStyle("arialbd.ttf", 10);

	// Set the axis stem to transparent
	c->xAxis()->setColors(Chart::Transparent, COLOR_GREY41, COLOR_GREY41, COLOR_GREY41);
	c->yAxis()->setColors(Chart::Transparent, COLOR_GREY41, COLOR_GREY41, COLOR_GREY41);
	// Configure x-axis label format
	// 	c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "{value|mm/yyyy} ",
	// 		Chart::StartOfMonthFilter(), "{value|mm}");

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle(m_strTitleY, "arialbi.ttf", 10, COLOR_GREY41);
	c->xAxis()->setTitle(m_strTitleX, "arialbi.ttf", 10, COLOR_GREY41);
	// Add a line layer to the chart using a line width of 2 pixels.

	// Add 3 data series to the line layer
	if (m_px) {
		LineLayer *layer = c->addLineLayer();
		layer->setLineWidth(1);
		DoubleArray xdata(m_px, m_nData);
		layer->setXData(xdata);

		DoubleArray data0; 

		if (m_py) {
			data0.data = m_py;
			data0.len = m_nData;
		}

		double dYMin = FLT_MAX;
		double dYMax = -FLT_MAX;
		double dCur;
		for (int i = 0; i < m_nData; i++)
		{
			dCur = m_py[i];
			if (dCur > dYMax) { dYMax = dCur; }
			if (dCur < dYMin) { dYMin = dCur; }
		}

		layer->addDataSet(data0, 0xff3333, m_strLegend);
		viewer->syncLinearAxisWithViewPort(m_strTitleX, c->xAxis());
		viewer->syncLinearAxisWithViewPort(m_strTitleY, c->yAxis());
		viewer->setFullRange(m_strTitleY, dYMin*1.1, dYMax*1.1);
		viewer->setFullRange(m_strTitleX, m_px[0], m_px[m_nData - 1]);
		c->yAxis()->setAutoScale(0,0.0,0.0);
		// 		c->xAxis()->setRounding(false, false);
	}

	delete viewer->getChart();
	// Assign the chart to the WinChartViewer
	viewer->setChart(c);
}


void CStaticChart::PreSubclassWindow()
{
	int nID = this->GetDlgCtrlID();
	CRect rect;
	GetClientRect(&rect);


	CChartViewer* pChart = (new CChartViewer());

	pChart->setMouseUsage(Chart::MouseUsageDefault);
	pChart->setScrollDirection(Chart::DirectionHorizontalVertical);
	pChart->setZoomDirection(Chart::DirectionHorizontalVertical);
	setLicenseCode("RDST-269V-GQM2-X8BZ-A621-8B39");
	pChart->Create(_T("Histo chart"), WS_CHILD | SS_BITMAP | WS_VISIBLE | WS_EX_CLIENTEDGE, rect, this, nID);
	pChart->ShowWindow(SW_SHOW);
	
	CStatic::PreSubclassWindow();

	m_pChart = (void*)pChart;
	DrawChart();
}
