#include "stdafx.h"
#include "ChartHistogram.h"
#include <sstream>
#include <vector>
using namespace std;

CChartHistogram::CChartHistogram()
{
	setMouseUsage(Chart::MouseUsageDefault);
	setScrollDirection(Chart::DirectionHorizontalVertical);
	setZoomDirection(Chart::DirectionHorizontalVertical);

	m_nData = 0;
	m_px = NULL;
	m_bScaleLog = TRUE;
	setLicenseCode("RDST-269V-GQM2-X8BZ-A621-8B39");
}


CChartHistogram::~CChartHistogram()
{
	delete getChart();
}

BEGIN_MESSAGE_MAP(CChartHistogram, CChartViewer)
	ON_WM_SIZE()
END_MESSAGE_MAP()

void CChartHistogram::DrawChart()
{
	CChartViewer *viewer = (CChartViewer *)this;
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

// 	ostringstream legend;
// 	legend << "<*size=10*><*img=@Square,color=0xff3333,width=10,height=10,edgeColor=0x000000*>" << " Reference" << "    ";
// 	legend << "<*size=10*><*img=@Square,color=0x008800,width=10,height=10,edgeColor=0x000000*>" << " Dark" << "    ";
// 	legend << "<*size=10*><*img=@Square,color=0x3333cc,width=10,height=10,edgeColor=0x000000*>" << " Measure" << "    ";
// 	legend << "<*size=10*><*img=@Square,color=0x708090,width=10,height=10,edgeColor=0x000000*>" << " Live";
// 
// 	DrawArea *d = c->getDrawArea();
// 	TTFText *t = d->text(legend.str().c_str(), "arial.ttf", 10);
// 	t->draw((plotArea->getLeftX() + plotArea->getRightX()) / 2, plotArea->getTopY() - 5, COLOR_DARKSLATEBLUE, Chart::BottomCenter);
// 	t->destroy();
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
	c->yAxis()->setTitle("Count", "arialbi.ttf", 10, COLOR_GREY41);
	c->xAxis()->setTitle("Intensity", "arialbi.ttf", 10, COLOR_GREY41);
	// Add a line layer to the chart using a line width of 2 pixels.

	// Add 3 data series to the line layer
	if (m_px) {
		double *xx = new double[m_nData];
		int idx_min, idx_max;
		for (int i = 0; i < m_nData; i++) 			xx[i] = m_width * i;
		
		idx_min = 0;
		for (int i = 0; i < m_nData; i++) {
			if (m_px[i] > 1) {
				idx_min = i;
				break;
			}
		}

		idx_max = m_nData;
		for (int i = m_nData-1; i > 0 ; i--) {
			if (m_px[i] > 1) {
				idx_max = i;
				break;
			}
		}

		DoubleArray dataHisto = DoubleArray(m_px, m_nData);
		BarLayer *histogramLayer = c->addBarLayer(dataHisto, 0x6699bb);
		histogramLayer->setBorderColor(0x336688);
		// The center of the bars span from minX + half_bar_width to maxX - half_bar_width
		histogramLayer->setXData(DoubleArray(xx,m_nData));

		// Configure the bars to touch each other with no gap in between
		histogramLayer->setBarGap(Chart::TouchBar);
		// Use rounded corners for decoration
		histogramLayer->setRoundedCorners();
		// Tool tip for the histogram
		histogramLayer->setHTMLImageMap("", "", "title='{value}'");
		
		// ChartDirector by default will extend the x-axis scale by 0.5 unit to cater for the bar width.
		// It is because a bar plotted at x actually occupies (x +/- half_bar_width), and the bar width
		// is normally 1 for label based x-axis. However, this chart is using a linear x-axis instead of
		// label based. So we disable the automatic extension and add a dummy layer to extend the x-axis
		// scale to cover minX to maxX.
		c->xAxis()->setIndent(false); 
		//c->addLineLayer()->setXData(idx_min*m_width,(idx_max)*m_width);

		// For the automatic y-axis labels, set the minimum spacing to 40 pixels.
		c->yAxis()->setTickDensity(40);
		if(m_bScaleLog)			c->yAxis()->setLogScale();
		viewer->syncLinearAxisWithViewPort("Intensity", c->xAxis());
//		viewer->syncLogAxisWithViewPort("Count", c->yAxis());
// 		if (m_bScaleLog)			
// 			viewer->syncLogAxisWithViewPort("Count", c->yAxis());
// 		else
//			viewer->syncLinearAxisWithViewPort("Count", c->yAxis());
		ArrayMath m = ArrayMath(dataHisto);

		viewer->setFullRange("Count",m.minValue(),m.maxValue());
		viewer->setFullRange("Intensity", idx_min*m_width, 4096);// (idx_max)*m_width);
		//viewer->setFullRange("Intensity", 0*m_width, (m_nData)*m_width);
		
		// Output the chart
		c->makeChart();		
		delete xx;
	}

	delete viewer->getChart();
	// Assign the chart to the WinChartViewer
	viewer->setChart(c);

}

void CChartHistogram::trackLineLabel(XYChart *c, int mouseX, int mouseY)
{
	if (!m_px) return;
	// Clear the current dynamic layer and get the DrawArea object to draw on it.
	DrawArea *d = c->initDynamicLayer();

	// The plot area object
	PlotArea *plotArea = c->getPlotArea();

	// Get the data x-value that is nearest to the mouse, and find its pixel coordinate.
	double xValue = c->getNearestXValue(mouseX);
	int xCoor = c->getXCoor(xValue);

	// Draw a vertical track line at the x-position
	d->vline(plotArea->getTopY(), plotArea->getBottomY(), mouseX, d->dashLineColor(0x000000, 0x0101));

	// Draw a label on the x-axis to show the track line position.
	int icount = 0;
	icount = m_px[int(xValue/m_width)];
	ostringstream xlabel;
	xlabel << "<*font,bgColor=000000*>(" << xValue <<", "<< icount
		<< ") <*/font*>";
	TTFText *t = d->text(xlabel.str().c_str(), "arialbd.ttf", 8);

	// Restrict the x-pixel position of the label to make sure it stays inside the chart image.
	int xLabelPos = max(0, min(xCoor - t->getWidth() / 2, c->getWidth() - t->getWidth()));
	t->draw(xLabelPos, plotArea->getBottomY() + 6, 0xffffff);
	t->destroy();

	// Iterate through all layers to draw the data labels
	for (int i = 0; i < c->getLayerCount(); ++i) {
		Layer *layer = c->getLayerByZ(i);

		// The data array index of the x-value
		int xIndex = layer->getXIndexOf(xValue);

		// Iterate through all the data sets in the layer
		for (int j = 0; j < layer->getDataSetCount(); ++j)
		{
			DataSet *dataSet = layer->getDataSetByZ(j);
			const char *dataSetName = dataSet->getDataName();

			// Get the color, name and position of the data label
			int color = dataSet->getDataColor();
			int yCoor = c->getYCoor(dataSet->getPosition(xIndex), dataSet->getUseYAxis());

			// Draw a track dot with a label next to it for visible data points in the plot area
			if ((yCoor >= plotArea->getTopY()) && (yCoor <= plotArea->getBottomY()) && (color !=
				Chart::Transparent))// && dataSetName && *dataSetName)
			{
				d->circle(xCoor, yCoor, 3, 3, color, -1);

				ostringstream label;
				label << "<*font,bgColor=" << hex << color << "*> "
					<< c->formatValue(dataSet->getValue(xIndex), "{value|P4}") << " <*font*>";
				t = d->text(label.str().c_str(), "arialbd.ttf", 8);

				// Draw the label on the right side of the dot if the mouse is on the left side the
				// chart, and vice versa. This ensures the label will not go outside the chart image.
				if (xCoor <= (plotArea->getLeftX() + plotArea->getRightX()) / 2)
					t->draw(xCoor + 5, yCoor, 0xffffff, Chart::Left);
				else
					t->draw(xCoor - 5, yCoor, 0xffffff, Chart::Right);

				t->destroy();
			}
		}
	}
}

void CChartHistogram::MouseMovePlotArea()
{
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		setMouseUsage(Chart::MouseUsageZoomIn);
	}
	else if (GetKeyState(VK_MENU) & 0x8000) {
		setMouseUsage(Chart::MouseUsageZoomOut);
	}
	else if (GetKeyState(VK_SHIFT) & 0x8000) {
		setMouseUsage(Chart::MouseUsageScroll);
	}
	else
		setMouseUsage(Chart::MouseUsageDefault);


	trackLineLabel((XYChart *)getChart(), getPlotAreaMouseX(), getPlotAreaMouseY());
	updateDisplay();
	// Hide the track cursor when the mouse leaves the plot area
	removeDynamicLayer(CVN_MouseLeavePlotArea);
}

void CChartHistogram::ViewPortChanged()
{
	if (needUpdateChart()) {
		DrawChart();
	}

}

void CChartHistogram::SetData(double *px, double  histoWidth, int nLen)
{
	m_nData = nLen;
	m_px = px;
	m_width = histoWidth;
	DrawChart();
	DrawChart();
}


void CChartHistogram::OnSize(UINT nType, int cx, int cy)
{
	CChartViewer::OnSize(nType, cx, cy);
	updateViewPort(FALSE, FALSE);
	DrawChart();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
