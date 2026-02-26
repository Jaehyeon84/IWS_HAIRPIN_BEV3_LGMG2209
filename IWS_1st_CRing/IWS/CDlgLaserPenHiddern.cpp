// CDlgLaserPenHiddern.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgLaserPenHiddern.h"
#include "afxdialogex.h"


// CDlgLaserPenHiddern 대화 상자

IMPLEMENT_DYNAMIC(CDlgLaserPenHiddern, CDialogEx)

CDlgLaserPenHiddern::CDlgLaserPenHiddern(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_LASERPEN_HIDDERN, pParent)
{

}

CDlgLaserPenHiddern::~CDlgLaserPenHiddern()
{
}

void CDlgLaserPenHiddern::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBOBOXEX_LASERPENS, m_cbLaserPens);
	DDX_Control(pDX, IDC_EDIT_PENPOWER, m_editLaserPower);
	DDX_Control(pDX, IDC_EDIT_PENSPEED, m_editLaserSpeed);
}


BEGIN_MESSAGE_MAP(CDlgLaserPenHiddern, CDialogEx)
	ON_CBN_SELCHANGE(IDC_COMBOBOXEX_LASERPENS, &CDlgLaserPenHiddern::OnCbnSelchangeComboboxexLaserpens)
	ON_BN_CLICKED(IDC_MFCBUTTON_LASERPEN_APPLY, &CDlgLaserPenHiddern::OnBnClickedMfcbuttonLaserpenApply)
	ON_BN_CLICKED(IDOK, &CDlgLaserPenHiddern::OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CDlgLaserPenHiddern::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	CString str;
	for (int i = 0; i < 10; i++)
	{
		str.Format(L"Laser Pen %d", i);
		m_cbLaserPens.AddStringC(str, g_clrPen[i]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgLaserPenHiddern::OnCbnSelchangeComboboxexLaserpens()
{
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0) 
		return;	
	
	m_editLaserPower.SetValue(m_dPenPower[iSel], L"%.1f");
	m_editLaserSpeed.SetValue(m_dPenSpeed[iSel], L"%.1f");
}

// CDlgLaserPenHiddern 메시지 처리기
void CDlgLaserPenHiddern::OnBnClickedMfcbuttonLaserpenApply()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSel = m_cbLaserPens.GetCurSel();
	if (iSel < 0)
		return;

	m_dPenPower[iSel] = m_editLaserPower.GetValue();
	m_dPenSpeed[iSel] = m_editLaserSpeed.GetValue();
}


void CDlgLaserPenHiddern::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}
