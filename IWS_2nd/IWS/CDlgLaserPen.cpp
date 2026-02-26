// CDlgLaserPen.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgLaserPen.h"
#include "afxdialogex.h"


// CDlgLaserPen 대화 상자

IMPLEMENT_DYNAMIC(CDlgLaserPen, CDialogEx)

CDlgLaserPen::CDlgLaserPen(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LASERPEN, pParent)
{

}

CDlgLaserPen::~CDlgLaserPen()
{
}

void CDlgLaserPen::DoDataExchange(CDataExchange* pDX)
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
}


BEGIN_MESSAGE_MAP(CDlgLaserPen, CDialogEx)

	ON_BN_CLICKED(IDOK, &CDlgLaserPen::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LASERPENS, &CDlgLaserPen::OnCbnSelchangeComboboxexLaserpens)
	ON_BN_CLICKED(IDC_MFCBUTTON_LASERPEN_APPLY, &CDlgLaserPen::OnBnClickedMfcbuttonLaserpenApply)
	ON_BN_CLICKED(IDC_CHECK_WOBBEL, &CDlgLaserPen::OnBnClickedCheckWobbel)
END_MESSAGE_MAP()


// CDlgLaserPen 메시지 처리기


void CDlgLaserPen::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}


BOOL CDlgLaserPen::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	int nPens = (int)m_pPens->size();
	for (int i = 0; i < nPens - 2; i++)
	{
		str.Format(L"Laser Pen %d", (*m_pPens)[i].m_iPenID);
		m_cbLaserPens.AddString(str);
	}

	//str = L"1st ReWeld Pen";
	//m_cbLaserPens.AddString(str);

	//str = L"2nd ReWeld Pen";
	//m_cbLaserPens.AddString(str);

	str = L"Simulation Pen";
	m_cbLaserPens.AddString(str);
	/*str = L"VScan Pen";
	m_cbLaserPens.AddString(str);*/

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgLaserPen::OnCbnSelchangeComboboxexLaserpens()
{
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) return;
	CLaserPen *pPen = &(*m_pPens)[iSel];
	m_editPenID.SetValue(pPen->m_iPenID);
	m_editLaserPower.SetValue(pPen->m_dblPower, L"%.1f");
	m_editFrequency.SetValue(pPen->m_dblFrequency, L"%.1f");
	m_editPulseWidth.SetValue(pPen->m_dblPulseWidth, L"%.3f");
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
	OnBnClickedCheckWobbel();
}


void CDlgLaserPen::OnBnClickedMfcbuttonLaserpenApply()
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
	CString str;
	str.Format(L"Pen%d was saved", iSel);
	AfxMessageBox(str);
}


void CDlgLaserPen::OnBnClickedCheckWobbel()
{
	BOOL bEnable = m_chkUseWobbel.GetCheck();
	m_editWbFreq.EnableWindow(bEnable);
	m_editWbLongitude.EnableWindow(bEnable);
	m_editWbTranseverse.EnableWindow(bEnable);
}
