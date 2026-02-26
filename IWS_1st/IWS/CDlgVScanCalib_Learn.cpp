// CDlgVScanCalib_Learn.cpp: 구현 파일
//
#include "./VRSaux/VRS.h"
#include "stdafx.h"
#include "IWS.h"
#include "IWSDoc.h"
#include "CDlgVScanCalib_Learn.h"
#include "afxdialogex.h"
#include "CDlgVScanCalib.h"
#include "./VRSaux/GrabImgDlg.h"
#include "./VRSaux/ScanImgsProc.h"

#include "CDlgVScanCalib_ConfGrid.h"




CDlgVScanCalib_Learn::CScanMap::CScanMap()
{
	m_nCntX = 10;
	m_nCntY = 10;
	m_pLearningDlg = NULL;
}


CDlgVScanCalib_Learn::CScanMap::~CScanMap()
{
}

COLORREF CDlgVScanCalib_Learn::CScanMap::OnGetDefBackColor(int section)
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

void CDlgVScanCalib_Learn::CScanMap::OnSetup()
{
	SetVScrollMode(UG_SCROLLNORMAL);
	SetHScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);// 2 = UG_CELLMODE_HIGHLIGHT
	SetHighlightRow(FALSE);

	SetNumberCols(m_nCntX);
	SetNumberRows(m_nCntY);
	SetTH_NumberRows(1);
	//SetTH_Height(0);


	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));


	SetHeadingDefault(&cell);



	CRect r;
	this->GetClientRect(r);
	int iHeaderW = (int)((r.Width() / (m_nCntX + 1)) + 0.5);
	int iHeaderH = (int)((r.Height() / (m_nCntY + 1)) + 0.5);

	//if (iHeaderW > iHeaderH)iHeaderW = iHeaderH;
	//else iHeaderH = iHeaderW;

	CString str;

	SetColWidth(-1, iHeaderW);
	for (int i = 0; i < m_nCntX; i++) {
		str.Format(L"%d", i);
		QuickSetText(i, -1, str);
		SetColWidth(i, iHeaderW);
	}
	SetRowHeight(-1, iHeaderH);
	for (int i = 0; i < m_nCntY; i++) {
		str.Format(L"%d", i);
		QuickSetText(-1, i, str);
		SetRowHeight(i, iHeaderH);
	}

	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	SetGridDefault(&cell);


	//CRect rc;
	//GetClientRect(rc);
	//int iw = int(double(rc.Width())*(1.0 - 0.1) / 2.0);// MAX_GRID_DIO);
	//int in = rc.Width() - iw * 2;// MAX_GRID_DIO;
	//this->SetColWidth(-1, in);
	//this->SetColWidth(0, iw);
	//this->SetColWidth(1, iw);

}

void CDlgVScanCalib_Learn::CScanMap::OnDClicked(int col, long row, RECT * rect, POINT * point, BOOL processed)
{
	TRACE(_T("\nDCLicked(%d,%d)\t\t"), col, row);
	if (m_pLearningDlg != NULL)
	{
		m_pLearningDlg->SelBlock(col, row);
	}
}

void CDlgVScanCalib_Learn::CScanMap::OnLClicked(int col, long row, int updn, RECT * rect, POINT * point, int processed)
{
}

void CDlgVScanCalib_Learn::CScanMap::OnKeyDown(UINT * vcKey, int processed)
{
}
void CDlgVScanCalib_Learn::CScanMap::SetBlockSize(int nX, int nY)
{
	ClearSelections();

	m_nCntX = nX;
	m_nCntY = nY;
	SetNumberCols(m_nCntX);
	SetNumberRows(m_nCntY);

	CRect r;
	this->GetClientRect(r);
	int iHeaderW = (int)((r.Width() / (m_nCntX + 1)) + 0.5);
	int iHeaderH = (int)((r.Height() / (m_nCntY + 1)) + 0.5);
	//if(iHeaderW > iHeaderH)iHeaderW = iHeaderH;
	//else iHeaderH = iHeaderW;

	CString str;

	SetColWidth(-1, iHeaderW);
	for (int i = 0; i < m_nCntX; i++) {
		str.Format(L"%d", i);
		QuickSetText(i, -1, str);
		SetColWidth(i, iHeaderW);
	}
	SetRowHeight(-1, iHeaderH);
	for (int i = 0; i < m_nCntY; i++) {
		str.Format(L"%d", i);
		QuickSetText(-1, i, str);
		SetRowHeight(i, iHeaderH);
	}
	CUGCell cell;
	GetGridDefault(&cell);

	for (int x = 0; x < m_nCntX; x++)
	{
		for (int y = 0; y < m_nCntY; y++)
		{
			cell.SetBackColor(RGB(0xF8, 0xF8, 0xF8));
			cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			SetCell(x, y, &cell);
		}
	}

	this->RedrawWindow();
}

BEGIN_MESSAGE_MAP(CDlgVScanCalib_Learn::CScanMap, CUGCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CDlgVScanCalib_Learn::CScanMap::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);

	//int iw = int((double(cx)*(1.0 - 0.02) / MAX_GRID_DIO));
	//int in = cx - iw * MAX_GRID_DIO;
	//this->SetColWidth(-1, in);
	//for (int i = 0; i < MAX_GRID_DIO; i++)
	//	this->SetColWidth(i, iw);

}
void CDlgVScanCalib_Learn::CScanMap::UpdateResult(CImageMerging* pMergeCtrl)
{
	if (pMergeCtrl == NULL) return;
	if (pMergeCtrl->m_pBlock == NULL) return;
	int nBlockX = pMergeCtrl->m_nBlockX;
	int nBlockY = pMergeCtrl->m_nBlockY;

	if (m_nCntX != nBlockX || nBlockY != m_nCntY)
	{

		return;
	}


	CUGCell cell;
	GetGridDefault(&cell);
	//int iStatus;
	for (int x = 0; x < nBlockX; x++)
	{
		for (int y = 0; y < nBlockY; y++)
		{
			ImgBlock* pBlock = pMergeCtrl->GetBlock(x, y);
					   
			if (pBlock != NULL && pBlock->iCalibStatus>=0)
			{

				if (pBlock->iCalibStatus < 25)
				{
					cell.SetBackColor(RGB(0xFF, 0xFF, 0xFF));
					cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
				}
				else
				{
					if (pBlock->pLearn.bLearnOK)
					{
						cell.SetBackColor(RGB(0, 255, 0));
						cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
					}
					else
					{
						cell.SetBackColor(RGB(255, 0, 0));
						cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
					}
				}
				SetCell(x, y, &cell);
			}
			else
			{
				cell.SetBackColor(RGB(0x00, 0x00, 0x00));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
				SetCell(x, y, &cell);
			}
		}
	}

	this->RedrawWindow();
}
void CDlgVScanCalib_Learn::CScanMap::UpdateStatus(CImageMerging* pMergeCtrl)
{
	if (pMergeCtrl == NULL) return;
	if (pMergeCtrl->m_pBlock == NULL) return;
	int nBlockX = pMergeCtrl->m_nBlockX;
	int nBlockY = pMergeCtrl->m_nBlockY;

	if (m_nCntX != nBlockX || nBlockY != m_nCntY)
	{
		
		return;
	}


	CUGCell cell;
	GetGridDefault(&cell);
	int iStatus;
	for (int x = 0; x < nBlockX; x++)
	{
		for (int y = 0; y < nBlockY; y++)
		{
			ImgBlock* pBlock = pMergeCtrl->GetBlock(x, y);

			if (pBlock == NULL)
			{
				cell.SetBackColor(RGB(0x00, 0x00, 0x00));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
				SetCell(x, y, &cell);
				continue;
			}
			iStatus = pBlock->iCalibStatus;
			if (pBlock->iCalibStatus < 25)
			{
				cell.SetBackColor(RGB(0xFF, 0xFF, 0xFF));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			}
			else if (pBlock->iCalibStatus < 40)
			{
				cell.SetBackColor(RGB(100, 100, 0x00));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			}
			else if (pBlock->iCalibStatus < 50)
			{
				cell.SetBackColor(RGB(180, 180, 0x00));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			}
			else if (pBlock->iCalibStatus < 60)
			{
				cell.SetBackColor(RGB(255, 0x00, 0x00));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			}
			else
			{
				cell.SetBackColor(RGB(0, 0xFF, 0x00));
				cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			}


			SetCell(x, y, &cell);




			//if (pBlock != NULL)
			//{
			//	if (pBlock->pLearn.bLearnOK)
			//	{
			//		cell.SetBackColor(RGB(0, 255, 0));
			//		cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			//	}
			//	else
			//	{
			//		cell.SetBackColor(RGB(255, 0, 0));
			//		cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			//	}
			//	SetCell(x, y, &cell);
			//}
			//else
			//{
			//	cell.SetBackColor(RGB(0x00, 0x00, 0x00));
			//	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
			//	SetCell(x, y, &cell);
			//}
		}
	}

	this->RedrawWindow();
}









// CDlgVScanCalib_Learn 대화 상자

IMPLEMENT_DYNAMIC(CDlgVScanCalib_Learn, CDialogEx)

CDlgVScanCalib_Learn::CDlgVScanCalib_Learn(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VSCANING_CALIB_LEARN, pParent)
	, m_pDlgCalib(NULL)
	, m_bInitDlg(FALSE)
	, m_pImgWnd(NULL)
	, m_pDoc(NULL)
	, m_pSnapImg(NULL)
	, m_pMergeCtrl(NULL)
	, m_iManualLearningStep(0)
{
	m_fZoomScale = 1.0f;
	m_fZoomScaleUnit = 0.05f;
	m_pSelBlock.x = m_pSelBlock.y = -1;
	m_pThreadLearning = NULL;
}

CDlgVScanCalib_Learn::~CDlgVScanCalib_Learn()
{

	if (m_pImgWnd != NULL)
	{
		delete m_pImgWnd;
		m_pImgWnd = NULL;
	}
	if (m_pSnapImg != NULL)
	{
		imaqDispose(m_pSnapImg);
		m_pSnapImg = NULL;
	}
}

void CDlgVScanCalib_Learn::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_STRENGTH_MIN, m_eStrengthMin);
	DDX_Control(pDX, IDC_EDIT_STRENGTH_MAX, m_eStrengthMax);
	DDX_Control(pDX, IDC_EDIT_LEARN_AREA_W, m_eLearnAreaW);
	DDX_Control(pDX, IDC_EDIT_LEARN_AREA_H, m_eLearnAreaH);
	DDX_Radio(pDX, IDC_RADIO_SHOW_ORG, m_iManualLearningStep);
}


BEGIN_MESSAGE_MAP(CDlgVScanCalib_Learn, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_DO_LEARN, &CDlgVScanCalib_Learn::OnBnClickedButtonDoLearn)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_P, &CDlgVScanCalib_Learn::OnBnClickedButtonZoomP)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_FIT, &CDlgVScanCalib_Learn::OnBnClickedButtonZoomFit)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_N, &CDlgVScanCalib_Learn::OnBnClickedButtonZoomN)
	ON_BN_CLICKED(IDC_RADIO_SHOW_ORG, &CDlgVScanCalib_Learn::OnBnClickedRadioShowOrg)
	ON_BN_CLICKED(IDC_RADIO_CONF_LGRID, &CDlgVScanCalib_Learn::OnBnClickedRadioConfLgrid)
	ON_BN_CLICKED(IDC_RADIO_DO_LEARN, &CDlgVScanCalib_Learn::OnBnClickedRadioDoLearn)
	ON_BN_CLICKED(IDC_RADIO_CONF_SGRID, &CDlgVScanCalib_Learn::OnBnClickedRadioConfSgrid)
	ON_BN_CLICKED(IDC_RADIO_STRETCHING, &CDlgVScanCalib_Learn::OnBnClickedRadioStretching)
	ON_BN_CLICKED(IDC_RADIO_CONF_CGRID, &CDlgVScanCalib_Learn::OnBnClickedRadioConfCgrid)
	ON_BN_CLICKED(IDC_BUTTON_INIT_PARAM, &CDlgVScanCalib_Learn::OnBnClickedButtonInitParam)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_CALC_STICHING_PARAM, &CDlgVScanCalib_Learn::OnBnClickedButtonCalcStichingParam)
END_MESSAGE_MAP()


// CDlgVScanCalib_Learn 메시지 처리기

void CDlgVScanCalib_Learn::OnOK()
{
	//CDialogEx::OnOK();
}


void CDlgVScanCalib_Learn::OnCancel()
{
	//CDialogEx::OnCancel();
}

void CDlgVScanCalib_Learn::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && m_bInitDlg == TRUE)
	{
		InitDlgData();
		


		if (m_pSnapImg != NULL)
		{
			imaqDispose(m_pSnapImg);
			m_pSnapImg = NULL;
		}
		m_LearningMap.SetBlockSize(m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY);
		m_LearningMap.UpdateResult(m_pMergeCtrl);

		m_pSnapImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
		imaqDuplicate(m_pSnapImg, m_pDlgCalib->m_pScanImage_org[m_pDlgCalib->m_nBlockX / 2][m_pDlgCalib->m_nBlockY / 2]);
		imaqDisplayImage(m_pSnapImg, m_pImgWnd->GetNiWndId(), FALSE);
		imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
		imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	}
}
BOOL CDlgVScanCalib_Learn::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pImgWnd = new CWorkImg(this);
	m_pImgWnd->AttachToParentWnd(GetDlgItem(IDC_STATIC_IMG_WND), TRUE, { 157, 157,   0, 0 });
	m_pImgWnd->_imaqShowScrollbars(TRUE);

	m_LearningMap.m_pLearningDlg = this;
	m_LearningMap.AttachGrid(this, IDC_STATIC_MAP);

	InitDlgData();

	
	m_iManualLearningStep = 0;
	UpdateData(FALSE);

	m_bInitDlg = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CDlgVScanCalib_Learn::InitDlgData()
{
	CString sVal;
	sVal.Format(_T("%.1f"), m_pDlgCalib->m_fLearnStrengthMin);	m_eStrengthMin.SetWindowText(sVal);
	sVal.Format(_T("%.1f"), m_pDlgCalib->m_fLearnStrengthMax);	m_eStrengthMax.SetWindowText(sVal);
}



void CDlgVScanCalib_Learn::OnBnClickedButtonInitParam()
{

	if (!m_pDlgCalib->m_bInitGridParam) {
		AfxMessageBox(_T("Please try after setting the Grid parameter."));
		return;
	}
	if (m_pDlgCalib->m_pScanImage_org == NULL) {
		AfxMessageBox(_T("Please try after scanning."));
		return;
	}
	CString sESMin, sESMax;
	m_eStrengthMin.GetWindowText(sESMin);
	m_eStrengthMax.GetWindowText(sESMax);

	BOOL bFindDarkObj = m_pDlgCalib->m_bFindDarkObj;
	m_pDlgCalib->m_fLearnStrengthMin = _ttof(sESMin);
	m_pDlgCalib->m_fLearnStrengthMax = _ttof(sESMax);

	if (m_pMergeCtrl->CreateMerginInfo3_InitParam(m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY,
		m_pDlgCalib->m_pScanImage_org, m_pDlgCalib->m_pGrabPos, m_pDlgCalib->m_rCopyArea, m_pDlgCalib->m_iGridUnitSize_px,
		m_pDlgCalib->m_fLearnStrengthMin, m_pDlgCalib->m_fLearnStrengthMax, bFindDarkObj) < 0)
	{
		AfxMessageBox(_T("Error !!"));
	}
	AfxMessageBox(_T("Finished"));

	m_LearningMap.UpdateResult(m_pMergeCtrl);
	m_pDlgCalib->GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(FALSE);
	m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);
	m_pDlgCalib->m_bSetCenterPos = FALSE;
}

UINT CDlgVScanCalib_Learn::StartThreadLearning(LPVOID pParam)
{
	CDlgVScanCalib_Learn *pMe = (CDlgVScanCalib_Learn*)pParam;
	pMe->ThreadLearning();
	return 0;
}
int CDlgVScanCalib_Learn::ThreadLearning()
{

	int nx = m_pMergeCtrl->m_nBlockX;
	int ny = m_pMergeCtrl->m_nBlockY;
	for (int x = 0; x < nx; x++)
	{
		for (int y = 0; y < ny; y++)
		{
			ImgBlock* pBlock = m_pMergeCtrl->GetBlock(x, y);
			if (pBlock != NULL) pBlock->iCalibStatus = 20;
		}
	}



	m_pMergeCtrl->CreateMerginInfo3_LearningAndScaling(m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY,
		m_pDlgCalib->m_pScanImage_org, m_pDlgCalib->m_pGrabPos, m_pDlgCalib->m_rCopyArea, m_pDlgCalib->m_iGridUnitSize_px,
		m_pDlgCalib->m_fLearnStrengthMin, m_pDlgCalib->m_fLearnStrengthMax, m_pDlgCalib->m_bFindDarkObj);

	m_pThreadLearning = NULL;
	return 0;
}

void CDlgVScanCalib_Learn::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 12011)
	{
		

		m_LearningMap.UpdateStatus(m_pMergeCtrl);


		if (m_pThreadLearning == NULL)
		{
			
			m_LearningMap.UpdateResult(m_pMergeCtrl);
			GetDlgItem(IDC_BUTTON_DO_LEARN)->EnableWindow(TRUE);
			KillTimer(12011);
			AfxMessageBox(_T("Finished Learning"));
		}
	}

	CDialogEx::OnTimer(nIDEvent);
}

void CDlgVScanCalib_Learn::OnBnClickedButtonDoLearn()
{
	if (!m_pDlgCalib->m_bInitGridParam) {
		AfxMessageBox(_T("Please try after setting the Grid parameter."));
		return;
	}
	if (m_pDlgCalib->m_pScanImage_org == NULL) {
		AfxMessageBox(_T("Please try after scanning."));
		return;
	}



	CString sESMin, sESMax;
	m_eStrengthMin.GetWindowText(sESMin);
	m_eStrengthMax.GetWindowText(sESMax);

	BOOL bFindDarkObj = m_pDlgCalib->m_bFindDarkObj;
	m_pDlgCalib->m_fLearnStrengthMin = _ttof(sESMin);
	m_pDlgCalib->m_fLearnStrengthMax = _ttof(sESMax);

	m_LearningMap.ClearSelections();
	GetDlgItem(IDC_BUTTON_DO_LEARN)->EnableWindow(FALSE);
	m_pThreadLearning = AfxBeginThread(StartThreadLearning, this);
	SetTimer(12011, 500, NULL);

	//if (m_pMergeCtrl->CreateMerginInfo3_LearningAndScaling(m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY,
	//	m_pDlgCalib->m_pScanImage_org, m_pDlgCalib->m_pGrabPos, m_pDlgCalib->m_rCopyArea, m_pDlgCalib->m_iGridUnitSize_px,
	//	m_pDlgCalib->m_fLearnStrengthMin, m_pDlgCalib->m_fLearnStrengthMax, bFindDarkObj) < 0)
	//{
	//	AfxMessageBox(_T("Error !!"));
	//}
	//AfxMessageBox(_T("Finished"));
	//
	//
	//
	//m_LearningMap.UpdateResult(m_pMergeCtrl);


	return;


	if (m_pMergeCtrl->CreateMerginInfo(m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY,
		m_pDlgCalib->m_pScanImage_org, m_pDlgCalib->m_pGrabPos, m_pDlgCalib->m_rCopyArea, m_pDlgCalib->m_iGridUnitSize_px,
		m_pDlgCalib->m_fLearnStrengthMin, m_pDlgCalib->m_fLearnStrengthMax, bFindDarkObj) < 0)
	{
		AfxMessageBox(_T("Error Learning!!"));
	}

	//m_spinBlockX.SetRange(0, m_nBlockX - 1);
	//m_spinBlockY.SetRange(0, m_nBlockY - 1);
	//CString sBCnt;
	//sBCnt.Format(_T("/ %d"), m_nBlockX - 1); m_stBlockCntX.SetWindowText(sBCnt);
	//sBCnt.Format(_T("/ %d"), m_nBlockY - 1); m_stBlockCntY.SetWindowText(sBCnt);
	//
	//OnBnClickedCheckShowCalib();
	AfxMessageBox(_T("Finished Learning"));
	m_pDlgCalib->m_bSetCenterPos = FALSE;




	float fResolution_umPpx = 1.0;
	//if (!m_bInitGridParam) {
	//	CString sLensPower;
	//	m_eLensPower.GetWindowText(sLensPower);
	//	fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / _ttof(sLensPower);
	//}
	//else
	//{
	fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / m_pDlgCalib->m_fLensPower;
	//}


	ImgBlock* pBlockMin = m_pMergeCtrl->GetBlock(0, 0);
	ImgBlock* pBlockMax = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1, m_pDlgCalib->m_nBlockY - 1);
	float fSAreaW_px = pBlockMax->pLearn.pInfo.pMergePos.x + pBlockMax->pLearn.pInfo.rCopyArea.width;
	float fSAreaH_px = pBlockMax->pLearn.pInfo.pMergePos.y + pBlockMax->pLearn.pInfo.rCopyArea.height;
	float fSAreaW_mm = 0.001f* fSAreaW_px * fResolution_umPpx;
	float fSAreaH_mm = 0.001f* fSAreaH_px * fResolution_umPpx;

	CString sVal;
	sVal.Format(_T("%.2f"), fSAreaW_mm);  m_eLearnAreaW.SetWindowText(sVal);
	sVal.Format(_T("%.2f"), fSAreaH_mm);  m_eLearnAreaH.SetWindowText(sVal);


	m_pDlgCalib->GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(FALSE);
	m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);
	m_pDlgCalib->m_bSetCenterPos = FALSE;
}


void CDlgVScanCalib_Learn::OnBnClickedButtonCalcStichingParam()
{

	if (!m_pDlgCalib->m_bInitGridParam) {
		AfxMessageBox(_T("Please try after setting the Grid parameter."));
		return;
	}
	if (m_pDlgCalib->m_pScanImage_org == NULL) {
		AfxMessageBox(_T("Please try after scanning."));
		return;
	}


	for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
	{
		for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
		{
			ImgBlock* pBlock = m_pMergeCtrl->GetBlock(x, y);

			if (pBlock != NULL && pBlock->iCalibStatus >= 0)
			{
				if (pBlock->iCalibStatus < 60)
				{
					CString sMsg;
					sMsg.Format(_T("A problem was found in block (%d,%d). Please try after solving."), x,y);
					AfxMessageBox(sMsg);
					return;
				}
			}
		}
	}




	if (m_pMergeCtrl->CreateMerginInfo3_CalcStitchingParam(m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY,
		m_pDlgCalib->m_pScanImage_org, m_pDlgCalib->m_pGrabPos, m_pDlgCalib->m_rCopyArea, m_pDlgCalib->m_iGridUnitSize_px,
		m_pDlgCalib->m_fLearnStrengthMin, m_pDlgCalib->m_fLearnStrengthMax, m_pDlgCalib->m_bFindDarkObj) < 0)
	{
		AfxMessageBox(_T("Error !!"));
		return;
	}
	AfxMessageBox(_T("Finished"));


	m_pDlgCalib->m_bSetCenterPos = FALSE;




	float fResolution_umPpx = 1.0;
	//if (!m_bInitGridParam) {
	//	CString sLensPower;
	//	m_eLensPower.GetWindowText(sLensPower);
	//	fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / _ttof(sLensPower);
	//}
	//else
	//{
	fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / m_pDlgCalib->m_fLensPower;
	//}


	ImgBlock* pBlockMin = m_pMergeCtrl->GetBlock(0, 0);
	ImgBlock* pBlockMax = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1, m_pDlgCalib->m_nBlockY - 1);
	float fSAreaW_px = pBlockMax->pLearn.pInfo.pMergePos.x + pBlockMax->pLearn.pInfo.rCopyArea.width;
	float fSAreaH_px = pBlockMax->pLearn.pInfo.pMergePos.y + pBlockMax->pLearn.pInfo.rCopyArea.height;
	float fSAreaW_mm = 0.001f* fSAreaW_px * fResolution_umPpx;
	float fSAreaH_mm = 0.001f* fSAreaH_px * fResolution_umPpx;

	CString sVal;
	sVal.Format(_T("%.2f"), fSAreaW_mm);  m_eLearnAreaW.SetWindowText(sVal);
	sVal.Format(_T("%.2f"), fSAreaH_mm);  m_eLearnAreaH.SetWindowText(sVal);



	m_pDlgCalib->GetDlgItem(IDC_RADIO_Stiching)->EnableWindow(TRUE);
	m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);
	m_pDlgCalib->m_bSetCenterPos = FALSE;
}



void CDlgVScanCalib_Learn::OnBnClickedButtonZoomP()
{

	//if (m_chkShowMerge.GetCheck())
	//{
	//	m_fZoomScaleMerge += m_fZoomScaleUnit;
	//
	//	if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_nBlockX &&
	//		m_pSelBlock.y >= 0 && m_pSelBlock.y < m_nBlockY)
	//	{
	//		CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
	//		m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
	//		m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
	//	}
	//	else
	//	{
	//		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	}
	//
	//	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	//}
	//else
	//{
	m_fZoomScale += m_fZoomScaleUnit;

	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	//}
}


void CDlgVScanCalib_Learn::OnBnClickedButtonZoomFit()
{
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);

	//if (m_chkShowMerge.GetCheck())
	//{
	//	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScaleMerge, &m_fZoomScaleMerge);
	//}
	//else
	//{

	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	//}



}


void CDlgVScanCalib_Learn::OnBnClickedButtonZoomN()
{
	//if (m_chkShowMerge.GetCheck())
	//{
	//
	//	if ((m_fZoomScaleMerge - m_fZoomScaleUnit) < 0.01)
	//	{
	//	}
	//	else
	//	{
	//		m_fZoomScaleMerge -= m_fZoomScaleUnit;
	//	}
	//
	//
	//	if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_nBlockX &&
	//		m_pSelBlock.y >= 0 && m_pSelBlock.y < m_nBlockY)
	//	{
	//		CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
	//		m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
	//		m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
	//	}
	//	else
	//	{
	//		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	}
	//
	//	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	//}
	//else
	//{

	if ((m_fZoomScale - m_fZoomScaleUnit) < 0.01)
	{
	}
	else
	{
		m_fZoomScale -= m_fZoomScaleUnit;
	}
	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	//}
}




void CDlgVScanCalib_Learn::OnBnClickedRadioShowOrg()
{
	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pBlock == NULL) return;

	imaqDisplayImage(pBlock->pLearn.pImgLearn, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
}


void CDlgVScanCalib_Learn::OnBnClickedRadioConfLgrid()
{
	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pBlock == NULL) return;

	CDlgVScanCalib_ConfGrid pDlg;
	pDlg.m_pImg = pBlock->pLearn.pImgLearn;
	pDlg.m_pGridParam = pBlock->gridLearn;
	if (pDlg.DoModal() != IDOK) { return; }


	CornerData pCornerOrg;
	if (CImageMerging::FindCornerInfo3(pBlock->pLearn.pImgLearn, &(pDlg.m_pGridParam), 
		(int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		return;
	}
	pBlock->gridLearn = pDlg.m_pGridParam;


	GetDlgItem(IDC_RADIO_DO_LEARN)->EnableWindow(TRUE);

}


void CDlgVScanCalib_Learn::OnBnClickedRadioDoLearn()
{
	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pBlock == NULL) return;

	if (CImageMerging::LearnImage3(pBlock->pLearn.pImgLearn, &(pBlock->gridLearn), (float)m_pDlgCalib->m_iGridUnitSize_px) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		GetDlgItem(IDC_RADIO_DO_LEARN)->EnableWindow(FALSE);
		return;
	}
	if (pBlock->pCalibImg != NULL)
	{
		imaqDispose(pBlock->pCalibImg);
		pBlock->pCalibImg = NULL;
	}
	pBlock->pCalibImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
	imaqDuplicate(pBlock->pCalibImg, pBlock->pLearn.pImgLearn);
	
	if (CImageMerging::ApplyCalib(pBlock->pLearn.pImgLearn, &(pBlock->pCalibImg), pBlock->gridLearn.left.bFindDarkObj) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		GetDlgItem(IDC_RADIO_DO_LEARN)->EnableWindow(FALSE);
		return;
	}

	CornerData pCorner;
	if (CImageMerging::FindCornerInfo3(pBlock->pCalibImg, &(pBlock->gridLearn),
		(int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCorner) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		GetDlgItem(IDC_RADIO_DO_LEARN)->EnableWindow(FALSE);
		return;
	}

	float fdiffL = fabs(pCorner.pLB.x - pCorner.pLT.x);
	float fdiffR = fabs(pCorner.pRB.x - pCorner.pRT.x);
	float fdiffT = fabs(pCorner.pLT.y - pCorner.pRT.y);
	float fdiffB = fabs(pCorner.pLB.y - pCorner.pRB.y);
	if (fdiffL > 5.0f || fdiffR > 5.0f || fdiffT > 5.0f || fdiffB > 5.0f)
	{
		AfxMessageBox(_T("The correction exceeds the tolerance.Please change the settings and try again"));
		GetDlgItem(IDC_RADIO_DO_LEARN)->EnableWindow(FALSE);
		
		TRACE(_T("\n RECT. chekck error(DEL(L)=%.1f, DEL(R)=%.1f, DEL(T)=%.1f, DEL(B)=%.1f)) \t\t"),
			fdiffL, fdiffR, fdiffT, fdiffB);
		
		return;
	}




	imaqDisplayImage(pBlock->pCalibImg, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	GetDlgItem(IDC_RADIO_CONF_SGRID)->EnableWindow(TRUE);
}


void CDlgVScanCalib_Learn::OnBnClickedRadioConfSgrid()
{
	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pBlock == NULL) return;

	CDlgVScanCalib_ConfGrid pDlg;
	pDlg.m_pImg = pBlock->pCalibImg;
	pDlg.m_pGridParam = pBlock->gridScaling;
	if (pDlg.DoModal() != IDOK) { return; }


	CornerData pCornerOrg;
	if (CImageMerging::FindCornerInfo3(pBlock->pCalibImg, &(pDlg.m_pGridParam),
		(int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		return;
	}
	pBlock->gridScaling = pDlg.m_pGridParam;


	GetDlgItem(IDC_RADIO_STRETCHING)->EnableWindow(TRUE);
}


void CDlgVScanCalib_Learn::OnBnClickedRadioStretching()
{

	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pBlock == NULL) return;

	CornerData pCorner;
	if (CImageMerging::FindCornerInfo3(pBlock->pCalibImg, &(pBlock->gridScaling), (int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCorner) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		GetDlgItem(IDC_RADIO_STRETCHING)->EnableWindow(FALSE);
		return;
	}
	float fRatioX = (float)m_pDlgCalib->m_iGridUnitSize_px / pCorner.fDivLngWUnit_px;
	float fRatioY = (float)m_pDlgCalib->m_iGridUnitSize_px / pCorner.fDivLngHUnit_px;
	int iOrgW, iOrgH;
	imaqGetImageSize(pBlock->pCalibImg, &iOrgW, &iOrgH);
	int iNewW = (int)(((float)iOrgW*fRatioX) + 0.5f);
	int iNewH = (int)(((float)iOrgH*fRatioY) + 0.5f);

	if (imaqResample(pBlock->pCalibImg, pBlock->pCalibImg, iNewW, iNewH, IMAQ_QUADRATIC, IMAQ_NO_RECT) == 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		GetDlgItem(IDC_RADIO_STRETCHING)->EnableWindow(FALSE);
		return;
	}
	imaqDisplayImage(pBlock->pCalibImg, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	GetDlgItem(IDC_RADIO_CONF_CGRID)->EnableWindow(TRUE);
}


void CDlgVScanCalib_Learn::OnBnClickedRadioConfCgrid()
{
	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pBlock == NULL) return;

	CDlgVScanCalib_ConfGrid pDlg;
	pDlg.m_pImg = pBlock->pCalibImg;
	pDlg.m_pGridParam = pBlock->gridStitching;
	if (pDlg.DoModal() != IDOK) { return; }



	//CornerData pCornerOrg;
	//if (CImageMerging::FindCornerInfo3(pBlock->pCalibImg, &(pDlg.m_pGridParam),
	//	(int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg) < 0)
	//{
	//	AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
	//	return;
	//}
	
	
	
	CornerData pCornerOrg;
	if (CImageMerging::FindGridInfo3(pBlock->pCalibImg, &(pDlg.m_pGridParam),
		(int)(m_pDlgCalib->m_iGridUnitSize_px*0.93), &pCornerOrg) < 0)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		return;
	}
	if (!pCornerOrg.bLB
		|| !pCornerOrg.bRB
		|| !pCornerOrg.bLT
		|| !pCornerOrg.bRT)
	{
		AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
		return;
	}
	
	
	pBlock->gridStitching = pDlg.m_pGridParam;

	pBlock->iCalibStatus = 60;
	pBlock->bCalibOK = TRUE;
	pBlock->pLearn.bLearnOK = TRUE;
	m_LearningMap.UpdateResult(m_pMergeCtrl);
	
}

void CDlgVScanCalib_Learn::SelBlock(int x, int y)
{
	m_iManualLearningStep = 0;
	UpdateData(FALSE);

	m_pSelBlock.x = -1;
	m_pSelBlock.y = -1;

	ImgBlock * pBlock = m_pMergeCtrl->GetBlock(x, y);
	if (pBlock == NULL) return;
	m_pSelBlock.x = x;
	m_pSelBlock.y = y;

	CString str;
	str.Format(_T("(%d, %d)"), x, y);
	GetDlgItem(IDC_STATIC_T_BLOCK)->SetWindowText(str);

	imaqDisplayImage(pBlock->pLearn.pImgLearn, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);

	TRACE(_T("\n Cur Block(%d,%d) status =%d \t\t"), x, y, pBlock->iCalibStatus);

	
	if (pBlock->iCalibStatus < 25)
	{
		AfxMessageBox(_T("초기화가 되어있지 않음"));
	}
	else if (pBlock->iCalibStatus < 30)
	{
		AfxMessageBox(_T("러닝 시도 중 실패 함"));
	}
	else if (pBlock->iCalibStatus < 40)
	{
		AfxMessageBox(_T("러닝후 크기보정을 위한 격자 인식 중 4개의 교점이 없거나, 교점들이 직사각형(허용범위 +-5px)을 형성하고 있지 않다."));
	}
	else if (pBlock->iCalibStatus < 50)
	{
		AfxMessageBox(_T("스캐일링 시도 중 실패 함"));
	}
	else if (pBlock->iCalibStatus < 60)
	{
		AfxMessageBox(_T("이어 붙이기를 위한 격자 인식 중 4개의 교점을 찾지 못함"));
	}

	GetDlgItem(IDC_RADIO_SHOW_ORG)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_CONF_LGRID	)->EnableWindow(TRUE);
	GetDlgItem(IDC_RADIO_DO_LEARN	)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_CONF_SGRID	)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_STRETCHING	)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_CONF_CGRID	)->EnableWindow(FALSE);

}


