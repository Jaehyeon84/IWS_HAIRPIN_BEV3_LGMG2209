// CDlgHairpinPattern.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgHairpinPattern.h"
#include "afxdialogex.h"
#include <sstream>
#include "MainFrm.h"


CDlgHairpinPattern::CGridRepeatPowerTbl::CGridRepeatPowerTbl()
{
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++) {
		m_dPower[i] = 100.0;
		m_dRepeat[i] = i;
	}
}

CDlgHairpinPattern::CGridRepeatPowerTbl::~CGridRepeatPowerTbl()
{
}

void CDlgHairpinPattern::CGridRepeatPowerTbl::OnSetup()
{
	this->SetNumberCols(1);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(COLOR_GREY30);
	cell.SetBorder(0);
	cell.SetTextColor(COLOR_WHITESMOKE);
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);
	// 	this->HideCurrentCell();
	SetHighlightRow(TRUE);

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("Repeat"));
	QuickSetText(0, -1, _T("Power"));
	SetDefRowHeight(18);
	CRect rect;
	GetClientRect(rect);
	int nWidData = int(double(rect.Width()) * 0.48);
	int nWidIdx = int(double(rect.Width()) * 0.4);

	SetColWidth(-1, nWidIdx);
	SetColWidth(0, nWidData);
	GetColDefault(0, &cell);

	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNRIGHT);
	cell.SetBackColor(COLOR_GREY30);
	cell.SetTextColor(COLOR_WHITESMOKE);
	SetGridDefault(&cell);
	SetColDefault(0, &cell);
}

COLORREF CDlgHairpinPattern::CGridRepeatPowerTbl::OnGetDefBackColor(int section)
{
	switch (section)
	{
	case UG_TOPHEADING:
		return RGB(0x38, 0x38, 0x38);
		break;
	case UG_SIDEHEADING:
		return RGB(0x38, 0x38, 0x38);
		break;
	case UG_GRID:
		return RGB(0x38, 0x38, 0x38);
		break;
	}
	return RGB(0x00, 0x00, 0x00);
}

void CDlgHairpinPattern::CGridRepeatPowerTbl::OnCharDown(UINT * vcKey, BOOL processed)
{
	this->StartEdit();
}

int CDlgHairpinPattern::CGridRepeatPowerTbl::OnEditContinue(int oldcol, long oldrow, int * newcol, long * newrow)
{
	return 0;
}

int CDlgHairpinPattern::CGridRepeatPowerTbl::OnEditFinish(int col, long row, CWnd * edit, LPCTSTR string, BOOL cancelFlag)
{
	double dval = _tstof(string);
	if (dval < 0 || dval > 100.0) return FALSE;
	m_dPower[row] = dval;
	DrawChart();
	return TRUE;
}

void CDlgHairpinPattern::CGridRepeatPowerTbl::SetMaxRow(int nRow)
{
	SetNumberRows(nRow);
	CString str;
	for (int i = 0; i < nRow; i++) {
		str.Format(L"%d", i );
		QuickSetText(-1, i, str);
		str.Format(L"%.2f", m_dPower[i]);
		QuickSetText(0, i, str);
	}
	RedrawAll();
	DrawChart();
}

void CDlgHairpinPattern::CGridRepeatPowerTbl::DrawChart()
{
	CChartViewer *viewer = (CChartViewer *)m_pChart;
	CRect rect;
	m_pChart->GetClientRect(rect);
	// Create a XYChart object of size 640 x 400 pixels
	XYChart *c = new XYChart(rect.Width(), rect.Height());

	// Add a title to the chart using 18 pts Times New Roman Bold Italic font
	//c->addTitle("Spectrum", "timesbi.ttf", 18);

	// Set the plotarea at (50, 55) with width 70 pixels less than chart width, and height 90 pixels
	// less than chart height. Use a vertical gradient from light blue (f0f6ff) to sky blue (a0c0ff)
	// as background. Set border to transparent and grid lines to white (ffffff).
	PlotArea *plotArea = c->setPlotArea(50, 20, c->getWidth() - 70, c->getHeight() - 70, COLOR_GREY22, -1, COLOR_GREY41,
		c->dashLineColor(COLOR_GREY60, DotLine), c->dashLineColor(COLOR_GREY60, DotLine));
	c->setBackground(COLOR_GREY30);
	c->setBorder(COLOR_GREY41);

// 	std::ostringstream legend;
// 	legend << "<*size=10*><*img=@Square,color=0xff3333,width=10,height=10,edgeColor=0x000000*>" << " Power" << "    ";
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
	c->xAxis()->setColors(Chart::Transparent, COLOR_WHITESMOKE, COLOR_WHITESMOKE, COLOR_WHITESMOKE);
	c->yAxis()->setColors(Chart::Transparent, COLOR_WHITESMOKE, COLOR_WHITESMOKE, COLOR_WHITESMOKE);
	// Configure x-axis label format
	// 	c->xAxis()->setMultiFormat(Chart::StartOfYearFilter(), "{value|mm/yyyy} ",
	// 		Chart::StartOfMonthFilter(), "{value|mm}");

	// Add axis title using 10pts Arial Bold Italic font
	c->yAxis()->setTitle("Power(%)", "arialbi.ttf", 10, COLOR_WHITESMOKE);
	c->xAxis()->setTitle("Repeat", "arialbi.ttf", 10, COLOR_WHITESMOKE);
	// Add a line layer to the chart using a line width of 2 pixels.

	// Add 3 data series to the line layer
	int nData = GetNumberRows();
	if (nData) {
		LineLayer *layer = c->addLineLayer();
		layer->setLineWidth(3);
		DoubleArray xdata(m_dRepeat, nData);
		layer->setXData(xdata);

		DoubleArray data0(m_dPower, nData);
		layer->addDataSet(data0, 0xff3333, "Power");
// 		viewer->syncLinearAxisWithViewPort("Repeat", c->xAxis());
// 		viewer->syncLinearAxisWithViewPort("Power(%)", c->yAxis());
// 		viewer->setFullRange("Power(%)", 0, 1.1);
// 		viewer->setFullRange("Repeat", 0, double(nData+20));
		c->xAxis()->setDateScale(0, nData-1, 2, 1);
		c->yAxis()->setDateScale(0,110, 20, 10);

		c->xAxis()->setRounding(false, false);
	}

	delete viewer->getChart();
	// Assign the chart to the WinChartViewer
	viewer->setChart(c);
}

// CDlgHairpinPattern 대화 상자

IMPLEMENT_DYNAMIC(CDlgHairpinPattern, CDialogEx)

CDlgHairpinPattern::CDlgHairpinPattern(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HAIRPIN_PATTERN, pParent)	
	, m_bUse1stPettern(FALSE)
	, m_bUse2ndPettern(FALSE)
	, m_bCloseSpiralP1(FALSE)
	, m_bCloseSpiralP2(FALSE)
	, m_bEnableOutToInP1(FALSE)
	, m_bEnableOutToInP2(FALSE)
{
	setLicenseCode("RDST-269V-GQM2-X8BZ-A621-8B39");

	m_bUseSpiralPtn = FALSE;
	m_bUseSandGlassPtn = FALSE;
}

CDlgHairpinPattern::~CDlgHairpinPattern()
{
	delete m_pChartProfile1st.getChart();
	delete m_pChartProfile2nd.getChart();
}

void CDlgHairpinPattern::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HP_LENGTH, m_editPinSizeL);
	DDX_Control(pDX, IDC_EDIT_HP_WIDTH, m_editPinSizeW);
	DDX_Control(pDX, IDC_EDIT_HP_REPEAT_P1, m_editRepeatP1);
	DDX_Control(pDX, IDC_EDIT_WELD_PEN_P1, m_editWeldPenP1);
	DDX_Radio(pDX, IDC_RADIO_Ell_P1, m_nWeldShapeP1);
	DDX_Control(pDX, IDC_EDIT_PREQ_P1, m_editWeldZigZagFreqP1);
	DDX_Radio(pDX, IDC_RADIO_1ST_1, m_n1ST);
	DDX_Control(pDX, IDC_EDIT_HP_2ND_WIDTH, m_editPinSize2nd);
	DDX_Control(pDX, IDC_EDIT_HP_REPEAT_P2, m_editRepeatP2);
	DDX_Control(pDX, IDC_EDIT_HP_OVERLEN, m_editOverLen);
	DDX_Control(pDX, IDC_EDIT_WELD_PEN_P2, m_editWeldPenP2);
	DDX_Radio(pDX, IDC_RADIO_Ell_P2, m_nWeldShapeP2);
	DDX_Control(pDX, IDC_EDIT_PREQ_P2, m_editWeldZigZagFreqP2);
	DDX_Radio(pDX, IDC_RADIO_2ND_1, m_n2ND);
	DDX_Check(pDX, IDC_CHECK_USE_1ST_PETTERN, m_bUse1stPettern);
	DDX_Check(pDX, IDC_CHECK_USE_2ND_PETTERN, m_bUse2ndPettern);
	DDX_Control(pDX, IDC_CHART_RPT_POWER1, m_pChartProfile1st);
	DDX_Control(pDX, IDC_CHART_RPT_POWER2, m_pChartProfile2nd);
	DDX_Control(pDX, IDC_CHECK_USE_POWER_PROFILE1, m_chkUseVariablePower1st);
	DDX_Control(pDX, IDC_CHECK_USE_POWER_PROFILE2, m_chkUseVariablePower2nd);
	DDX_Control(pDX, IDC_EDIT_1ST_ENERGY, m_edit1StWeldEnergy);
	DDX_Control(pDX, IDC_EDIT_2ND_ENERGY, m_edit2ndWeldEnergy);
	DDX_Control(pDX, IDC_EDIT_TOTAL_ENERGY, m_editTotalWeldEnergy);
	DDX_Control(pDX, IDC_EDIT_1ST_POWER, m_edit1StPower);
	DDX_Control(pDX, IDC_EDIT_1ST_SPEED, m_edit1StSpeed);
	DDX_Control(pDX, IDC_EDIT_2ND_POWER, m_edit2ndPower);
	DDX_Control(pDX, IDC_EDIT_2ND_SPEED, m_edit2ndSpeed);
	DDX_Control(pDX, IDC_EDIT_TOTAL_ENERGY2, m_editConfigWeldEnergy);

	DDX_Control(pDX, IDC_EDIT_HP_ROTATION, m_editPtnRotation);
	DDX_Control(pDX, IDC_EDIT_HP_ROTATION2, m_editPtnRotation2);

	DDX_Control(pDX, IDC_EDIT_HP_2ND_WIDTH2, m_editPinSize2nd2);
	DDX_Check(pDX, IDC_CHECK_CLOSE_P1, m_bCloseSpiralP1);
	DDX_Check(pDX, IDC_CHECK_CLOSE_P2, m_bCloseSpiralP2);
	DDX_Check(pDX, IDC_CHECK_OUTIN_P1, m_bEnableOutToInP1);
	DDX_Check(pDX, IDC_CHECK_OUTIN_P2, m_bEnableOutToInP2);

}


BEGIN_MESSAGE_MAP(CDlgHairpinPattern, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgHairpinPattern::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CDlgHairpinPattern::OnBnClickedCancel)
	ON_EN_KILLFOCUS(IDC_EDIT_HP_LENGTH, &CDlgHairpinPattern::OnEnKillfocusEditHpLength)
	ON_EN_KILLFOCUS(IDC_EDIT_HP_WIDTH, &CDlgHairpinPattern::OnEnKillfocusEditHpWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_HP_REPEAT_P1, &CDlgHairpinPattern::OnEnKillfocusEditHpRepeatP1)
	ON_EN_KILLFOCUS(IDC_EDIT_WELD_PEN_P1, &CDlgHairpinPattern::OnEnKillfocusEditWeldPenP1)
	ON_EN_KILLFOCUS(IDC_EDIT_PREQ_P1, &CDlgHairpinPattern::OnEnKillfocusEditPreqP1)	
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_Ell_P1, IDC_RADIO_SPIRAL_P1, &CDlgHairpinPattern::OnBnClickedRadio1ST)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_1ST_1, IDC_RADIO_1ST_7, &CDlgHairpinPattern::OnBnClickedRadioLayerSelect1ST)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_Ell_P2, IDC_RADIO_SPIRAL_P2, &CDlgHairpinPattern::OnBnClickedRadio2ND)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_2ND_1, IDC_RADIO_2ND_7, &CDlgHairpinPattern::OnBnClickedRadioLayerSelect2ND)
	ON_EN_KILLFOCUS(IDC_EDIT_HP_2ND_WIDTH, &CDlgHairpinPattern::OnEnKillfocusEditHp2ndWidth)
	ON_EN_KILLFOCUS(IDC_EDIT_HP_REPEAT_P2, &CDlgHairpinPattern::OnEnKillfocusEditHpRepeatP2)
	ON_EN_KILLFOCUS(IDC_EDIT_HP_OVERLEN, &CDlgHairpinPattern::OnEnKillfocusEditHpOverlen)
	ON_EN_KILLFOCUS(IDC_EDIT_WELD_PEN_P2, &CDlgHairpinPattern::OnEnKillfocusEditWeldPenP2)
	ON_EN_KILLFOCUS(IDC_EDIT_PREQ_P2, &CDlgHairpinPattern::OnEnKillfocusEditPreqP2)	
	ON_BN_CLICKED(IDC_CHECK_USE_1ST_PETTERN, &CDlgHairpinPattern::OnBnClickedCheckUse1stPettern)
	ON_BN_CLICKED(IDC_CHECK_USE_2ND_PETTERN, &CDlgHairpinPattern::OnBnClickedCheckUse2ndPettern)
	ON_BN_CLICKED(IDC_CHECK_USE_POWER_PROFILE1, &CDlgHairpinPattern::OnBnClickedCheckUsePowerProfile1)
	ON_BN_CLICKED(IDC_CHECK_USE_POWER_PROFILE2, &CDlgHairpinPattern::OnBnClickedCheckUsePowerProfile2)
END_MESSAGE_MAP()


// CDlgHairpinPattern 메시지 처리기


BOOL CDlgHairpinPattern::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_editPinSizeL.SetValue(m_dHpSize);
	m_editPinSizeW.SetValue(m_dHpSizeW);
	m_editPinSize2nd.SetValue(m_dHpSize2ndW);
	m_editRepeatP1.SetValue(m_iRptP1);
	m_editRepeatP2.SetValue(m_iRptP2);
	m_editOverLen.SetValue(m_dHpOverLen);

	m_editWeldPenP1.SetValue(m_nWeldPenP1);
	m_editWeldPenP2.SetValue(m_nWeldPenP2);

	m_editWeldZigZagFreqP1.SetValue(m_nWeldZigZagFreqP1);
	m_editWeldZigZagFreqP2.SetValue(m_nWeldZigZagFreqP2);

	// sjyi 2023.04.14 헤어핀 회전 추가
	m_editPtnRotation.SetValue(m_dPtnRotation);
	m_editPtnRotation2.SetValue(m_dPtnRotation2);

	m_editPinSize2nd2.SetValue(m_dHpSize2ndW2);

	m_n1ST = 6;
	m_n2ND = 6;

	m_bUse1stPettern = TRUE;
	m_bUse2ndPettern = TRUE;

	GetDlgItem(IDC_CHECK_USE_1ST_PETTERN)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_CHECK_USE_2ND_PETTERN)->ShowWindow(SW_HIDE);

// 	CRect rect;
// 	//rect.SetRectEmpty();
// 	GetDlgItem(IDC_CHART_RPT_POWER1)->GetClientRect(rect);
// 	
// 	m_pChartProfile1st.Create(_T("repeat power chart"), WS_CHILD | SS_BITMAP | WS_VISIBLE | WS_EX_CLIENTEDGE, rect, this, IDC_CHART_RPT_POWER1);
// 	m_pChartProfile1st.ShowWindow(SW_SHOW);
	m_gridRptPower1st.AttachGrid(this, IDC_GRID_RPT_POWER1);
	m_gridRptPower2nd.AttachGrid(this, IDC_GRID_RPT_POWER2);
	
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++) {
		m_gridRptPower1st.m_dPower[i] = m_dPower1st[i] * 100.0;
		m_gridRptPower2nd.m_dPower[i] = m_dPower2nd[i] * 100.0;
	}
	m_gridRptPower1st.SetChart(&m_pChartProfile1st);
	m_gridRptPower2nd.SetChart(&m_pChartProfile2nd);
	m_gridRptPower1st.SetMaxRow(m_iRptP1);
	m_gridRptPower2nd.SetMaxRow(m_iRptP2);
	m_gridRptPower1st.EnableWindow(m_bUseRptPowerProfile1st);
	m_gridRptPower2nd.EnableWindow(m_bUseRptPowerProfile2nd);
	m_chkUseVariablePower1st.SetCheck(m_bUseRptPowerProfile1st);
	m_chkUseVariablePower2nd.SetCheck(m_bUseRptPowerProfile2nd);
	
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
			
	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];
	
	m_edit1StPower.SetValue(pPen1->m_dblPower);
	m_edit1StSpeed.SetValue(pPen1->m_dblMarkSpeed);
	m_edit2ndPower.SetValue(pPen2->m_dblPower);
	m_edit2ndSpeed.SetValue(pPen2->m_dblMarkSpeed);

	if (pDoc->m_work.m_JobGroup.size() > 0)
	{ 
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
		//double d1StEnergy, d2ndEnergy;

		//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_nWeldZigZagFreqP1, m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
		//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_dHpOverLen, m_nWeldZigZagFreqP2, m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);

		//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
		//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");
		//m_editTotalWeldEnergy.SetValue(m_edit1StWeldEnergy.GetValue() + m_edit2ndWeldEnergy.GetValue(), L"%.2f");		
		UpdateEnergyCalc();
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
	}
	else
	{
		m_edit1StWeldEnergy.SetValue(0.0, L"%.2f");
		m_edit2ndWeldEnergy.SetValue(0.0, L"%.2f");
		m_editTotalWeldEnergy.SetValue(0.0, L"%.2f");
	}

	m_editConfigWeldEnergy.SetValue(pDoc->m_config.m_dLimitEnergy, L"%.2f");

	//m_editPinSize2nd2.SetValue(m_dHpSize2ndW2);

	UpdateData(FALSE);

	UpdateUI_P2(); // sjyi 2022-05-19 ZigZag일 경우, 패턴 변경 못하도록 수정
	UpdateUI_P1(); // sjyi 2024.02.01 추가

	Check2ndZigZag2(); // sjyi 2022-05-19 ZigZag일 경우, 패턴 변경 못하도록 수정


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgHairpinPattern::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	UpdateData();
	m_dHpSize = m_editPinSizeL.GetValue();
	m_dHpSizeW = m_editPinSizeW.GetValue();
	m_dHpSize2ndW = m_editPinSize2nd.GetValue();
	m_iRptP1 = m_editRepeatP1.GetValue();
	m_iRptP2 = m_editRepeatP2.GetValue();
	m_dHpOverLen = m_editOverLen.GetValue();
	
	m_nWeldPenP1 = m_editWeldPenP1.GetValue();
	m_nWeldPenP2 = m_editWeldPenP2.GetValue();

	m_nWeldZigZagFreqP1 = m_editWeldZigZagFreqP1.GetValue();
	m_nWeldZigZagFreqP2 = m_editWeldZigZagFreqP2.GetValue();

	// sjyi 2023.04.14 헤어핀 회전 추가
	m_dPtnRotation = m_editPtnRotation.GetValue();
	m_dPtnRotation2 = m_editPtnRotation2.GetValue();

	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++) {
		m_dPower1st[i] = m_gridRptPower1st.m_dPower[i] / 100.0;
		m_dPower2nd[i] = m_gridRptPower2nd.m_dPower[i] / 100.0;
	}
	m_bUseRptPowerProfile1st = m_chkUseVariablePower1st.GetCheck();
	m_bUseRptPowerProfile2nd = m_chkUseVariablePower2nd.GetCheck();

	m_dHpSize2ndW2 = m_editPinSize2nd2.GetValue();


	
	if (pDoc->m_config.m_dLimitEnergy < m_editTotalWeldEnergy.GetValue())
	{
		CString strTmp;		
		strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);		
		AfxMessageBox(strTmp, MB_ICONERROR);
		_tracer(TRACE_ERROR, 1, strTmp);
		return;
	}

	CDialogEx::OnOK();

}

void CDlgHairpinPattern::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	_tracer(TRACE_NORMAL, 1, L"Hairpin Pattern has been canceled.");

	if (pDoc->m_config.m_dLimitEnergy < m_editTotalWeldEnergy.GetValue())
	{
		CString strTmp;
		strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
		AfxMessageBox(strTmp, MB_ICONERROR);

		return;
	}

	CDialogEx::OnCancel();
}

void CDlgHairpinPattern::OnEnKillfocusEditHpLength()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	_tracer(TRACE_NORMAL, 1, L"1St Pattern Length has been changed. [%.4f] -> [%.4f]", m_dHpSize, m_editPinSizeL.GetValue());

	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
	//double d1StEnergy, d2ndEnergy;
	//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_editPinSizeL.GetValue(), m_nWeldZigZagFreqP1, m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
	//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_dHpOverLen, m_nWeldZigZagFreqP2, m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
	//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");

	///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
	//{
	//	CString strTmp;
	//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
	//	AfxMessageBox(strTmp, MB_ICONERROR);
	//	_tracer(TRACE_ERROR, 1, strTmp);
	//	m_editTotalWeldEnergy.SetValue(0.0, L"%.2f");
	//}
	//else*/
	//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");

	///*if (m_editPinSizeL.GetValue() <= pDoc->m_config.m_d1StLength)	
	//	_tracer(TRACE_NORMAL, 1, L"1St Pattern Length has been changed. [%.4f] -> [%.4f]", m_dHpSize, m_editPinSizeL.GetValue());
	//else
	//{
	//	CString strMsg;
	//	strMsg.Format(_T("1st Pattern Length, the maximum value can be entered up to %.2f mm."), pDoc->m_config.m_d1StLength);
	//	AfxMessageBox(strMsg, MB_ICONERROR);
	//	m_editPinSizeL.SetValue(0.0f);
	//}*/

	UpdateEnergyCalc();
	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
}

void CDlgHairpinPattern::OnEnKillfocusEditHpWidth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	if (m_editPinSizeW.GetValue() <= pDoc->m_config.m_d1StWidth)
	{
		_tracer(TRACE_NORMAL, 1, L"1St Pattern Width has been changed. [%.4f] -> [%.4f]", m_dHpSizeW, m_editPinSizeW.GetValue());
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
		//double d1StEnergy, d2ndEnergy;		
		//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_editPinSizeW.GetValue(), m_dHpSize, m_nWeldZigZagFreqP1, m_editRepeatP1.GetValue(), pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
		//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
		//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_dHpOverLen, m_nWeldZigZagFreqP2, m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
		//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");
		//
		///*if(pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
		//{
		//	CString strTmp;
		//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
		//	AfxMessageBox(strTmp, MB_ICONERROR);
		//	_tracer(TRACE_ERROR, 1, strTmp);
		//	
		//	return;
		//}
		//else*/
		//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
		UpdateEnergyCalc();
		
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
	}		
	else
	{
		CString strMsg;
		strMsg.Format(_T("1st Pattern Width, the maximum value can be entered up to %.2f mm."), pDoc->m_config.m_d1StWidth);
		AfxMessageBox(strMsg, MB_ICONERROR);
		m_editPinSizeW.SetValue(0.0f);
	}
}


void CDlgHairpinPattern::OnEnKillfocusEditHpRepeatP1()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	if (m_editRepeatP1.GetValue() <= pDoc->m_config.m_n1StRepeat)
	{
		m_gridRptPower1st.SetMaxRow(m_editRepeatP1.GetValue());
		_tracer(TRACE_NORMAL, 1, L"1St Pattern Repeat has been changed. [%d] -> [%d]", m_iRptP1, m_editRepeatP1.GetValue());

		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
		//double d1StEnergy, d2ndEnergy;
		//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_nWeldZigZagFreqP1, m_editRepeatP1.GetValue(), pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
		//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
		//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_dHpOverLen, m_nWeldZigZagFreqP2, m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
		//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");

		///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
		//{
		//	CString strTmp;
		//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
		//	AfxMessageBox(strTmp, MB_ICONERROR);
		//	_tracer(TRACE_ERROR, 1, strTmp);

		//	return;
		//}
		//else*/
		//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
		UpdateEnergyCalc();
		
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
	}
	else
	{
		CString strMsg;
		strMsg.Format(_T("1st Pattern Width, the maximum value can be entered up to %d."), pDoc->m_config.m_n1StRepeat);
		AfxMessageBox(strMsg, MB_ICONERROR);
		m_editRepeatP1.SetValue(0);
	}
}

void CDlgHairpinPattern::OnEnKillfocusEditPreqP1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_tracer(TRACE_NORMAL, 1, L"1St Pattern Zigzag Frequency has been changed. [%d] -> [%d]", m_nWeldZigZagFreqP1, m_editWeldZigZagFreqP1.GetValue());
	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
	//CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	//CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	//CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	//CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	//double d1StEnergy, d2ndEnergy;
	//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_editWeldZigZagFreqP1.GetValue(), m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
	//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_editOverLen.GetValue(), m_nWeldZigZagFreqP2, m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
	//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");
	///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
	//{
	//	CString strTmp;
	//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
	//	AfxMessageBox(strTmp, MB_ICONERROR);
	//	_tracer(TRACE_ERROR, 1, strTmp);

	//	return;
	//}
	//else*/
	//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
	UpdateEnergyCalc();
	
	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
}

void CDlgHairpinPattern::OnEnKillfocusEditWeldPenP1()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_tracer(TRACE_NORMAL, 1, L"1St Pattern PenID has been changed. [%d] -> [%d]", m_nWeldPenP1, m_editWeldPenP1.GetValue());
}

void CDlgHairpinPattern::OnBnClickedRadio1ST(UINT uiD)
{
	CString strBefWeldType, strAftWeldType;

	//변경 전 Welding Type을 알기위해서..
	switch (m_nWeldShapeP1)
	{
		case IWS_1ST_ELLIPSE:	strBefWeldType.Format(L"Ellipse");	break;
		case IWS_1ST_LINE:		strBefWeldType.Format(L"Line");		break;
		case IWS_1ST_ZIGZAG:	strBefWeldType.Format(L"Zigzag");	break;
		case IWS_1ST_OCTAGON:	strBefWeldType.Format(L"Octagon");	break;
		case IWS_1ST_SPIRAL:	strBefWeldType.Format(L"Spiral");	break;
	}

	UpdateData();

	//변경 후 Welding Type을 알기위해서..
	switch (m_nWeldShapeP1)
	{
		case IWS_1ST_ELLIPSE:	strAftWeldType.Format(L"Ellipse");	break;
		case IWS_1ST_LINE:		strAftWeldType.Format(L"Line");		break;
		case IWS_1ST_ZIGZAG:	strAftWeldType.Format(L"Zigzag");	break;
		case IWS_1ST_OCTAGON:	strAftWeldType.Format(L"Octagon");	break;
		case IWS_1ST_SPIRAL:	strAftWeldType.Format(L"Spiral");	break;
	}

	_tracer(TRACE_NORMAL, 1, L"1St Pattern WeldShape has been changed. [%s] -> [%s]", strBefWeldType, strAftWeldType);

	UpdateUI_P1();
}

void CDlgHairpinPattern::OnBnClickedRadio2ND(UINT uiD)
{
	CString strBefWeldType, strAftWeldType;

	//변경 전 Welding Type을 알기위해서..
	switch (m_nWeldShapeP2)
	{
		case IWS_2ND_ELLIPSE:	strBefWeldType.Format(L"Ellipse");	break;
		case IWS_2ND_LINE:		strBefWeldType.Format(L"Line");		break;
		case IWS_2ND_ZIGZAG:	strBefWeldType.Format(L"Zigzag");	break;
		case IWS_2ND_ZIGZAG2:	strBefWeldType.Format(L"Zigzag2");	break; // sjyi 2022-05-23 ZigZag2 패턴 추가
		case IWS_2ND_SANDGLASS:	strBefWeldType.Format(L"SandGlass");	break; // sjyi 2024.05.25 모래시계 추가
		case IWS_2ND_SPIRAL:	strBefWeldType.Format(L"Spiral");	break; // sjyi 2024.05.25 스파이럴 추가
	}

	UpdateData();

	//변경 후 Welding Type을 알기위해서..
	switch (m_nWeldShapeP2)
	{
		case IWS_2ND_ELLIPSE:	strAftWeldType.Format(L"Ellipse");	break;
		case IWS_2ND_LINE:		strAftWeldType.Format(L"Line");		break;
		case IWS_2ND_ZIGZAG:	strAftWeldType.Format(L"Zigzag");	break;
		case IWS_2ND_ZIGZAG2:	strAftWeldType.Format(L"Zigzag2");	break; // sjyi 2022-05-23 ZigZag2 패턴 추가
		case IWS_2ND_SANDGLASS:	strAftWeldType.Format(L"SandGlass");	break; // sjyi 2024.05.25 모래시계 추가
		case IWS_2ND_SPIRAL:	strAftWeldType.Format(L"Spiral");	break; // sjyi 2024.05.25 스파이럴 추가
	}

	_tracer(TRACE_NORMAL, 1, L"2St Pattern WeldShape has been changed. [%s] -> [%s]", strBefWeldType, strAftWeldType);

	UpdateUI_P2();
}

void CDlgHairpinPattern::OnBnClickedRadioLayerSelect1ST(UINT uiD)
{
	CString strBefType, strAftType;

	//변경 전 Type을 알기위해서..
	switch (m_n1ST)
	{
		case 0:	strBefType.Format(L"All");	break;
		case 1:	strBefType.Format(L"1");	break;
		case 2:	strBefType.Format(L"2");	break;
		case 3:	strBefType.Format(L"3");	break;
		case 4:	strBefType.Format(L"4");	break;
		case 5:	strBefType.Format(L"5");	break;
		case 6:	strBefType.Format(L"None");	break;
	}

	UpdateData();

	//변경 후 Type을 알기위해서..
	switch (m_n1ST)
	{
		case 0:	strAftType.Format(L"All");	break;
		case 1:	strAftType.Format(L"1");	break;
		case 2:	strAftType.Format(L"2");	break;
		case 3:	strAftType.Format(L"3");	break;
		case 4:	strAftType.Format(L"4");	break;
		case 5:	strAftType.Format(L"5");	break;
		case 6:	strAftType.Format(L"None");	break;
	}

	_tracer(TRACE_NORMAL, 1, L"1St Pattern Apply Layer has been changed. [%s] -> [%s]", strBefType, strAftType);
}

void CDlgHairpinPattern::OnBnClickedRadioLayerSelect2ND(UINT uiD)
{
	CString strBefType, strAftType;

	//변경 전 Type을 알기위해서..
	switch (m_n2ND)
	{
	case 0:	strBefType.Format(L"All");	break;
	case 1:	strBefType.Format(L"1");	break;
	case 2:	strBefType.Format(L"2");	break;
	case 3:	strBefType.Format(L"3");	break;
	case 4:	strBefType.Format(L"4");	break;
	case 5:	strBefType.Format(L"5");	break;
	case 6:	strBefType.Format(L"None");	break;
	}

	UpdateData();

	//변경 후 Type을 알기위해서..
	switch (m_n2ND)
	{
	case 0:	strAftType.Format(L"All");	break;
	case 1:	strAftType.Format(L"1");	break;
	case 2:	strAftType.Format(L"2");	break;
	case 3:	strAftType.Format(L"3");	break;
	case 4:	strAftType.Format(L"4");	break;
	case 5:	strAftType.Format(L"5");	break;
	case 6:	strAftType.Format(L"None");	break;
	}

	_tracer(TRACE_NORMAL, 1, L"2ND Pattern Apply Layer has been changed. [%s] -> [%s]", strBefType, strAftType);
}

void CDlgHairpinPattern::OnEnKillfocusEditHp2ndWidth()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	if (m_editPinSize2nd.GetValue() <= pDoc->m_config.m_d2ndWidth)
	{
		_tracer(TRACE_NORMAL, 1, L"2ND Pattern Width has been changed. [%.4f] -> [%.4f]", m_dHpSize2ndW, m_editPinSize2nd.GetValue());

		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
		//double d1StEnergy, d2ndEnergy;
		//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_nWeldZigZagFreqP1, m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
		//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
		//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_editPinSize2nd.GetValue(), m_dHpOverLen, m_nWeldZigZagFreqP2, m_iRptP1, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
		//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");

		///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
		//{
		//	CString strTmp;
		//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
		//	AfxMessageBox(strTmp, MB_ICONERROR);
		//	_tracer(TRACE_ERROR, 1, strTmp);

		//	return;
		//}
		//else*/
		//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
		UpdateEnergyCalc();
		
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
	}		
	else
	{
		CString strMsg;
		strMsg.Format(_T("1st Pattern Width, the maximum value can be entered up to %.2f mm."), pDoc->m_config.m_d2ndWidth);
		AfxMessageBox(strMsg, MB_ICONERROR);				
	}
}


void CDlgHairpinPattern::OnEnKillfocusEditHpRepeatP2()
{	
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	if (m_editRepeatP2.GetValue() <= pDoc->m_config.m_n2ndRepeat)
	{
		m_gridRptPower2nd.SetMaxRow(m_editRepeatP2.GetValue());
		_tracer(TRACE_NORMAL, 1, L"2ND Pattern Repeat has been changed. [%d] -> [%d]", m_iRptP2, m_editRepeatP2.GetValue());

		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
		//double d1StEnergy, d2ndEnergy;
		//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_nWeldZigZagFreqP1, m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
		//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
		//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_dHpOverLen, m_nWeldZigZagFreqP2, m_editRepeatP2.GetValue(), pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
		//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");

		///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
		//{
		//	CString strTmp;
		//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
		//	AfxMessageBox(strTmp, MB_ICONERROR);
		//	_tracer(TRACE_ERROR, 1, strTmp);

		//	return;
		//}
		//else*/
		//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
		UpdateEnergyCalc();
		
		// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
	}	
	else
	{
		CString strMsg;
		strMsg.Format(_T("2nd Pattern Repeat, the maximum value can be entered up to %d."), pDoc->m_config.m_n2ndRepeat);
		AfxMessageBox(strMsg, MB_ICONERROR);
		m_editRepeatP2.SetValue(0);
	}
}


void CDlgHairpinPattern::OnEnKillfocusEditHpOverlen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	_tracer(TRACE_NORMAL, 1, L"2ND Pattern Over Legnth has been changed. [%.4f] -> [%.4f]", m_dHpOverLen, m_editOverLen.GetValue());

	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
	//double d1StEnergy, d2ndEnergy;
	//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_nWeldZigZagFreqP1, m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
	//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_editOverLen.GetValue(), m_nWeldZigZagFreqP2, m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
	//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");

	///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
	//{
	//	CString strTmp;
	//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
	//	AfxMessageBox(strTmp, MB_ICONERROR);
	//	_tracer(TRACE_ERROR, 1, strTmp);
	//	
	//	return;
	//}
	//else*/
	//	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");

	///*if(m_editOverLen.GetValue() < pDoc->m_config.m_d2ndLength)
	//	_tracer(TRACE_NORMAL, 1, L"2ND Pattern Over Legnth has been changed. [%.4f] -> [%.4f]", m_dHpOverLen, m_editOverLen.GetValue());
	//else
	//{
	//	CString strMsg;
	//	strMsg.Format(_T("2nd Pattern OverLength, the maximum value can be entered up to %.2f mm."), pDoc->m_config.m_d2ndLength);
	//	AfxMessageBox(strMsg, MB_ICONERROR);
	//	m_editRepeatP2.SetValue(0);
	//}*/

	UpdateEnergyCalc();
	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
}


void CDlgHairpinPattern::OnEnKillfocusEditWeldPenP2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	_tracer(TRACE_NORMAL, 1, L"2ND Pattern PenID has been changed. [%d] -> [%d]", m_nWeldPenP2, m_editWeldPenP2.GetValue());
}


void CDlgHairpinPattern::OnEnKillfocusEditPreqP2()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	_tracer(TRACE_NORMAL, 1, L"1St Pattern Zigzag Frequency has been changed. [%d] -> [%d]", m_nWeldZigZagFreqP2, m_editWeldZigZagFreqP2.GetValue());

	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
	//double d1StEnergy, d2ndEnergy;
	//d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeW, m_dHpSize, m_nWeldZigZagFreqP1, m_iRptP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	//m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
	//d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndW, m_dHpOverLen, m_editWeldZigZagFreqP2.GetValue(), m_iRptP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
	//m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");
	///*if (pDoc->m_config.m_dLimitEnergy < d1StEnergy + d2ndEnergy)
	//{
	//	CString strTmp;
	//	strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
	//	AfxMessageBox(strTmp, MB_ICONERROR);
	//	_tracer(TRACE_ERROR, 1, strTmp);

	//	return;
	//}
	//else*/
	//m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
	UpdateEnergyCalc();
	// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---
}

void CDlgHairpinPattern::OnBnClickedCheckUse1stPettern()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	if (m_bUse1stPettern)
	{
		m_editPinSizeL.EnableWindow(TRUE);
		m_editPinSizeW.EnableWindow(TRUE);
		m_editRepeatP1.EnableWindow(TRUE);
		m_editWeldPenP1.EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_Ell_P1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_Line_P1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_Zigzag_P1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_OCTAGON_P1)->EnableWindow(TRUE);

		m_editWeldZigZagFreqP1.EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_1ST_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1ST_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1ST_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1ST_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1ST_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1ST_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_1ST_7)->EnableWindow(TRUE);
	}
	else
	{
		m_editPinSizeL.EnableWindow(FALSE);
		m_editPinSizeW.EnableWindow(FALSE);
		m_editRepeatP1.EnableWindow(FALSE);
		m_editWeldPenP1.EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_Ell_P1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_Line_P1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_Zigzag_P1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_OCTAGON_P1)->EnableWindow(FALSE);

		m_editWeldZigZagFreqP1.EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_1ST_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_1ST_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_1ST_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_1ST_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_1ST_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_1ST_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_1ST_7)->EnableWindow(FALSE);
	}
}


void CDlgHairpinPattern::OnBnClickedCheckUse2ndPettern()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData();

	if (m_bUse2ndPettern)
	{
		m_editPinSize2nd.EnableWindow(TRUE);
		m_editRepeatP2.EnableWindow(TRUE);
		m_editOverLen.EnableWindow(TRUE);
		m_editWeldPenP2.EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_Ell_P2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_Line_P2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_Zigzag_P2)->EnableWindow(TRUE);

		m_editWeldZigZagFreqP2.EnableWindow(TRUE);

		GetDlgItem(IDC_RADIO_2ND_1)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_2ND_2)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_2ND_3)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_2ND_4)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_2ND_5)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_2ND_6)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_2ND_7)->EnableWindow(TRUE);
	}
	else
	{
		m_editPinSize2nd.EnableWindow(FALSE);
		m_editRepeatP2.EnableWindow(FALSE);
		m_editOverLen.EnableWindow(FALSE);
		m_editWeldPenP2.EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_Ell_P2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_Line_P2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_Zigzag_P2)->EnableWindow(FALSE);

		m_editWeldZigZagFreqP2.EnableWindow(FALSE);

		GetDlgItem(IDC_RADIO_2ND_1)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_2ND_2)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_2ND_3)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_2ND_4)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_2ND_5)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_2ND_6)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_2ND_7)->EnableWindow(FALSE);
	}
}

void CDlgHairpinPattern::OnBnClickedCheckUsePowerProfile1()
{
	BOOL bCheck = m_chkUseVariablePower1st.GetCheck();
	_tracer(TRACE_NORMAL, 1, L"1st pattern power shape has been changed. [%d] -> [%d]", m_bUseRptPowerProfile1st, bCheck);
	m_gridRptPower1st.EnableWindow(bCheck);
}


void CDlgHairpinPattern::OnBnClickedCheckUsePowerProfile2()
{
	BOOL bCheck = m_chkUseVariablePower2nd.GetCheck();
	_tracer(TRACE_NORMAL, 1, L"2nd pattern power shape has been changed. [%d] -> [%d]", m_bUseRptPowerProfile2nd, bCheck);
	m_gridRptPower2nd.EnableWindow(bCheck);
}

// sjyi 2022-05-23 ZigZag2 패턴 넘버 추가
// sjyi 2022-05-19 ZigZag 패턴 체크 함수
// 2nd 패턴이 지그재그인 경우, 지그재그로만 변경 가능
// 그 외의 경우에는 지그재그로 변경 불가
void CDlgHairpinPattern::Check2ndZigZag2()
{
	if (m_nWeldShapeP2 == IWS_2ND_ZIGZAG2) // ZigZag2
	{
		GetDlgItem(IDC_RADIO_Ell_P2)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_Line_P2)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_Zigzag_P2)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SPIRAL_P2)->ShowWindow(FALSE);
		GetDlgItem(IDC_RADIO_SandGlass_P2)->ShowWindow(FALSE);

		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Length"));
	}
	else
	{
		GetDlgItem(IDC_RADIO_Zigzag2_P2)->ShowWindow(FALSE);
		GetDlgItem(IDC_EDIT_HP_ROTATION2)->ShowWindow(FALSE);
		GetDlgItem(IDC_STATIC_ROTATION_P2)->ShowWindow(FALSE);
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Over Length"));
	}
}

// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 --->
void CDlgHairpinPattern::UpdateEditCtrolTemp()
{
	m_dHpSizeTemp = m_editPinSizeL.GetValue();
	m_dHpSizeWTemp = m_editPinSizeW.GetValue();
	m_dHpSize2ndWTemp = m_editPinSize2nd.GetValue();
	m_dHpOverLenTemp = m_editOverLen.GetValue();
	m_iRptP1Temp = m_editRepeatP1.GetValue();
	m_iRptP2Temp = m_editRepeatP2.GetValue();

	m_nWeldPenP1Temp = m_editWeldPenP1.GetValue();
	m_nWeldPenP2Temp = m_editWeldPenP2.GetValue();

	m_nWeldZigZagFreqP1Temp = m_editWeldZigZagFreqP1.GetValue();
	m_nWeldZigZagFreqP2Temp = m_editWeldZigZagFreqP2.GetValue();
}

void CDlgHairpinPattern::UpdateEnergyCalc()
{
	UpdateEditCtrolTemp();

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	double d1StEnergy, d2ndEnergy;
	d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, m_nWeldShapeP1, m_dHpSizeWTemp, m_dHpSizeTemp, m_nWeldZigZagFreqP1Temp, m_iRptP1Temp, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, m_nWeldShapeP2, m_dHpSize2ndWTemp, m_dHpOverLenTemp, m_nWeldZigZagFreqP2Temp, m_iRptP2Temp, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);

	m_edit1StWeldEnergy.SetValue(d1StEnergy, L"%.2f");
	m_edit2ndWeldEnergy.SetValue(d2ndEnergy, L"%.2f");
	m_editTotalWeldEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
}
// sjyi 2022-05-20 에너지 계산 관련 함수 추가 적용 <---

//void CDlgHairpinPattern::InitUI()
//{
//	if (m_bUseSpiralPtn)
//	{
//		GetDlgItem(IDC_RADIO_SPIRAL_P1)->ShowWindow(SW_SHOW);
//		GetDlgItem(IDC_CHECK_OUTIN_P1)->ShowWindow(SW_SHOW);
//		GetDlgItem(IDC_CHECK_CLOSE_P1)->ShowWindow(SW_SHOW);
//	}
//	else
//	{
//		GetDlgItem(IDC_RADIO_SPIRAL_P1)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_CHECK_OUTIN_P1)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_CHECK_CLOSE_P1)->ShowWindow(SW_HIDE);
//	}
//
//	if (m_bUseSpiralPtn)
//	{
//		GetDlgItem(IDC_RADIO_SPIRAL_P2)->ShowWindow(SW_SHOW);
//		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_SHOW);
//		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_SHOW);
//	}
//	else
//	{
//		GetDlgItem(IDC_RADIO_SPIRAL_P2)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_HIDE);
//	}
//
//	if (m_bUseSandGlassPtn)
//	{
//		GetDlgItem(IDC_RADIO_SandGlass_P2)->ShowWindow(SW_SHOW);
//	}
//	else
//	{
//		GetDlgItem(IDC_RADIO_SandGlass_P2)->ShowWindow(SW_HIDE);
//	}
//
//	Check2ndZigZag2(); // sjyi 2022-05-19 ZigZag일 경우, 패턴 변경 못하도록 수정
//
//	UpdateUI_P1();
//	UpdateUI_P2();
//}


void CDlgHairpinPattern::UpdateUI_P1()
{

	if (m_bUseSpiralPtn)
	{
		GetDlgItem(IDC_RADIO_SPIRAL_P1)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_RADIO_SPIRAL_P1)->ShowWindow(SW_HIDE);
	}

	switch (m_nWeldShapeP1)
	{
	case IWS_1ST_ELLIPSE:
	case IWS_1ST_LINE:
	case IWS_1ST_OCTAGON:
		SetDlgItemText(IDC_STATIC_LENGTH_P1, _T("Length"));
		SetDlgItemText(IDC_STATIC_WIDTH_P1, _T("Width"));
		GetDlgItem(IDC_CHECK_OUTIN_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CLOSE_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FREQ_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_PREQ_P1)->ShowWindow(SW_HIDE);
		break;
	case IWS_1ST_ZIGZAG:
		SetDlgItemText(IDC_STATIC_LENGTH_P1, _T("Length"));
		SetDlgItemText(IDC_STATIC_WIDTH_P1, _T("Width"));
		SetDlgItemText(IDC_STATIC_FREQ_P1, _T("Zigzag Frequency"));
		GetDlgItem(IDC_CHECK_OUTIN_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CLOSE_P1)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FREQ_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PREQ_P1)->ShowWindow(SW_SHOW);
		break;
	case IWS_1ST_SPIRAL:
		SetDlgItemText(IDC_STATIC_LENGTH_P1, _T("Diameter1(X)"));
		SetDlgItemText(IDC_STATIC_WIDTH_P1, _T("Diameter2(Y)"));
		SetDlgItemText(IDC_STATIC_FREQ_P1, _T("No of Tuns"));
		GetDlgItem(IDC_CHECK_OUTIN_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_CLOSE_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FREQ_P1)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PREQ_P1)->ShowWindow(SW_SHOW);
		break;
	}
}

// sjyi 2022-05-23 ZigZag2 패턴 넘버 추가
// sjyi 2022-05-19 ZigZag 패턴 체크 함수
// 2nd 패턴이 지그재그인 경우, 지그재그로만 변경 가능
// 그 외의 경우에는 지그재그로 변경 불가
void CDlgHairpinPattern::UpdateUI_P2()
{
	if (m_bUseSpiralPtn)
	{
		GetDlgItem(IDC_RADIO_SPIRAL_P2)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_RADIO_SPIRAL_P2)->ShowWindow(SW_HIDE);
	}

	if (m_bUseSandGlassPtn)
	{
		GetDlgItem(IDC_RADIO_SandGlass_P2)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_RADIO_SandGlass_P2)->ShowWindow(SW_HIDE);
	}


	switch (m_nWeldShapeP2)
	{
	case IWS_2ND_ELLIPSE:
	case IWS_2ND_LINE:
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Over Length"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH1, _T("Weld Width"));
		GetDlgItem(IDC_STATIC_2ND_WIDTH2)->ShowWindow(SW_HIDE);
		m_editPinSize2nd2.ShowWindow(SW_HIDE);
		m_editOverLen.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OVERLENGTH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_HIDE);
		m_editPtnRotation2.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ROTATION_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FREQ_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_PREQ_P2)->ShowWindow(SW_HIDE);
		break;
	case IWS_2ND_ZIGZAG:
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Over Length"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH1, _T("Weld Width"));
		GetDlgItem(IDC_STATIC_2ND_WIDTH2)->ShowWindow(SW_HIDE);
		m_editPinSize2nd2.ShowWindow(SW_HIDE);
		m_editOverLen.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OVERLENGTH)->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_STATIC_FREQ_P2, _T("Zigzag Frequency"));
		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_HIDE);
		m_editPtnRotation2.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ROTATION_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FREQ_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PREQ_P2)->ShowWindow(SW_SHOW);
		break;
	case IWS_2ND_ZIGZAG2:
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Pin Size(L)"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH1, _T("Pin Size(W)"));
		GetDlgItem(IDC_STATIC_2ND_WIDTH2)->ShowWindow(SW_HIDE);
		m_editPinSize2nd2.ShowWindow(SW_HIDE);
		m_editOverLen.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OVERLENGTH)->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_STATIC_FREQ_P2, _T("Zigzag Frequency"));
		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_HIDE);
		m_editPtnRotation2.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_ROTATION_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_FREQ_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PREQ_P2)->ShowWindow(SW_SHOW);
		break;
	case IWS_2ND_SANDGLASS:
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Over Length"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH1, _T("Weld Width 1"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH2, _T("Weld Width 2"));
		GetDlgItem(IDC_STATIC_2ND_WIDTH2)->ShowWindow(SW_SHOW);
		m_editPinSize2nd2.ShowWindow(SW_SHOW);
		m_editOverLen.ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_OVERLENGTH)->ShowWindow(SW_SHOW);
		SetDlgItemText(IDC_STATIC_FREQ_P2, _T("Zigzag Frequency"));
		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_HIDE);
		m_editPtnRotation2.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ROTATION_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FREQ_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_PREQ_P2)->ShowWindow(SW_HIDE);
		break;
	case IWS_2ND_SPIRAL: // sjyi 2023.12.18 추가
		SetDlgItemText(IDC_STATIC_2ND_WIDTH1, _T("Diameter1(X)"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH2, _T("Diameter2(Y)"));
		GetDlgItem(IDC_STATIC_2ND_WIDTH2)->ShowWindow(SW_SHOW);
		m_editPinSize2nd2.ShowWindow(SW_SHOW);
		m_editOverLen.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_OVERLENGTH)->ShowWindow(SW_HIDE);
		SetDlgItemText(IDC_STATIC_FREQ_P2, _T("No of Tuns"));
		GetDlgItem(IDC_CHECK_OUTIN_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_CLOSE_P2)->ShowWindow(SW_SHOW);
		m_editPtnRotation2.ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_ROTATION_P2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_FREQ_P2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_PREQ_P2)->ShowWindow(SW_SHOW);
		break;
	}
}