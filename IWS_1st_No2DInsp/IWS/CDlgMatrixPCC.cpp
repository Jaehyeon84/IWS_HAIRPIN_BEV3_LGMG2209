// CDlgMatrixPCC.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgMatrixPCC.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "IWSDoc.h"
#include "ChartDirect/ChartViewer.h"

// CDlgMatrixPCC 대화 상자

IMPLEMENT_DYNAMIC(CDlgMatrixPCC, CBaseDialog)

CDlgMatrixPCC::CDlgMatrixPCC(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_DIALOG_MATRIX_PCC, pParent)
{
	m_pDoc = NULL;		
	m_pChart = NULL;
	m_dMatrix[0] = 1;
	m_dMatrix[1] = 0;
	m_dMatrix[2] = 0;
	m_dMatrix[3] = 0;
	m_dMatrix[4] = 1;
	m_dMatrix[5] = 0;

	m_nJigNo = -1;
}

CDlgMatrixPCC::~CDlgMatrixPCC()
{
	delete m_pChart->getChart();
	delete ((CChartViewer*)m_pChart);	
}

void Func_LM_alglib(const alglib::real_1d_array &pdParam, alglib::real_1d_array &pdFuncVal, void *ptr)
{	
	CIWSDoc* pBody = (CIWSDoc*)ptr;

	if (pBody->m_Vec3DData.size() != pBody->m_VecIWSData.size())	return;
	if (pBody->m_Vec3DData.size() == 0 && pBody->m_VecIWSData.size() == 0)	return;

	double *pMat = const_cast<double*>(pdParam.getcontent());
	int nData = (int)(pBody->m_Vec3DData.size());

	double dx, dy, dx1, dy1;

	for (int i = 0; i < nData; i++) 
	{
		dx = pBody->m_Vec3DData[i].dX*pMat[0] + pBody->m_Vec3DData[i].dY*pMat[1] + pMat[2];
		dy = pBody->m_Vec3DData[i].dX*pMat[3] + pBody->m_Vec3DData[i].dY*pMat[4] + pMat[5];
		dx1 = pBody->m_VecIWSData[i].dX;
		dy1 = pBody->m_VecIWSData[i].dY;
		pdFuncVal[i] = sqrt((dx1 - dx)*(dx1 - dx) + (dy1 - dy)*(dy1 - dy));
		//pdFuncVal[i] = sqrt(dx*dx + dy * dy) - sqrt(dx1*dx1 + dy1 * dy1);
	}	
}

void CDlgMatrixPCC::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PLC_JIG, m_stcPlcJig);
}


BEGIN_MESSAGE_MAP(CDlgMatrixPCC, CBaseDialog)
	ON_BN_CLICKED(IDC_BUTTON_PLC_UPDATE, &CDlgMatrixPCC::OnBnClickedButtonPLCUpdate)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDlgMatrixPCC::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CDlgMatrixPCC::OnBnClickedButtonCalc)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CDlgMatrixPCC::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgMatrixPCC::OnBnClickedButtonSave)	
	ON_BN_CLICKED(IDC_BUTTON_MATRIX_VSCAN, &CDlgMatrixPCC::OnBnClickedButtonMatrixVscan)
	ON_BN_CLICKED(IDC_BUTTON_MATRIX_PCC, &CDlgMatrixPCC::OnBnClickedButtonMatrixPcc)
END_MESSAGE_MAP()


// CDlgMatrixPCC 메시지 처리기


BOOL CDlgMatrixPCC::OnInitDialog()
{
	CBaseDialog::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.	
	setLicenseCode("RDST-269V-GQM2-X8BZ-A621-8B39");
	m_pChart = new CChartViewer();
	m_pChart->setMouseUsage(Chart::MouseUsageDefault);
	m_pChart->setScrollDirection(Chart::DirectionHorizontalVertical);
	m_pChart->setZoomDirection(Chart::DirectionHorizontalVertical);
	
	CRect rect;
	GetDlgItem(IDC_STATIC_CHART)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_CHART)->ClientToScreen(rect);
	ScreenToClient(rect);
	m_pChart->Create(_T("Matrix"), WS_CHILD | SS_BITMAP | WS_VISIBLE | WS_EX_CLIENTEDGE | SS_NOTIFY, rect, this, IDC_STATIC_CHART);
	m_pChart->ShowWindow(SW_SHOW);

	GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(SW_HIDE);

	DrawChart();
		
	return TRUE;  
}

void CDlgMatrixPCC::DrawChart()
{
	CChartViewer *viewer = (CChartViewer*)m_pChart;
		
	CRect rect;
	GetDlgItem(IDC_STATIC_CHART)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_CHART)->ClientToScreen(rect);
	ScreenToClient(rect);

	XYChart *c = new XYChart(rect.Width(), rect.Height());
	c->setBackground(0x525252);
	c->addTitle("Coordinate Correction", "timesbi.ttf", 18);
	/*c->setPlotArea(50, 55, c->getWidth() - 70, c->getHeight() - 90, c->linearGradientColor(255, 0, 0,
		c->getHeight() - 35, 0xf0f6ff, 0xa0c0ff), -1, Chart::Transparent, 0xffffff, 0xffffff);	*/
	c->setPlotArea(50, 72, c->getWidth() - 70, c->getHeight() - 110, c->linearGradientColor(255, 0, 0,
		c->getHeight() - 35, 0xf0f6ff, 0x525252), -1, Chart::Transparent, 0xffffff, 0xffffff);
	
		
	// Set axis label style to 8pts Arial Bold
	c->xAxis()->setLabelStyle("arialbd.ttf", 8);
	c->yAxis()->setLabelStyle("arialbd.ttf", 8);
	c->addLegend(50, 30, false, "timesbi.ttf", 12)->setBackground(Chart::BackgroundColor);
	c->setClipping();
	// Set the axis stem to transparent
	c->xAxis()->setColors(Chart::Transparent);
	c->yAxis()->setColors(Chart::Transparent);
	
	// Configure x-axis label format
	//c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "{value|mm/yyyy} ",
	//	Chart::StartOfMonthFilter(), "{value|mm}");

	// Add axis title using 10pts Arial Bold Italic font
	c->xAxis()->setTitle("x", "arialbi.ttf", 10);
	c->yAxis()->setTitle("y", "arialbi.ttf", 10);

	if (m_pDoc->m_VecFitdata.size()) 
	{
		size_t nData = m_pDoc->m_VecFitdata.size();
		double *px = new double[nData];
		double *py = new double[nData];

		int idx = 0;
		for (auto p : m_pDoc->m_VecFitdata) 
		{
			px[idx] = p.dX;
			py[idx] = p.dY;
			idx++;
		}

		c->addScatterLayer(DoubleArray(px, (int)nData), DoubleArray(py, (int)nData)
			, "Fitted", Chart::CrossSymbol, 9, 0x33fff);

		delete[] px;
		delete[] py;
	}

	if (m_pDoc->m_Vec3DData.size()) 
	{
		size_t nData = m_pDoc->m_Vec3DData.size();
		double *px = new double[nData];
		double *py = new double[nData];

		int idx = 0;
		for (auto p : m_pDoc->m_Vec3DData) 
		{
			px[idx] = p.dX;
			py[idx] = p.dY;
			idx++;
		}

		c->addScatterLayer(DoubleArray(px, (int)nData), DoubleArray(py, (int)nData)
			, "3DData", Chart::DiamondSymbol, 10, 0xff9933);

		delete[] px;
		delete[] py;
	}

	if (m_pDoc->m_VecIWSData.size()) 
	{
		size_t nData = m_pDoc->m_VecIWSData.size();
		double *px = new double[nData];
		double *py = new double[nData];

		int idx = 0;
		for (auto p : m_pDoc->m_VecIWSData) 
		{
			px[idx] = p.dX;
			py[idx] = p.dY;
			idx++;
		}

		c->addScatterLayer(DoubleArray(px, (int)nData), DoubleArray(py, (int)nData)
			, "IWSData", Chart::TriangleSymbol, 9, 0x33ff33);

		delete[] px;
		delete[] py;
	}

	viewer->syncLinearAxisWithViewPort("x", c->xAxis());
	viewer->syncLinearAxisWithViewPort("y", c->yAxis());
	viewer->setFullRange("x", -100, 100);
	viewer->setFullRange("y", -100, 100);

	delete viewer->getChart();
	viewer->setChart(c);
}

void CDlgMatrixPCC::OnBnClickedButtonPLCUpdate()
{
	//if (!m_pDoc->m_config.m_bUseHairpin)	return;

	int nJig = m_pDoc->m_plc.GetWorkJig();
	int nJigAddress = 0;
	CString strMsg;
	strMsg.Format(L"Matrix PLC Data Read Work Jig(PCC): %d", nJig);
	_tracer(TRACE_NORMAL, 1, strMsg);
	
	if      (nJig == 1)	{	nJigAddress = PLC_READ_INSPECT_A;}
	else if (nJig == 2)	{	nJigAddress = PLC_READ_INSPECT_B;}
	else                {	nJigAddress = PLC_READ_INSPECT_A;}
	_tracer(TRACE_NORMAL, 1, L"Matrix PLC Data Read Work Jig(PCC): %d, StartAddress: %d", nJig, nJigAddress);

	UpdatePlcJigNo();
	
	int nWordCnt = (PCC_POINTS * 16);
	strMsg.Format(L"Matrix PLC Data Read Word Count(PCC): %d", nWordCnt);
	_tracer(TRACE_NORMAL, 1, strMsg);
	
	if (!m_pDoc->m_bImportPLCData)
	{
		//PLC Data Read
		if (!m_pDoc->m_plcData->UpdateInput_InspData(nJigAddress, nWordCnt))
		{
			_tracer(TRACE_ERROR, 1, _T("Matrix PLC Data Read Fail(PCC)"));
			return;
		}
		PLCReadDataAdd(nWordCnt, TRUE);
	}
	DrawChart();
}

//void CDlgMatrixPCC::PLCReadDataAdd(int nWordCnt)
void CDlgMatrixPCC::PLCReadDataAdd(int nWordCnt, BOOL bPCC /*= FALSE*/)
{
	if (m_pDoc->m_work.m_JobGroup.size() == 0)
	{
		_tracer(TRACE_ERROR, 1, _T("Job group size does not exist."));
		return;
	}		

	m_pDoc->m_Vec3DData.clear();
	for (int i = 0; i < nWordCnt; i += 16)
	{
		//dhchoi 20220710 add. exclude the hairpin that is PreInspection falsed
		//BOOL bIsOK = m_pDoc->m_plcData->m_rDataInsp[i + 0];
		//if (bIsOK == FALSE) continue;

		IWS_LOCATION pt;
		//Inner layer
		pt.dX = m_pDoc->m_plcData->m_rDataInsp[i + 6] / 100.0;
		pt.dY = m_pDoc->m_plcData->m_rDataInsp[i + 7] / 100.0;
		m_pDoc->m_Vec3DData.push_back(pt);

		//outer layer
		if (bPCC == FALSE) {		
			pt.dX = m_pDoc->m_plcData->m_rDataInsp[i + 8] / 100.0;
			pt.dY = m_pDoc->m_plcData->m_rDataInsp[i + 9] / 100.0;
			m_pDoc->m_Vec3DData.push_back(pt);
		}
	}

	DrawChart();
}

void CDlgMatrixPCC::OnBnClickedButtonCalc()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (m_pDoc->m_Vec3DData.size() == 0 && m_pDoc->m_VecIWSData.size() == 0)
	{
		AfxMessageBox(_T("Pre-inspection data and IWS data do not exist."), MB_OK | MB_ICONERROR);
		return;
	}
	if (m_pDoc->m_Vec3DData.size() != m_pDoc->m_VecIWSData.size())
	{
		AfxMessageBox(_T("Matrix operation is not possible because the pre-inspection data and IWS data are not the same."), MB_OK | MB_ICONERROR);
		return;
	}

	StartOptimizer();
	FittedData();
}

double CDlgMatrixPCC::CalcError()
{	
	double derr = -9999;
	if ((m_pDoc->m_VecIWSData.size() - m_pDoc->m_VecFitdata.size())) return derr;
	derr = 0;
	double dx, dy;
	int nsize = (int)m_pDoc->m_VecIWSData.size();
	for (int i = 0; i < nsize; i++) {
		dx = m_pDoc->m_VecFitdata[i].dX - m_pDoc->m_VecIWSData[i].dX;
		dy = m_pDoc->m_VecFitdata[i].dY - m_pDoc->m_VecIWSData[i].dY;
		derr += (dx*dx + dy*dy);
	}

	return derr;
}

void CDlgMatrixPCC::FittedData()
{
	m_pDoc->m_VecFitdata.clear();

	for (auto p : m_pDoc->m_Vec3DData)
	{
		IWS_LOCATION pt;

		pt.dX = p.dX * m_dMatrix[0] + p.dY * m_dMatrix[1] + m_dMatrix[2];
		pt.dY = p.dX * m_dMatrix[3] + p.dY * m_dMatrix[4] + m_dMatrix[5];
		m_pDoc->m_VecFitdata.push_back(pt);
	}
	DrawChart();
}

void CDlgMatrixPCC::OnBnClickedButtonApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	//계산된 Matrix config에 저장
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 -->
	//m_pDoc->m_config.m_dCalibMtx_a11 = m_dMatrix[0];
	//m_pDoc->m_config.m_dCalibMtx_a12 = m_dMatrix[1];
	//m_pDoc->m_config.m_dCalibMtx_a13 = m_dMatrix[2];
	//m_pDoc->m_config.m_dCalibMtx_a21 = m_dMatrix[3];
	//m_pDoc->m_config.m_dCalibMtx_a22 = m_dMatrix[4];
	//m_pDoc->m_config.m_dCalibMtx_a23 = m_dMatrix[5];
	//switch (m_nJigNo)
	//{
	//case 1:
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a11 = m_dMatrix[0];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a12 = m_dMatrix[1];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a13 = m_dMatrix[2];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a21 = m_dMatrix[3];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a22 = m_dMatrix[4];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a23 = m_dMatrix[5];
	//	break;
	//case 2:
	//	m_pDoc->m_config.m_MatrixJigB.m_dCalibMtx_a11 = m_dMatrix[0];
	//	m_pDoc->m_config.m_MatrixJigB.m_dCalibMtx_a12 = m_dMatrix[1];
	//	m_pDoc->m_config.m_MatrixJigB.m_dCalibMtx_a13 = m_dMatrix[2];
	//	m_pDoc->m_config.m_MatrixJigB.m_dCalibMtx_a21 = m_dMatrix[3];
	//	m_pDoc->m_config.m_MatrixJigB.m_dCalibMtx_a22 = m_dMatrix[4];
	//	m_pDoc->m_config.m_MatrixJigB.m_dCalibMtx_a23 = m_dMatrix[5];
	//	break;
	//default:
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a11 = m_dMatrix[0];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a12 = m_dMatrix[1];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a13 = m_dMatrix[2];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a21 = m_dMatrix[3];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a22 = m_dMatrix[4];
	//	m_pDoc->m_config.m_MatrixJigA.m_dCalibMtx_a23 = m_dMatrix[5];
	//	break;
	//}
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 <--

	CString strMsg = L"";
	switch (m_nJigNo)
	{
	case 1:
		for (int i = 0; i < MATRIX_DIVISION; i++) {
			m_pDoc->m_config.m_MatrixA[i].a11 = m_dMatrix[0];
			m_pDoc->m_config.m_MatrixA[i].a12 = m_dMatrix[1];
			m_pDoc->m_config.m_MatrixA[i].a13 = m_dMatrix[2];
			m_pDoc->m_config.m_MatrixA[i].a21 = m_dMatrix[3];
			m_pDoc->m_config.m_MatrixA[i].a22 = m_dMatrix[4];
			m_pDoc->m_config.m_MatrixA[i].a23 = m_dMatrix[5];
		}
		break;
	case 2:
		for (int i = 0; i < MATRIX_DIVISION; i++) {
			m_pDoc->m_config.m_MatrixB[i].a11 = m_dMatrix[0];
			m_pDoc->m_config.m_MatrixB[i].a12 = m_dMatrix[1];
			m_pDoc->m_config.m_MatrixB[i].a13 = m_dMatrix[2];
			m_pDoc->m_config.m_MatrixB[i].a21 = m_dMatrix[3];
			m_pDoc->m_config.m_MatrixB[i].a22 = m_dMatrix[4];
			m_pDoc->m_config.m_MatrixB[i].a23 = m_dMatrix[5];
		}
		break;
	default:
		for (int i = 0; i < MATRIX_DIVISION; i++) {
			m_pDoc->m_config.m_MatrixA[i].a11 = m_dMatrix[0];
			m_pDoc->m_config.m_MatrixA[i].a12 = m_dMatrix[1];
			m_pDoc->m_config.m_MatrixA[i].a13 = m_dMatrix[2];
			m_pDoc->m_config.m_MatrixA[i].a21 = m_dMatrix[3];
			m_pDoc->m_config.m_MatrixA[i].a22 = m_dMatrix[4];
			m_pDoc->m_config.m_MatrixA[i].a23 = m_dMatrix[5];
		}
	}
	m_pDoc->m_config.WriteConfig();

	strMsg.Format(L"MATRIX(PCC) [%.6f][%.6f][%.6f][%.6f][%.6f][%.6f]", m_dMatrix[0], m_dMatrix[1], m_dMatrix[2], m_dMatrix[3], m_dMatrix[4], m_dMatrix[5]);
	_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Value [a11 ~ a13, a21 ~ a23]= %s", strMsg);
	_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Error rate = %.3f", CalcError());

}

void CDlgMatrixPCC::OnBnClickedButtonClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	m_pDoc->m_Vec3DData.clear();
	m_pDoc->m_VecIWSData.clear();
	m_pDoc->m_VecFitdata.clear();

	DrawChart();
}

void CDlgMatrixPCC::OnBnClickedButtonSave()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_pDoc->m_config.m_bUseHairpin)	return;

	if (m_pDoc->m_Vec3DData.size() != m_pDoc->m_VecIWSData.size())
	{
		_tracer(TRACE_NORMAL, 1, L"3D coordinate data and IWS coordinate data are not the same.");
		return;
	}

	if (m_pDoc->m_Vec3DData.size() == 0 && m_pDoc->m_VecIWSData.size() == 0)
	{
		_tracer(TRACE_NORMAL, 1, L"3D Coordinate Data and IWS Coordinate Data Null.");
		return;
	}

	FileSave();
}

void CDlgMatrixPCC::FileSave()
{
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strStartUpPath = path;
	strStartUpPath = strStartUpPath.Left(strStartUpPath.ReverseFind('\\'));

	// sjyi 2022-07-08 JigA, JigB를 구분하기 위해, 수정 -->
//	strStartUpPath = strStartUpPath + L"\\MatrixData.csv";
	switch (m_nJigNo)
	{
	case 1: // Jig A
		strStartUpPath = strStartUpPath + L"\\MatrixData_JigA.csv";
		break;
	case 2: // Jig B
		strStartUpPath = strStartUpPath + L"\\MatrixData_JigB.csv";
		break;
	default:	// Unknown
		strStartUpPath = strStartUpPath + L"\\MatrixData.csv";
		break;
	}
	// sjyi 2022-07-08 JigA, JigB를 구분하기 위해, 수정 <--

	CFile cfile;
	if (!cfile.Open(strStartUpPath, CFile::modeCreate | CFile::modeReadWrite))
	{
		_tracer(TRACE_ERROR, 1, _T("File Save Fail Matrix Data."));
		return;
	}

	CString strHeader;
	strHeader.Format(L"Slot, Layer, 3DDataX, 3DDataY, IWSDataX, IWSDataY\n");
	cfile.Write(strHeader, strHeader.GetLength() * sizeof(TCHAR));

	int nLayer = 1;
	int nSlot = 1;
	for(int i = 0; i < m_pDoc->m_Vec3DData.size(); i++)
	{
		strHeader.Format(L"%d, %d,", nSlot, nLayer);
		cfile.Write(strHeader, strHeader.GetLength() * sizeof(TCHAR));

		IWS_LOCATION pt, pt1;

		pt.dX = m_pDoc->m_Vec3DData[i].dX;
		pt.dY = m_pDoc->m_Vec3DData[i].dY;

		pt1.dX = m_pDoc->m_VecIWSData[i].dX;
		pt1.dY = m_pDoc->m_VecIWSData[i].dY;

		CString strData;
		strData.Format(L"%.4f, %.4f, %.4f, %.4f\n", pt.dX, pt.dY, pt1.dX, pt1.dY);
		cfile.Write(strData, strData.GetLength() * sizeof(TCHAR));
		
		nLayer++;

		if (nLayer > m_pDoc->m_work.m_hp.nLayer)
		{
			nLayer = 1;

			nSlot++;
		}		
	}
	cfile.Close();
}

void CDlgMatrixPCC::StartOptimizer()
{	
	double arr_xl[6] = { 0.5,	-0.5,	-2.0,	-0.5,	0.5,	-2 };
	double arr_xu[6] = { 1.5,	0.5,	2.0,	0.5,	1.5,	2 };
	double epsx = 1e-64;

	alglib::setnworkers(4);
	alglib::real_1d_array x, bndl, bndu;

	x.setcontent(6, m_dMatrix);
	bndl.setcontent(6, arr_xl);
	bndu.setcontent(6, arr_xu);

	alglib::ae_int_t maxits = 0;
	alglib::minlmstate state;
	
	alglib::minlmreport rep;
	minlmcreatev(m_pDoc->m_Vec3DData.size(), x, 1e-12, state);
	minlmsetbc(state, bndl, bndu);
	minlmsetcond(state, epsx, maxits);
	minlmsetacctype(state, 1);
	minlmoptimize(state, Func_LM_alglib, NULL, m_pDoc);
	minlmresults(state, x, rep);
	memcpy(m_dMatrix, x.getcontent(), sizeof(double) * 6);
	TRACE("%f,%f,%f\n%f,%f,%f\n", m_dMatrix[0], m_dMatrix[1], m_dMatrix[2],
		m_dMatrix[3], m_dMatrix[4], m_dMatrix[5]);
}

// sjyi 2022-07-08 PLC로부터 JigNo 획득하기 위한 함수
// Jig No 획득하여, 화면에 표시
void CDlgMatrixPCC::UpdatePlcJigNo()
{
	m_nJigNo  = m_pDoc->m_plc.GetWorkJig();
	CString strTemp;

	switch (m_nJigNo)
	{
	case 1:
		strTemp.Format(_T("JigA"));
		break;
	case 2:
		strTemp.Format(_T("JigB"));
		break;
	default:
		strTemp.Format(_T("Unknown"));
		break;
	}

	m_stcPlcJig.SetWindowTextW(strTemp);
}

void CDlgMatrixPCC::OnBnClickedButtonMatrixVscan()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	if (pDoc->GetUseInterface())
	{
		AfxMessageBox(L"Change To Offline");
		return;
	}
	pDoc->m_work.DoVScan();
}

void CDlgMatrixPCC::OnBnClickedButtonMatrixPcc()
{
	//if (!m_pDoc->m_config.m_bUseHairpin) return;

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CFileFind finder;
	CString strFile = pDoc->m_strImgPath + STR_REFIMG_FILE;
	if (finder.FindFile(strFile) == FALSE) {
		_tracer(TRACE_NORMAL, 9999, L"Reference Image Error : %s", STR_REFIMG_FILE);
		return;
	}

	POINTFLOAT pPoint[PCC_POINTS];
	pDoc->GetPccResults(pPoint);

	// 
	m_pDoc->m_VecIWSData.clear();

	IWS_LOCATION pt;
	for (int i=0; i < PCC_POINTS; i++)
	{
		pt.dX = pPoint[i].x;
		pt.dY = pPoint[i].y;
		m_pDoc->m_VecIWSData.push_back(pt);
	}
	DrawChart();
}



