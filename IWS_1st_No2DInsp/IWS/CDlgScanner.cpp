// CDlgScanner.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgScanner.h"
#include "afxdialogex.h"
#include "IWSDoc.h"


// CDlgScanner 대화 상자

IMPLEMENT_DYNAMIC(CDlgScanner, CDialogEx)
BOOL CDlgScanner::m_bUseScaleFactor = FALSE;
BOOL CDlgScanner::m_bUseScaleRatio = FALSE;
BOOL CDlgScanner::m_bUseAutoScale = FALSE;

double CDlgScanner::m_dBaseSize = 1.0;
double CDlgScanner::m_dRealSizeX = 1.0;
double CDlgScanner::m_dRealSizeY = 1.0;


CDlgScanner::CDlgScanner(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SCANNER, pParent)
{
	m_iCurrentLens = 0;
}

CDlgScanner::~CDlgScanner()
{
}

void CDlgScanner::SetDocument(CIWSDoc *pDoc)
{
	m_pDoc = pDoc;
}

void CDlgScanner::UpdateControl()
{
	INT_PTR n = m_pDoc->m_config.m_arCorrParam.GetSize();
	CString str;
	m_cbLensDB.ResetContent();
	for (INT_PTR i = 0; i < n; i++) {
		str.Format(L"%s", m_pDoc->m_config.m_arCorrParam[i].szName);
		m_cbLensDB.AddString(str);
	}
	int iCurrent = m_pDoc->m_config.m_iCurrLens;
	m_cbLensDB.SetCurSel(m_iCurrentLens);

	/*m_editImgCX.SetValue(m_pCorrParam->dblImgCenX, L"%.3f");
	m_editImgCY.SetValue(m_pCorrParam->dblImgCenY, L"%.3f");
	m_editImgSizeX.SetValue(m_pCorrParam->dblImgSizeX, L"%.3f");
	m_editImgSizeY.SetValue(m_pCorrParam->dblImgSizeY, L"%.3f");*/
	m_editWorkX.SetValue(m_pCorrParam->dblWorkX, L"%.3f");
	m_editWorkY.SetValue(m_pCorrParam->dblWorkY, L"%.3f");
	m_editCcdCalStep.SetValue(m_pCorrParam->dblWorkX / 4, L"%.1f");
	m_editLensPowerX.SetValue(m_pCorrParam->dblLensPowerX, L"%.3f");
	m_editLensPowerY.SetValue(m_pCorrParam->dblLensPowerY, L"%.3f");
	m_editImageScaleX.SetValue(m_pCorrParam->dblImgScaleX, L"%.3f");
	m_editImageScaleY.SetValue(m_pCorrParam->dblImgScaleY, L"%.3f");

	CString strPath = m_pCorrParam->szCorFile;
	m_editCorrFile.EnableFileBrowseButton(_T("ct5"), _T("Correction file|*.ct5||"));
	CString strCcdFile = m_pCorrParam->szCcdFile;
	m_editCCDCorrFile.SetWindowTextW(strCcdFile);

	m_editCorrFile.SetWindowTextW(strPath);
	m_editScaleFactor.SetMinMax(0.01, 10000, m_pCorrParam->dblScaleFactor);
	m_editScaleFactor.SetValue(m_pCorrParam->dblScaleFactor, _T("%.1f"));
	m_editScaleKX.SetValue(m_pCorrParam->dblKX, _T("%.3f"));
	m_editOffsetX.SetMinMax(-10, 10, 0.0);
	m_editOffsetY.SetMinMax(-10, 10, 0.0);

	m_editScaleKY.SetValue(m_pCorrParam->dblKY, _T("%.3f"));
	//m_editScaleKY.SetMinMax(0.8, 1.2, 1.0);
	m_editRotation.SetValue(m_pCorrParam->dblTheta, _T("%.3f"));
	m_editOffsetX.SetValue(m_pCorrParam->dblOffsetX / m_pCorrParam->dblScaleFactor, _T("%.3f"));
	m_editOffsetY.SetValue(m_pCorrParam->dblOffsetY / m_pCorrParam->dblScaleFactor, _T("%.3f"));
	m_chkUseScaleRatio.SetCheck(m_bUseScaleRatio); OnBnClickedScaleRateChange();
	m_chkUseScalFactor.SetCheck(m_bUseScaleFactor); OnBnClickedScaleFactorChange();
	m_chkUseAutoScale.SetCheck(m_bUseAutoScale); OnBnClickedBaseSizeChange();
	CString strBaseSize;
	strBaseSize.Format(L"0.0");

	strBaseSize = theApp.GetProfileString(STR_SECTION_CORRPARAM, STR_KEY_CORR_BASESIZE, strBaseSize);
	double dval = _tstof(strBaseSize);
	m_editBaseSize.SetValue(dval, _T("%.3f"));
	m_bUpdate = FALSE;
}

void CDlgScanner::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_CORR_FILE, m_editCorrFile);
	DDX_Control(pDX, IDC_SCALE_FACTOR, m_editScaleFactor);
	DDX_Control(pDX, IDC_SCALE_RATE_KX, m_editScaleKX);
	DDX_Control(pDX, IDC_SCALE_RATE_KY, m_editScaleKY);
	DDX_Control(pDX, IDC_ROTATION, m_editRotation);
	DDX_Control(pDX, IDC_OFFSETX, m_editOffsetX);
	DDX_Control(pDX, IDC_OFFSETY, m_editOffsetY);
	DDX_Control(pDX, IDC_SCALE_FACTOR_CHANGE, m_chkUseScalFactor);
	DDX_Control(pDX, IDC_SCALE_RATE_CHANGE, m_chkUseScaleRatio);
	DDX_Control(pDX, IDC_BASE_SIZE, m_editBaseSize);
	DDX_Control(pDX, IDC_REAL_SIZEX, m_editRealSizeX);
	DDX_Control(pDX, IDC_REAL_SIZEY, m_editRealSizeY);
	DDX_Control(pDX, IDC_BASE_SIZE_CHANGE, m_chkUseAutoScale);
	DDX_Control(pDX, IDC_MFCEDITBROWSE_CCD_FILE, m_editCCDCorrFile);
	DDX_Control(pDX, IDC_EDIT_SCAN_X, m_editWorkX);
	DDX_Control(pDX, IDC_EDIT_SCAN_Y, m_editWorkY);
	DDX_Control(pDX, IDC_COMBO_LENSDB, m_cbLensDB);
	DDX_Control(pDX, IDC_EDIT_CCD_CAL_STEP, m_editCcdCalStep);
	DDX_Control(pDX, IDC_EDIT_LENS_POWER_X, m_editLensPowerX);
	DDX_Control(pDX, IDC_EDIT_LENS_POWER_Y, m_editLensPowerY);
	DDX_Control(pDX, IDC_EDIT_IMAGE_SCALE_X, m_editImageScaleX);
	DDX_Control(pDX, IDC_EDIT_IMAGE_SCALE_Y, m_editImageScaleY);
}


BEGIN_MESSAGE_MAP(CDlgScanner, CDialogEx)
	ON_BN_CLICKED(IDC_SCALE_FACTOR_CHANGE, &CDlgScanner::OnBnClickedScaleFactorChange)
	ON_BN_CLICKED(IDC_SCALE_RATE_CHANGE, &CDlgScanner::OnBnClickedScaleRateChange)
	ON_BN_CLICKED(IDC_BASE_SIZE_CHANGE, &CDlgScanner::OnBnClickedBaseSizeChange)
	ON_BN_CLICKED(IDC_AUTO_CHANGE, &CDlgScanner::OnBnClickedAutoChange)
	ON_BN_CLICKED(IDC_MFCBUTTON_SCANNER_APPLY, &CDlgScanner::OnBnClickedMfcbuttonScannerApply)
	ON_BN_CLICKED(IDOK, &CDlgScanner::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO_LENSDB, &CDlgScanner::OnSelchangeComboLensdb)
END_MESSAGE_MAP()


// CDlgScanner 메시지 처리기


BOOL CDlgScanner::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	m_iCurrentLens = m_pDoc->m_config.m_iCurrLens;
	UpdateControl();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgScanner::OnBnClickedScaleFactorChange()
{
	m_bUseScaleFactor = m_chkUseScalFactor.GetCheck();
	m_editScaleFactor.EnableWindow(m_bUseScaleFactor);
}


void CDlgScanner::OnBnClickedScaleRateChange()
{
	m_bUseScaleRatio = m_chkUseScaleRatio.GetCheck();
	m_editScaleKX.EnableWindow(m_bUseScaleRatio);
	m_editScaleKY.EnableWindow(m_bUseScaleRatio);
}


void CDlgScanner::OnBnClickedBaseSizeChange()
{
	m_bUseAutoScale = m_chkUseAutoScale.GetCheck();
	m_editBaseSize.EnableWindow(m_bUseAutoScale);
	if (!m_bUseAutoScale) {
		m_dRealSizeX = m_dBaseSize;
		m_dRealSizeY = m_dBaseSize;
	}
}

void CDlgScanner::OnBnClickedAutoChange()
{
	m_dBaseSize = m_editBaseSize.GetValue();
	m_dRealSizeX = m_editRealSizeX.GetValue();
	m_dRealSizeY = m_editRealSizeY.GetValue();

	if (m_dRealSizeX <= 0 || m_dRealSizeY <= 0) {
		AfxMessageBox(_T("The real size is must be greater than zero."));
		return;
	}
	double rateX, rateY;
	rateX = m_dBaseSize / m_dRealSizeX;
	rateY = m_dBaseSize / m_dRealSizeY;
	CString str;
	int scaleX, scaleY;
	double spareX, spareY;
	double dScaleFactor = m_editScaleFactor.GetValue();
	double dScaleKX = m_editScaleKX.GetValue();
	double dScaleKY = m_editScaleKY.GetValue();
	scaleX = (int)(rateX * dScaleFactor * dScaleKX);
	scaleY = (int)(rateY * dScaleFactor * dScaleKY);
	spareX = (rateX * dScaleFactor * dScaleKX) / scaleX;
//	spareY = (rateY * dScaleFactor * dScaleKY) / scaleX; // sjyi 2022-08-24 scaleY로 나누는 것이 맞음.(추가 확인 필요)
	spareY = (rateY * dScaleFactor * dScaleKY) / scaleY;

	if (spareX < 0.8) {
		str.Format(_T("The rate of X is %.3lf<0.8, Do you want automatically converted to 0.8?"), spareX);
		if (MessageBox(str, _T("Message Box"), MB_YESNO | MB_ICONSTOP) != IDYES) {
			m_dRealSizeX = m_dBaseSize;
			m_dRealSizeY = m_dBaseSize;
			m_editRealSizeX.SetValue(m_dRealSizeX, _T("%0.3f"));
			m_editRealSizeY.SetValue(m_dRealSizeY, _T("%0.3f"));
			return;
		}
		spareX = 0.8;
	}
	if (spareX > 1.2) {
		str.Format(_T("The rate of X is %.3lf>1.2, Do you want automatically converted to 1.2?"), spareX);
		if (MessageBox(str, _T("Message Box"), MB_YESNO | MB_ICONSTOP) != IDYES) {
			m_dRealSizeX = m_dBaseSize;
			m_dRealSizeY = m_dBaseSize;
			m_editRealSizeX.SetValue(m_dRealSizeX, _T("%0.3f"));
			m_editRealSizeY.SetValue(m_dRealSizeY, _T("%0.3f"));
			return;
		}
		spareX = 1.2;
	}
	if (spareY < 0.8) {
		str.Format(_T("The rate of Y is %.3lf<0.8, Do you want automatically converted to 0.8?"), spareY);
		if (MessageBox(str, _T("Message Box"), MB_YESNO | MB_ICONSTOP) != IDYES) {
			m_dRealSizeX = m_dBaseSize;
			m_dRealSizeY = m_dBaseSize;
			m_editRealSizeX.SetValue(m_dRealSizeX, _T("%0.3f"));
			m_editRealSizeY.SetValue(m_dRealSizeY, _T("%0.3f"));
			return;
		}
		spareY = 0.8;
	}
	if (spareY > 1.2) {
		str.Format(_T("The rate of Y is %.3lf>1.2, Do you want automatically converted to 1.2?"), spareY);
		if (MessageBox(str, _T("Message Box"), MB_YESNO | MB_ICONSTOP) != IDYES) {
			m_dRealSizeX = m_dBaseSize;
			m_dRealSizeY = m_dBaseSize;
			m_editRealSizeX.SetValue(m_dRealSizeX, _T("%0.3f"));
			m_editRealSizeY.SetValue(m_dRealSizeY, _T("%0.3f"));
			return;
		}
		spareY = 1.2;
	}
	double dec = 1000.0;
	dScaleFactor = scaleX;
	dScaleKX = (int)(spareX*dec + 0.5) / dec;
	dScaleKY = (int)(spareY*dec + 0.5) / dec;
	m_dRealSizeX = m_dBaseSize;
	m_dRealSizeY = m_dBaseSize;

	m_editScaleFactor.SetValue(dScaleFactor, _T("%0.3f"));
	m_editScaleKX.SetValue(dScaleKX, _T("%0.3f"));
	m_editScaleKY.SetValue(dScaleKY, _T("%0.3f"));
	m_editRealSizeX.SetValue(m_dRealSizeX, _T("%0.3f"));
	m_editRealSizeY.SetValue(m_dRealSizeY, _T("%0.3f"));
}

void CDlgScanner::OnBnClickedMfcbuttonScannerApply()
{
	double dScaleFactor = m_editScaleFactor.GetValue();
	double dScaleKX = m_editScaleKX.GetValue();
	double dScaleKY = m_editScaleKY.GetValue();
	double dTheta = m_editRotation.GetValue();
	double dOffsetX = m_editOffsetX.GetValue();
	double dOffsetY = m_editOffsetY.GetValue();

	m_iCurrentLens = m_cbLensDB.GetCurSel();
	double dWorkX = m_editWorkX.GetValue();
	double dWorkY = m_editWorkY.GetValue();

	double dImageScaleX = m_editImageScaleX.GetValue();
	double dImageScaleY = m_editImageScaleY.GetValue();

	/*double dImgSizeX = m_editImgSizeX.GetValue();
	double dImgSizeY = m_editImgSizeY.GetValue();

	double dImgCX = m_editImgCX.GetValue();
	double dImgCY = m_editImgCY.GetValue();*/

	double dLensPowerX = m_editLensPowerX.GetValue();
	double dLensPowerY = m_editLensPowerY.GetValue();

	CString strPath;
	m_editCorrFile.GetWindowText(strPath);
	CString strCcdFile;
	m_editCCDCorrFile.GetWindowText(strCcdFile);

	BOOL bChange = FALSE;
	CString strTmp = m_pCorrParam->szCorFile;
	if (strTmp != strPath)								bChange = TRUE;
	if (strCcdFile != m_pCorrParam->szCcdFile)			bChange = TRUE;
	if (fabs(m_pCorrParam->dblScaleFactor - dScaleFactor) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblKX - dScaleKX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblKY - dScaleKY) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblOffsetX - dOffsetX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblOffsetY - dOffsetY) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblTheta - dTheta) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblWorkX - dWorkX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblWorkY - dWorkY) > 0.001) bChange = TRUE;
	/*if (fabs(m_pCorrParam->dblImgCenX - dImgCX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblImgCenY - dImgCY) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblImgSizeX - dImgSizeX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblImgSizeY - dImgSizeY) > 0.001) bChange = TRUE;*/
	if (fabs(m_pCorrParam->dblLensPowerX - dLensPowerX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblLensPowerY - dLensPowerY) > 0.001) bChange = TRUE;

	if (fabs(m_pCorrParam->dblImgScaleX - dImageScaleX) > 0.001) bChange = TRUE;
	if (fabs(m_pCorrParam->dblImgScaleY - dImageScaleY) > 0.001) bChange = TRUE;
	if (bChange) {
		if (AfxMessageBox(L"Do you want to apply the change?", MB_YESNO) != IDYES) return;
	}

	if (m_pCorrParam->szCorFile != strPath)
	{
		_tracer(TRACE_NORMAL, 2000, L"Correction File (%d) : %s -> %s", m_iCurrentLens, m_pCorrParam->szCorFile, strPath);
		memset(m_pCorrParam->szCorFile, 0, sizeof(TCHAR)*MAX_PATH);
		wmemcpy_s(m_pCorrParam->szCorFile, MAX_PATH, strPath.GetBuffer(), strPath.GetLength());
	}

	if (m_pCorrParam->szCcdFile != strCcdFile)
	{
		_tracer(TRACE_NORMAL, 2000, L"CCD Correction File (%d) : %s -> %s", m_iCurrentLens, m_pCorrParam->szCcdFile, strCcdFile);
		memset(m_pCorrParam->szCcdFile, 0, sizeof(TCHAR)*MAX_PATH);
		wmemcpy_s(m_pCorrParam->szCcdFile, MAX_PATH, strCcdFile.GetBuffer(), strCcdFile.GetLength());
	}

	// Scanner
	if (m_pCorrParam->dblScaleFactor != dScaleFactor) {
		_tracer(TRACE_NORMAL, 2000, L"Scale Factor (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblScaleFactor, dScaleFactor);
		m_pCorrParam->dblScaleFactor = dScaleFactor;
	}
	if (m_pCorrParam->dblKX != dScaleKX) {
		_tracer(TRACE_NORMAL, 2000, L"Scale KX (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblKX, dScaleKX);
		m_pCorrParam->dblKX = dScaleKX;
	}
	if (m_pCorrParam->dblKY != dScaleKY) {
		_tracer(TRACE_NORMAL, 2000, L"Scale KY (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblKY, dScaleKY);
		m_pCorrParam->dblKY = dScaleKY;
	}
	if ((m_pCorrParam->dblOffsetX / dScaleFactor) != dOffsetX) {
		_tracer(TRACE_NORMAL, 2000, L"Scale KX (%d) : %.3f -> %.3f", m_iCurrentLens, (m_pCorrParam->dblOffsetX / dScaleFactor), dOffsetX);
		m_pCorrParam->dblOffsetX = dOffsetX * dScaleFactor;
	}
	if ((m_pCorrParam->dblOffsetY / dScaleFactor) != dOffsetY) {
		_tracer(TRACE_NORMAL, 2000, L"Scale KY (%d) : %.3f -> %.3f", m_iCurrentLens, (m_pCorrParam->dblOffsetY / dScaleFactor), dOffsetY);
		m_pCorrParam->dblOffsetY = dOffsetY * dScaleFactor;
	}
	if (m_pCorrParam->dblTheta != dTheta) {
		_tracer(TRACE_NORMAL, 2000, L"Rotation Theta (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblTheta, dTheta);
		m_pCorrParam->dblTheta = dTheta;
	}

	// F-Theta lens
	if (m_pCorrParam->dblWorkX != dWorkX) {
		_tracer(TRACE_NORMAL, 2000, L"Lens Work X (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblWorkX, dWorkX);
		m_pCorrParam->dblWorkX = dWorkX;
	}
	if (m_pCorrParam->dblWorkY != dWorkY) {
		_tracer(TRACE_NORMAL, 2000, L"Lens Work Y (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblWorkY, dWorkY);
		m_pCorrParam->dblWorkY = dWorkY;
	}
	/*m_pCorrParam->dblImgCenX = dImgCX;
	m_pCorrParam->dblImgCenY = dImgCY;
	m_pCorrParam->dblImgSizeX = dImgSizeX;
	m_pCorrParam->dblImgSizeY = dImgSizeY;*/
	if (m_pCorrParam->dblLensPowerX != dLensPowerX) {
		_tracer(TRACE_NORMAL, 2000, L"Lens Power Scale X (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblLensPowerX, dLensPowerX);
		m_pCorrParam->dblLensPowerX = dLensPowerX;
	}
	if (m_pCorrParam->dblLensPowerY != dLensPowerY) {
		_tracer(TRACE_NORMAL, 2000, L"Lens Power Scale Y (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblLensPowerY, dLensPowerY);
		m_pCorrParam->dblLensPowerY = dLensPowerY;
	}
	if (m_pCorrParam->dblImgScaleX != dImageScaleX) {
		_tracer(TRACE_NORMAL, 2000, L"Lens Image Scale X (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblImgScaleX, dImageScaleX);
		m_pCorrParam->dblImgScaleX = dImageScaleX;
	}
	if (m_pCorrParam->dblImgScaleY != dImageScaleY) {
		_tracer(TRACE_NORMAL, 2000, L"Lens Image Scale Y (%d) : %.3f -> %.3f", m_iCurrentLens, m_pCorrParam->dblImgScaleY, dImageScaleY);
		m_pCorrParam->dblImgScaleY = dImageScaleY;
	}

	CString strBaseSize;
	strBaseSize.Format(L"%f", m_editBaseSize.GetValue());
	theApp.WriteProfileStringW(STR_SECTION_CORRPARAM, STR_KEY_CORR_BASESIZE, strBaseSize);
	m_bUpdate = TRUE;

	if (m_pDoc->m_config.m_iCurrLens != m_cbLensDB.GetCurSel()) {
		CString sCurName, sNewName;
		m_cbLensDB.GetLBText(m_pDoc->m_config.m_iCurrLens, sCurName);
		m_cbLensDB.GetLBText(m_cbLensDB.GetCurSel(), sNewName);
		_tracer(TRACE_NORMAL, 2000, L"Current Length change : %s(%d) -> %s(%d)", sCurName, m_pDoc->m_config.m_iCurrLens,
			sNewName, m_cbLensDB.GetCurSel());
		m_pDoc->m_config.m_iCurrLens = m_cbLensDB.GetCurSel();
	}

	//DHChoi 20200818, 스캔보정을 사용하는 경우 스캔보정데이터 저장부분에서 m_dWPCalibStep을 조작합니다.
	//  CDlgVScanCalib::OnBnClickedButtonDoSaveLearn()함수에서 값을 수정합니다.
	if (m_pCorrParam->bUseCalib == FALSE)
	{
		if (m_pDoc->m_config.m_dWPCalibStep != dWorkX / 4.0)
		{
			m_pDoc->m_config.m_dWPCalibStep = dWorkX / 4.0;
			AfxMessageBox(_T("Be sure to try Welding spot correction again"));
		}
	}
	AfxMessageBox(L"If you changed the parameters, restart the program ");

	// 20.03.04 hslee
	m_pDoc->ChangeScanRange();
}


void CDlgScanner::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnOK();
}



void CDlgScanner::OnSelchangeComboLensdb()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int iSel = m_cbLensDB.GetCurSel();
	if (iSel >= m_pDoc->m_config.m_nLens) return;
	if (iSel < 0) return;
	m_iCurrentLens = iSel;
	m_pCorrParam = &(m_pDoc->m_config.m_arCorrParam[iSel]);
	UpdateControl();
}
