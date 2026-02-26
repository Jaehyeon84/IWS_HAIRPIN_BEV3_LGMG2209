// DlgCadRoundInput.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "DlgCadRoundInput.h"
#include "afxdialogex.h"


// CDlgCadRoundInput 대화 상자
double CDlgCadRoundInput::m_dRound = 3.0;

IMPLEMENT_DYNAMIC(CDlgCadRoundInput, CBaseDialog)

CDlgCadRoundInput::CDlgCadRoundInput(CWnd* pParent /*=nullptr*/)
	: CBaseDialog(IDD_DIALOG_CAD_ROUND, pParent)
{
}

CDlgCadRoundInput::~CDlgCadRoundInput()
{
}

void CDlgCadRoundInput::DoDataExchange(CDataExchange* pDX)
{
	CBaseDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_ROUND, m_editRound);
}


BEGIN_MESSAGE_MAP(CDlgCadRoundInput, CBaseDialog)
	ON_BN_CLICKED(IDOK, &CDlgCadRoundInput::OnBnClickedOk)
END_MESSAGE_MAP()

// CDlgCadRoundInput 메시지 처리기
BOOL CDlgCadRoundInput::OnInitDialog()
{
	CBaseDialog::OnInitDialog();
	m_editRound.SetValue(m_dRound, L"%.3f");
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgCadRoundInput::OnBnClickedOk()
{
	m_dRound = m_editRound.GetValue();
	OnOK();
}



