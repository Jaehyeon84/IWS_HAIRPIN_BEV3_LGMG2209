// CDlgLaserPenCW.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgLaserPenPulseShape.h"
#include "afxdialogex.h"


// CDlgLaserPen 대화 상자
DECLARE_USER_MESSAGE(UWM_CGRIDPULSESHAPE)

IMPLEMENT_DYNAMIC(CDlgLaserPenPulseShape, CDialogEx)

CDlgLaserPenPulseShape::CDlgLaserPenPulseShape(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LASERPEN_PULSESHAPE, pParent)
{

}

CDlgLaserPenPulseShape::~CDlgLaserPenPulseShape()
{
	delete m_pChart;
}

void CDlgLaserPenPulseShape::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_PENID, m_editPenID);
	DDX_Control(pDX, IDC_EDIT_FREQUENCY, m_editFrequency);
	DDX_Control(pDX, IDC_EDIT_PULSEWIDTH, m_editPulseWidth);
	DDX_Control(pDX, IDC_EDIT_JUMPSPEED, m_editJumpSpeed);
	DDX_Control(pDX, IDC_EDIT_WELDSPEED, m_editWeldSpeed);
	DDX_Control(pDX, IDC_EDIT_JUMPDELAY, m_editJumpDelay);
	DDX_Control(pDX, IDC_EDIT_OFFDELAY, m_editLaserOffDelay);
	DDX_Control(pDX, IDC_EDIT_ONDELAY, m_editLaserOnDelay);
	DDX_Control(pDX, IDC_EDIT_WELDDELAY, m_editWeldDelay);
	DDX_Control(pDX, IDC_EDIT_POLYDELAY, m_editPolygonDelay);
	DDX_Control(pDX, IDC_COMBOBOXEX_LASERPENS, m_cbLaserPens);
	DDX_Control(pDX, IDC_EDIT_LASERPOWER, m_editLaserPower);
	DDX_Control(pDX, IDC_CHECK_WOBBEL, m_chkUseWobbel);
	DDX_Control(pDX, IDC_EDIT_WB_FREQ, m_editWbFreq);
	DDX_Control(pDX, IDC_EDIT_WB_LONGI, m_editWbLongitude);
	DDX_Control(pDX, IDC_EDIT_WB_TRANSE, m_editWbTranseverse);
	DDX_Control(pDX, IDC_CHECK_PULSE_SHAPE, m_chkPulseShape);
	DDX_Control(pDX, IDC_STATIC_PULSEWID, m_stcPulseWidth);
}


BEGIN_MESSAGE_MAP(CDlgLaserPenPulseShape, CDialogEx)

	ON_BN_CLICKED(IDOK, &CDlgLaserPenPulseShape::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LASERPENS, &CDlgLaserPenPulseShape::OnCbnSelchangeComboboxexLaserpens)
	ON_BN_CLICKED(IDC_MFCBUTTON_LASERPEN_APPLY, &CDlgLaserPenPulseShape::OnBnClickedMfcbuttonLaserpenApply)
	ON_BN_CLICKED(IDC_CHECK_WOBBEL, &CDlgLaserPenPulseShape::OnBnClickedCheckWobbel)
	ON_REGISTERED_MESSAGE(UWM_CGRIDPULSESHAPE, &CDlgLaserPenPulseShape::OnUwmCgridPowerShape)
	ON_BN_CLICKED(IDC_MFCBUTTON_ADD_ROW, &CDlgLaserPenPulseShape::OnBnClickedMfcbuttonAddRow)
	ON_BN_CLICKED(IDC_MFCBUTTON_DEL_ROW, &CDlgLaserPenPulseShape::OnBnClickedMfcbuttonDelRow)
	ON_BN_CLICKED(IDC_CHECK_PULSE_SHAPE, &CDlgLaserPenPulseShape::OnClickedCheckPulseShape)
//	ON_EN_CHANGE(IDC_EDIT_FREQUENCY, &CDlgLaserPenPulseShape::OnEnChangeEditFrequency)
ON_EN_CHANGE(IDC_EDIT_PULSEWIDTH, &CDlgLaserPenPulseShape::OnChangeEditPulsewidth)
END_MESSAGE_MAP()


// CDlgLaserPen 메시지 처리기


void CDlgLaserPenPulseShape::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


BOOL CDlgLaserPenPulseShape::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	int nPens = (int)m_pPens->size();
	for (int i = 0; i < nPens - 2; i++)
	{
		str.Format(L"Laser Pen %d", (*m_pPens)[i].m_iPenID);
		m_cbLaserPens.AddStringC(str, g_clrPen[i]);

	}
	//str = L"1st ReWeld Pen";
	//m_cbLaserPens.AddStringC(str, g_clrPen[nPens - 4]);

	//str = L"2nd ReWeld Pen";
	//m_cbLaserPens.AddStringC(str, g_clrPen[nPens - 3]);

	str = L"Simulation Pen";
	//m_cbLaserPens.AddString(str);
	m_cbLaserPens.AddStringC(str, g_clrPen[nPens-2]);
	//str = L"VScan Pen";
	////m_cbLaserPens.AddString(str);
	//m_cbLaserPens.AddStringC(str, g_clrPen[nPens-1]);
	
	CRect rect;
	GetDlgItem(IDC_STATIC_PWCHART)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_PWCHART)->ClientToScreen(rect);
	ScreenToClient(rect);
	m_pChart = new CChartPowerShape;
	m_pChart->Create(_T("Histo chart"), WS_CHILD | SS_BITMAP | WS_VISIBLE | WS_EX_CLIENTEDGE, rect, this, IDC_STATIC_PWCHART/* IDC_STC_ROI_IMG*/);

	m_pChart->ShowWindow(SW_SHOW);
	m_pChart->m_strTitleX = "Time(T)";
	m_pChart->DrawChart();
	m_gridShape.AttachGrid(this, IDC_STATIC_PULSEGRID);
	m_gridShape.SetEventReciveHandle(this->GetSafeHwnd());
	GetDlgItem(IDC_MFCBUTTON_DEL_ROW)->EnableWindow(FALSE);
	GetDlgItem(IDC_MFCBUTTON_ADD_ROW)->EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgLaserPenPulseShape::OnCbnSelchangeComboboxexLaserpens()
{
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;
	CLaserPen *pPen = &(*m_pPens)[iSel];
	m_editPenID.SetValue(pPen->m_iPenID);
	m_editLaserPower.SetValue(pPen->m_dblPower, L"%.1f");
	if (pPen->m_dblPulseWidth < 200)
		pPen->m_dblPulseWidth = 200;
	else if (pPen->m_dblPulseWidth > 50000)
		pPen->m_dblPulseWidth = 50000;
	m_editPulseWidth.SetMinMax(200, 50000);
	m_editPulseWidth.SetValue(pPen->m_dblPulseWidth, L"%.3f");

	double dHz = 1000.0 / (pPen->m_dblPulseWidth / PULSE_DUTY_CYCLE);
	m_editFrequency.SetValue(dHz, L"%.1f");
	m_editJumpSpeed.SetValue(pPen->m_dblJumpSpeed, L"%.1f");;
	m_editWeldSpeed.SetValue(pPen->m_dblMarkSpeed, L"%.1f");;
	m_editJumpDelay.SetValue(pPen->m_nJumpDelay);
	m_editLaserOffDelay.SetValue(pPen->m_nLaserOffDelay);
	m_editLaserOnDelay.SetValue(pPen->m_nLaserOnDelay);
	m_editWeldDelay.SetValue(pPen->m_nMarkDelay);
	m_editPolygonDelay.SetValue(pPen->m_nPolygonDelay);
	m_chkUseWobbel.SetCheck(pPen->m_bUseWobbel);
	m_editWbFreq.SetValue(pPen->m_dWbFreq);
	m_editWbLongitude.SetValue(pPen->m_dWbLongitudinal);
	m_editWbTranseverse.SetValue(pPen->m_dWbTransbersal);
	if (pPen->m_bUsePulseShape) {
		m_pulseShape.resize(pPen->m_pulseShape.size());
		std::copy(pPen->m_pulseShape.begin(), pPen->m_pulseShape.end(), m_pulseShape.begin());
	}
	else
		m_pulseShape.clear();
	m_chkPulseShape.SetCheck(pPen->m_bUsePulseShape);

	UpdateChart(pPen);
	UpdateGrid(pPen);

	OnBnClickedCheckWobbel();
	GetDlgItem(IDC_MFCBUTTON_DEL_ROW)->EnableWindow(pPen->m_bUsePulseShape);
	GetDlgItem(IDC_MFCBUTTON_ADD_ROW)->EnableWindow(pPen->m_bUsePulseShape);
}

void CDlgLaserPenPulseShape::UpdateChart(CLaserPen *pPen)
{
	BOOL bUsePwrShape = m_chkPulseShape.GetCheck();

	if (m_pChart->m_px)	delete[] m_pChart->m_px;
	if (m_pChart->m_pdata) delete[] m_pChart->m_pdata;

	m_pChart->m_px = NULL;
	m_pChart->m_pdata = NULL;
	m_pChart->m_nData = 0;
	if (!bUsePwrShape) {
		m_pChart->DrawChart();
		return;
	}

	size_t nSize = m_pulseShape.size();

	m_pChart->m_px = new double[nSize];
	m_pChart->m_pdata = new double[nSize];

	for (size_t i = 0; i < nSize; i++) {
		m_pChart->m_px[i] = m_pulseShape[i].x;
		m_pChart->m_pdata[i] = m_pulseShape[i].y;
	}
	m_pChart->m_nData = (int)nSize;

	m_pChart->DrawChart();
}


void CDlgLaserPenPulseShape::UpdateGrid(CLaserPen *pPen)
{
	BOOL bUsePwrShape = m_chkPulseShape.GetCheck();
	if (!bUsePwrShape) {
		m_gridShape.SetNumberRows(0);
		return;
	}
	size_t nSize = m_pulseShape.size();
	m_gridShape.SetNumberRows((long)nSize);
	for (size_t i = 0; i < nSize; i++) {
		m_gridShape.QuickSetNumber(-1, (long)i, (double)i);
		m_gridShape.SetDouble(0, (long)i, m_pulseShape[i].x);
		m_gridShape.SetDouble(1, (long)i, m_pulseShape[i].y);
	}
	m_gridShape.RedrawAll();
}

void CDlgLaserPenPulseShape::OnBnClickedMfcbuttonLaserpenApply()
{
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;
	CLaserPen *pPen = &(*m_pPens)[iSel];
	pPen->m_iPenID = m_editPenID.GetValue();
	pPen->m_dblPower = m_editLaserPower.GetValue();
	pPen->m_dblFrequency = m_editFrequency.GetValue();
	pPen->m_dblPulseWidth = m_editPulseWidth.GetValue();
	pPen->m_dblJumpSpeed = m_editJumpSpeed.GetValue();
	pPen->m_dblMarkSpeed = m_editWeldSpeed.GetValue();
	pPen->m_nJumpDelay = m_editJumpDelay.GetValue();
	pPen->m_nLaserOffDelay = m_editLaserOffDelay.GetValue();
	pPen->m_nLaserOnDelay = m_editLaserOnDelay.GetValue();
	pPen->m_nMarkDelay = m_editWeldDelay.GetValue();
	pPen->m_nPolygonDelay = m_editPolygonDelay.GetValue();
	pPen->m_bUseWobbel = m_chkUseWobbel.GetCheck();
	pPen->m_dWbFreq = m_editWbFreq.GetValue();
	pPen->m_dWbLongitudinal = m_editWbLongitude.GetValue();
	pPen->m_dWbTransbersal = m_editWbTranseverse.GetValue();
	pPen->m_bUsePulseShape = m_chkPulseShape.GetCheck();
	if (pPen->m_bUsePulseShape) {
		pPen->m_pulseShape.clear();
		pPen->m_pulseShape.resize(m_pulseShape.size());
		std::copy(m_pulseShape.begin(), m_pulseShape.end(), pPen->m_pulseShape.begin());
	}
	CString str;
	str.Format(L"Pen%d was saved", iSel);
	AfxMessageBox(str);
}


void CDlgLaserPenPulseShape::OnBnClickedCheckWobbel()
{
	BOOL bEnable = m_chkUseWobbel.GetCheck();
	m_editWbFreq.EnableWindow(bEnable);
	m_editWbLongitude.EnableWindow(bEnable);
	m_editWbTranseverse.EnableWindow(bEnable);
}


afx_msg LRESULT CDlgLaserPenPulseShape::OnUwmCgridPowerShape(WPARAM wParam, LPARAM lParam)
{
	int irow = (int)lParam;
	TRACE("OnUwmCgridPowerShape\n");
	m_pulseShape[irow].x = m_gridShape.GetDouble(0, irow);
	m_pulseShape[irow].y = m_gridShape.GetDouble(1, irow);
	UpdateChart(NULL);
	UpdateGrid(NULL);
	return 0;
}


void CDlgLaserPenPulseShape::OnBnClickedMfcbuttonAddRow()
{
	int iRow = m_gridShape.GetCurrentRow();
	if (iRow >= m_pulseShape.size())
		m_pulseShape.push_back(m_pulseShape[iRow - 1]);
	else
		m_pulseShape.insert(m_pulseShape.begin() + iRow + 1, m_pulseShape[iRow]);
	UpdateChart(NULL);
	UpdateGrid(NULL);
}


void CDlgLaserPenPulseShape::OnBnClickedMfcbuttonDelRow()
{
	int iRow = m_gridShape.GetCurrentRow();
	m_pulseShape.erase(m_pulseShape.begin() + iRow);
	UpdateChart(NULL);
	UpdateGrid(NULL);
}


void CDlgLaserPenPulseShape::OnClickedCheckPulseShape()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = m_chkPulseShape.GetCheck();
	if (bCheck) {
		if (!m_pulseShape.size()) {
			POINTDOUBLE var;
			var.x = 0;
			var.y = m_editLaserPower.GetValue();
			m_pulseShape.push_back(var);
			var.x = 100;
			var.y = m_editLaserPower.GetValue();
			m_pulseShape.push_back(var);
		}
		if (m_editFrequency.GetValue() > 1)
			m_editFrequency.SetValue(1.0);
		UpdateChart(NULL);
		UpdateGrid(NULL);
	}

	m_chkUseWobbel.EnableWindow(!bCheck);

	if (m_chkUseWobbel.GetCheck()) {
		m_chkUseWobbel.SetCheck(FALSE);
	}
	OnBnClickedCheckWobbel();
	GetDlgItem(IDC_MFCBUTTON_DEL_ROW)->EnableWindow(bCheck);
	GetDlgItem(IDC_MFCBUTTON_ADD_ROW)->EnableWindow(bCheck);
}


//void CDlgLaserPenPulseShape::OnEnChangeEditFrequency()
//{
//	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
//	// CDialogEx::OnInitDialog() 함수를 재지정 
//	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
//	// 이 알림 메시지를 보내지 않습니다.
//	double dHz = 1000.0 / (m_editPulseWidth.GetValue() / PULSE_DUTY_CYCLE);
//	m_editFrequency.SetValue(dHz);
//	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
//}



void CDlgLaserPenPulseShape::OnChangeEditPulsewidth()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.
	double dHz = 1000.0 / (m_editPulseWidth.GetValue() / PULSE_DUTY_CYCLE);
	m_editFrequency.SetValue(dHz);
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}
