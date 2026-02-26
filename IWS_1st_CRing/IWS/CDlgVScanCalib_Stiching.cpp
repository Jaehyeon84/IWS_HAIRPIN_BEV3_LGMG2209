// CDlgVScanCalib_Stiching.cpp: 구현 파일
//
#include "./VRSaux/VRS.h"
#include "stdafx.h"
#include "IWS.h"
#include "IWSDoc.h"
#include "CDlgVScanCalib_Stiching.h"
#include "afxdialogex.h"
#include "CDlgVScanCalib.h"
#include "./VRSaux/GrabImgDlg.h"
#include "./VRSaux/ScanImgsProc.h"


// CDlgVScanCalib_Stiching 대화 상자

IMPLEMENT_DYNAMIC(CDlgVScanCalib_Stiching, CDialogEx)

CDlgVScanCalib_Stiching::CDlgVScanCalib_Stiching(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VSCANING_CALIB_STICHING, pParent)
	, m_pDlgCalib(NULL)
	, m_bInitDlg(FALSE)
	, m_pImgWnd(NULL)
	, m_pDoc(NULL)
	, m_pLearningImg(NULL)
	, m_pMergeCtrl(NULL)
	, m_iDisTarget(0)
{

	m_pSelBlock = imaqMakePoint(-1, -1);
	m_fZoomScale = 1.0f;
	m_fZoomScaleUnit = 0.05f;
	m_fZoomScaleMerge = 1.0f;
	m_enShowTarget = SHOW_IMG_NONE;
}

CDlgVScanCalib_Stiching::~CDlgVScanCalib_Stiching()
{

	if (m_pImgWnd != NULL)
	{
		delete m_pImgWnd;
		m_pImgWnd = NULL;
	}
	if (m_pLearningImg != NULL)
	{
		imaqDispose(m_pLearningImg);
		m_pLearningImg = NULL;
	}
}

void CDlgVScanCalib_Stiching::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT_LEARN_AREA_SX, m_eLearnAreaSX);
	DDX_Control(pDX, IDC_EDIT_LEARN_AREA_SY, m_eLearnAreaSY);
	DDX_Control(pDX, IDC_EDIT_LEARN_AREA_EX, m_eLearnAreaEX);
	DDX_Control(pDX, IDC_EDIT_LEARN_AREA_EY, m_eLearnAreaEY);

	DDX_Control(pDX, IDC_EDIT_BLOCK_X, m_eBlockX);
	DDX_Control(pDX, IDC_EDIT_BLOCK_Y, m_eBlockY);
	DDX_Control(pDX, IDC_SPIN_BLOCK_X, m_spinBlockX);
	DDX_Control(pDX, IDC_SPIN_BLOCK_Y, m_spinBlockY);
	DDX_Control(pDX, IDC_STATIC_BLOCK_CNT_X, m_stBlockCntX);
	DDX_Control(pDX, IDC_STATIC_BLOCK_CNT_Y, m_stBlockCntY);

	//DDX_Control(pDX, IDC_CHECK_SHOW_ORIGIN, m_chkShowOrgin);
	//DDX_Control(pDX, IDC_CHECK_SHOW_CALIB, m_chkShowCalib);
	//DDX_Control(pDX, IDC_CHECK_SHOW_LEARN, m_chkShowLearn);
	//DDX_Control(pDX, IDC_CHECK_SHOW_MERGED, m_chkShowMerge);

	DDX_Control(pDX, IDC_EDIT_BLOCK_MOVE_UNIT, m_eMoveBlockUnit_px);
	DDX_Control(pDX, IDC_CHECK_SHOW_GRID_LINE, m_chkShowGridLine);


	DDX_Radio(pDX, IDC_RADIO_SHOW_ORG, m_iDisTarget);
	DDX_Control(pDX, IDC_CHECK_SHOW_COPY_AREA, m_chkShowCopyArea);
}


BEGIN_MESSAGE_MAP(CDlgVScanCalib_Stiching, CDialogEx)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_P, &CDlgVScanCalib_Stiching::OnBnClickedButtonZoomP)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_FIT, &CDlgVScanCalib_Stiching::OnBnClickedButtonZoomFit)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_N, &CDlgVScanCalib_Stiching::OnBnClickedButtonZoomN)


	ON_BN_CLICKED(IDC_BUTTON_DO_CALIB_MERGE, &CDlgVScanCalib_Stiching::OnBnClickedButtonDoCalibMerge)
	ON_BN_CLICKED(IDC_BUTTON_SET_CENTER_BLOCK, &CDlgVScanCalib_Stiching::OnBnClickedButtonSetCenterBlock)



	//ON_BN_CLICKED(IDC_CHECK_SHOW_ORIGIN, &CDlgVScanCalib_Stiching::OnBnClickedCheckShowOrigin)
	//ON_BN_CLICKED(IDC_CHECK_SHOW_CALIB, &CDlgVScanCalib_Stiching::OnBnClickedCheckShowCalib)
	//ON_BN_CLICKED(IDC_CHECK_SHOW_LEARN, &CDlgVScanCalib_Stiching::OnBnClickedCheckShowLearn)
	//ON_BN_CLICKED(IDC_CHECK_SHOW_MERGED, &CDlgVScanCalib_Stiching::OnBnClickedCheckShowMerged)



	ON_BN_CLICKED(IDC_BUTTON_SEL_BLOCK_UP, &CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockUp)
	ON_BN_CLICKED(IDC_BUTTON_SEL_BLOCK_LEFT, &CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockLeft)
	ON_BN_CLICKED(IDC_BUTTON_SEL_BLOCK_DOWN, &CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockDown)
	ON_BN_CLICKED(IDC_BUTTON_SEL_BLOCK_RIGHT, &CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockRight)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLOCK_X, &CDlgVScanCalib_Stiching::OnDeltaposSpinBlockX)
	ON_NOTIFY(UDN_DELTAPOS, IDC_SPIN_BLOCK_Y, &CDlgVScanCalib_Stiching::OnDeltaposSpinBlockY)



	ON_BN_CLICKED(IDC_CHECK_SHOW_GRID_LINE, &CDlgVScanCalib_Stiching::OnBnClickedCheckShowGridLine)
	ON_BN_CLICKED(IDC_BUTTON_BMOVE_Y_N, &CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveYN)
	ON_BN_CLICKED(IDC_BUTTON_BMOVE_Y_P, &CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveYP)
	ON_BN_CLICKED(IDC_BUTTON_BMOVE_X_N, &CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveXN)
	ON_BN_CLICKED(IDC_BUTTON_BMOVE_X_P, &CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveXP)

	ON_MESSAGE(UWM_WORKIMG_EVENT, &CDlgVScanCalib_Stiching::OnWorkImgEvent)


	ON_BN_CLICKED(IDC_RADIO_SHOW_ORG, &CDlgVScanCalib_Stiching::OnBnClickedRadioShowOrg)
	ON_BN_CLICKED(IDC_RADIO_SHOW_LEARN, &CDlgVScanCalib_Stiching::OnBnClickedRadioShowLearn)
	ON_BN_CLICKED(IDC_RADIO_SHOW_CALIB, &CDlgVScanCalib_Stiching::OnBnClickedRadioShowCalib)
	ON_BN_CLICKED(IDC_RADIO_SHOW_MERGED, &CDlgVScanCalib_Stiching::OnBnClickedRadioShowMerged)
	ON_BN_CLICKED(IDC_CHECK_SHOW_COPY_AREA, &CDlgVScanCalib_Stiching::OnBnClickedCheckShowCopyArea)
END_MESSAGE_MAP()


// CDlgVScanCalib_Stiching 메시지 처리기

void CDlgVScanCalib_Stiching::OnOK()
{
	//CDialogEx::OnOK();
}


void CDlgVScanCalib_Stiching::OnCancel()
{
	//CDialogEx::OnCancel();
}

void CDlgVScanCalib_Stiching::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow && m_bInitDlg == TRUE)
	{
		InitDlgData();
	}
}
BOOL CDlgVScanCalib_Stiching::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pImgWnd = new CWorkImg(this);
	m_pImgWnd->AttachToParentWnd(GetDlgItem(IDC_STATIC_IMG_WND), TRUE, { 157, 157,   0, 0 });
	m_pImgWnd->_imaqShowScrollbars(TRUE);



	CString sVal;

	sVal.Format(_T("%d"), m_pDlgCalib->m_iGridUnitSize_px);
	m_eMoveBlockUnit_px.SetWindowText(sVal);

	m_eBlockX.SetWindowText(_T("0"));
	m_eBlockY.SetWindowText(_T("0"));
	//m_chkShowOrgin.SetCheck(FALSE);
	//m_chkShowOrgin.SetCheck(FALSE);
	//m_chkShowCalib.SetCheck(FALSE);
	//m_chkShowMerge.SetCheck(FALSE);

	InitDlgData();


	m_iDisTarget = 0;
	UpdateData(FALSE);

	m_bInitDlg = TRUE;
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CDlgVScanCalib_Stiching::InitDlgData()
{


	m_spinBlockX.SetRange(0, m_pDlgCalib->m_nBlockX - 1);
	m_spinBlockY.SetRange(0, m_pDlgCalib->m_nBlockY - 1);
	CString sBCnt;
	sBCnt.Format(_T("/ %d"), m_pDlgCalib->m_nBlockX - 1); m_stBlockCntX.SetWindowText(sBCnt);
	sBCnt.Format(_T("/ %d"), m_pDlgCalib->m_nBlockY - 1); m_stBlockCntY.SetWindowText(sBCnt);

	if (m_pMergeCtrl->m_pBlock != NULL
		&& m_pMergeCtrl->m_nBlockX != 0
		&& m_pMergeCtrl->m_nBlockY != 0)
	{

		ImgBlock* pBlockMin = m_pMergeCtrl->GetBlock(0, 0);
		ImgBlock* pBlockMax = m_pMergeCtrl->GetBlock(m_pMergeCtrl->m_nBlockX - 1, m_pMergeCtrl->m_nBlockY - 1);
		float xMin = pBlockMin->pLearn.pInfo.pStartPos_mm.x;
		float xMax = pBlockMax->pLearn.pInfo.pEndPos_mm.x;

		float yMax = pBlockMin->pLearn.pInfo.pStartPos_mm.y;
		float yMin = pBlockMax->pLearn.pInfo.pEndPos_mm.y;

		CString sVal;
		sVal.Format(_T("%.2f"), xMin);  m_eLearnAreaSX.SetWindowText(sVal);
		sVal.Format(_T("%.2f"), yMin);  m_eLearnAreaSY.SetWindowText(sVal);
		sVal.Format(_T("%.2f"), xMax);  m_eLearnAreaEX.SetWindowText(sVal);
		sVal.Format(_T("%.2f"), yMax);  m_eLearnAreaEY.SetWindowText(sVal);
		//sVal.Format(_T("%.2f"), xMax - xMin);  m_eLearnAreaW.SetWindowText(sVal);
		//sVal.Format(_T("%.2f"), yMax - yMin);  m_eLearnAreaH.SetWindowText(sVal);
	}

}


void CDlgVScanCalib_Stiching::OnBnClickedButtonDoCalibMerge()
{

		if (!m_pDlgCalib->m_bInitGridParam) {
			AfxMessageBox(_T("Please try after setting the Grid parameter."));
			return;
		}
		if (m_pDlgCalib->m_pScanImage_org == NULL) {
			AfxMessageBox(_T("Please try after scanning."));
			return;
		}
		if (m_pMergeCtrl->m_pBlock == NULL) {
			AfxMessageBox(_T("Please try after learning."));
			return;
		}

		ImgBlock* pBlock = NULL;

		long long llW=0;
		for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
		{
			pBlock = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 2, y);
			llW += (pBlock->pLearn.pInfo.pMergePos.x + pBlock->pLearn.pInfo.rCopyArea.width);
		}
		float fWtmp = (float)llW / (float)(m_pDlgCalib->m_nBlockY);
		int AvrStepW = (int)((fWtmp / (float)(m_pDlgCalib->m_nBlockX - 1))+0.5);

		pBlock = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1,0);
		int iMergeW = pBlock->pLearn.pInfo.pMergePos.x + AvrStepW;
		int iCurW;
		for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
		{
			pBlock = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1, y);
			if ((pBlock->pLearn.pInfo.rCopyArea.left + AvrStepW) < (pBlock->pLearn.pInfo.iScaleSizeW - LT_MARGIN))
			{
				iCurW = pBlock->pLearn.pInfo.pMergePos.x + AvrStepW;
			}
			else
			{
				iCurW = pBlock->pLearn.pInfo.pMergePos.x + (pBlock->pLearn.pInfo.iScaleSizeW - LT_MARGIN - pBlock->pLearn.pInfo.rCopyArea.left);
			}
			if (iCurW < iMergeW)
				iMergeW = iCurW;
		}
		for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
		{
			pBlock = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1, y);
			pBlock->pLearn.pInfo.rCopyArea.width = iMergeW - pBlock->pLearn.pInfo.pMergePos.x;
			pBlock->rBlockArea.right = pBlock->rBlockArea.left + pBlock->pLearn.pInfo.rCopyArea.width;

		}



		long long llH = 0;
		for (int x = 0; x < m_pDlgCalib->m_nBlockX ; x++)
		{
			pBlock = m_pMergeCtrl->GetBlock(x, m_pDlgCalib->m_nBlockY - 2);
			llH += (pBlock->pLearn.pInfo.pMergePos.y + pBlock->pLearn.pInfo.rCopyArea.height);
		}
		float fHtmp = (float)llH / (float)(m_pDlgCalib->m_nBlockX);
		int AvrStepH = (int)((fHtmp / (float)(m_pDlgCalib->m_nBlockY - 1)) + 0.5);

		pBlock = m_pMergeCtrl->GetBlock(0, m_pDlgCalib->m_nBlockY - 1);
		int iMergeH = pBlock->pLearn.pInfo.pMergePos.y + AvrStepH;
		int iCurH;
		for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
		{
			pBlock = m_pMergeCtrl->GetBlock(x, m_pDlgCalib->m_nBlockY - 1);
			if ((pBlock->pLearn.pInfo.rCopyArea.top + AvrStepH) < (pBlock->pLearn.pInfo.iScaleSizeH - LT_MARGIN))
			{
				iCurH = pBlock->pLearn.pInfo.pMergePos.y + AvrStepH;
			}
			else
			{
				iCurH = pBlock->pLearn.pInfo.pMergePos.y + (pBlock->pLearn.pInfo.iScaleSizeH - LT_MARGIN - pBlock->pLearn.pInfo.rCopyArea.top);
			}
			if (iCurH < iMergeH)
				iMergeH = iCurH;
		}
		for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
		{
			pBlock = m_pMergeCtrl->GetBlock(x, m_pDlgCalib->m_nBlockY - 1);
			pBlock->pLearn.pInfo.rCopyArea.height = iMergeH - pBlock->pLearn.pInfo.pMergePos.y;
			pBlock->rBlockArea.bottom = pBlock->rBlockArea.top + pBlock->pLearn.pInfo.rCopyArea.height;
		}


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
		double dSAreaW_px = (double)(pBlockMax->pLearn.pInfo.pMergePos.x + pBlockMax->pLearn.pInfo.rCopyArea.width );
		double dSAreaH_px = (double)(pBlockMax->pLearn.pInfo.pMergePos.y + pBlockMax->pLearn.pInfo.rCopyArea.height);
		double dSAreaW_mm = 0.001* dSAreaW_px * fResolution_umPpx;
		double dSAreaH_mm = 0.001* dSAreaH_px * fResolution_umPpx;

		//CString sVal;
		//sVal.Format(_T("%.2f"), dSAreaW_mm);  m_eLearnAreaW.SetWindowText(sVal);
		//sVal.Format(_T("%.2f"), dSAreaH_mm);  m_eLearnAreaH.SetWindowText(sVal);




		if (m_pMergeCtrl->MakeMergedImage(0, 0, m_pDlgCalib->m_nBlockX, m_pDlgCalib->m_nBlockY, m_pDlgCalib->m_pScanImage_org,(BOOL)m_pDlgCalib->m_bFindDarkObj) < 0)
		{
			AfxMessageBox(_T("An error occurred while merging images."));
			return;
		}

		//OnBnClickedCheckShowMerged();
		m_iDisTarget = 3;
		UpdateData(FALSE);
		OnBnClickedRadioShowMerged();


		OnBnClickedButtonZoomFit();
		AfxMessageBox(_T("Calibration and merging are complete."));

		m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(FALSE);
		m_pDlgCalib->m_bSetCenterPos = FALSE;
}

void CDlgVScanCalib_Stiching::OnDeltaposSpinBlockX(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

		CString sY;
		m_eBlockY.GetWindowText(sY);
		int iY = _ttoi(sY);

		int iX = pNMUpDown->iPos + pNMUpDown->iDelta;
		if (iX < 0) iX = 0;
		if (iX > (m_pDlgCalib->m_nBlockX - 1))iX = (m_pDlgCalib->m_nBlockX - 1);
		TRACE(_T("\n%d, %d \t\t"), iX, iY);
		;


		Point pt;
		pt.x = iX;
		pt.y = iY;
		SelectImageBlock(pt);

	*pResult = 0;
}

void CDlgVScanCalib_Stiching::OnDeltaposSpinBlockY(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMUPDOWN pNMUpDown = reinterpret_cast<LPNMUPDOWN>(pNMHDR);

		CString sX;
		m_eBlockX.GetWindowText(sX);

		int iX = _ttoi(sX);
		int iY = pNMUpDown->iPos + pNMUpDown->iDelta;
		if (iY < 0) iY = 0;
		if (iY > (m_pDlgCalib->m_nBlockY - 1))iY = (m_pDlgCalib->m_nBlockY - 1);
		TRACE(_T("\n%d, %d \t\t"), iX, iY);

		Point pt;
		pt.x = iX;
		pt.y = iY;
		SelectImageBlock(pt);

	*pResult = 0;
}

void CDlgVScanCalib_Stiching::SelectImageBlock(Point pt)
{


		ImgBlock* pBlock = m_pMergeCtrl->GetBlock(pt);
		int iNIWndID = m_pImgWnd->GetNiWndId();

		switch (m_enShowTarget)
		{
		case SHOW_IMG_NONE:
			break;
		case SHOW_IMG_ORG:
			if (m_pDlgCalib->m_pScanImage_org != NULL)
			{
				if (pt.x < 0 || pt.x >= m_pDlgCalib->m_nBlockX) return;
				if (pt.y < 0 || pt.y >= m_pDlgCalib->m_nBlockY) return;
				CString str;
				str.Format(_T("%d"), pt.x); m_eBlockX.SetWindowText(str);
				str.Format(_T("%d"), pt.y); m_eBlockY.SetWindowText(str);

				m_pSelBlock = pt;

				Image* pImg = m_pDlgCalib->m_pScanImage_org[pt.x][pt.y];
				
				imaqClearOverlay(pImg,NULL);

				CornerData pCornerOrg;
				if (CImageMerging::FindCornerInfo3(pImg, &(pBlock->gridLearn), (int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg) >= 0)
				{
					RGBValue IMAQ_RGB_RED = RGB_RED;
					if (pCornerOrg.bLT && pCornerOrg.bRT)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pLT),
							PointFloat2Point(pCornerOrg.pRT), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bRT && pCornerOrg.bRB)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pRT),
							PointFloat2Point(pCornerOrg.pRB), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bRB && pCornerOrg.bLB)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pRB),
							PointFloat2Point(pCornerOrg.pLB), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bLB && pCornerOrg.bLT)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pLB),
							PointFloat2Point(pCornerOrg.pLT), &IMAQ_RGB_RED, NULL);
				}
				RGBValue IMAQ_RGB_YELLOW = RGB_RED;
				imaqOverlayRect(pImg, m_pDlgCalib->m_rCopyArea, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
				imaqDisplayImage(pImg, m_pImgWnd->GetNiWndId(), FALSE);
				imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
				imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);






			}

			break;
		case SHOW_IMG_LEARN:
			if (pBlock != NULL && pBlock->pLearn.pImgLearn!=NULL)
			{
				CString str;
				str.Format(_T("%d"), pt.x); m_eBlockX.SetWindowText(str);
				str.Format(_T("%d"), pt.y); m_eBlockY.SetWindowText(str);

				if (m_pLearningImg != NULL) { imaqDispose(m_pLearningImg); m_pLearningImg = NULL; }
				m_pLearningImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
				imaqDuplicate(m_pLearningImg, m_pDlgCalib->m_pScanImage_org[pt.x][pt.y]);

				if (CImageMerging::ApplyCalib(pBlock->pLearn.pImgLearn, &(m_pLearningImg), pBlock->gridLearn.left.bFindDarkObj) < 0)
				{
					AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
					GetDlgItem(IDC_RADIO_DO_LEARN)->EnableWindow(FALSE);
					return;
				}


	
				imaqClearOverlay(m_pLearningImg,NULL);
				CornerData pCornerOrg;
				if (CImageMerging::FindCornerInfo3(m_pLearningImg, &(pBlock->gridScaling), (int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg) >= 0)
				{
					RGBValue IMAQ_RGB_RED = RGB_RED;
					if (pCornerOrg.bLT && pCornerOrg.bRT)
						imaqOverlayLine(m_pLearningImg, PointFloat2Point(pCornerOrg.pLT),
							PointFloat2Point(pCornerOrg.pRT), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bRT && pCornerOrg.bRB)
						imaqOverlayLine(m_pLearningImg, PointFloat2Point(pCornerOrg.pRT),
							PointFloat2Point(pCornerOrg.pRB), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bRB && pCornerOrg.bLB)
						imaqOverlayLine(m_pLearningImg, PointFloat2Point(pCornerOrg.pRB),
							PointFloat2Point(pCornerOrg.pLB), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bLB && pCornerOrg.bLT)
						imaqOverlayLine(m_pLearningImg, PointFloat2Point(pCornerOrg.pLB),
							PointFloat2Point(pCornerOrg.pLT), &IMAQ_RGB_RED, NULL);
				}
				imaqOverlayRect(m_pLearningImg, pBlock->pLearn.pInfo.rCopyArea, &IMAQ_RGB_RED, IMAQ_DRAW_VALUE, NULL);
				imaqOverlayRect(m_pLearningImg, m_pDlgCalib->m_rCopyArea, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
				imaqDisplayImage(m_pLearningImg, m_pImgWnd->GetNiWndId(), FALSE);
				//imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
				//imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);

				imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);

				//if (m_pSnapImg != NULL) { imaqDispose(m_pSnapImg); m_pSnapImg = NULL; }
				//m_pSnapImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
				//imaqDuplicate(m_pSnapImg, pImg);

			}
			break;
		case SHOW_IMG_CALIB:
			if (pBlock != NULL && pBlock->pCalibImg!=NULL)
			{
				CString str;
				str.Format(_T("%d"), pt.x); m_eBlockX.SetWindowText(str);
				str.Format(_T("%d"), pt.y); m_eBlockY.SetWindowText(str);
				Image* pImg = pBlock->pCalibImg;

				imaqClearOverlay(pImg,NULL);
				CornerData pCornerOrg;
				if (CImageMerging::FindCornerInfo3(pImg, &(pBlock->gridStitching), (int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg) >= 0)
				{
					RGBValue IMAQ_RGB_RED = RGB_RED;
					if (pCornerOrg.bLT && pCornerOrg.bRT)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pLT),
							PointFloat2Point(pCornerOrg.pRT), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bRT && pCornerOrg.bRB)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pRT),
							PointFloat2Point(pCornerOrg.pRB), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bRB && pCornerOrg.bLB)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pRB),
							PointFloat2Point(pCornerOrg.pLB), &IMAQ_RGB_RED, NULL);

					if (pCornerOrg.bLB && pCornerOrg.bLT)
						imaqOverlayLine(pImg, PointFloat2Point(pCornerOrg.pLB),
							PointFloat2Point(pCornerOrg.pLT), &IMAQ_RGB_RED, NULL);
				}
				//float fEdgeStrength = 25.0;
				//if (pBlock != NULL && pBlock->fEdgeStrength > 0.0)
				//{
				//	fEdgeStrength = pBlock->fEdgeStrength;
				//	CString str;
				//	str.Format(_T("%.1f"), fEdgeStrength);
				//	m_eStrength.SetWindowText(str);
				//}
				//else
				//{
				//	fEdgeStrength = m_fEdgeMinThreshold;
				//}
				//
				//CornerData pCornerOrg;
				//imaqClearOverlay(pImg, NULL);
				//FindCornerNDraw(pImg, &pCornerOrg, fEdgeStrength, m_iGridUnitSize_px);
				imaqOverlayRect(pImg, pBlock->pLearn.pInfo.rCopyArea, &IMAQ_RGB_RED, IMAQ_DRAW_VALUE, NULL);
				imaqOverlayRect(pImg, m_pDlgCalib->m_rCopyArea, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
				imaqDisplayImage(pImg, m_pImgWnd->GetNiWndId(), FALSE);
				//imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
				//imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
				imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
				//if (m_pSnapImg != NULL) { imaqDispose(m_pSnapImg); m_pSnapImg = NULL; }
				//m_pSnapImg = imaqCreateImage(IMAQ_IMAGE_U8, 7);
				//imaqDuplicate(m_pSnapImg, pImg);

			}
			break;
		case SHOW_IMG_MERGE:
			if (m_pMergeCtrl->m_pMergeImg != NULL)
			{
				Image* pImg = m_pMergeCtrl->m_pMergeImg;
				imaqClearOverlay(pImg, NULL);
				if (pBlock != NULL)
				{
					CString str;
					str.Format(_T("%d"), pt.x); m_eBlockX.SetWindowText(str);
					str.Format(_T("%d"), pt.y); m_eBlockY.SetWindowText(str);
					CRect cr = pBlock->rBlockArea;
					Rect r;
					r.left = cr.left;
					r.top = cr.top;
					r.width = cr.Width();
					r.height = cr.Height();

					m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
					m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;

					if (m_chkShowGridLine.GetCheck())
					{
						CornerData pCornerOrg;
						//FindCornerNDraw(m_pMergeCtrl->m_pBlock[0][0].pCalibImg, &pCornerOrg, m_fEdgeMinThreshold, m_iGridUnitSize_px);

						//CImageMerging::FindGridInfo(m_pMergeCtrl->m_pBlock[0][0].pCalibImg,
						//	m_rCopyArea,
						//	m_iGridUnitSize_px*0.93,
						//	&pCornerOrg,
						//	m_pMergeCtrl->m_pBlock[0][0].fEdgeStrength,
						//	m_bFindDarkObj);

						
						CImageMerging::FindGridInfo3(m_pMergeCtrl->m_pBlock[0][0].pCalibImg, &(m_pMergeCtrl->m_pBlock[0][0].gridStitching),
							(int)(m_pDlgCalib->m_iGridUnitSize_px*0.93), &pCornerOrg);
						//{
						//	AfxMessageBox(_T("It didn't solve the problem. Try changing the settings again"));
						//	return;
						//}


						if (pCornerOrg.bLT)
						{
							Point pStartPos;
							pStartPos.x = ((int)(pCornerOrg.pLT.x + 0.5f)) - m_pMergeCtrl->m_pBlock[0][0].pLearn.pInfo.rCopyArea.left;
							pStartPos.y = ((int)(pCornerOrg.pLT.y + 0.5f)) - m_pMergeCtrl->m_pBlock[0][0].pLearn.pInfo.rCopyArea.top;

							int iMW, iMH;
							imaqGetImageSize(pImg, &iMW, &iMH);

							//RGBValue IMAQ_RGB_VIOLET = RGB_VIOLET;

							Point ps, pe;
							ps = pe = pStartPos;
							ps.y = 0;
							pe.y = iMH;

							while (1)
							{
								imaqOverlayLine(pImg, ps, pe, &IMAQ_RGB_RED, NULL);
								ps.x = pe.x = (ps.x + m_pDlgCalib->m_iGridUnitSize_px);
								if (ps.x > iMW)
								{
									break;
								}
							}

							ps = pe = pStartPos;
							ps.x = 0;
							pe.x = iMW;
							while (1)
							{
								imaqOverlayLine(pImg, ps, pe, &IMAQ_RGB_RED, NULL);
								ps.y = pe.y = (ps.y + m_pDlgCalib->m_iGridUnitSize_px);
								if (ps.y > iMH)
								{
									break;
								}
							}
						}
					}

					if (m_chkShowCopyArea.GetCheck())
					{
						for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
						{
							for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
							{
								ImgBlock* pB = m_pMergeCtrl->GetBlock(x, y);
								if (pB != NULL)
								{
									CRect rBlock = m_pMergeCtrl->GetBlock(x, y)->rBlockArea;
									Rect rCL;
									rCL.left = rBlock.left;
									rCL.top = rBlock.top;
									rCL.width = rBlock.Width();
									rCL.height = rBlock.Height();
									if (y % 2 == 0)
									{
										if (x % 2 != 0)
										{
											imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
										}
										else
										{
											imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_PAINT_VALUE, NULL);
											//imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_HIGHLIGHT_VALUE, NULL);
										}
									}
									else
									{
										if (x % 2 == 0)
										{
											imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
										}
										else
										{
											imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_PAINT_VALUE, NULL);
											//imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_HIGHLIGHT_VALUE, NULL);
										}
									}
									//imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
									//imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_PAINT_VALUE, NULL);
									//imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_HIGHLIGHT_VALUE, NULL);
								}
							}
						}
						imaqOverlayRect(pImg, r, &IMAQ_RGB_GREEN, IMAQ_PAINT_VALUE, NULL);
					}
					else
					{
						imaqOverlayRect(pImg, r, &IMAQ_RGB_GREEN, IMAQ_DRAW_VALUE, NULL);
					}

				}

				if (m_pMergeCtrl->m_pfCenterPos.x != 0.0f && m_pMergeCtrl->m_pfCenterPos.y != 0.0f)
				{
					RGBValue IMAQ_RGB_VIOLET = RGB_VIOLET;
					Rect boundingBox;
					int iR = 10;
					boundingBox.left = (int)(m_pMergeCtrl->m_pfCenterPos.x + 0.5f) - iR;
					boundingBox.top = (int)(m_pMergeCtrl->m_pfCenterPos.y + 0.5f) - iR;
					boundingBox.width = iR + iR + 1;
					boundingBox.height = iR + iR + 1;
					imaqOverlayOval(pImg, boundingBox, &IMAQ_RGB_VIOLET, IMAQ_PAINT_VALUE, NULL);
				}
				imaqDisplayImage(pImg, m_pImgWnd->GetNiWndId(), FALSE);
				imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);

				//imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
			}
			break;
		}



}

void CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockUp()
{
	CString sX;
	CString sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);
	int iX = _ttoi(sX);
	int iY = _ttoi(sY);
	iY--;

	if (iX < 0) iX = 0;
	if (iX > (m_pDlgCalib->m_nBlockX - 1))iX = (m_pDlgCalib->m_nBlockX - 1);
	if (iY < 0) iY = 0;
	if (iY > (m_pDlgCalib->m_nBlockY - 1))iY = (m_pDlgCalib->m_nBlockY - 1);
	TRACE(_T("\n%d, %d \t\t"), iX, iY);
	sY.Format(_T("%d"), iY);
	m_eBlockY.SetWindowText(sY);

	Point pt;
	pt.x = iX;
	pt.y = iY;
	SelectImageBlock(pt);
	m_pSelBlock = pt;
}

void CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockDown()
{
	CString sX;
	CString sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);
	int iX = _ttoi(sX);
	int iY = _ttoi(sY);
	iY++;

	if (iX < 0) iX = 0;
	if (iX > (m_pDlgCalib->m_nBlockX - 1))iX = (m_pDlgCalib->m_nBlockX - 1);
	if (iY < 0) iY = 0;
	if (iY > (m_pDlgCalib->m_nBlockY - 1))iY = (m_pDlgCalib->m_nBlockY - 1);
	TRACE(_T("\n%d, %d \t\t"), iX, iY);
	sY.Format(_T("%d"), iY);
	m_eBlockY.SetWindowText(sY);

	Point pt;
	pt.x = iX;
	pt.y = iY;
	SelectImageBlock(pt);
	m_pSelBlock = pt;
}


void CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockLeft()
{
	CString sX;
	CString sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);
	int iX = _ttoi(sX);
	int iY = _ttoi(sY);
	iX--;

	if (iX < 0) iX = 0;
	if (iX > (m_pDlgCalib->m_nBlockX - 1))iX = (m_pDlgCalib->m_nBlockX - 1);
	if (iY < 0) iY = 0;
	if (iY > (m_pDlgCalib->m_nBlockY - 1))iY = (m_pDlgCalib->m_nBlockY - 1);
	TRACE(_T("\n%d, %d \t\t"), iX, iY);
	sX.Format(_T("%d"), iX);
	m_eBlockX.SetWindowText(sX);

	Point pt;
	pt.x = iX;
	pt.y = iY;
	SelectImageBlock(pt);
	m_pSelBlock = pt;
}



void CDlgVScanCalib_Stiching::OnBnClickedButtonSelBlockRight()
{
	CString sX;
	CString sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);
	int iX = _ttoi(sX);
	int iY = _ttoi(sY);
	iX++;

	if (iX < 0) iX = 0;
	if (iX > (m_pDlgCalib->m_nBlockX - 1))iX = (m_pDlgCalib->m_nBlockX - 1);
	if (iY < 0) iY = 0;
	if (iY > (m_pDlgCalib->m_nBlockY - 1))iY = (m_pDlgCalib->m_nBlockY - 1);
	TRACE(_T("\n%d, %d \t\t"), iX, iY);
	sX.Format(_T("%d"), iX);
	m_eBlockX.SetWindowText(sX);

	Point pt;
	pt.x = iX;
	pt.y = iY;
	SelectImageBlock(pt);
	m_pSelBlock = pt;
}


void CDlgVScanCalib_Stiching::ShiftBlock(Point StartBlock, int iShiftX, int iShiftY)
{

	int isx = StartBlock.x - 1;
	int isy = StartBlock.y - 1;
	if (isx < 0)isx = 0;
	if (isy < 0)isy = 0;
	ImgBlock* pBlock = m_pMergeCtrl->GetBlock(StartBlock.x, StartBlock.y);
	imaqCopyRect(m_pMergeCtrl->m_pMergeImg, m_pDlgCalib->m_pDarkImg, pBlock->pLearn.pInfo.rCopyArea,
		pBlock->pLearn.pInfo.pMergePos);

	m_pDlgCalib->m_bSetCenterPos = FALSE;

	BOOL bBreak = FALSE;
	BOOL bTarget = FALSE;
	for (int y = isy; y < m_pDlgCalib->m_nBlockY; y++)
	{
		if (bBreak)
			break;

		for (int x = isx; x < m_pDlgCalib->m_nBlockX; x++)
		{
			//TRACE(_T("\n Try copy (%d, %d) \t\t\t"), x, y);
			pBlock = m_pMergeCtrl->GetBlock(x, y);
			Image * pTmp = pBlock->pCalibImg;
			bTarget = (x >= StartBlock.x) && (y >= StartBlock.y);
			if (bTarget)
			{
				int dd = 0;
			}
			Point pTP;
			if (x == 0 && y == 0)
			{
				pTP.x = 0;
				pTP.y = 0;
				if (bTarget)
				{
					pTP.x += iShiftX;
					pTP.y += iShiftY;
				}
				TRACE(_T("\n (%d,%d)  \t\t"), pTP.x, pTP.y);
				//if (x == isx)
				//{
				//	pBlock->pLearn.pInfo.rCopyArea.width = pBlock->pLearn.pInfo.iScaleSizeW - pBlock->pLearn.pInfo.rCopyArea.left;
				//}
				//if (y == isy)
				//{
				//	pBlock->pLearn.pInfo.rCopyArea.height = pBlock->pLearn.pInfo.iScaleSizeH - pBlock->pLearn.pInfo.rCopyArea.top;
				//}

				Rect rArea = pBlock->pLearn.pInfo.rCopyArea;

				if (pTP.x < 0)
				{
					rArea.left = rArea.left - pTP.x;
					pTP.x = 0;
				}
				else
				{
					int iX = rArea.left - pTP.x;
					if (iX < LT_MARGIN)
					{
						pTP.x -= (rArea.left - LT_MARGIN);
						rArea.left = LT_MARGIN;
					}
					else
					{
						rArea.left = iX;
						pTP.x = 0;
					}
				}

				if (pTP.y < 0)
				{
					rArea.top -= pTP.y;
					pTP.y = 0;
				}
				else
				{
					int iY = rArea.top - pTP.y;
					if (iY < LT_MARGIN)
					{
						pTP.y -= (rArea.top - LT_MARGIN);
						rArea.top = LT_MARGIN;
					}
					else
					{
						rArea.top = iY;
						pTP.y = 0;
					}
				}

				TRACE(_T("\n (%d,%d)  %d, %d, %d, %d \t\t"), pTP.x, pTP.y, rArea.left, rArea.top, rArea.width, rArea.height);
				//imaqCopyRect(m_pMergeCtrl->m_pMergeImg, pTmp, pBlock->pLearn.pInfo.rCopyArea, pTP);
				imaqCopyRect(m_pMergeCtrl->m_pMergeImg, pTmp, rArea, pTP);

				pBlock->rBlockArea.left = pTP.x;
				pBlock->rBlockArea.top = pTP.y;
				pBlock->rBlockArea.right = pTP.x + rArea.width;
				pBlock->rBlockArea.bottom = pTP.y + rArea.height;
				pBlock->pLearn.pInfo.rCopyArea = rArea;
				pBlock->pLearn.pInfo.pMergePos = pTP;
				//if (x > 0) m_pMergeCtrl->m_pBlock[x - 1][y].rBlockArea.right = pTP.x;
				//if (y > 0)	m_pMergeCtrl->m_pBlock[x][y - 1].rBlockArea.bottom = pTP.y;
				//if (x == StartBlock.x&&x > 0)
				//{
				//	m_pMergeCtrl->GetBlock(x - 1, y)->pLearn.pInfo.rCopyArea.width
				//		= pTP.x - m_pMergeCtrl->GetBlock(x - 1, y)->pLearn.pInfo.pMergePos.x;
				//}
				//if (y == StartBlock.y&&y > 0)
				//{
				//	m_pMergeCtrl->GetBlock(x, y - 1)->pLearn.pInfo.rCopyArea.height
				//		= pTP.y - m_pMergeCtrl->GetBlock(x, y - 1)->pLearn.pInfo.pMergePos.y;
				//}
				//

			}
			else if (x == 0)
			{
				pTP.x = pBlock->rBlockArea.left;
				pTP.y = pBlock->rBlockArea.top;
				if (bTarget)
				{
					pTP.x += iShiftX;
					pTP.y += iShiftY;
				}
				Rect rArea = pBlock->pLearn.pInfo.rCopyArea;
				if (pTP.x < 0)
				{
					rArea.left = rArea.left - pTP.x;
					pTP.x = 0;
				}
				else
				{
					int iX = rArea.left - pTP.x;
					if (iX < LT_MARGIN)
					{
						pTP.x -= (rArea.left - LT_MARGIN);
						rArea.left = LT_MARGIN;
					}
					else
					{
						rArea.left = iX;
						pTP.x = 0;
					}
				}

				if (pTP.y < 0)
				{
					rArea.top -= pTP.y;
					pTP.y = 0;
				}
				else
				{
					int iY = rArea.top - pTP.y;
					if (iY < LT_MARGIN)
					{
						pTP.y -= (rArea.top - LT_MARGIN);
						rArea.top = LT_MARGIN;
					}
					else
					{
						rArea.top = iY;
						pTP.y = 0;
					}
				}


				if (x == isx)
				{
					rArea.width = pBlock->pLearn.pInfo.iScaleSizeW - rArea.left;
				}
				if (y == isy)
				{
					rArea.height = pBlock->pLearn.pInfo.iScaleSizeH - rArea.top;
				}

				if (imaqCopyRect(m_pMergeCtrl->m_pMergeImg, pTmp, rArea, pTP) == 0)
				{
					int iErr = imaqGetLastError();
					TRACE(_T("\n Copy error(%d, %d), code = %d \t\t\t"), x, y, iErr);
				}
				//imaqDispose(pTmp);
				pBlock->rBlockArea.left = pTP.x;
				pBlock->rBlockArea.top = pTP.y;
				pBlock->rBlockArea.right = pTP.x + rArea.width;
				pBlock->rBlockArea.bottom = pTP.y + rArea.height;
				pBlock->pLearn.pInfo.rCopyArea = rArea;
				pBlock->pLearn.pInfo.pMergePos = pTP;
				m_pMergeCtrl->m_pBlock[x][y - 1].rBlockArea.bottom = pTP.y;
				if (y == StartBlock.y)
				{
					m_pMergeCtrl->GetBlock(x, y - 1)->pLearn.pInfo.rCopyArea.height
						= pTP.y - m_pMergeCtrl->GetBlock(x, y - 1)->pLearn.pInfo.pMergePos.y;
				}
			}
			else
			{

				pTP.x = pBlock->rBlockArea.left;
				pTP.y = pBlock->rBlockArea.top;
				if (bTarget)
				{
					pTP.x += iShiftX;
					pTP.y += iShiftY;
				}

				Rect rArea = pBlock->pLearn.pInfo.rCopyArea;

				if (pTP.x < 0)
				{
					rArea.left = rArea.left - pTP.x;
					pTP.x = 0;
				}
				else
				{
					int iX = rArea.left - pTP.x;
					if (iX < LT_MARGIN)
					{
						pTP.x -= (rArea.left - LT_MARGIN);
						rArea.left = LT_MARGIN;
					}
					else
					{
						rArea.left = iX;
						pTP.x = 0;
					}
				}

				if (pTP.y < 0)
				{
					rArea.top -= pTP.y;
					pTP.y = 0;
				}
				else
				{
					int iY = rArea.top - pTP.y;
					if (iY < LT_MARGIN)
					{
						pTP.y -= (rArea.top - LT_MARGIN);
						rArea.top = LT_MARGIN;
					}
					else
					{
						rArea.top = iY;
						pTP.y = 0;
					}
				}

				if (imaqCopyRect(m_pMergeCtrl->m_pMergeImg, pTmp, rArea, pTP) == 0)
				{
					int iErr = imaqGetLastError();
					TRACE(_T("\n Copy error(%d, %d), code = %d \t\t\t"), x, y, iErr);
				}
				//imaqDispose(pTmp);
				pBlock->rBlockArea.left = pTP.x;
				pBlock->rBlockArea.top = pTP.y;
				pBlock->rBlockArea.right = pTP.x + rArea.width;
				pBlock->rBlockArea.bottom = pTP.y + rArea.height;
				pBlock->pLearn.pInfo.rCopyArea = rArea;
				pBlock->pLearn.pInfo.pMergePos = pTP;
				if (x > 0) m_pMergeCtrl->m_pBlock[x - 1][y].rBlockArea.right = pTP.x;
				if (y > 0)	m_pMergeCtrl->m_pBlock[x][y - 1].rBlockArea.bottom = pTP.y;
				if (x == StartBlock.x&&x > 0)
				{
					m_pMergeCtrl->GetBlock(x - 1, y)->pLearn.pInfo.rCopyArea.width
						= pTP.x - m_pMergeCtrl->GetBlock(x - 1, y)->pLearn.pInfo.pMergePos.x;
				}
				if (y == StartBlock.y&&y > 0)
				{
					m_pMergeCtrl->GetBlock(x, y - 1)->pLearn.pInfo.rCopyArea.height
						= pTP.y - m_pMergeCtrl->GetBlock(x, y - 1)->pLearn.pInfo.pMergePos.y;
				}
			}


			//imaqCopyRect(m_pMergeImg, pTmp, m_pMergeCtrl->m_rValidArea, m_pBlock[x][y].pMergePos);
			//imaqDispose(pTmp);
		}
	}



	int iMergeW = 0;
	int iMergeH = 0;
	int  iAvrWCnt = 0;
	int  iAvrHCnt = 0;

	int idx = 0;
	for (int y = 0; y < m_pDlgCalib->m_nBlockY - 1; y++)
	{
		iMergeH += m_pMergeCtrl->GetBlock(idx, y)->pLearn.pInfo.rCopyArea.height;
		iAvrHCnt++;
	}
	for (int x = 0; x < m_pDlgCalib->m_nBlockX - 1; x++)
	{
		iMergeW += m_pMergeCtrl->GetBlock(x, idx)->pLearn.pInfo.rCopyArea.width;
		iAvrWCnt++;
	}


	int iAvrW = iMergeW / iAvrWCnt;
	int iAvrH = iMergeH / iAvrHCnt;
	iMergeW += iAvrW;
	iMergeH += iAvrH;

	idx = m_pDlgCalib->m_nBlockX - 1;
	for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
	{
		m_pMergeCtrl->GetBlock(idx, y)->pLearn.pInfo.rCopyArea.width =
			iMergeW - m_pMergeCtrl->GetBlock(idx, y)->pLearn.pInfo.pMergePos.x;
	}
	idx = m_pDlgCalib->m_nBlockY - 1;
	for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
	{
		m_pMergeCtrl->GetBlock(x, idx)->pLearn.pInfo.rCopyArea.height =
			iMergeH - m_pMergeCtrl->GetBlock(x, idx)->pLearn.pInfo.pMergePos.y;
	}

	Image* pImg = m_pMergeCtrl->m_pMergeImg;
	imaqClearOverlay(pImg, NULL);


	if (m_chkShowGridLine.GetCheck())
	{
		CornerData pCornerOrg;
		CImageMerging::FindGridInfo3(m_pMergeCtrl->m_pBlock[0][0].pCalibImg, &(m_pMergeCtrl->m_pBlock[0][0].gridStitching),
			(int)(m_pDlgCalib->m_iGridUnitSize_px*0.93), &pCornerOrg);

		//FindCornerNDraw(m_pMergeCtrl->m_pBlock[0][0].pCalibImg, &pCornerOrg, m_fEdgeMinThreshold, m_iGridUnitSize_px);
		if (pCornerOrg.bLT)
		{
			Point pStartPos;
			pStartPos.x = ((int)(pCornerOrg.pLT.x + 0.5f)) - m_pMergeCtrl->m_pBlock[0][0].pLearn.pInfo.rCopyArea.left;
			pStartPos.y = ((int)(pCornerOrg.pLT.y + 0.5f)) - m_pMergeCtrl->m_pBlock[0][0].pLearn.pInfo.rCopyArea.top;

			int iMW, iMH;
			imaqGetImageSize(pImg, &iMW, &iMH);

			//RGBValue IMAQ_RGB_VIOLET = RGB_VIOLET;

			Point ps, pe;
			ps = pe = pStartPos;
			ps.y = 0;
			pe.y = iMH;

			while (1)
			{
				imaqOverlayLine(pImg, ps, pe, &IMAQ_RGB_RED, NULL);
				ps.x = pe.x = (ps.x + m_pDlgCalib->m_iGridUnitSize_px);
				if (ps.x > iMW)
				{
					break;
				}
			}

			ps = pe = pStartPos;
			ps.x = 0;
			pe.x = iMW;
			while (1)
			{
				imaqOverlayLine(pImg, ps, pe, &IMAQ_RGB_RED, NULL);
				ps.y = pe.y = (ps.y + m_pDlgCalib->m_iGridUnitSize_px);
				if (ps.y > iMH)
				{
					break;
				}
			}
		}
	}

	if (m_chkShowCopyArea.GetCheck())
	{
		for (int y = 0; y < m_pDlgCalib->m_nBlockY; y++)
		{
			for (int x = 0; x < m_pDlgCalib->m_nBlockX; x++)
			{
				ImgBlock* pB = m_pMergeCtrl->GetBlock(x, y);
				if (pB != NULL)
				{
					CRect rBlock = m_pMergeCtrl->GetBlock(x, y)->rBlockArea;
					Rect rCL;
					rCL.left = rBlock.left;
					rCL.top = rBlock.top;
					rCL.width = rBlock.Width();
					rCL.height = rBlock.Height();
					imaqOverlayRect(pImg, rCL, &IMAQ_RGB_YELLOW, IMAQ_DRAW_VALUE, NULL);
				}
			}
		}
	}


	CRect cr;
	cr = m_pMergeCtrl->GetBlock(StartBlock.x, StartBlock.y)->rBlockArea;
	Rect r;
	r.left = cr.left;
	r.top = cr.top;
	r.width = cr.Width();
	r.height = cr.Height();

	imaqOverlayRect(pImg, r, &IMAQ_RGB_GREEN, IMAQ_DRAW_VALUE, NULL);


	imaqDisplayImage(pImg, m_pImgWnd->GetNiWndId(), FALSE);


}


void CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveYN()
{
	if (m_pSelBlock.x < 0 || m_pSelBlock.y < 0) return;
	//if (m_pSelBlock.x == 0 && m_pSelBlock.y == 0) return;
	CString sVal;
	m_eMoveBlockUnit_px.GetWindowText(sVal);
	int iShfit = _ttoi(sVal.GetBuffer());
	ShiftBlock(m_pSelBlock, 0, -iShfit);
}


void CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveYP()
{
	if (m_pSelBlock.x < 0 || m_pSelBlock.y < 0) return;
	//if (m_pSelBlock.x == 0 && m_pSelBlock.y == 0) return;
	CString sVal;
	m_eMoveBlockUnit_px.GetWindowText(sVal);
	int iShfit = _ttoi(sVal.GetBuffer());
	ShiftBlock(m_pSelBlock, 0, iShfit);
}


void CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveXN()
{
	if (m_pSelBlock.x < 0 || m_pSelBlock.y < 0) return;
	//if (m_pSelBlock.x == 0 && m_pSelBlock.y == 0) return;
	CString sVal;
	m_eMoveBlockUnit_px.GetWindowText(sVal);
	int iShfit = _ttoi(sVal.GetBuffer());
	ShiftBlock(m_pSelBlock, -iShfit, 0);
}


void CDlgVScanCalib_Stiching::OnBnClickedButtonBmoveXP()
{
	if (m_pSelBlock.x < 0 || m_pSelBlock.y < 0) return;
	//if (m_pSelBlock.x == 0 && m_pSelBlock.y == 0) return;
	CString sVal;
	m_eMoveBlockUnit_px.GetWindowText(sVal);
	int iShfit = _ttoi(sVal.GetBuffer());
	ShiftBlock(m_pSelBlock, iShfit, 0);
}

void CDlgVScanCalib_Stiching::OnBnClickedButtonSetCenterBlock()
{
	if (m_pMergeCtrl->m_pMergeImg == NULL)
	{
		AfxMessageBox(_T("Please try after creating the merged image."));
		return;
	}
	if (!m_pDlgCalib->m_bInitGridParam) {
		AfxMessageBox(_T("Please try after setting the Grid parameter."));
		return;
	}

	BOOL bFindDarkObj = m_pDlgCalib->m_bFindDarkObj;
	CornerData pCornerOrg;
	ImgBlock* pCenterBlock = m_pMergeCtrl->GetBlock(m_pSelBlock);
	if (pCenterBlock == NULL)
	{
		AfxMessageBox(_T("Select image block and try again."));
		return;
	}
	if (CImageMerging::FindCornerInfo3(pCenterBlock->pCalibImg, &(pCenterBlock->gridLearn), (int)(m_pDlgCalib->m_iGridUnitSize_px*0.94), &pCornerOrg)< 0)
	//if (CImageMerging::FindCornerInfo(pCenterBlock->pCalibImg, m_pDlgCalib->m_iGridUnitSize_px * 0.94, &pCornerOrg, m_pDlgCalib->m_fEdgeMinThreshold, bFindDarkObj) < 0)
	{
		AfxMessageBox(_T("Grid intersection cannot be found."));
		return;
	}
	ImgBlock* pBlockFirst = m_pMergeCtrl->GetBlock(0, 0);
	ImgBlock* pBlockLast  = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1,m_pDlgCalib->m_nBlockY - 1);

	CornerData pCornerFirst;
	CImageMerging::FindGridInfo3(pBlockFirst->pCalibImg, &(pBlockFirst->gridStitching),
		(int)(m_pDlgCalib->m_iGridUnitSize_px*0.93), &pCornerFirst);

	
	CornerData pCornerLast;
	CImageMerging::FindGridInfo3(pBlockLast->pCalibImg, &(pBlockLast->gridScaling),
		(int)(m_pDlgCalib->m_iGridUnitSize_px*0.93), &pCornerLast);


	float fResolutionX_umPpx = (float)(m_pDoc->m_config.m_fCcdResol_X / m_pDlgCalib->m_fLensPower);
	float fResolutionY_umPpx = (float)(m_pDoc->m_config.m_fCcdResol_Y / m_pDlgCalib->m_fLensPower);
	if (pCornerFirst.bLT)
	{
		Point pStartPos;
		pStartPos.x = ((int)(pCornerFirst.pLT.x + 0.5f)) - pBlockFirst->pLearn.pInfo.rCopyArea.left;
		pStartPos.y = ((int)(pCornerFirst.pLT.y + 0.5f)) - pBlockFirst->pLearn.pInfo.rCopyArea.top;

		Point pEndPos;
		pEndPos.x = ((int)(pCornerLast.pRB.x + 0.5f)) - pBlockLast->pLearn.pInfo.rCopyArea.left	+pBlockLast->pLearn.pInfo.pMergePos.x;
		pEndPos.y = ((int)(pCornerLast.pRB.y + 0.5f)) - pBlockLast->pLearn.pInfo.rCopyArea.top	+pBlockLast->pLearn.pInfo.pMergePos.y;


		int iMW, iMH;
		imaqGetImageSize(m_pMergeCtrl->m_pMergeImg, &iMW, &iMH);


		int iValideLng = pEndPos.x - pStartPos.x;
		int nGridLine = (int)(((double)iValideLng / m_pDlgCalib->m_iGridUnitSize_px)+0.5);
		
		//CString sGridRealSize_mm;
		//m_eGridUnitSize_mm.GetWindowText(sGridRealSize_mm);
		float fGridRealSize_mm =  m_pDlgCalib->m_fGridUnitSize_mm;

		double dLng_mm = fGridRealSize_mm * nGridLine;
		double dLng_px = nGridLine * m_pDlgCalib->m_iGridUnitSize_px;

		TRACE(_T("\ngrid = %d  Lng=%fmm(%f), Lng=%fpx \t\t"), nGridLine, dLng_mm, fGridRealSize_mm, dLng_px);

		double dMag = m_pDoc->m_config.m_fCcdResol_X / (dLng_mm / (dLng_px / 1000.0));
		double dCurLng = ((m_pDoc->m_config.m_fCcdResol_X / m_pDlgCalib->m_fLensPower / 1000.0)*nGridLine*m_pDlgCalib->m_iGridUnitSize_px);

		fResolutionX_umPpx = (float)(m_pDoc->m_config.m_fCcdResol_X / dMag);
		fResolutionY_umPpx = (float)(m_pDoc->m_config.m_fCcdResol_Y / dMag);

		//CString sVal;
		//sVal.Format(_T("%f"), dMag);
		//m_eLensPowerX.SetWindowText(sVal);
		//m_eLensPowerY.SetWindowText(sVal);
	}






	float xRelCen = (pCornerOrg.pLT.x + pCornerOrg.pRT.x + pCornerOrg.pLB.x + pCornerOrg.pRB.x) / 4.0f;
	float yRelCen = (pCornerOrg.pLT.y + pCornerOrg.pRT.y + pCornerOrg.pLB.y + pCornerOrg.pRB.y) / 4.0f;

	PointFloat pfCenter;
	pfCenter.x = (xRelCen - pCenterBlock->pLearn.pInfo.rCopyArea.left) + pCenterBlock->pLearn.pInfo.pMergePos.x;
	pfCenter.y = (yRelCen - pCenterBlock->pLearn.pInfo.rCopyArea.top) + pCenterBlock->pLearn.pInfo.pMergePos.y;


	////float resolX_umPcnt, resolY_umPcnt;
	////m_pDoc->GetCcdResolution(&resolX_umPcnt, &resolY_umPcnt);
	//float fResolution_umPpx = 1.0;
	////if (!m_bInitGridParam) {
	////	CString sLensPower;
	////	m_eLensPower.GetWindowText(sLensPower);
	////	fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / _ttof(sLensPower);
	////}
	////else
	////{
	//fResolution_umPpx = m_pDoc->m_config.m_fCcdResol_X / m_pDlgCalib->m_fLensPower;
	////}














	m_pMergeCtrl->m_pCenterBlockIdx = m_pSelBlock;

	m_pMergeCtrl->SetMergeCenter(pfCenter, (double)fResolutionX_umPpx);// fResolution_umPpx);
	SelectImageBlock(m_pSelBlock);
	m_pDlgCalib->m_bSetCenterPos = TRUE;

	ImgBlock* pBlockMin = m_pMergeCtrl->GetBlock(0, 0);
	ImgBlock* pBlockMax = m_pMergeCtrl->GetBlock(m_pDlgCalib->m_nBlockX - 1, m_pDlgCalib->m_nBlockY - 1);
	float xMin = pBlockMin->pLearn.pInfo.pStartPos_mm.x;
	float xMax = pBlockMax->pLearn.pInfo.pEndPos_mm.x;

	float yMax = pBlockMin->pLearn.pInfo.pStartPos_mm.y;
	float yMin = pBlockMax->pLearn.pInfo.pEndPos_mm.y;

	CString sVal;
	sVal.Format(_T("%.2f"), xMin);  m_eLearnAreaSX.SetWindowText(sVal);
	sVal.Format(_T("%.2f"), yMin);  m_eLearnAreaSY.SetWindowText(sVal);
	sVal.Format(_T("%.2f"), xMax);  m_eLearnAreaEX.SetWindowText(sVal);
	sVal.Format(_T("%.2f"), yMax);  m_eLearnAreaEY.SetWindowText(sVal);

	m_pDlgCalib->GetDlgItem(IDC_RADIO_SAVE)->EnableWindow(TRUE);
}



void CDlgVScanCalib_Stiching::SelBlockArrowKey(int iXDiff, int iYDiff)
{



	CString sX;
	CString sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);
	int iX = _ttoi(sX) + iXDiff;
	int iY = _ttoi(sY) + iYDiff;


	if (iX < 0) iX = 0;
	if (iX > (m_pDlgCalib->m_nBlockX - 1))iX = (m_pDlgCalib->m_nBlockX - 1);
	if (iY < 0) iY = 0;
	if (iY > (m_pDlgCalib->m_nBlockY - 1))iY = (m_pDlgCalib->m_nBlockY - 1);

	TRACE(_T("\n%d, %d \t\t"), iX, iY);
	sX.Format(_T("%d"), iX);
	m_eBlockX.SetWindowText(sX);
	sY.Format(_T("%d"), iY);
	m_eBlockY.SetWindowText(sY);



	Point pt;
	pt.x = iX;
	pt.y = iY;
	SelectImageBlock(pt);
	m_pSelBlock = pt;
}

BOOL CDlgVScanCalib_Stiching::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN) {
		BOOL bUsed = FALSE;
		switch (pMsg->wParam) {
		case VK_UP		: SelBlockArrowKey(0, -1);	bUsed = TRUE; break;
		case VK_DOWN	: SelBlockArrowKey(0, 1);	bUsed = TRUE; break;
		case VK_LEFT	: SelBlockArrowKey(-1, 0);	bUsed = TRUE; break;
		case VK_RIGHT	: SelBlockArrowKey(1, 0);	bUsed = TRUE; break;
		}
		if (bUsed) return TRUE;
	}

	return CDialogEx::PreTranslateMessage(pMsg);
}


LRESULT CDlgVScanCalib_Stiching::OnWorkImgEvent(WPARAM wParam, LPARAM lParam) //DHChoi 20200623
{

	int iNiWnd = (int)wParam;
	CWorkImg::NI_EVENT pEvt = (CWorkImg::NI_EVENT) lParam;

	if (iNiWnd != m_pImgWnd->GetNiWndId()) return 1;

	switch (pEvt)
	{
	case CWorkImg::WORKIMG_EVENT_LBUTTON_DOWN:
	{
		Tool CurTool = IMAQ_NO_TOOL;
		imaqGetCurrentTool(&CurTool);
		if (CurTool == IMAQ_POINT_TOOL)
		{
			Point pt;
			int iWndID = m_pImgWnd->GetNiWndId();
			if (imaqGetMousePos(&pt, &iWndID) == 0)
			{
				return 0;
			}
			if (m_enShowTarget != SHOW_IMG_MERGE)
			{
				return 0;
			}
			if (m_pMergeCtrl->m_pBlock == NULL) return 0;

			m_pSelBlock = m_pMergeCtrl->PtInAreaIdx(pt);
			TRACE(_T("\nL-Down : %d,%d ->Block(%d,%d) \t\t"), pt.x, pt.y, m_pSelBlock.x, m_pSelBlock.y);
			SelectImageBlock(m_pSelBlock);
		}
		else if (CurTool == IMAQ_LINE_TOOL)
		{

			int iWndID = m_pImgWnd->GetNiWndId();
			m_bMouseDown = TRUE;
			imaqGetMousePos(&m_pMDN, &iWndID);
		}

	}
	break;
	case CWorkImg::WORKIMG_EVENT_LBUTTON_UP:
	{


		Tool CurTool = IMAQ_NO_TOOL;
		imaqGetCurrentTool(&CurTool);
		if (CurTool == IMAQ_LINE_TOOL && m_bMouseDown == TRUE)
		{
			int iWndID = m_pImgWnd->GetNiWndId();
			m_bMouseDown = TRUE;
			imaqGetMousePos(&m_pMUP, &iWndID);

			m_bMouseDown = FALSE;
			imaqSetCurrentTool(IMAQ_NO_TOOL);
			imaqSetWindowROI(iWndID, NULL);

			int iMx = m_pMUP.x - m_pMDN.x;
			int iMy = m_pMUP.y - m_pMDN.y;
			m_pMergeCtrl->AdjustGridPos(iMx, iMy);

			imaqDisplayImage(m_pMergeCtrl->m_pMergeImg, m_pImgWnd->GetNiWndId(), FALSE);

		}
	}
	break;
	case CWorkImg::WORKIMG_EVENT_MOUSE_MOVE:   break;

	case CWorkImg::WORKIMG_EVENT_KB_VK_UP		: SelBlockArrowKey(0, -1);	break;
	case CWorkImg::WORKIMG_EVENT_KB_VK_DOWN		: SelBlockArrowKey(0, 1);	break;
	case CWorkImg::WORKIMG_EVENT_KB_VK_LEFT		: SelBlockArrowKey(-1, 0);	break;
	case CWorkImg::WORKIMG_EVENT_KB_VK_RIGHT	: SelBlockArrowKey(1, 0);	break;

	}

	return 0;
}


void CDlgVScanCalib_Stiching::OnBnClickedButtonZoomP()
{
	UpdateData(TRUE);
	if (m_iDisTarget==3)
	{
		m_fZoomScaleMerge += m_fZoomScaleUnit;
	
		if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_pDlgCalib->m_nBlockX &&
			m_pSelBlock.y >= 0 && m_pSelBlock.y < m_pDlgCalib->m_nBlockY)
		{
			CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
			m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
			m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
		}
		else
		{
			imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
		}
	
		imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	}
	else
	{
	m_fZoomScale += m_fZoomScaleUnit;

	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	}
}


void CDlgVScanCalib_Stiching::OnBnClickedButtonZoomFit()
{
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	UpdateData(TRUE);
	if (m_iDisTarget == 3)
	{
		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
		imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScaleMerge, &m_fZoomScaleMerge);
	}
	else
	{

		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
		imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	}



}


void CDlgVScanCalib_Stiching::OnBnClickedButtonZoomN()
{
	UpdateData(TRUE);
	if (m_iDisTarget == 3)
	{
	
		if ((m_fZoomScaleMerge - m_fZoomScaleUnit) < 0.01)
		{
		}
		else
		{
			m_fZoomScaleMerge -= m_fZoomScaleUnit;
		}
	
	
		if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_pDlgCalib->m_nBlockX &&
			m_pSelBlock.y >= 0 && m_pSelBlock.y < m_pDlgCalib->m_nBlockY)
		{
			CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
			m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
			m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
		}
		else
		{
			imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
		}
	
		imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	}
	else
	{

		if ((m_fZoomScale - m_fZoomScaleUnit) < 0.01)
		{
		}
		else
		{
			m_fZoomScale -= m_fZoomScaleUnit;
		}
		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
		imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	}
}




void CDlgVScanCalib_Stiching::OnBnClickedRadioShowOrg()
{
	if (m_pDlgCalib->m_pScanImage_org == NULL)
	{
		ShowBtnUICheck(m_enShowTarget);
		return;
	}
	m_enShowTarget = SHOW_IMG_ORG;
	ShowBtnUICheck(m_enShowTarget);

	imaqSetCurrentTool(IMAQ_NO_TOOL);

	CString sX, sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);

	Point pt;
	pt.x = _ttoi(sX);
	pt.y = _ttoi(sY);

	TRACE(_T("\n%d, %d \t\t"), pt.x, pt.y);
	SelectImageBlock(pt);
}


void CDlgVScanCalib_Stiching::OnBnClickedRadioShowLearn()
{
	UpdateData(TRUE);
	if (m_pMergeCtrl->m_pBlock == NULL)
	{
		ShowBtnUICheck(m_enShowTarget);
		return;
	}
	

	m_enShowTarget = SHOW_IMG_LEARN;
	ShowBtnUICheck(m_enShowTarget);

	imaqSetCurrentTool(IMAQ_NO_TOOL);

	CString sX, sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);

	Point pt;
	pt.x = _ttoi(sX);
	pt.y = _ttoi(sY);

	TRACE(_T("\n%d, %d \t\t"), pt.x, pt.y);
	SelectImageBlock(pt);
}


void CDlgVScanCalib_Stiching::OnBnClickedRadioShowCalib()
{
	UpdateData(TRUE);
	CString sX, sY;
	m_eBlockX.GetWindowText(sX);
	m_eBlockY.GetWindowText(sY);

	Point pt;
	pt.x = _ttoi(sX);
	pt.y = _ttoi(sY);


	if (m_pMergeCtrl->m_pBlock[pt.x][pt.y].pCalibImg == NULL)
	{
		ShowBtnUICheck(m_enShowTarget);
		return;
	}
	m_enShowTarget = SHOW_IMG_CALIB;
	ShowBtnUICheck(m_enShowTarget);

	imaqSetCurrentTool(IMAQ_NO_TOOL);



	TRACE(_T("\n%d, %d \t\t"), pt.x, pt.y);
	SelectImageBlock(pt);
}


void CDlgVScanCalib_Stiching::OnBnClickedRadioShowMerged()
{
	UpdateData(TRUE);
	if (m_pMergeCtrl->m_pMergeImg == NULL)
	{
		ShowBtnUICheck(m_enShowTarget);
		return;
	}
	m_enShowTarget = SHOW_IMG_MERGE;
	ShowBtnUICheck(m_enShowTarget);

	imaqSetCurrentTool(IMAQ_POINT_TOOL);

	TRACE(_T("\n%d, %d \t\t"), m_pSelBlock.x, m_pSelBlock.y);
	SelectImageBlock(m_pSelBlock);
}

void CDlgVScanCalib_Stiching::OnBnClickedCheckShowGridLine()
{
	SelectImageBlock(m_pSelBlock);
}


void CDlgVScanCalib_Stiching::OnBnClickedCheckShowCopyArea()
{
	SelectImageBlock(m_pSelBlock);
}




void CDlgVScanCalib_Stiching::ShowBtnUICheck(ShowTarget pTarget)
{
	

	if (pTarget == SHOW_IMG_MERGE)
	{
		GetDlgItem(IDC_EDIT_BLOCK_MOVE_UNIT)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BMOVE_X_N)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BMOVE_X_P)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BMOVE_Y_N)->EnableWindow(TRUE);
		GetDlgItem(IDC_BUTTON_BMOVE_Y_P)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SHOW_GRID_LINE)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SHOW_COPY_AREA)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_BLOCK_MOVE_UNIT)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BMOVE_X_N)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BMOVE_X_P)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BMOVE_Y_N)->EnableWindow(FALSE);
		GetDlgItem(IDC_BUTTON_BMOVE_Y_P)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SHOW_GRID_LINE)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SHOW_COPY_AREA)->EnableWindow(FALSE);
	}


}

