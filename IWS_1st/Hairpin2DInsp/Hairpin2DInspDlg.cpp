
// Hairpin2DInspDlg.cpp: 구현 파일
//

#include "pch.h"
#include "framework.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "afxdialogex.h"

#include "DlgMakeUnrolled.h"
#include "DlgROI.h"
#include "DlgRemoveNoise.h"
#include "DlgInterpolation.h"
#include "DlgDetectBlob.h"
#include "DlgCriteria.h"
#include "DetectionAlgorithm.h"
#include "DlgBatchInspection.h"
#include "DlgHairpin.h"
#include "DlgDataSave.h"
#include "DlgRecipeManage.h"
#include "zip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// CHairpin2DInspDlg 대화 상자

CHairpin2DInspDlg::CHairpin2DInspDlg(CDetectionAlgorithm* pDetectionAlgorithm, CWnd* pParent /*=nullptr*/)
	: CDialog(IDD_HAIRPIN2DINSP_DIALOG, pParent)
	, m_pImageDisplay(nullptr)
	, m_pDlgBatchInspection(nullptr)
	, m_pDlgMakeUnrolled(nullptr)
	, m_pDlgROI(nullptr)
	, m_pDlgRemoveNoise(nullptr)
	, m_pDlgInterpolation(nullptr)
	, m_pDlgDetectBlob(nullptr)
	, m_pDlgCriteria(nullptr)
	, m_pDetectionAlgorithm(nullptr)
	, m_pDlgHairpin(nullptr)
	, m_pbyImage(nullptr)
	, m_nOriginalImageWidth(8553)
	, m_nOriginalImageHeight(8494)
	, m_nOriginalImageSize(m_nOriginalImageWidth * m_nOriginalImageHeight)
	, m_nUnrolledImageWidth(UNROLLED_IMAGE_WIDTH)
	, m_nUnrolledImageHeight(UNROLLED_IMAGE_HEIGHT)
	, m_nUnrolledImageSize(m_nUnrolledImageWidth * m_nUnrolledImageHeight)
	, m_bDetectionAlgorithminDlg(FALSE)
	, m_bResult(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	if(pDetectionAlgorithm != nullptr)
	{
		m_pDetectionAlgorithm = pDetectionAlgorithm;
	}
}

CHairpin2DInspDlg::~CHairpin2DInspDlg()
{
	delete m_pDlgBatchInspection;
	delete m_pImageDisplay;
	delete m_pDlgMakeUnrolled;
	delete m_pDlgROI;
	delete m_pDlgRemoveNoise;
	delete m_pDlgInterpolation;
	delete m_pDlgDetectBlob;
	delete m_pDlgCriteria;
	delete m_pDlgHairpin;

	if(m_pDetectionAlgorithm != nullptr && m_bDetectionAlgorithminDlg == TRUE)
	{
		delete m_pDetectionAlgorithm;
		m_pDetectionAlgorithm = nullptr;
	}
}

void CHairpin2DInspDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB_DETECTION, m_tabDetection);
	DDX_Control(pDX, IDC_STATIC_RESULT, m_lblResult);
}

BEGIN_MESSAGE_MAP(CHairpin2DInspDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB_DETECTION, &CHairpin2DInspDlg::OnTcnSelchangeTabDetection)
	ON_COMMAND(ID_FILE_RECIPEOPEN, &CHairpin2DInspDlg::OnFileRecipeopen)
	ON_COMMAND(ID_FILE_RECIPESAVEAS, &CHairpin2DInspDlg::OnFileRecipesaveas)
	ON_COMMAND(ID_IMAGE_IMAGEOPEN, &CHairpin2DInspDlg::OnImageImageopen)
	ON_COMMAND(ID_INSPECTION_INSPECTION, &CHairpin2DInspDlg::OnInspectionInspection)
	ON_COMMAND(ID_INSPECTION_BATCHINSPECTION, &CHairpin2DInspDlg::OnInspectionBatchinspection)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CHairpin2DInspDlg::OnBnClickedBtnInspection)
	ON_COMMAND(ID_FILE_DATASAVE, &CHairpin2DInspDlg::OnFileDatasave)
	ON_COMMAND(ID_FILE_ALLSAVEFORDEBUGGING, &CHairpin2DInspDlg::OnFileAllsavefordebugging)
	ON_COMMAND(ID_FILE_RECIPEMANAGE, &CHairpin2DInspDlg::OnFileRecipemanage)
END_MESSAGE_MAP()


// CHairpin2DInspDlg 메시지 처리기

BOOL CHairpin2DInspDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	//HUBSDK::ThreadedProgress progressX(_T("HUBIS Hairpin 2D Inspection System is starting...."));

	// Detection Algorithm
	if(m_pDetectionAlgorithm == nullptr)
	{
		m_pDetectionAlgorithm = new CDetectionAlgorithm;
		m_pDetectionAlgorithm->SetInspInfo(m_nOriginalImageWidth, m_nOriginalImageHeight);
		m_bDetectionAlgorithminDlg = TRUE;
	}

	// 2D Viewer
	m_pImageDisplay = new CImageDisplay;
	m_pImageDisplay->Initialize(IDC_STATIC_2D_VIEWER, this, RGB(50, 50, 50));
	m_pImageDisplay->SetCrossLineColor(RGB(255, 0, 0));
	m_pImageDisplay->SetCustomLineColor(RGB(255, 0, 0));

	// Tab
	m_tabDetection.InsertItem(0, _T("Make Unrolled"));
	m_tabDetection.InsertItem(1, _T("ROI"));
	m_tabDetection.InsertItem(2, _T("Remove Noise"));
	m_tabDetection.InsertItem(3, _T("Interpolation"));
	m_tabDetection.InsertItem(4, _T("Detect Blob"));
	m_tabDetection.InsertItem(5, _T("Hairpin Center"));
	m_tabDetection.InsertItem(6, _T("Criteria"));
	m_tabDetection.SetCurSel(0);

	CRect rtTabDetection;
	GetDlgItem(IDC_TAB_DETECTION)->GetWindowRect(rtTabDetection);
	ScreenToClient(rtTabDetection);

	// Batch Inspection 생성
	m_pDlgBatchInspection = new CDlgBatchInspection(this);
	m_pDlgBatchInspection->Create(CDlgBatchInspection::IDD, this);
	m_pDlgBatchInspection->ShowWindow(SW_HIDE);

	// Make Unrolled Image Dialog
	m_pDlgMakeUnrolled = new CDlgMakeUnrolled(this);
	m_pDlgMakeUnrolled->Create(CDlgMakeUnrolled::IDD, this);
	m_pDlgMakeUnrolled->SetParent(this);
	m_pDlgMakeUnrolled->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgMakeUnrolled->ShowWindow(SW_SHOW);

	// Remove Noise Dialog
	m_pDlgRemoveNoise = new CDlgRemoveNoise(this);
	m_pDlgRemoveNoise->Create(CDlgRemoveNoise::IDD, this);
	m_pDlgRemoveNoise->SetParent(this);
	m_pDlgRemoveNoise->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgRemoveNoise->ShowWindow(SW_HIDE);

	// InterpolationHairpin Dialog
	m_pDlgInterpolation = new CDlgInterpolation(this);
	m_pDlgInterpolation->Create(CDlgInterpolation::IDD, this);
	m_pDlgInterpolation->SetParent(this);
	m_pDlgInterpolation->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgInterpolation->ShowWindow(SW_HIDE);

	// ROI Dialog
	m_pDlgROI = new CDlgROI(this);
	m_pDlgROI->Create(CDlgROI::IDD, this);
	m_pDlgROI->SetParent(this);
	m_pDlgROI->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgROI->ShowWindow(SW_HIDE);

	// DetectBlob Dialog
	m_pDlgDetectBlob = new CDlgDetectBlob(this);
	m_pDlgDetectBlob->Create(CDlgDetectBlob::IDD, this);
	m_pDlgDetectBlob->SetParent(this);
	m_pDlgDetectBlob->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgDetectBlob->ShowWindow(SW_HIDE);

	// Hairpin Dialog
	m_pDlgHairpin = new CDlgHairpin(this);
	m_pDlgHairpin->Create(CDlgHairpin::IDD, this);
	m_pDlgHairpin->SetParent(this);
	m_pDlgHairpin->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgHairpin->ShowWindow(SW_HIDE);

	// Criteria Dialog
	m_pDlgCriteria = new CDlgCriteria(this);
	m_pDlgCriteria->Create(CDlgCriteria::IDD, this);
	m_pDlgCriteria->SetParent(this);
	m_pDlgCriteria->SetWindowPos(nullptr, rtTabDetection.left, rtTabDetection.bottom, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_pDlgCriteria->ShowWindow(SW_HIDE);

	// Recipe open...
	m_strRecipeFileName = _T("");
	if(m_pDetectionAlgorithm->m_strCurRecipeFilePath != _T("No Recipe"))
	{
		LoadRecipe(m_pDetectionAlgorithm->m_strCurRecipeFilePath);
		m_strRecipeFileName = REMOVE_FOLDER_FROM_PATHNAME(m_pDetectionAlgorithm->m_strCurRecipeFilePath);
	}

	// Image...
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == TRUE)
	{
		m_pbyImage = ::g_MemoryManager.GetByteImage();
		m_pImageDisplay->SetImage(m_pbyImage, m_pDetectionAlgorithm->m_nOriginalImageWidth, m_pDetectionAlgorithm->m_nOriginalImageHeight);

		m_nOriginalImageWidth = m_pDetectionAlgorithm->m_nOriginalImageWidth;
		m_nOriginalImageHeight = m_pDetectionAlgorithm->m_nOriginalImageHeight;
		m_nOriginalImageSize = m_nOriginalImageWidth * m_nOriginalImageHeight;
	}

	// 임시..
	//LoadRecipe(_T("C:\\Work\\VisionScan2DHairpin\\Recipe\\76 Jig_B.recipe"));
	//ImageOpen2D(_T("C:\\Work\\VisionScan2DHairpin\\Image\\1111.bmp"));

	SetResult(_T(""));

	return TRUE;  // 포커스를 컨트롤에 설정하지 않으면 TRUE를 반환합니다.
}

BOOL CHairpin2DInspDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg->message == WM_KEYDOWN)
	{
		switch(pMsg->wParam)
		{
		case VK_RETURN:
		case VK_ESCAPE:
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void CHairpin2DInspDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	CDialog::OnSysCommand(nID, lParam);
}

// 대화 상자에 최소화 단추를 추가할 경우 아이콘을 그리려면
//  아래 코드가 필요합니다.  문서/뷰 모델을 사용하는 MFC 응용 프로그램의 경우에는
//  프레임워크에서 이 작업을 자동으로 수행합니다.

void CHairpin2DInspDlg::OnPaint()
{
	if(IsIconic())
	{
		CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 클라이언트 사각형에서 아이콘을 가운데에 맞춥니다.
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 아이콘을 그립니다.
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// 사용자가 최소화된 창을 끄는 동안에 커서가 표시되도록 시스템에서
//  이 함수를 호출합니다.
HCURSOR CHairpin2DInspDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CHairpin2DInspDlg::ImageOpen2D(const CString strPath)
{
	m_pImageDisplay->Overlay_RemoveAll();

	long width(0), height(0), bpp(0);

	BYTE* pbyBuffer = nullptr;
	if(!HUBSDK::ImageInterface::ImageOpenBMP(strPath, pbyBuffer, m_nOriginalImageWidth * m_nOriginalImageHeight, &width, &height, &bpp))
	{
		return FALSE;
	}

	if(m_nOriginalImageWidth != width || m_nOriginalImageHeight != height)
	{
		m_nOriginalImageWidth = width;
		m_nOriginalImageHeight = height;
		m_nOriginalImageSize = width * height;

		m_pDetectionAlgorithm->SetInspInfo(m_nOriginalImageWidth, m_nOriginalImageHeight);
	}

	m_pbyImage = ::g_MemoryManager.GetByteImage();

	// 불러온 image를 모든 buffer에 복사
	memcpy(m_pbyImage, pbyBuffer, m_nOriginalImageSize);

	m_pImageDisplay->SetImage(m_pbyImage, m_nOriginalImageWidth, m_nOriginalImageHeight, m_nOriginalImageWidth);

	HUBIPHelper::FreeMemory(pbyBuffer);

	m_pDetectionAlgorithm->m_bLoadOriginalImage = TRUE;

	return TRUE;
}

void CHairpin2DInspDlg::OnTcnSelchangeTabDetection(NMHDR *pNMHDR, LRESULT *pResult)
{
	const long cursel = m_tabDetection.GetCurSel();

	switch(cursel)
	{
	case 0:
		m_pDlgMakeUnrolled->ShowWindow(SW_SHOW);
		m_pDlgROI->ShowWindow(SW_HIDE);
		m_pDlgRemoveNoise->ShowWindow(SW_HIDE);
		m_pDlgInterpolation->ShowWindow(SW_HIDE);
		m_pDlgDetectBlob->ShowWindow(SW_HIDE);
		m_pDlgHairpin->ShowWindow(SW_HIDE);
		m_pDlgCriteria->ShowWindow(SW_HIDE);
		break;
	case 1:
		m_pDlgMakeUnrolled->ShowWindow(SW_HIDE);
		m_pDlgROI->ShowWindow(SW_SHOW);
		m_pDlgRemoveNoise->ShowWindow(SW_HIDE);
		m_pDlgInterpolation->ShowWindow(SW_HIDE);
		m_pDlgDetectBlob->ShowWindow(SW_HIDE);
		m_pDlgHairpin->ShowWindow(SW_HIDE);
		m_pDlgCriteria->ShowWindow(SW_HIDE);
		break;
	case 2:
		m_pDlgMakeUnrolled->ShowWindow(SW_HIDE);
		m_pDlgROI->ShowWindow(SW_HIDE);
		m_pDlgRemoveNoise->ShowWindow(SW_SHOW);
		m_pDlgInterpolation->ShowWindow(SW_HIDE);
		m_pDlgDetectBlob->ShowWindow(SW_HIDE);
		m_pDlgHairpin->ShowWindow(SW_HIDE);
		m_pDlgCriteria->ShowWindow(SW_HIDE);
		break;
	case 3:
		m_pDlgMakeUnrolled->ShowWindow(SW_HIDE);
		m_pDlgROI->ShowWindow(SW_HIDE);
		m_pDlgRemoveNoise->ShowWindow(SW_HIDE);
		m_pDlgInterpolation->ShowWindow(SW_SHOW);
		m_pDlgDetectBlob->ShowWindow(SW_HIDE);
		m_pDlgHairpin->ShowWindow(SW_HIDE);
		m_pDlgCriteria->ShowWindow(SW_HIDE);
		break;
	case 4:
		m_pDlgMakeUnrolled->ShowWindow(SW_HIDE);
		m_pDlgROI->ShowWindow(SW_HIDE);
		m_pDlgRemoveNoise->ShowWindow(SW_HIDE);
		m_pDlgInterpolation->ShowWindow(SW_HIDE);
		m_pDlgDetectBlob->ShowWindow(SW_SHOW);
		m_pDlgHairpin->ShowWindow(SW_HIDE);
		m_pDlgCriteria->ShowWindow(SW_HIDE);
		break;
	case 5:
		m_pDlgMakeUnrolled->ShowWindow(SW_HIDE);
		m_pDlgROI->ShowWindow(SW_HIDE);
		m_pDlgRemoveNoise->ShowWindow(SW_HIDE);
		m_pDlgInterpolation->ShowWindow(SW_HIDE);
		m_pDlgDetectBlob->ShowWindow(SW_HIDE);
		m_pDlgHairpin->ShowWindow(SW_SHOW);
		m_pDlgCriteria->ShowWindow(SW_HIDE);
		break;
	case 6:
		m_pDlgMakeUnrolled->ShowWindow(SW_HIDE);
		m_pDlgROI->ShowWindow(SW_HIDE);
		m_pDlgRemoveNoise->ShowWindow(SW_HIDE);
		m_pDlgInterpolation->ShowWindow(SW_HIDE);
		m_pDlgDetectBlob->ShowWindow(SW_HIDE);
		m_pDlgHairpin->ShowWindow(SW_HIDE);
		m_pDlgCriteria->ShowWindow(SW_SHOW);
		break;
	}

	*pResult = 0;
}


void CHairpin2DInspDlg::OnFileRecipeopen()
{
	BOOL bOk(FALSE);

	CFileDialog Dlg(TRUE, _T("recipe"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Recipe File (*.recipe)|*.recipe||"), NULL, NULL, FALSE);

	if(Dlg.DoModal() == IDOK)
	{
		bOk = TRUE;
	}

	if(bOk == TRUE)
	{
		CString strPath = Dlg.GetPathName();
		m_strRecipeFileName = Dlg.GetFileName();

		bOk = LoadRecipe(strPath);

		if(bOk == FALSE)
		{
			return;
		}
	}
}


void CHairpin2DInspDlg::OnFileRecipesaveas()
{
	BOOL bOk(FALSE);

	CFileDialog Dlg(FALSE, _T("recipe"), m_strRecipeFileName, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Recipe File (*.recipe)|*.recipe||"), NULL, NULL, FALSE);

	if(Dlg.DoModal() == IDOK)
	{
		bOk = TRUE;
	}

	if(bOk == TRUE)
	{
		CString strPath = Dlg.GetPathName();

		bOk = SaveRecipe(strPath);
		bOk = LoadRecipe(strPath);

		if(bOk == TRUE)
		{
			::AfxMessageBox(_T("Save Complete!"), MB_ICONINFORMATION);
		}
		else
		{
			::AfxMessageBox(_T("Save Failed!"), MB_ICONERROR);
		}
	}
}

void CHairpin2DInspDlg::OnFileRecipemanage()
{
	CDlgRecipeManage DlgRecipeManage;

	if(DlgRecipeManage.DoModal() == IDOK)
	{
	}
}


void CHairpin2DInspDlg::OnFileDatasave()
{
	auto &params = ::g_RecipeManager.m_sPublicParams;

	CDlgDataSave DlgDataSave(
		params.bSaveRawImage,
		params.strRawImageSavePath,
		params.bSaveResultData,
		params.strResultDataSavePath,
		params.bSaveResultImage,
		params.strResultImageSavePath,
		params.bSaveResultImageForJigA,
		params.strResultImageSavePathForJigA,
		params.bSaveResultImageForJigB,
		params.strResultImageSavePathForJigB);

	if(DlgDataSave.DoModal() == IDOK)
	{
		params.bSaveRawImage = DlgDataSave.m_bSaveRawImage;
		params.strRawImageSavePath = DlgDataSave.m_strRawImageSavePath;
		params.bSaveResultData = DlgDataSave.m_bSaveResultData;
		params.strResultDataSavePath = DlgDataSave.m_strResultDataSavePath;
		params.bSaveResultImage = DlgDataSave.m_bSaveResultImage;
		params.strResultImageSavePath = DlgDataSave.m_strResultImageSavePath;
		params.bSaveResultImageForJigA = DlgDataSave.m_bSaveResultImageForJigA;
		params.strResultImageSavePathForJigA = DlgDataSave.m_strResultImageSavePathForJigA;
		params.bSaveResultImageForJigB = DlgDataSave.m_bSaveResultImageForJigB;
		params.strResultImageSavePathForJigB = DlgDataSave.m_strResultImageSavePathForJigB;
	}
}

void CHairpin2DInspDlg::OnFileAllsavefordebugging()
{
	if(m_pDetectionAlgorithm->m_strCurRecipeFilePath == _T("No Recipe"))
	{
		::AfxMessageBox(_T("No Recipe"), MB_ICONERROR);
		return;
	}

	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		::AfxMessageBox(_T("No Raw Data"), MB_ICONERROR);
		return;
	}

	CString strFilter;
	strFilter = _T("2D File Save (*.zip)|*.zip||");
	CString strTitle = _T("Save All file");

	CString strDefaultFilename = _T("");
	CFileDialog dlgFile(
		FALSE,														//BOOL bOpenFileDialog,
		_T("*.zip"),												//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	dlgFile.m_ofn.lpstrTitle = strTitle;
	if(dlgFile.DoModal() != IDOK)
	{
		return;
	}

	CString sZipFilePathName = dlgFile.GetPathName();

	long ret = FileAllsavefordebugging(sZipFilePathName);

	switch(ret)
	{
	case 0:
		::AfxMessageBox(_T("Success"), MB_ICONINFORMATION);
		break;
	case 1:
		::AfxMessageBox(_T("Save Failed!(Zip Create Failed)"), MB_ICONERROR);
		break;
	case 2:
		::AfxMessageBox(_T("Save Failed!(File Zip Failed)"), MB_ICONERROR);
		break;
	case 3:
		::AfxMessageBox(_T("Save Failed!(Close Zip Failed)"), MB_ICONERROR);
		break;
	case 4:
		::AfxMessageBox(_T("비정상적인 압축 종료"), MB_ICONERROR);
		break;
	}
}

void CHairpin2DInspDlg::UpdateUI()
{
	m_pDlgMakeUnrolled->UpdateUI();
	m_pDlgROI->UpdateUI();
	m_pDlgRemoveNoise->UpdateUI();
	m_pDlgInterpolation->UpdateUI();
	m_pDlgDetectBlob->UpdateUI();
	m_pDlgHairpin->UpdateUI();
	m_pDlgCriteria->UpdateUI();
}

BOOL CHairpin2DInspDlg::LoadRecipe(const LPCTSTR szPathName)
{
	::g_RecipeManager.InitParameters();
	::g_RecipeManager.LoadRecipeParameters(szPathName);

	UpdateUI();

	m_pDetectionAlgorithm->m_strCurRecipeFilePath = szPathName;

	CString strTitle;
	strTitle.Format(_T("HUBIS Hairpin 2D Inspection System - %s"), REMOVE_FOLDER_FROM_PATHNAME(m_pDetectionAlgorithm->m_strCurRecipeFilePath));
	SetWindowText(strTitle);

	return TRUE;
}

BOOL CHairpin2DInspDlg::SaveRecipe(const LPCTSTR szPathName)
{
	m_pDlgMakeUnrolled->UpdateParameters();
	m_pDlgROI->UpdateParameters();
	m_pDlgRemoveNoise->UpdateParameters();
	m_pDlgInterpolation->UpdateParameters();
	m_pDlgDetectBlob->UpdateParameters();
	m_pDlgCriteria->UpdateParameters();

	::g_RecipeManager.SaveRecipeParameters(szPathName);

	return TRUE;
}

void CHairpin2DInspDlg::ShowOriginal(const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;
	auto imageWidth = m_pDetectionAlgorithm->m_nSubOriginalImageWidth;
	auto imageHeight = m_pDetectionAlgorithm->m_nSubOriginalImageHeight;

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyImage, imageWidth, imageHeight, imageWidth);

	if(bShowOverlay == TRUE)
	{
		auto startPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayStartPoint;
		auto endPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayEndPoint;
		auto slotNo1Point = m_pDetectionAlgorithm->m_vecptSlotNo1OverlayPoint;
		auto slotNo2Point = m_pDetectionAlgorithm->m_vecptSlotNo2OverlayPoint;

		if(VECTOR_SIZE(startPoint) <= 0 || VECTOR_SIZE(endPoint) <= 0 || VECTOR_SIZE(slotNo1Point) <= 0 || VECTOR_SIZE(slotNo2Point) <= 0)
		{
			return;
		}

		for(long idx = 0; idx < (long)startPoint.size(); idx++)
		{
			long index = params.nSlotCount - params.nStartSlotIndex + 1 + idx + 1; // ehjang. start slot index
			if(index > params.nSlotCount)
			{
				index -= params.nSlotCount;
			}

			strTemp.Format(_T("%d"), index);

			m_pImageDisplay->Overlay_AddLine(startPoint[idx], endPoint[idx], RGB(255, 255, 0));
			m_pImageDisplay->Overlay_AddText(slotNo1Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
			m_pImageDisplay->Overlay_AddText(slotNo2Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowMakeUnrolled(const BOOL bRemoveOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyUnrolledImage, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
}

void CHairpin2DInspDlg::ShowROI(const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyUnrolledImage, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);

	if(bShowOverlay == TRUE)
	{
		auto rois = m_pDetectionAlgorithm->m_vecrtHairpin;

		if(VECTOR_SIZE(rois) <= 0)
		{
			return;
		}

		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0; layer < params.nLayerCount / 2; layer++)
			{
				auto roi = rois[slot][layer];

				if(slot == 0) // layer 번호 표시
				{
					strTemp.Format(_T("%d"), params.nLayerCount - (layer * 2));
					m_pImageDisplay->Overlay_AddText(CPoint(roi.left + roi.Width() / 4, roi.CenterPoint().y - 50), strTemp, RGB(0, 255, 0), 125, _T("Tahoma"));
					strTemp.Format(_T("%d"), params.nLayerCount - (layer * 2) - 1);
					m_pImageDisplay->Overlay_AddText(CPoint(roi.left + roi.Width() * 3 / 4, roi.CenterPoint().y - 50), strTemp, RGB(0, 255, 0), 125, _T("Tahoma"));
				}

				if(layer == 0) // slot 번호 표시
				{
					long index = params.nSlotCount - params.nStartSlotIndex + 1 + slot + 1; // ehjang. start slot index
					if(index > params.nSlotCount)
					{
						index -= params.nSlotCount;
					}

					strTemp.Format(_T("%d(%d)"), slot + 1, index);
					m_pImageDisplay->Overlay_AddText(CPoint(roi.left - 100, roi.CenterPoint().y), strTemp, RGB(0, 255, 0), 125, _T("Tahoma"));
				}

				m_pImageDisplay->Overlay_AddRectangle(roi, RGB(0, 255, 0));

				auto rtRoiHairpin = m_pDetectionAlgorithm->m_vecrtHairpin[slot][layer];
				rtRoiHairpin.left = rtRoiHairpin.left + params.nHairpinAreaROICenterToLeftOffset;
				rtRoiHairpin.top = rtRoiHairpin.top + params.nHairpinAreaROICenterToTopOffset;
				rtRoiHairpin.right = rtRoiHairpin.right - params.nHairpinAreaROICenterToRightOffset;
				rtRoiHairpin.bottom = rtRoiHairpin.bottom - params.nHairpinAreaROICenterToBottomOffset;

				m_pImageDisplay->Overlay_AddRectangle(rtRoiHairpin, RGB(255, 255, 0));
			}
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowRemoveNoise(const IMAGE image, const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyRemoveNoiseImage = ::g_MemoryManager.GetByteImage((long)image);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyRemoveNoiseImage, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);

	if(bShowOverlay == TRUE)
	{
		auto blobCount = m_pDetectionAlgorithm->m_nRemoveNoiseBlobTotalCount;
		auto blobList = m_pDetectionAlgorithm->m_vecRemoveNoiseBlobInfoList;

		if(blobCount <= 0 || VECTOR_SIZE(blobList) <= 0)
		{
			return;
		}

		CString strNo;
		for(long blob = 0; blob < blobCount; blob++)
		{
			auto info = blobList[blob];

			m_pImageDisplay->Overlay_AddRectangle(info.m_roi, RGB(0, 255, 0));

			m_pImageDisplay->Overlay_AddCrPoint(info.m_roi.CenterPoint(), RGB(255, 0, 0));

			strNo.Format(_T("%d"), blob + 1);
			m_pImageDisplay->Overlay_AddText(CPoint(info.m_roi.right, info.m_roi.bottom), strNo, RGB(0, 255, 0), 125, _T("Tahoma"));
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowInterpolation(const IMAGE image, const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto params = ::g_RecipeManager.m_sROIParams;
	auto interParams = ::g_RecipeManager.m_sInterHairpinParams;
	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyInterImage = ::g_MemoryManager.GetByteImage((long)image);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyInterImage, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);

	if(bShowOverlay == TRUE)
	{
		auto blobCount = m_pDetectionAlgorithm->m_nInterpolationBlobTotalCount;
		auto blobList = m_pDetectionAlgorithm->m_vecInterpolationBlobInfoList;

		if(blobCount <= 0 || VECTOR_SIZE(blobList) <= 0)
		{
			return;
		}

		CString strNo;
		for(long blob = 0; blob < blobCount; blob++)
		{
			auto info = blobList[blob];

			m_pImageDisplay->Overlay_AddRectangle(info.m_roi, RGB(0, 255, 0));

			m_pImageDisplay->Overlay_AddCrPoint(info.m_roi.CenterPoint(), RGB(255, 0, 0));

			strNo.Format(_T("%d"), blob + 1);
			m_pImageDisplay->Overlay_AddText(CPoint(info.m_roi.right, info.m_roi.bottom), strNo, RGB(0, 255, 0), 125, _T("Tahoma"));
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowDetectBlob(const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto params = ::g_RecipeManager.m_sROIParams;

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_THRESHOLD);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyThreshold, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);

	if(bShowOverlay == TRUE)
	{
		auto detectBlobList = m_pDetectionAlgorithm->m_vecDetectBlobBlobInfoList;

		if(VECTOR_SIZE(detectBlobList) <= 0)
		{
			return;
		}

		CString strNo;
		long index(0);
		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0; layer < params.nLayerCount / 2; layer++)
			{
				auto blobInfo = detectBlobList[slot][layer];

				for(long blob = 0; blob < (long)blobInfo.size(); blob++)
				{
					auto info = blobInfo[blob];

					m_pImageDisplay->Overlay_AddRectangle(info.m_roi, RGB(0, 255, 0));

					m_pImageDisplay->Overlay_AddCrPoint(info.m_roi.CenterPoint(), RGB(255, 0, 0));

					strNo.Format(_T("%d"), index + 1);
					m_pImageDisplay->Overlay_AddText(CPoint(info.m_roi.right, info.m_roi.bottom), strNo, RGB(0, 255, 0), 125, _T("Tahoma"));

					index++;
				}
			}
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowHairpinOverlay_Unrolled(const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto params = ::g_RecipeManager.m_sROIParams;

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyThreshold, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);

	if(bShowOverlay == TRUE)
	{
		auto detectBlobList = m_pDetectionAlgorithm->m_vecDetectBlobBlobInfoList;

		if(VECTOR_SIZE(detectBlobList) <= 0)
		{
			return;
		}

		CString strNo;
		long index(0);
		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0, layer2 = 0; layer < params.nLayerCount / 2; layer++, layer2 += 2)
			{
				auto blobInfo = detectBlobList[slot][layer];

				for(long blob = 0; blob < (long)blobInfo.size(); blob++)
				{
					auto info = blobInfo[blob];

					m_pImageDisplay->Overlay_AddRectangle(info.m_roi, RGB(0, 255, 0));

					index++;
				}

				m_pImageDisplay->Overlay_AddCrPoint(m_pDetectionAlgorithm->m_vecptCriteriaGravityCenter[slot][layer2], RGB(255, 0, 0));
				m_pImageDisplay->Overlay_AddCrPoint(m_pDetectionAlgorithm->m_vecptCriteriaGravityCenter[slot][layer2 + 1], RGB(255, 0, 0));
			}
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowHairpinOverlay_Original(const BOOL bRemoveOverlay, const BOOL bShowOverlay)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;

	auto imageWidth = m_pDetectionAlgorithm->m_nSubOriginalImageWidth;
	auto imageHeight = m_pDetectionAlgorithm->m_nSubOriginalImageHeight;

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyImage, imageWidth, imageHeight, imageWidth);

	auto ptResult = m_pDetectionAlgorithm->m_vecptResult;

	if(VECTOR_SIZE(ptResult) <= 0)
	{
		return;
	}

	if(bShowOverlay == TRUE)
	{
		CString strNo;
		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0; layer < params.nLayerCount; layer++)
			{
				auto centerPoint = ptResult[slot][layer];

				// 0, 0 일 경우 표시하지 않는다.
				if(centerPoint == CPoint(0, 0))
				{
					continue;
				}

				CRect rtCenter(0, 0, 0, 0);
				rtCenter.left = centerPoint.x - 5;
				rtCenter.top = centerPoint.y - 5;
				rtCenter.right = centerPoint.x + 5;
				rtCenter.bottom = centerPoint.y + 5;

				if(VECTOR_SIZE(m_pDetectionAlgorithm->m_vecbReplace3DPosition) > 0)
				{
					auto replace = m_pDetectionAlgorithm->m_vecbReplace3DPosition[slot][layer];
					if(replace == TRUE)
					{
						m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(0, 0, 255));
					}
					else
					{
						m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(255, 0, 0));
					}
				}
				else
				{
					m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(255, 0, 0));
				}
			}
		}

		auto startPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayStartPoint;
		auto endPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayEndPoint;
		auto slotNo1Point = m_pDetectionAlgorithm->m_vecptSlotNo1OverlayPoint;
		auto slotNo2Point = m_pDetectionAlgorithm->m_vecptSlotNo2OverlayPoint;

		if(VECTOR_SIZE(startPoint) <= 0 || VECTOR_SIZE(endPoint) <= 0 || VECTOR_SIZE(slotNo1Point) <= 0 || VECTOR_SIZE(slotNo2Point) <= 0)
		{
			return;
		}

		for(long idx = 0; idx < (long)startPoint.size(); idx++)
		{
			long index = params.nSlotCount - params.nStartSlotIndex + 1 + idx + 1; // ehjang. start slot index
			if(index > params.nSlotCount)
			{
				index -= params.nSlotCount;
			}

			strTemp.Format(_T("%d"), index);

			m_pImageDisplay->Overlay_AddLine(startPoint[idx], endPoint[idx], RGB(255, 255, 0));
			m_pImageDisplay->Overlay_AddText(slotNo1Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
			m_pImageDisplay->Overlay_AddText(slotNo2Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
		}
	}

	m_pImageDisplay->Overlay_Show(TRUE);
}

void CHairpin2DInspDlg::ShowCriteriaOverlay_Unrolled(const BOOL bRemoveOverlay, const BOOL bShowOverlay, const long slot, const long layer, const BOOL bSelect)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	auto params = ::g_RecipeManager.m_sROIParams;

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;

	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyThreshold, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);

	if(bSelect == TRUE)
	{
		m_pImageDisplay->SetFieldOfView(m_pDetectionAlgorithm->m_vecrtHairpin[slot][layer]);
	}

	if(bShowOverlay == TRUE)
	{
		auto detectBlobList = m_pDetectionAlgorithm->m_vecDetectBlobBlobInfoList;

		if(VECTOR_SIZE(detectBlobList) <= 0)
		{
			return;
		}

		CString strNo;
		long index(0);
		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0, layer2 = 0; layer < params.nLayerCount / 2; layer++, layer2 += 2)
			{
				auto blobInfo = detectBlobList[slot][layer];

				for(long blob = 0; blob < (long)blobInfo.size(); blob++)
				{
					auto info = blobInfo[blob];

					m_pImageDisplay->Overlay_AddRectangle(info.m_roi, RGB(0, 255, 0));

					strNo.Format(_T("%d"), index + 1);
					m_pImageDisplay->Overlay_AddText(CPoint(info.m_roi.right, info.m_roi.bottom), strNo, RGB(0, 255, 0), 125, _T("Tahoma"));

					index++;
				}

				m_pImageDisplay->Overlay_AddCrPoint(m_pDetectionAlgorithm->m_vecptCriteriaGravityCenter[slot][layer2], RGB(255, 0, 0));
				m_pImageDisplay->Overlay_AddCrPoint(m_pDetectionAlgorithm->m_vecptCriteriaGravityCenter[slot][layer2 + 1], RGB(255, 0, 0));
			}
		}

		m_pImageDisplay->Overlay_Show(TRUE);
	}
}

void CHairpin2DInspDlg::ShowCriteriaOverlay_Original(const BOOL bRemoveOverlay, const BOOL bShowOverlay, const long slot, const long layer, const BOOL bSelect)
{
	if(m_pDetectionAlgorithm->m_bLoadOriginalImage == FALSE)
	{
		return;
	}

	CString strTemp;

	auto params = ::g_RecipeManager.m_sROIParams;
	auto resultList = m_pDetectionAlgorithm->m_vecsCriteriaResult;

	auto imageWidth = m_pDetectionAlgorithm->m_nSubOriginalImageWidth;
	auto imageHeight = m_pDetectionAlgorithm->m_nSubOriginalImageHeight;

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::SUB_ORIG);

	if(bRemoveOverlay == TRUE)
	{
		m_pImageDisplay->Overlay_RemoveAll();
	}

	m_pImageDisplay->SetImage(pbyImage, imageWidth, imageHeight, imageWidth);

	auto layerPairPoint = m_pDetectionAlgorithm->m_vecptLayerPairOverlayPoint;

	if(VECTOR_SIZE(layerPairPoint) <= 0)
	{
		return;
	}

	if(bSelect == TRUE)
	{
		auto pt = layerPairPoint[slot][(params.nLayerCount / 2) - 1 -layer];
		CRect fov(pt.x - 300, pt.y - 300, pt.x + 300, pt.y + 300);

		m_pImageDisplay->SetFieldOfView(fov);
	}

	auto ptResult = m_pDetectionAlgorithm->m_vecptResult;
	auto inspResult = m_pDetectionAlgorithm->m_vecsCriteriaResult;

	if(VECTOR_SIZE(ptResult) <= 0 || VECTOR_SIZE(inspResult) <= 0)
	{
		return;
	}

	if(bShowOverlay == TRUE)
	{
		CString strNo;
		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0; layer < params.nLayerCount; layer++)
			{
				auto centerPoint = ptResult[slot][layer];
				auto result = resultList[slot][layer / 2].nErrorCode;

				// 0, 0 일 경우 표시하지 않는다.
				if(centerPoint == CPoint(0, 0))
				{
					continue;
				}

				// 높이 불량일 경우 표시하지 않는다.
				if(result == INSP_ERR_H1 || result == INSP_ERR_H2 || result == INSP_ERR_H3)
				{
					continue;
				}

				CRect rtCenter(0, 0, 0, 0);
				rtCenter.left = centerPoint.x - 5;
				rtCenter.top = centerPoint.y - 5;
				rtCenter.right = centerPoint.x + 5;
				rtCenter.bottom = centerPoint.y + 5;

				auto replace = m_pDetectionAlgorithm->m_vecbReplace3DPosition[slot][layer];
				if(replace == TRUE)
				{
					m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(0, 0, 255));
				}
				else
				{
					m_pImageDisplay->Overlay_AddEllipse2(rtCenter, RGB(255, 0, 0));
				}
			}
		}

		for(long slot = 0; slot < params.nSlotCount; slot++)
		{
			for(long layer = 0; layer < params.nLayerCount / 2; layer++)
			{
				if(inspResult[slot][layer].nErrorCode == INSP_ERR_OK)
				{
					continue;
				}

				auto points = layerPairPoint[slot][(params.nLayerCount / 2) - 1 - layer];

				CRect ellipse1(points.x - 99, points.y - 99, points.x + 99, points.y + 99);
				CRect ellipse2(points.x - 100, points.y - 100, points.x + 100, points.y + 100);
				CRect ellipse3(points.x - 101, points.y - 101, points.x + 101, points.y + 101);
				m_pImageDisplay->Overlay_AddEllipse(ellipse1, RGB(255, 0, 0));
				m_pImageDisplay->Overlay_AddEllipse(ellipse2, RGB(255, 0, 0));
				m_pImageDisplay->Overlay_AddEllipse(ellipse3, RGB(255, 0, 0));
			}
		}

		auto startPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayStartPoint;
		auto endPoint = m_pDetectionAlgorithm->m_vecptSlotLineOverlayEndPoint;
		auto slotNo1Point = m_pDetectionAlgorithm->m_vecptSlotNo1OverlayPoint;
		auto slotNo2Point = m_pDetectionAlgorithm->m_vecptSlotNo2OverlayPoint;

		if(VECTOR_SIZE(startPoint) <= 0 || VECTOR_SIZE(endPoint) <= 0 || VECTOR_SIZE(slotNo1Point) <= 0 || VECTOR_SIZE(slotNo2Point) <= 0)
		{
			return;
		}

		for(long idx = 0; idx < (long)startPoint.size(); idx++)
		{
			long index = params.nSlotCount - params.nStartSlotIndex + 1 + idx + 1; // ehjang. start slot index
			if(index > params.nSlotCount)
			{
				index -= params.nSlotCount;
			}

			strTemp.Format(_T("%d"), index);

			m_pImageDisplay->Overlay_AddLine(startPoint[idx], endPoint[idx], RGB(255, 255, 0));
			m_pImageDisplay->Overlay_AddText(slotNo1Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
			m_pImageDisplay->Overlay_AddText(slotNo2Point[idx], strTemp, RGB(255, 255, 0), 150, _T("Tahoma"));
		}
	}

	m_pImageDisplay->Overlay_Show(TRUE);
}

BOOL CHairpin2DInspDlg::UpdateRemoveNoiseThresholdImage(const BOOL bDisplay)
{
	m_pDetectionAlgorithm->DoMakeSubOriginalImage();
	m_pDetectionAlgorithm->DoMakeUnrolledImage();
	m_pDetectionAlgorithm->DoSetDetectionROI();

	UpdateRemoveNoiseProcImage(FALSE); // Processing

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto removeNoiseParams = ::g_RecipeManager.m_sRemoveNoiseParams;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_THRESHOLD); // Threshold Image
	BYTE* pbyHysteresisTemp = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_HYSTERESIS_TEMP); // Hysteresis Temp Image
	BYTE* pbyHysteresisLowHighMerge = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_HYSTERESIS_MERGE); // Hysteresis Merge Image

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;
	auto unrolledImageSize = m_nUnrolledImageSize;
	auto imageRect = CRect(0, 0, unrolledImageWidth, unrolledImageHeight);

	memcpy(pbyThreshold, pbyProcImage, unrolledImageSize);
	memset(pbyHysteresisTemp, 0, unrolledImageSize);
	memset(pbyHysteresisLowHighMerge, 0, unrolledImageSize);

	// Threshold
	if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		HUBIP::Binarization(pbyProcImage, unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_rtHairpinWhole, (BYTE)removeNoiseParams.nThresholdValue, (bool)removeNoiseParams.bThresholdReverse, pbyThreshold);
	}
	else if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::DOUBLE)
	{
		::g_Algorithm.DoubleThreshold(pbyProcImage, unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_rtHairpinWhole, removeNoiseParams.nThresholdLow, removeNoiseParams.nThresholdHigh, removeNoiseParams.bThresholdReverse, pbyThreshold);
	}
	else if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
		for(long slot = 0; slot < roiParams.nSlotCount; slot++)
		{
			for(long layer = 0; layer < roiParams.nLayerCount / 2; layer++)
			{
				HUBIP::BinarizeAdaptive(pbyProcImage, unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_vecrtHairpin[slot][layer], CSize(100, 100), removeNoiseParams.nAdaptiveOffset, pbyThreshold);

				if(removeNoiseParams.bThresholdReverse == TRUE)
				{
					HUBIP::BitwiseNot(unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_vecrtHairpin[slot][layer], pbyThreshold);
				}
			}
		}
	}
	else if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		HUBIP::BinarizeHysteresis(pbyProcImage, unrolledImageWidth, unrolledImageHeight, imageRect, (BYTE)removeNoiseParams.nHysteresisLow, (BYTE)removeNoiseParams.nHysteresisHigh, false, (bool)removeNoiseParams.bThresholdReverse, removeNoiseParams.nHighThresholdRegionNoiseFilteringSize, unrolledImageWidth, unrolledImageHeight, pbyHysteresisTemp, pbyThreshold);

		HUBIP::BinarizationHysteresisForSetupImage(pbyProcImage, unrolledImageWidth, unrolledImageHeight, (BYTE)removeNoiseParams.nHysteresisLow, (BYTE)removeNoiseParams.nHysteresisHigh, pbyHysteresisLowHighMerge);
	}

	// Display
	if(bDisplay)
	{
		if(removeNoiseParams.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
		{
			m_pImageDisplay->SetImage(pbyHysteresisLowHighMerge, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
		}
		else
		{
			m_pImageDisplay->SetImage(pbyThreshold, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
		}
	}

	return TRUE;
}

BOOL CHairpin2DInspDlg::UpdateRemoveNoiseProcImage(const BOOL bDisplay)
{
	auto &params = ::g_RecipeManager.m_sRemoveNoiseParams;
	auto &procParams = params.sProcParam;

	BYTE* pbyUnrolledImage = ::g_MemoryManager.GetByteImage((long)IMAGE::UNROLLED); // Unrolled Image
	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_PROC); // Processing Image

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;
	auto unrolledImageSize = m_nUnrolledImageSize;

	if((long)procParams.m_vecnProc.size() > 0 && (long)procParams.m_vecnProcParams.size() > 0)
	{
		HUBIP::ImageProcessingManage(pbyUnrolledImage, unrolledImageWidth, unrolledImageHeight, CRect(0, 0, unrolledImageWidth, unrolledImageHeight), &procParams.m_vecnProc[0], &procParams.m_vecnProcParams[0], (long)procParams.m_vecnProc.size(), pbyProcImage);
	}
	else
	{
		memcpy(pbyProcImage, pbyUnrolledImage, unrolledImageSize);
	}

	// Display
	if(bDisplay)
	{
		m_pImageDisplay->SetImage(pbyProcImage, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
	}

	return TRUE;
}

BOOL CHairpin2DInspDlg::UpdateInterThresholdImage(const BOOL bDisplay)
{
	m_pDetectionAlgorithm->DoMakeSubOriginalImage();
	m_pDetectionAlgorithm->DoMakeUnrolledImage();
	m_pDetectionAlgorithm->DoSetDetectionROI();
	m_pDetectionAlgorithm->DoRemoveNoise();

	UpdateInterProcImage(FALSE); // Processing

	auto roiParams = ::g_RecipeManager.m_sROIParams;
	auto interHairpinParams = ::g_RecipeManager.m_sInterHairpinParams;

	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_PROC); // Proc. Image
	BYTE* pbyThreshold = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_THRESHOLD); // Threshold Image
	BYTE* pbyHysteresisTemp = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_HYSTERESIS_TEMP); // Hysteresis Temp Image
	BYTE* pbyHysteresisLowHighMerge = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_HYSTERESIS_MERGE); // Hysteresis Merge Image

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;
	auto unrolledImageSize = m_nUnrolledImageSize;
	auto imageRect = CRect(0, 0, unrolledImageWidth, unrolledImageHeight);

	memcpy(pbyThreshold, pbyProcImage, unrolledImageSize);

	// Threshold
	if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::NORMAL)
	{
		HUBIP::Binarization(pbyProcImage, unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_rtHairpinWhole, (BYTE)interHairpinParams.nThresholdValue, (bool)interHairpinParams.bThresholdReverse, pbyThreshold);
	}
	else if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::ADAPTIVE)
	{
		for(long slot = 0; slot < roiParams.nSlotCount; slot++)
		{
			for(long layer = 0; layer < roiParams.nLayerCount / 2; layer++)
			{
				HUBIP::BinarizeAdaptive(pbyProcImage, unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_vecrtHairpin[slot][layer], CSize(100, 100), interHairpinParams.nAdaptiveOffset, pbyThreshold);

				if(interHairpinParams.bThresholdReverse == TRUE)
				{
					HUBIP::BitwiseNot(unrolledImageWidth, unrolledImageHeight, m_pDetectionAlgorithm->m_vecrtHairpin[slot][layer], pbyThreshold);
				}
			}
		}
	}
	else if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
	{
		HUBIP::BinarizeHysteresis(pbyProcImage, unrolledImageWidth, unrolledImageHeight, imageRect, (BYTE)interHairpinParams.nHysteresisLow, (BYTE)interHairpinParams.nHysteresisHigh, false, (bool)interHairpinParams.bThresholdReverse, interHairpinParams.nHighThresholdRegionNoiseFilteringSize, unrolledImageWidth, unrolledImageHeight, pbyHysteresisTemp, pbyThreshold);

		HUBIP::BinarizationHysteresisForSetupImage(pbyProcImage, unrolledImageWidth, unrolledImageHeight, (BYTE)interHairpinParams.nHysteresisLow, (BYTE)interHairpinParams.nHysteresisHigh, pbyHysteresisLowHighMerge);
	}

	// Display
	if(bDisplay)
	{
		if(interHairpinParams.nSelectThreshold == (long)THRESHOLD::HYSTERISIS)
		{
			m_pImageDisplay->SetImage(pbyHysteresisLowHighMerge, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
		}
		else
		{
			m_pImageDisplay->SetImage(pbyThreshold, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
		}
	}

	return TRUE;
}

BOOL CHairpin2DInspDlg::UpdateInterProcImage(const BOOL bDisplay)
{
	auto &params = ::g_RecipeManager.m_sInterHairpinParams;
	auto &procParams = params.sProcParam;

	BYTE* pbyRemoveNoiseResult = ::g_MemoryManager.GetByteImage((long)IMAGE::REMOVE_NOISE_RESULT); // Remove Noise Result Image
	BYTE* pbyProcImage = ::g_MemoryManager.GetByteImage((long)IMAGE::INTER_PROC); // Processing Image

	auto unrolledImageWidth = m_nUnrolledImageWidth;
	auto unrolledImageHeight = m_nUnrolledImageHeight;
	auto unrolledImageSize = m_nUnrolledImageSize;

	if((long)procParams.m_vecnProc.size() > 0 && (long)procParams.m_vecnProcParams.size() > 0)
	{
		HUBIP::ImageProcessingManage(pbyRemoveNoiseResult, unrolledImageWidth, unrolledImageHeight, CRect(0, 0, unrolledImageWidth, unrolledImageHeight), &procParams.m_vecnProc[0], &procParams.m_vecnProcParams[0], (long)procParams.m_vecnProc.size(), pbyProcImage);
	}
	else
	{
		memcpy(pbyProcImage, pbyRemoveNoiseResult, unrolledImageSize);
	}

	// Display
	if(bDisplay)
	{
		m_pImageDisplay->SetImage(pbyProcImage, unrolledImageWidth, unrolledImageHeight, unrolledImageWidth);
	}

	return TRUE;
}

void CHairpin2DInspDlg::OnImageImageopen()
{
	CFileDialog Dlg(TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST, _T("Bmp File (*.bmp)|*.bmp||"), NULL, NULL, FALSE);

	if(Dlg.DoModal() == IDOK)
	{
		CString strPath = Dlg.GetPathName();

		ImageOpen2D(strPath);
	}
}


void CHairpin2DInspDlg::OnInspectionInspection()
{
	m_pDetectionAlgorithm->DoInspection();

	ShowCriteriaOverlay_Original(TRUE, TRUE);

	SetResult(m_pDetectionAlgorithm->m_strTotalResult);
}


void CHairpin2DInspDlg::OnInspectionBatchinspection()
{
	m_pDlgBatchInspection->SetData();

	m_pDlgBatchInspection->ShowWindow(SW_SHOW);
}


void CHairpin2DInspDlg::OnBnClickedBtnInspection()
{
	m_pDetectionAlgorithm->DoInspection();

	ShowCriteriaOverlay_Original(TRUE, TRUE);

	SetResult(m_pDetectionAlgorithm->m_strTotalResult);
}

void CHairpin2DInspDlg::SetResult(const CString strResult)
{
	CString strInspResult;

	if(strResult != _T(""))
	{
		if(m_bResult)
		{
			strInspResult = _T("/") + strResult;
		}
		else
		{
			strInspResult = strResult + _T("/");
		}
	}

	COLORREF color;
	if(strResult == _T("OK"))
	{
		color = RGB(0, 255, 0);
	}
	else if(strResult == _T("NG"))
	{
		color = RGB(255, 0, 0);
	}
	else
	{
		color = RGB(0, 0, 0);
	}

	m_lblResult.SetText(strInspResult);
	m_lblResult.SetFontSize(60);
	m_lblResult.SetFontBold(TRUE);
	m_lblResult.SetTextColor(color);
	m_lblResult.SetBorder(TRUE);

	m_bResult = !m_bResult;
}

BOOL CHairpin2DInspDlg::CreateFolder(CString sDirPath)
{
	int iLng = sDirPath.GetLength();
	int iPos1 = sDirPath.ReverseFind(_T('\\')) + 1;
	int iPos2 = sDirPath.ReverseFind(_T('/'));
	if(iLng != iPos1 && iLng != iPos2)
	{
		sDirPath += _T("\\");
	}

	LPCTSTR lpszPath = (LPCTSTR)sDirPath;

	TCHAR szPathBuffer[MAX_PATH];

	size_t len = _tcslen(lpszPath);

	for(size_t i = 0; i < len; i++)
	{
		szPathBuffer[i] = *(lpszPath + i);
		if(szPathBuffer[i] == _T('\\') || szPathBuffer[i] == _T('/'))
		{
			szPathBuffer[i + 1] = NULL;
			if(!PathFileExists(szPathBuffer))
			{
				if(!::CreateDirectory(szPathBuffer, NULL))
				{
					if(GetLastError() != ERROR_ALREADY_EXISTS)
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}


long CHairpin2DInspDlg::FileAllsavefordebugging(const CString sZipFilePathName)
{
	HUBSDK::ThreadedProgress progressX(_T("Data Saving ...."));

	int iPos = sZipFilePathName.ReverseFind(_T('.'));
	CString sCreateFileName = sZipFilePathName.Mid(0, iPos);
	CreateFolder(sCreateFileName);

	iPos = sCreateFileName.ReverseFind(_T('\\'));

	// 시간 가져오기...
	COleDateTime currentTime;
	CString sZipFileName;
	currentTime = COleDateTime::GetCurrentTime();
	sZipFileName = currentTime.Format(_T("%Y%m%d%H%M%S")); // 날짜, 시간, 분, 초

	CString sFileName[2], sCopyFilePath[2];

	// Raw image save -> zip
	sFileName[0] = sZipFileName + _T("_raw.bmp");
	sCopyFilePath[0] = sCreateFileName + _T("\\") + sFileName[0];

	BYTE* pbyImage = ::g_MemoryManager.GetByteImage((long)IMAGE::ORIG);
	HUBSDK::ImageInterface::ImageSaveBMP(sCopyFilePath[0], pbyImage, m_nOriginalImageWidth, m_nOriginalImageHeight, 8, CRect(0, 0, m_nOriginalImageWidth, m_nOriginalImageHeight), TRUE);

	// Recipe Copy
	sFileName[1] = REMOVE_FOLDER_FROM_PATHNAME(m_pDetectionAlgorithm->m_strCurRecipeFilePath);
	sCopyFilePath[1] = m_pDetectionAlgorithm->m_strCurRecipeFilePath;

	// HZIP
	HZIP hz = CreateZip(sZipFilePathName, 0);
	if(hz == 0)
	{
		return 1;
	}

	// 파일 압축
	ZRESULT zr;
	for(int i = 0; i < 2; i++) // 압축할 파일 2개
	{
		zr = ZipAdd(hz, sFileName[i], sCopyFilePath[i]);
		if(zr != ZR_OK)
		{
			zr = CloseZip(hz);

			return 2;
		}
	}

	zr = CloseZip(hz);
	if(zr != ZR_OK)
	{
		return 3;
	}

	// raw 복사해둔 폴더 삭제 필요   sCreateFileName, sCopyFilePath[0]
	BOOL bRval = FALSE;
	CFileFind find;

	bRval = find.FindFile(sCopyFilePath[0]);
	if(bRval == FALSE)
	{
		return 4;
	}

	while(bRval)
	{
		bRval = find.FindNextFile();
		if(find.IsDots())
			continue;

		DeleteFile(find.GetFilePath());
	}

	find.Close();
	bRval = RemoveDirectory(sCreateFileName);

	if(bRval == FALSE)
	{
		return 4;
	}

	return 0;
}
