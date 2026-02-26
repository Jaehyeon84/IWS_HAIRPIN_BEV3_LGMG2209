// CDlgMatrix.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgMatrix.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "IWSDoc.h"
#include "ChartDirect/ChartViewer.h"

// CDlgMatrix 대화 상자

IMPLEMENT_DYNAMIC(CDlgMatrix, CBaseDialog)

CDlgMatrix::CDlgMatrix(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_DIALOG_MATRIX, pParent)
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

CDlgMatrix::~CDlgMatrix()
{
	delete m_pChart->getChart();
	delete ((CChartViewer*)m_pChart);	
}

// 2022.08.15 jh.kim 변경 - Matrix 분할 ( 4분할 )
//void Func_LM_alglib(const alglib::real_1d_array &pdParam, alglib::real_1d_array &pdFuncVal, void *ptr)
void Func_LM_alglibDiv(const alglib::real_1d_array &pdParam, alglib::real_1d_array &pdFuncVal, void *ptr)
{	
	CIWSDoc* pBody = (CIWSDoc*)ptr;

	if (pBody->m_Vec3DData.size() != pBody->m_VecIWSData.size())	return;
	if (pBody->m_Vec3DData.size() == 0 && pBody->m_VecIWSData.size() == 0)	return;

	double *pMat = const_cast<double*>(pdParam.getcontent());
	//int nData = (int)(pBody->m_Vec3DData.size());
	int nData = (int)(pBody->m_VecData.size());
	int nInx = pBody->m_nVecIndex;
	int nCount = pBody->m_nVecCount;

	double dx, dy, dx1, dy1;
	for (int i = 0; i < nData; i++) 
	{
		//dx = pBody->m_Vec3DData[i].dX*pMat[0] + pBody->m_Vec3DData[i].dY*pMat[1] + pMat[2];
		//dy = pBody->m_Vec3DData[i].dX*pMat[3] + pBody->m_Vec3DData[i].dY*pMat[4] + pMat[5];
		dx = pBody->m_VecData[i].dX*pMat[0] + pBody->m_VecData[i].dY*pMat[1] + pMat[2];
		dy = pBody->m_VecData[i].dX*pMat[3] + pBody->m_VecData[i].dY*pMat[4] + pMat[5];

		//dx1 = pBody->m_VecIWSData[i].dX;
		//dy1 = pBody->m_VecIWSData[i].dY;
		dx1 = pBody->m_VecIWSData[nCount*nInx + i].dX;
		dy1 = pBody->m_VecIWSData[nCount*nInx + i].dY;

		pdFuncVal[i] = sqrt((dx1 - dx)*(dx1 - dx) + (dy1 - dy)*(dy1 - dy));
		//pdFuncVal[i] = sqrt(dx*dx + dy * dy) - sqrt(dx1*dx1 + dy1 * dy1);
	}	
}

void CDlgMatrix::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_PLC_JIG, m_stcPlcJig);
}


BEGIN_MESSAGE_MAP(CDlgMatrix, CBaseDialog)
	ON_BN_CLICKED(IDC_BUTTON_PLC_UPDATE, &CDlgMatrix::OnBnClickedButtonPLCUpdate)
	ON_BN_CLICKED(IDC_BUTTON_IWS_UPDATE, &CDlgMatrix::OnBnClickedButtonIWSUpdate)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR, &CDlgMatrix::OnBnClickedButtonClear)
	ON_BN_CLICKED(IDC_BUTTON_CALC, &CDlgMatrix::OnBnClickedButtonCalc)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, &CDlgMatrix::OnBnClickedButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_SAVE, &CDlgMatrix::OnBnClickedButtonSave)	
END_MESSAGE_MAP()


// CDlgMatrix 메시지 처리기


BOOL CDlgMatrix::OnInitDialog()
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

	DrawChart();
		
	return TRUE;  
}

void CDlgMatrix::DrawChart()
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

		c->addScatterLayer(DoubleArray(px, nData), DoubleArray(py, nData)
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

		c->addScatterLayer(DoubleArray(px, nData), DoubleArray(py, nData)
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

		c->addScatterLayer(DoubleArray(px, nData), DoubleArray(py, nData)
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

void CDlgMatrix::OnBnClickedButtonPLCUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_pDoc->m_config.m_bUseHairpin)
		return;

	int nJig = m_pDoc->m_plc.GetWorkJig();
	int nJigAddress = 0;
	CString strMsg;
	strMsg.Format(L"Matrix PLC Data Read Work Jig: %d", nJig);
	_tracer(TRACE_NORMAL, 1, strMsg);

	if (nJig == 1)
	{
		nJigAddress = PLC_READ_INSPECT_A;
	}
	else if(nJig == 2)
	{
		nJigAddress = PLC_READ_INSPECT_B;
	}
	else
	{		
		strMsg.Format(L"Matrix PLC Data Read Work Jig: %d, StartAddress: %d", nJig, nJigAddress);
		_tracer(TRACE_ERROR, 1, strMsg);
	}
	UpdatePlcJigNo();

	int nWordCnt = (m_pDoc->m_work.m_hp.nDrawSlot * 16) * (m_pDoc->m_work.m_hp.nLayer / 2);
	strMsg.Format(L"Matrix PLC Data Read Word Count: %d", nWordCnt);
	_tracer(TRACE_NORMAL, 1, strMsg);
	
	if (!m_pDoc->m_bImportPLCData)
	{
		//PLC Data Read
		if (!m_pDoc->m_plcData->UpdateInput_InspData(nJigAddress, nWordCnt))
		{
			_tracer(TRACE_ERROR, 1, _T("Matrix PLC Data Read Fail"));
			return;
		}
		PLCReadDataAdd(nWordCnt);
	}	
}

void CDlgMatrix::PLCReadDataAdd(int nWordCnt)
{
	if (m_pDoc->m_work.m_JobGroup.size() == 0)
	{
		_tracer(TRACE_ERROR, 1, _T("Job group size does not exist."));
		return;
	}		

	int nIdx = 0;
	int nMaxLayer = m_pDoc->m_work.m_nDefLayer / 2;
	int nLayer = 0;

	m_pDoc->m_Vec3DData.clear();
	for (int i = 0; i < nWordCnt; i += 16)
	{
		/*nLayer = m_pDoc->m_work.m_JobGroup[nIdx]->m_nIndex % (m_pDoc->m_work.m_nDefLayer / 2) + 1;
		if (nLayer > nMaxLayer)
			continue;*/

		//BOOL bIsOK = m_pDoc->m_plcData->m_rDataInsp[i + 0];
		//if (bIsOK == FALSE)
		//{
		//	continue;
		//}

		_IWS_LOCATION pt;

		//Inner slot
		pt.dX = m_pDoc->m_plcData->m_rDataInsp[i + 6] / 100.0;
		pt.dY = m_pDoc->m_plcData->m_rDataInsp[i + 7] / 100.0;

		m_pDoc->m_Vec3DData.push_back(pt);

		//outter slot
		pt.dX = m_pDoc->m_plcData->m_rDataInsp[i + 8] / 100.0;
		pt.dY = m_pDoc->m_plcData->m_rDataInsp[i + 9] / 100.0;

		m_pDoc->m_Vec3DData.push_back(pt);

		// 8Layer가 아닐 경우 Address Jump
		/*if (nLayer == nMaxLayer)
			i += ((4 - nMaxLayer) * 16);*/

		nIdx++;
	}

	//Test
	/*m_pDoc->m_Vec3DData.clear();
	CString szFilter = L"All Files(*.*)|*.*||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_HIDEREADONLY, szFilter);
	if (IDOK != dlg.DoModal()) return;

	CString strPathName = dlg.GetPathName();

	CStdioFile pFile;
	if (pFile.Open(strPathName, CFile::modeRead | CFile::typeText, NULL) == FALSE)
	{
		CString str;
		str.Format(_T("Load Results Fail : %s"), strPathName);
		AfxMessageBox(str);
		return;
	}
	CString str, token;
	int pos = 0;
	std::vector<_IWS_LOCATION> vecb;
	while (pFile.ReadString(str))
	{
		pos = 0;
		token = str.Tokenize(L",", pos);
		token = str.Tokenize(L",", pos);
		float ax = (float)_tstof(token);
		token = str.Tokenize(L",", pos);
		float ay = (float)_tstof(token);
		token = str.Tokenize(L",", pos);
		float bx = (float)_tstof(token);
		token = str.Tokenize(L",", pos);
		float by = (float)_tstof(token);
		_IWS_LOCATION pt;
		pt.dX = ax;
		pt.dY = ay;

		_IWS_LOCATION pt1;
		pt1.dX = bx;
		pt1.dY = by;
		m_pDoc->m_Vec3DData.push_back(pt);
		m_pDoc->m_Vec3DData.push_back(pt1);
	}

	m_pDoc->m_Vec3DData.insert(m_pDoc->m_Vec3DData.end(), vecb.begin(), vecb.end());
	pFile.Close();*/

	DrawChart();
}


void CDlgMatrix::OnBnClickedButtonIWSUpdate()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	if (!m_pDoc->m_config.m_bUseHairpin)
		return;

	if (!m_pDoc->m_work.m_JobGroup.size())
		return;

	//dhchoi 20220710 add. exclude the hairpin that is PreInspection falsed
	if (m_pDoc->m_Vec3DData.size() < 1)
	{
		return;
	}


	IWSReadDataAdd();
}

void CDlgMatrix::IWSReadDataAdd()
{
	m_pDoc->m_VecIWSData.clear();
	int i3DDataIdx = 0;
	int iFalseCnt = -1;
	short *p3DDataMem = NULL;
	for (auto group : m_pDoc->m_work.m_JobGroup)
	{
		if (group->m_jobs.size() < 2)
			continue;

		////dhchoi 20220710 add. exclude the hairpin that is PreInspection falsed
		BOOL bIsOK = m_pDoc->m_plcData->m_rDataInsp[i3DDataIdx + 0];
		//i3DDataIdx += 16;
		//if (bIsOK == FALSE)
		//{
		//	continue;
		//}

		_IWS_LOCATION pt;
		//1차 패턴에 대한 좌표값 확인
		for (int i = 0; i < 2; i++)
		{
			if (bIsOK) {
				CEntity *pEnt = group->m_jobs[i]->GetEntity();
				pt.dX = FLOAT(pEnt->m_cen.x + pEnt->m_offset.x);
				pt.dY = FLOAT(pEnt->m_cen.y + pEnt->m_offset.y);
			}
			else
			{
				pt.dX = 0.0f;
				pt.dY = 0.0f;
			}

			m_pDoc->m_VecIWSData.push_back(pt);
		}
	}
	
	DrawChart();
}

void CDlgMatrix::OnBnClickedButtonCalc()
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

	//StartOptimizer();
	//FittedData();

	// 2022.08.15 jh.kim 추가 - Matrix 분할 ( 4분할 )
	m_pDoc->m_VecFitdata.clear();
	for (int i = 0; i < MATRIX_DIVISION; i++)
	{
		StartOptimizer(i);

		m_dMatrixDiv[i][0] = m_dMatrix[0];
		m_dMatrixDiv[i][1] = m_dMatrix[1];
		m_dMatrixDiv[i][2] = m_dMatrix[2];
		m_dMatrixDiv[i][3] = m_dMatrix[3];
		m_dMatrixDiv[i][4] = m_dMatrix[4];
		m_dMatrixDiv[i][5] = m_dMatrix[5];
	}

	DrawChart();
}

double CDlgMatrix::CalcError(int inx)
{	
	double derr = -9999;
	if ((m_pDoc->m_VecIWSData.size() - m_pDoc->m_VecFitdata.size())) return derr;
	derr = 0;
	double dx, dy;
	//int nsize = m_pDoc->m_VecIWSData.size();
	int nsize = m_pDoc->m_VecIWSData.size() / MATRIX_DIVISION;
	int n = 0;
	for (int i = 0; i < nsize; i++) {
		n = inx * nsize + i;
		dx = m_pDoc->m_VecFitdata[n].dX - m_pDoc->m_VecIWSData[n].dX;
		dy = m_pDoc->m_VecFitdata[n].dY - m_pDoc->m_VecIWSData[n].dY;
		derr += (dx*dx + dy*dy);
	}

	return derr;
}

void CDlgMatrix::FittedData()
{
	m_pDoc->m_VecFitdata.clear();

	for (auto p : m_pDoc->m_Vec3DData)
	{
		_IWS_LOCATION pt;

		pt.dX = p.dX * m_dMatrix[0] + p.dY * m_dMatrix[1] + m_dMatrix[2];
		pt.dY = p.dX * m_dMatrix[3] + p.dY * m_dMatrix[4] + m_dMatrix[5];
		m_pDoc->m_VecFitdata.push_back(pt);
	}
	DrawChart();
}

void CDlgMatrix::OnBnClickedButtonApply()
{
	//계산된 Matrix config에 저장
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 -->
	//m_pDoc->m_config.m_dCalibMtx_a11 = m_dMatrix[0];
	//m_pDoc->m_config.m_dCalibMtx_a12 = m_dMatrix[1];
	//m_pDoc->m_config.m_dCalibMtx_a13 = m_dMatrix[2];
	//m_pDoc->m_config.m_dCalibMtx_a21 = m_dMatrix[3];
	//m_pDoc->m_config.m_dCalibMtx_a22 = m_dMatrix[4];
	//m_pDoc->m_config.m_dCalibMtx_a23 = m_dMatrix[5];
	CString strMsg = L"";
	switch (m_nJigNo)
	{
	case 1:
		for (int i = 0; i < MATRIX_DIVISION; i++) {
			m_pDoc->m_config.m_MatrixA[i].a11 = m_dMatrixDiv[i][0];
			m_pDoc->m_config.m_MatrixA[i].a12 = m_dMatrixDiv[i][1];
			m_pDoc->m_config.m_MatrixA[i].a13 = m_dMatrixDiv[i][2];
			m_pDoc->m_config.m_MatrixA[i].a21 = m_dMatrixDiv[i][3];
			m_pDoc->m_config.m_MatrixA[i].a22 = m_dMatrixDiv[i][4];
			m_pDoc->m_config.m_MatrixA[i].a23 = m_dMatrixDiv[i][5];

			strMsg.Format(L"MATRIX %02d [%.6f][%.6f][%.6f][%.6f][%.6f][%.6f]", i + 1, m_dMatrixDiv[i][0], m_dMatrixDiv[i][1], m_dMatrixDiv[i][2], m_dMatrixDiv[i][3], m_dMatrixDiv[i][4], m_dMatrixDiv[i][5]);
			_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Value [a11 ~ a13, a21 ~ a23]= %s", strMsg);
			_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Error rate = %.3f", CalcError(i));
		}
		break;
	case 2:
		for (int i = 0; i < MATRIX_DIVISION; i++) {
			m_pDoc->m_config.m_MatrixB[i].a11 = m_dMatrixDiv[i][0];
			m_pDoc->m_config.m_MatrixB[i].a12 = m_dMatrixDiv[i][1];
			m_pDoc->m_config.m_MatrixB[i].a13 = m_dMatrixDiv[i][2];
			m_pDoc->m_config.m_MatrixB[i].a21 = m_dMatrixDiv[i][3];
			m_pDoc->m_config.m_MatrixB[i].a22 = m_dMatrixDiv[i][4];
			m_pDoc->m_config.m_MatrixB[i].a23 = m_dMatrixDiv[i][5];

			strMsg.Format(L"MATRIX %02d [%.6f][%.6f][%.6f][%.6f][%.6f][%.6f]", i + 1, m_dMatrixDiv[i][0], m_dMatrixDiv[i][1], m_dMatrixDiv[i][2], m_dMatrixDiv[i][3], m_dMatrixDiv[i][4], m_dMatrixDiv[i][5]);
			_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Value [a11 ~ a13, a21 ~ a23]= %s", strMsg);
			_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Error rate = %.3f", CalcError(i));
		}
		break;
	default:
		for (int i = 0; i < MATRIX_DIVISION; i++) {
			m_pDoc->m_config.m_MatrixA[i].a11 = m_dMatrixDiv[i][0];
			m_pDoc->m_config.m_MatrixA[i].a12 = m_dMatrixDiv[i][1];
			m_pDoc->m_config.m_MatrixA[i].a13 = m_dMatrixDiv[i][2];
			m_pDoc->m_config.m_MatrixA[i].a21 = m_dMatrixDiv[i][3];
			m_pDoc->m_config.m_MatrixA[i].a22 = m_dMatrixDiv[i][4];
			m_pDoc->m_config.m_MatrixA[i].a23 = m_dMatrixDiv[i][5];

			strMsg.Format(L"MATRIX %02d [%.6f][%.6f][%.6f][%.6f][%.6f][%.6f]", i + 1, m_dMatrixDiv[i][0], m_dMatrixDiv[i][1], m_dMatrixDiv[i][2], m_dMatrixDiv[i][3], m_dMatrixDiv[i][4], m_dMatrixDiv[i][5]);
			_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Value [a11 ~ a13, a21 ~ a23]= %s", strMsg);
			_tracer(TRACE_NORMAL, 1, L"Calibration Matrix Error rate = %.3f", CalcError(i));
		}
		break;
	}
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 <--

	m_pDoc->m_config.WriteConfig();
}

void CDlgMatrix::OnBnClickedButtonClear()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.	
	m_pDoc->m_Vec3DData.clear();
	m_pDoc->m_VecIWSData.clear();
	m_pDoc->m_VecFitdata.clear();

	DrawChart();
}

void CDlgMatrix::OnBnClickedButtonSave()
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

void CDlgMatrix::FileSave()
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

		_IWS_LOCATION pt, pt1;

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

// 2022.08.15 jh.kim 변경 - Matrix 분할 ( 4분할 )
//void CDlgMatrix::StartOptimizer()
void CDlgMatrix::StartOptimizer(int inx)
{	
	int loopCount = (int)(m_pDoc->m_Vec3DData.size() / MATRIX_DIVISION);
	m_pDoc->m_VecData.clear();

	_IWS_LOCATION pt;
	for (int j = 0; j < loopCount; j++) {
		pt = m_pDoc->m_Vec3DData.at(inx*loopCount + j);
		m_pDoc->m_VecData.push_back(pt);
	}
	m_pDoc->m_nVecIndex = inx;
	m_pDoc->m_nVecCount = loopCount;


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
	//minlmcreatev(m_pDoc->m_Vec3DData.size(), x, 1e-12, state);
	minlmcreatev(m_pDoc->m_VecData.size(), x, 1e-12, state);
	minlmsetbc(state, bndl, bndu);
	minlmsetcond(state, epsx, maxits);
	minlmsetacctype(state, 1);
	//minlmoptimize(state, Func_LM_alglib, NULL, m_pDoc);
	minlmoptimize(state, Func_LM_alglibDiv, NULL, m_pDoc);
	minlmresults(state, x, rep);
	memcpy(m_dMatrix, x.getcontent(), sizeof(double) * 6);
	TRACE("%f,%f,%f\n%f,%f,%f\n", m_dMatrix[0], m_dMatrix[1], m_dMatrix[2],	m_dMatrix[3], m_dMatrix[4], m_dMatrix[5]);

	for (auto p : m_pDoc->m_VecData)
	{
		pt.dX = p.dX * m_dMatrix[0] + p.dY * m_dMatrix[1] + m_dMatrix[2];
		pt.dY = p.dX * m_dMatrix[3] + p.dY * m_dMatrix[4] + m_dMatrix[5];
		m_pDoc->m_VecFitdata.push_back(pt);
	}
}

// sjyi 2022-07-08 PLC로부터 JigNo 획득하기 위한 함수
// Jig No 획득하여, 화면에 표시
void CDlgMatrix::UpdatePlcJigNo()
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