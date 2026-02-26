// CDlgLaserPenCW.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgLaserPenCW.h"
#include "afxdialogex.h"
#include "MainFrm.h"

// CDlgLaserPen 대화 상자
DECLARE_USER_MESSAGE(UWM_CGRIDPOWERSHAPE)

IMPLEMENT_DYNAMIC(CDlgLaserPenCW, CDialogEx)

CDlgLaserPenCW::CDlgLaserPenCW(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LASERPEN_CW, pParent)
{

}

CDlgLaserPenCW::~CDlgLaserPenCW()
{
	delete m_pChart;
}

void CDlgLaserPenCW::DoDataExchange(CDataExchange* pDX)
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
	DDX_Control(pDX, IDC_CHECK_POWER_SHAPE, m_chkPwrShape);
	DDX_Control(pDX, IDC_EDIT_1ST_LENGTH, m_edit1StLength);
	DDX_Control(pDX, IDC_EDIT_1ST_WIDTH, m_edit1StWidth);
	DDX_Control(pDX, IDC_EDIT_1ST_RP, m_edit1StRP);
	DDX_Control(pDX, IDC_EDIT_1ST_PEN, m_edit1StPen);
	DDX_Control(pDX, IDC_EDIT8, m_edit2ndWidth);
	DDX_Control(pDX, IDC_EDIT9, m_edit2ndRP);
	DDX_Control(pDX, IDC_EDIT10, m_edit2ndLength);
	DDX_Control(pDX, IDC_EDIT11, m_edit2ndPen);
	DDX_Control(pDX, IDC_EDIT_1ST_ENERGY, m_edit1StEnergy);
	DDX_Control(pDX, IDC_EDIT_2ND_ENERGY, m_edit2ndEnergy);
	DDX_Control(pDX, IDC_EDIT_TOTAL_ENERGY, m_editTotalEnergy);
	DDX_Control(pDX, IDC_EDIT_CONFIG_ENERGY, m_editConfigEnergy);
}

BEGIN_MESSAGE_MAP(CDlgLaserPenCW, CDialogEx)

	ON_BN_CLICKED(IDOK, &CDlgLaserPenCW::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LASERPENS, &CDlgLaserPenCW::OnCbnSelchangeComboboxexLaserpens)
	ON_BN_CLICKED(IDC_MFCBUTTON_LASERPEN_APPLY, &CDlgLaserPenCW::OnBnClickedMfcbuttonLaserpenApply)
	ON_BN_CLICKED(IDC_CHECK_WOBBEL, &CDlgLaserPenCW::OnBnClickedCheckWobbel)
	ON_REGISTERED_MESSAGE(UWM_CGRIDPOWERSHAPE, &CDlgLaserPenCW::OnUwmCgridPowerShape)
	ON_BN_CLICKED(IDC_MFCBUTTON_ADD_ROW, &CDlgLaserPenCW::OnBnClickedMfcbuttonAddRow)
	ON_BN_CLICKED(IDC_MFCBUTTON_DEL_ROW, &CDlgLaserPenCW::OnBnClickedMfcbuttonDelRow)
	ON_BN_CLICKED(IDC_CHECK_POWER_SHAPE, &CDlgLaserPenCW::OnBnClickedCheckPowerShape)
	ON_BN_CLICKED(IDC_BUTTON_COPY, &CDlgLaserPenCW::OnBnClickedButtonCopy)
	ON_BN_CLICKED(IDC_BUTTON_PASTE, &CDlgLaserPenCW::OnBnClickedButtonPaste)
	ON_EN_KILLFOCUS(IDC_EDIT_WELDSPEED, &CDlgLaserPenCW::OnEnKillfocusEditWeldspeed)
	ON_EN_KILLFOCUS(IDC_EDIT_JUMPSPEED, &CDlgLaserPenCW::OnEnKillfocusEditJumpspeed)
	ON_EN_KILLFOCUS(IDC_EDIT_LASERPOWER, &CDlgLaserPenCW::OnEnKillfocusEditLaserpower)
END_MESSAGE_MAP()


// CDlgLaserPen 메시지 처리기


void CDlgLaserPenCW::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	
	if (pDoc->m_config.m_dLimitEnergy < m_editTotalEnergy.GetValue())
	{
		CString strTmp;
		strTmp.Format(_T("The limit energy value has been exceeded.Limit Value = [%.2f]."), pDoc->m_config.m_dLimitEnergy);
		AfxMessageBox(strTmp, MB_ICONERROR);
		_tracer(TRACE_ERROR, 1, strTmp);

		return;
	}	

	CDialogEx::OnOK();
}


BOOL CDlgLaserPenCW::OnInitDialog()
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
	m_cbLaserPens.AddStringC(str, g_clrPen[nPens - 2]);
	//str = L"VScan Pen";
	////m_cbLaserPens.AddString(str);
	//m_cbLaserPens.AddStringC(str, g_clrPen[nPens - 1]);
	CRect rect;
	GetDlgItem(IDC_STATIC_PWCHART)->GetClientRect(rect);
	GetDlgItem(IDC_STATIC_PWCHART)->ClientToScreen(rect);
	ScreenToClient(rect);
	m_pChart = new CChartPowerShape;
	m_pChart->Create(_T("Histo chart"), WS_CHILD | SS_BITMAP | WS_VISIBLE | WS_EX_CLIENTEDGE, rect, this, IDC_STATIC_PWCHART/* IDC_STC_ROI_IMG*/);	
		
	m_pChart->ShowWindow(SW_SHOW);
	m_pChart->DrawChart();
	m_gridShape.AttachGrid(this, IDC_STATIC_PWGRID);

	m_gridShape.SetEventReciveHandle(this->GetSafeHwnd());
	GetDlgItem(IDC_MFCBUTTON_DEL_ROW)->EnableWindow(FALSE);
	GetDlgItem(IDC_MFCBUTTON_ADD_ROW)->EnableWindow(FALSE);

	//22.05.03 에너지 전달률 정보
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	m_edit1StLength.SetValue(pDoc->m_work.m_hp.HpSize);
	m_edit1StWidth.SetValue(pDoc->m_work.m_hp.HpSizeW);
	m_edit1StRP.SetValue(pDoc->m_work.m_hp.nCntP1);
	m_edit1StPen.SetValue(pDoc->m_work.m_hp.nPenP1);

	m_edit2ndWidth.SetValue(pDoc->m_work.m_hp.HpSize2nd);
	m_edit2ndRP.SetValue(pDoc->m_work.m_hp.nCntP2);
	m_edit2ndLength.SetValue(pDoc->m_work.m_hp.HpOverLen);
	m_edit2ndPen.SetValue(pDoc->m_work.m_hp.nPenP2);

	if (pDoc->m_work.m_JobGroup.size() > 0)
	{
		// sjyi 2022-07-06 헤어핀 사용이 아닐 경우, 오류 발생
		// 헤어핀 사용일 경우만 2차 패턴에 대한 에너지 계산.
		// 헤어핀 모드가 아닐 경우에는 1차 패턴 에너지 계산.
		if (pDoc->m_config.m_bUseHairpin)
		{
			m_edit1StEnergy.SetValue(pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW, pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed), L"%.2f");
			m_edit2ndEnergy.SetValue(pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, pDoc->m_work.m_hp.nWeldShapeP2, pDoc->m_work.m_hp.HpSize2nd, pDoc->m_work.m_hp.HpOverLen, pDoc->m_work.m_hp.nWeldZigZagFreqP2, pDoc->m_work.m_hp.nCntP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed), L"%.2f");
			m_editTotalEnergy.SetValue(m_edit1StEnergy.GetValue() + m_edit2ndEnergy.GetValue(), L"%.2f");
		}
		else
		{
			m_edit1StEnergy.SetValue(pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW, pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed), L"%.2f");
		}
	}	
	else
	{
		m_edit1StEnergy.SetValue(0.0, L"%.2f");
		m_edit2ndEnergy.SetValue(0.0, L"%.2f");
		m_editTotalEnergy.SetValue(0.0, L"%.2f");
	}

	m_editConfigEnergy.SetValue(pDoc->m_config.m_dLimitEnergy, L"%.2f");

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CDlgLaserPenCW::OnCbnSelchangeComboboxexLaserpens()
{
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;
	CLaserPen *pPen = &(*m_pPens)[iSel];
	m_editPenID.SetValue(pPen->m_iPenID);
	m_editLaserPower.SetValue(pPen->m_dblPower, L"%.1f");
	m_editFrequency.SetValue(pPen->m_dblFrequency, L"%.1f");
	m_editPulseWidth.SetValue(pPen->m_dblPulseWidth, L"%.3f");
	m_editJumpSpeed.SetValue(pPen->m_dblJumpSpeed, L"%.1f");;
	m_editWeldSpeed.SetValue(pPen->m_dblMarkSpeed, L"%.1f");;
	//m_editJumpDelay.SetValue(pPen->m_nJumpDelay);					//20251202 멕시코 LG마그나 요청으로 주석처리
	m_editJumpDelay.SetValue(pDoc->m_config.m_nJumpDelay);			//20251202 멕시코 LG마그나 요청 1로 고정 pDoc->m_config.m_nJumpDelay;
	m_editLaserOffDelay.SetValue(pPen->m_nLaserOffDelay);
	m_editLaserOnDelay.SetValue(pPen->m_nLaserOnDelay);
	m_editWeldDelay.SetValue(pPen->m_nMarkDelay);
	m_editPolygonDelay.SetValue(pPen->m_nPolygonDelay);
	m_chkUseWobbel.SetCheck(pPen->m_bUseWobbel);
	m_editWbFreq.SetValue(pPen->m_dWbFreq);
	m_editWbLongitude.SetValue(pPen->m_dWbLongitudinal);
	m_editWbTranseverse.SetValue(pPen->m_dWbTransbersal);
	if (pPen->m_bUsePowerShape) {
		m_pwrShape.resize(pPen->m_pwrShape.size());
		std::copy(pPen->m_pwrShape.begin(), pPen->m_pwrShape.end(), m_pwrShape.begin());
	}
	else
		m_pwrShape.clear();
	m_chkPwrShape.SetCheck(pPen->m_bUsePowerShape);

	UpdateChart(pPen);
	UpdateGrid(pPen);

	OnBnClickedCheckWobbel();
	GetDlgItem(IDC_MFCBUTTON_DEL_ROW)->EnableWindow(pPen->m_bUsePowerShape);
	GetDlgItem(IDC_MFCBUTTON_ADD_ROW)->EnableWindow(pPen->m_bUsePowerShape);


	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	double d1StEnergy = 0.0f, d2ndEnergy = 0.0f;

	// sjyi 2022-07-06 헤어핀 사용이 아닐 경우, 오류 발생
	// 헤어핀 사용일 경우만 2차 패턴에 대한 에너지 계산.
	// 헤어핀 모드가 아닐 경우에는 1차 패턴 에너지 계산.
	if (pDoc->m_config.m_bUseHairpin)
	{
		d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
			pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);

		d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, pDoc->m_work.m_hp.nWeldShapeP2, pDoc->m_work.m_hp.HpSize2nd,
			pDoc->m_work.m_hp.HpSize2nd, pDoc->m_work.m_hp.nWeldZigZagFreqP2, pDoc->m_work.m_hp.nCntP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);

	}
	else
	{
		d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
			pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);

		d2ndEnergy = 0.0;

	}

	m_edit1StEnergy.SetValue(d1StEnergy, L"%.2f");
	m_edit2ndEnergy.SetValue(d2ndEnergy, L"%.2f");
	m_editTotalEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");
}

void CDlgLaserPenCW::UpdateChart(CLaserPen *pPen)
{
	BOOL bUsePwrShape = m_chkPwrShape.GetCheck();

	if (m_pChart->m_px)	delete[] m_pChart->m_px; 
	if (m_pChart->m_pdata) delete[] m_pChart->m_pdata;

	m_pChart->m_px = NULL;
	m_pChart->m_pdata = NULL;
	m_pChart->m_nData = 0;
	if (!bUsePwrShape) {
		m_pChart->DrawChart();
		return;
	}

	size_t nSize = m_pwrShape.size();

	m_pChart->m_px = new double[nSize];
	m_pChart->m_pdata = new double[nSize];


	for (size_t i = 0; i < nSize; i++) {
		m_pChart->m_px[i] = m_pwrShape[i].x;
		m_pChart->m_pdata[i] = m_pwrShape[i].y;
	}
	m_pChart->m_nData =(int)nSize;

	m_pChart->DrawChart();
}


void CDlgLaserPenCW::UpdateGrid(CLaserPen *pPen)
{
	BOOL bUsePwrShape = m_chkPwrShape.GetCheck();
	if (!bUsePwrShape) {
		m_gridShape.SetNumberRows(0);
		return;
	}
	size_t nSize = m_pwrShape.size();
	m_gridShape.SetNumberRows((int)nSize);
	for (size_t i = 0; i < nSize; i++) {
		m_gridShape.QuickSetNumber(-1, (long)i, (double)i);
		m_gridShape.SetDouble(0, (long)i, m_pwrShape[i].x);
		m_gridShape.SetDouble(1, (long)i, m_pwrShape[i].y);
	}
	m_gridShape.RedrawAll();


}

void CDlgLaserPenCW::OnBnClickedMfcbuttonLaserpenApply()
{	
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	USES_CONVERSION;

	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;
	CLaserPen *pPen = &(*m_pPens)[iSel];
	pPen->m_iPenID = m_editPenID.GetValue();
	pPen->m_dblPower = m_editLaserPower.GetValue();
	pPen->m_dblFrequency = m_editFrequency.GetValue();
	pPen->m_dblPulseWidth = m_editPulseWidth.GetValue();
	pPen->m_dblJumpSpeed = m_editJumpSpeed.GetValue();
	if (pPen->m_dblJumpSpeed < pDoc->m_config.m_dJumpSpdmin[iSel] || pPen->m_dblJumpSpeed > pDoc->m_config.m_dJumpSpdmax[iSel])
	{
		AfxMessageBox(L"Jump Speed ​​can only be input within the range of 2000 ~ 3000 mm/s.", MB_ICONERROR);
		GetDlgItem(IDC_EDIT_JUMPSPEED)->SetFocus();
		return;
	}

	pPen->m_dblMarkSpeed = m_editWeldSpeed.GetValue();
	if (pPen->m_dblMarkSpeed < pDoc->m_config.m_dWeldSpdmin[iSel] || pPen->m_dblMarkSpeed > pDoc->m_config.m_dWeldSpdmax[iSel])
	{
		AfxMessageBox(L"Weld Speed ​​can only be input within the range of 200 ~ 300 mm/s.", MB_ICONERROR);
		GetDlgItem(IDC_EDIT_WELDSPEED)->SetFocus();
		return;
	}

	pPen->m_nJumpDelay = m_editJumpDelay.GetValue();
	pPen->m_nLaserOffDelay = m_editLaserOffDelay.GetValue();
	pPen->m_nLaserOnDelay = m_editLaserOnDelay.GetValue();
	pPen->m_nMarkDelay = m_editWeldDelay.GetValue();
	pPen->m_nPolygonDelay = m_editPolygonDelay.GetValue();
	pPen->m_bUseWobbel = m_chkUseWobbel.GetCheck();
	pPen->m_dWbFreq = m_editWbFreq.GetValue();
	pPen->m_dWbLongitudinal = m_editWbLongitude.GetValue();
	pPen->m_dWbTransbersal = m_editWbTranseverse.GetValue();
	pPen->m_bUsePowerShape = m_chkPwrShape.GetCheck();		

	if (pPen->m_bUsePowerShape) {
		pPen->m_pwrShape.clear();
		pPen->m_pwrShape.resize(m_pwrShape.size());
		std::copy(m_pwrShape.begin(), m_pwrShape.end(), pPen->m_pwrShape.begin());
	}
	CString str;
	str.Format(L"Pen%d was saved", iSel);
	AfxMessageBox(str);
}


void CDlgLaserPenCW::OnBnClickedCheckWobbel()
{
	BOOL bEnable = m_chkUseWobbel.GetCheck();
	m_editWbFreq.EnableWindow(bEnable);
	m_editWbLongitude.EnableWindow(bEnable);
	m_editWbTranseverse.EnableWindow(bEnable);
}


afx_msg LRESULT CDlgLaserPenCW::OnUwmCgridPowerShape(WPARAM wParam, LPARAM lParam)
{
	int irow = (int)lParam;
	TRACE("OnUwmCgridPowerShape\n");
	m_pwrShape[irow].x = m_gridShape.GetDouble(0, irow);
	m_pwrShape[irow].y = m_gridShape.GetDouble(1, irow);
	UpdateChart(NULL);
	UpdateGrid(NULL);
	return 0;
}


void CDlgLaserPenCW::OnBnClickedMfcbuttonAddRow()
{
	int iRow = m_gridShape.GetCurrentRow();
	if (iRow >= m_pwrShape.size())
		m_pwrShape.push_back(m_pwrShape[iRow - 1]);
	else
		m_pwrShape.insert(m_pwrShape.begin() + iRow +1 , m_pwrShape[iRow]);
	UpdateChart(NULL);
	UpdateGrid(NULL);
}


void CDlgLaserPenCW::OnBnClickedMfcbuttonDelRow()
{
	int iRow = m_gridShape.GetCurrentRow();
	m_pwrShape.erase(m_pwrShape.begin() + iRow);
	UpdateChart(NULL);
	UpdateGrid(NULL);
}


void CDlgLaserPenCW::OnBnClickedCheckPowerShape()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	BOOL bCheck = m_chkPwrShape.GetCheck();
	if (bCheck) {
		if (!m_pwrShape.size()) {
			POINTDOUBLE var;
			var.x = 0;
			var.y = m_editLaserPower.GetValue();
			m_pwrShape.push_back(var);
			var.x = 100;
			var.y = m_editLaserPower.GetValue();
			m_pwrShape.push_back(var);
		}
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


void CDlgLaserPenCW::OnBnClickedButtonCopy()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_dblCopyPower = m_editLaserPower.GetValue();
	m_dblCopyFrequency = m_editFrequency.GetValue();
	m_dblCopyPulseWidth = m_editPulseWidth.GetValue();
	m_dblCopyJumpSpeed = m_editJumpSpeed.GetValue();
	m_dblCopyMarkSpeed = m_editWeldSpeed.GetValue();
	m_nCopyJumpDelay = m_editJumpDelay.GetValue();
	m_nCopyLaserOffDelay = m_editLaserOffDelay.GetValue();
	m_nCopyLaserOnDelay = m_editLaserOnDelay.GetValue();
	m_nCopyMarkDelay = m_editWeldDelay.GetValue();
	m_nCopyPolygonDelay = m_editPolygonDelay.GetValue();
}


void CDlgLaserPenCW::OnBnClickedButtonPaste()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_editLaserPower.SetValue(m_dblCopyPower, L"%.1f");
	m_editFrequency.SetValue(m_dblCopyFrequency, L"%.1f");
	m_editPulseWidth.SetValue(m_dblCopyPulseWidth, L"%.3f");
	m_editJumpSpeed.SetValue(m_dblCopyJumpSpeed, L"%.1f");;
	m_editWeldSpeed.SetValue(m_dblCopyMarkSpeed, L"%.1f");;
	m_editJumpDelay.SetValue(m_nCopyJumpDelay);
	m_editLaserOffDelay.SetValue(m_nCopyLaserOffDelay);
	m_editLaserOnDelay.SetValue(m_nCopyLaserOnDelay);
	m_editWeldDelay.SetValue(m_nCopyMarkDelay);
	m_editPolygonDelay.SetValue(m_nCopyPolygonDelay);
}


void CDlgLaserPenCW::OnEnKillfocusEditWeldspeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;
	
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CString strTmp;
	double dTmp;
	
	GetDlgItemText(IDC_EDIT_WELDSPEED, strTmp);
	
	USES_CONVERSION;
	dTmp = atof(W2A(strTmp));

	if (dTmp < pDoc->m_config.m_dWeldSpdmin[iSel] || dTmp > pDoc->m_config.m_dWeldSpdmax[iSel])
	{
		AfxMessageBox(L"Weld Speed ​​can only be input within the range of 200 ~ 300 mm/s.", MB_ICONERROR);				
		GetDlgItem(IDC_EDIT_WELDSPEED)->SetFocus();
		SetDlgItemText(IDC_EDIT_WELDSPEED, L"");
		return;
	}

	//1차 패턴에 설정된 Pen 일 경우만 에너지 전달률 계산
	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	int nSel = m_cbLaserPens.GetCurSel();
	
	double d1StEnergy = 0.0f, d2ndEnergy = 0.0f;

	if (pDoc->m_work.m_hp.nPenP1 == nSel)
	{		
		d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
			pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, m_editWeldSpeed.GetValue());		

		d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, pDoc->m_work.m_hp.nWeldShapeP2, pDoc->m_work.m_hp.HpSize2nd,
			pDoc->m_work.m_hp.HpSize2nd, pDoc->m_work.m_hp.nWeldZigZagFreqP2, pDoc->m_work.m_hp.nCntP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
	}
	//2차 패턴에 설정된 Pen 일 경우만 에너지 전달률 계산
	else if (pDoc->m_work.m_hp.nPenP2 == nSel)
	{
		d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
			pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen2->m_dblPower, m_editWeldSpeed.GetValue());		

		d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
			pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	}
	m_edit1StEnergy.SetValue(d1StEnergy, L"%.2f");
	m_edit2ndEnergy.SetValue(d2ndEnergy, L"%.2f");
	m_editTotalEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");	
}


void CDlgLaserPenCW::OnEnKillfocusEditJumpspeed()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;

	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();

	CString strTmp;
	double dTmp;

	GetDlgItemText(IDC_EDIT_JUMPSPEED, strTmp);

	USES_CONVERSION;
	dTmp = atof(W2A(strTmp));

	if (dTmp < pDoc->m_config.m_dJumpSpdmin[iSel] || dTmp > pDoc->m_config.m_dJumpSpdmax[iSel])
	{
		AfxMessageBox(L"Jump Speed ​​can only be input within the range of 2000 ~ 3000 mm/s.", MB_ICONERROR);
		GetDlgItem(IDC_EDIT_JUMPSPEED)->SetFocus();
		SetDlgItemText(IDC_EDIT_JUMPSPEED, L"");
		return;
	}	
}


void CDlgLaserPenCW::OnEnKillfocusEditLaserpower()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
		
	CLaserPen *pPen1 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP1];
	CLaserPen *pPen2 = &(pDoc->m_pens)[pDoc->m_work.m_hp.nPenP2];

	int nSel = m_cbLaserPens.GetCurSel();

	//1차 패턴에 설정된 Pen 일 경우만 에너지 전달률 계산
	double d1StEnergy = 0.0f, d2ndEnergy = 0.0f;
	if (pDoc->m_work.m_hp.nPenP1 == nSel)
	{
		d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
												pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, m_editLaserPower.GetValue(), pPen1->m_dblMarkSpeed);
		
		d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, pDoc->m_work.m_hp.nWeldShapeP2, pDoc->m_work.m_hp.HpSize2nd,
			pDoc->m_work.m_hp.HpSize2nd, pDoc->m_work.m_hp.nWeldZigZagFreqP2, pDoc->m_work.m_hp.nCntP2, pPen2->m_dblPower, pPen2->m_dblMarkSpeed);
	}
	//2차 패턴에 설정된 Pen 일 경우만 에너지 전달률 계산
	else if (pDoc->m_work.m_hp.nPenP2 == nSel)
	{		
		d2ndEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_2ND, pDoc->m_work.m_hp.nWeldShapeP2, pDoc->m_work.m_hp.HpSize2nd,
												pDoc->m_work.m_hp.HpSize2nd, pDoc->m_work.m_hp.nWeldZigZagFreqP2, pDoc->m_work.m_hp.nCntP2, m_editLaserPower.GetValue(), pPen2->m_dblMarkSpeed);
		
		d1StEnergy = pDoc->m_work.GetWeldingEnergy(PATTERN_TYPE_1ST, pDoc->m_work.m_hp.nWeldShapeP1, pDoc->m_work.m_hp.HpSizeW,
			pDoc->m_work.m_hp.HpSize, pDoc->m_work.m_hp.nWeldZigZagFreqP1, pDoc->m_work.m_hp.nCntP1, pPen1->m_dblPower, pPen1->m_dblMarkSpeed);
	}

	m_edit1StEnergy.SetValue(d1StEnergy, L"%.2f");
	m_edit2ndEnergy.SetValue(d2ndEnergy, L"%.2f");
	m_editTotalEnergy.SetValue(d1StEnergy + d2ndEnergy, L"%.2f");	
}
