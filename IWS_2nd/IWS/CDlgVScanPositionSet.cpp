// CDlgRegImgSet.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgVScanPositionSet.h"
#include "afxdialogex.h"
#include "VRSaux/GrabImgRunner.h"
//#include"VRSaux/GrabImgDlg.h"
#include "CDlgVScanCalib.h"
#include "MainFrm.h"
#include "IWSDoc.h"
// CDlgVScanPositionSet 대화 상자


IMPLEMENT_DYNAMIC(CDlgVScanPositionSet, CDialogEx)

CDlgVScanPositionSet::CDlgVScanPositionSet(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VSCANIMG_POS, pParent)
{
	
}

CDlgVScanPositionSet::~CDlgVScanPositionSet()
{
}

void CDlgVScanPositionSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL1_X, m_editModel1X);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL1_Y, m_editModel1Y);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL2_X, m_editModel2X);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL2_Y, m_editModel2Y);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL3_X, m_editModel3X);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL3_Y, m_editModel3Y);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL4_X, m_editModel4X);
	DDX_Control(pDX, IDC_EDIT_CEN_MODEL4_Y, m_editModel4Y);

	DDX_Control(pDX, IDC_EDIT_VSCAN_SX, m_editSx);
	DDX_Control(pDX, IDC_EDIT_VSCAN_SY, m_editSy);
	DDX_Control(pDX, IDC_EDIT_VSCAN_EX, m_editEx);
	DDX_Control(pDX, IDC_EDIT_VSCAN_EY, m_editEy);

	DDX_Control(pDX, IDC_EDIT_IMGCEN_X, m_editImgCenX);
	DDX_Control(pDX, IDC_EDIT_IMGCEN_Y, m_editImgCenY);


	DDX_Control(pDX, IDC_EDIT_IMG_X, m_EditImgX);
	DDX_Control(pDX, IDC_EDIT_IMG_Y, m_EditImgY);
	DDX_Control(pDX, IDC_EDIT_LIGHT_POWER, m_editLightPower);
}


BEGIN_MESSAGE_MAP(CDlgVScanPositionSet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgVScanPositionSet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_LIGHT_POWER, &CDlgVScanPositionSet::OnBnClickedBtnLightPower)
	ON_BN_CLICKED(IDC_BUTTON_SCAN_CALIBRATION, &CDlgVScanPositionSet::OnBnClickedButtonScanCalibration)
END_MESSAGE_MAP()


// CDlgRegImgSet 메시지 처리기


BOOL CDlgVScanPositionSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_editSx.SetValue(m_sx, L"%.0f");
	m_editSy.SetValue(m_sy, L"%.0f");
	m_editEx.SetValue(m_ex, L"%.0f");
	m_editEy.SetValue(m_ey, L"%.0f");

	m_editImgCenX.SetValue(m_dblCenImgX, L"%.2f");
	m_editImgCenY.SetValue(m_dblCenImgY, L"%.2f");

	m_editModel1X.SetValue(m_dModel1_X, L"%.2f");
	m_editModel1Y.SetValue(m_dModel1_Y, L"%.2f");
	m_editModel2X.SetValue(m_dModel2_X, L"%.2f");
	m_editModel2Y.SetValue(m_dModel2_Y, L"%.2f");
	m_editModel3X.SetValue(m_dModel3_X, L"%.2f");
	m_editModel3Y.SetValue(m_dModel3_Y, L"%.2f");
	m_editModel4X.SetValue(m_dModel4_X, L"%.2f");
	m_editModel4Y.SetValue(m_dModel4_Y, L"%.2f");

	float fovX, fovY;
	g_pGrabImgRunner->GetCurFullFov(&fovX, &fovY);

	CString str;
	str.Format(L"%.3f", fovX);
	GetDlgItem(IDC_EDIT_IMG_X)->SetWindowText(str);

	str.Format(L"%.3f", fovY);
	GetDlgItem(IDC_EDIT_IMG_Y)->SetWindowText(str);

	//m_editLightPower.SetValue(m_nLightPower);

	/*str.Format(L"%.3f", m_pCameraInfo->c);
	GetDlgItem(IDC_EDIT_IMG_CX)->SetWindowText(str);

	str.Format(L"%.3f", m_pCameraInfo->fLocalFov_X);
	GetDlgItem(IDC_EDIT_IMG_CY2)->SetWindowText(str);*/
	m_editLightPower.ShowWindow(SW_HIDE);

	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc * pDoc = (CIWSDoc *)pMain->GetActiveDocument();

	CorrParam *param = pDoc->m_config.pGetCorrParam(pDoc->m_config.m_iCurrLens);
	float fsrHx = param->dblWorkX*0.5f;
	float fsrHy = param->dblWorkY*0.5f;
	str.Format(_T("/%.2fmm"), -fsrHx);
	GetDlgItem(IDC_STATIC_X_MIN)->SetWindowText(str);
	str.Format(_T("/%.2fmm"), fsrHx);
	GetDlgItem(IDC_STATIC_X_MAX)->SetWindowText(str);
	str.Format(_T("/%.2fmm"), -fsrHy);
	GetDlgItem(IDC_STATIC_Y_MIN)->SetWindowText(str);
	str.Format(_T("/%.2fmm"), fsrHy);
	GetDlgItem(IDC_STATIC_Y_MAX)->SetWindowText(str);



	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgVScanPositionSet::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_sx = m_editSx.GetValue();
	m_sy = m_editSy.GetValue();
	m_ex = m_editEx.GetValue();
	m_ey = m_editEy.GetValue();

	if (m_sx >= m_ex || m_sy >= m_ey)
	{
		AfxMessageBox(_T("Need to check scan-range"));
		return;
	}
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc * pDoc = (CIWSDoc *)pMain->GetActiveDocument();
	CorrParam *param = pDoc->m_config.pGetCorrParam(pDoc->m_config.m_iCurrLens);
	float fsrHx = param->dblWorkX*0.5f;
	float fsrHy = param->dblWorkY*0.5f;

	if(  m_sx <= -fsrHx || m_sx >= fsrHx
	  || m_ex <= -fsrHx || m_ex >= fsrHx
	  || m_sy <= -fsrHy || m_sy >= fsrHy
	  || m_ey <= -fsrHy || m_ey >= fsrHy)
	{
		AfxMessageBox(_T("Need to check scan-range"));
		return;
	}




	m_dblCenImgX = m_editImgCenX.GetValue();
	m_dblCenImgY = m_editImgCenY.GetValue();

	m_dModel1_X = m_editModel1X.GetValue();
	m_dModel1_Y = m_editModel1Y.GetValue();
	m_dModel2_X = m_editModel2X.GetValue();
	m_dModel2_Y = m_editModel2Y.GetValue();
	m_dModel3_X = m_editModel3X.GetValue();
	m_dModel3_Y = m_editModel3Y.GetValue();
	m_dModel4_X = m_editModel4X.GetValue();
	m_dModel4_Y = m_editModel4Y.GetValue();

	//m_nLightPower = m_editLightPower.GetValue();
	CDialogEx::OnOK();
}


void CDlgVScanPositionSet::OnBnClickedBtnLightPower()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_pCtrl->SetLight(m_editLightPower.GetValue());
}

//DHChoi 20200629
void CDlgVScanPositionSet::OnBnClickedButtonScanCalibration()
{
	CMainFrame* pMain = (CMainFrame*)AfxGetMainWnd();
	CIWSDoc * pDoc = (CIWSDoc *)pMain->GetActiveDocument();
	CDlgVScanCalib pDlg(pDoc,this);
	if (pDlg.DoModal() != IDOK) return;

	CorrParam *param = pDoc->m_config.pGetCorrParam(pDoc->m_config.m_iCurrLens);
	float fsrHx = param->dblWorkX*0.5f;
	float fsrHy = param->dblWorkY*0.5f;
	CString str;
	str.Format(_T("/%.2fmm"), -fsrHx);
	GetDlgItem(IDC_STATIC_X_MIN)->SetWindowText(str);
	str.Format(_T("/%.2fmm"), fsrHx);
	GetDlgItem(IDC_STATIC_X_MAX)->SetWindowText(str);
	str.Format(_T("/%.2fmm"), -fsrHy);
	GetDlgItem(IDC_STATIC_Y_MIN)->SetWindowText(str);
	str.Format(_T("/%.2fmm"), fsrHy);
	GetDlgItem(IDC_STATIC_Y_MAX)->SetWindowText(str);

}
