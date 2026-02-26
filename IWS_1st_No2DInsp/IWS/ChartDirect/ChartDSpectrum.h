#pragma once
namespace chartdirect
{
#include "ChartViewer.h"
	class CChartDSpectrum :
		public CChartViewer
	{
	public:
		CChartDSpectrum();
		~CChartDSpectrum();
		void DrawChart();
		void trackLineLabel(XYChart *c, int mouseX, int mouseY);
		virtual void MouseMovePlotArea();
		virtual void ViewPortChanged();

		void SetData(double *px, double *py0, double *py1, double *py2, double *py3, int nLen);
	protected:
		double *m_pdata[4];
		double *m_px;
		int m_nData;
	};


}
