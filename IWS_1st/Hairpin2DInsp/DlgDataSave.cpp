// DlgDataSave.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "DlgDataSave.h"
#include "afxdialogex.h"


// CDlgDataSave 대화 상자

IMPLEMENT_DYNAMIC(CDlgDataSave, CDialogEx)

CDlgDataSave::CDlgDataSave(BOOL bSaveRawImage, CString strRawImageSavePath, BOOL bSaveResultData, CString strResultDataSavePath, BOOL bSaveResultImage, CString strResultImageSavePath, BOOL bSaveResultImageForJigA, CString strResultImageSavePathForJigA, BOOL bSaveResultImageForJigB, CString strResultImageSavePathForJigB, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_DATA_SAVE, pParent)
	, m_bSaveRawImage(bSaveRawImage)
	, m_strRawImageSavePath(strRawImageSavePath)
	, m_bSaveResultData(bSaveResultData)
	, m_strResultDataSavePath(strResultDataSavePath)
	, m_bSaveResultImage(bSaveResultImage)
	, m_strResultImageSavePath(strResultImageSavePath)
	, m_bSaveResultImageForJigA(bSaveResultImageForJigA)
	, m_strResultImageSavePathForJigA(strResultImageSavePathForJigA)
	, m_bSaveResultImageForJigB(bSaveResultImageForJigB)
	, m_strResultImageSavePathForJigB(strResultImageSavePathForJigB)
{
}

CDlgDataSave::~CDlgDataSave()
{
}

void CDlgDataSave::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_SAVE_RAW_IMAGE, m_checkSaveRawImage);
	DDX_Text(pDX, IDC_EDIT_SAVE_RAW_IMAGE, m_strRawImageSavePath);
	DDX_Control(pDX, IDC_CHECK_SAVE_RESULT_DATA, m_checkSaveResultData);
	DDX_Text(pDX, IDC_EDIT_SAVE_RESULT_DATA_PATH, m_strResultDataSavePath);
	DDX_Control(pDX, IDC_CHECK_SAVE_RESULT_IMAGE, m_checkSaveResultImage);
	DDX_Text(pDX, IDC_EDIT_SAVE_RESULT_IMAGE_PATH, m_strResultImageSavePath);
	DDX_Text(pDX, IDC_EDIT_SAVE_RESULT_IMAGE_PATH_FOR_JIGA, m_strResultImageSavePathForJigA);
	DDX_Text(pDX, IDC_EDIT_SAVE_RESULT_IMAGE_PATH_FOR_JIGB, m_strResultImageSavePathForJigB);
	DDX_Control(pDX, IDC_CHECK_SAVE_RESULT_IMAGE_FOR_JIGA, m_checkSaveResultImageForJigA);
	DDX_Control(pDX, IDC_CHECK_SAVE_RESULT_IMAGE_FOR_JIGB, m_checkSaveResultImageForJigB);
}


BEGIN_MESSAGE_MAP(CDlgDataSave, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_SAVE_RAW_IMAGE, &CDlgDataSave::OnBnClickedBtnSaveRawImage)
	ON_BN_CLICKED(IDC_BTN_SAVE_RESULT_DATA, &CDlgDataSave::OnBnClickedBtnSaveResultData)
	ON_BN_CLICKED(IDC_BTN_SAVE_RESULT_IMAGE, &CDlgDataSave::OnBnClickedBtnSaveResultImage)
	ON_BN_CLICKED(IDOK, &CDlgDataSave::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgDataSave 메시지 처리기

BOOL CDlgDataSave::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_checkSaveRawImage.SetCheck(m_bSaveRawImage);
	m_checkSaveResultData.SetCheck(m_bSaveResultData);
	m_checkSaveResultImage.SetCheck(m_bSaveResultImage);
	m_checkSaveResultImageForJigA.SetCheck(m_bSaveResultImageForJigA);
	m_checkSaveResultImageForJigB.SetCheck(m_bSaveResultImageForJigB);

	return TRUE;
}

void CDlgDataSave::OnBnClickedBtnSaveRawImage()
{
	UpdateData(TRUE);

	BROWSEINFO brif;

	memset(&brif, 0, sizeof(brif));

	brif.lpszTitle = _T("Select Image Save Directory");

	if(LPITEMIDLIST pidl = SHBrowseForFolder(&brif))
	{
		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(TCHAR) * MAX_PATH);

		if(SHGetPathFromIDList(pidl, szPath))
		{
			m_strRawImageSavePath = szPath;
			GetDlgItem(IDC_EDIT_SAVE_RESULT_IMAGE_PATH)->SetWindowTextW(m_strRawImageSavePath);
		}
	}

	UpdateData(FALSE);
}

void CDlgDataSave::OnBnClickedBtnSaveResultImage()
{
	UpdateData(TRUE);

	BROWSEINFO brif;

	memset(&brif, 0, sizeof(brif));

	brif.lpszTitle = _T("Select Image Save Directory");

	if(LPITEMIDLIST pidl = SHBrowseForFolder(&brif))
	{
		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(TCHAR) * MAX_PATH);

		if(SHGetPathFromIDList(pidl, szPath))
		{
			m_strResultImageSavePath = szPath;
			GetDlgItem(IDC_EDIT_SAVE_RESULT_IMAGE_PATH)->SetWindowTextW(m_strResultImageSavePath);
		}
	}

	UpdateData(FALSE);
}

void CDlgDataSave::OnBnClickedBtnSaveResultData()
{
	UpdateData(TRUE);

	BROWSEINFO brif;

	memset(&brif, 0, sizeof(brif));

	brif.lpszTitle = _T("Select Data Save Directory");

	if(LPITEMIDLIST pidl = SHBrowseForFolder(&brif))
	{
		TCHAR szPath[MAX_PATH];
		memset(szPath, 0, sizeof(TCHAR) * MAX_PATH);

		if(SHGetPathFromIDList(pidl, szPath))
		{
			m_strResultDataSavePath = szPath;
			GetDlgItem(IDC_EDIT_SAVE_RESULT_DATA_PATH)->SetWindowTextW(m_strResultDataSavePath);
		}
	}

	UpdateData(FALSE);
}


void CDlgDataSave::OnBnClickedOk()
{
	UpdateData(TRUE);

	m_bSaveRawImage = m_checkSaveRawImage.GetCheck();
	m_bSaveResultData = m_checkSaveResultData.GetCheck();
	m_bSaveResultImage = m_checkSaveResultImage.GetCheck();
	m_bSaveResultImageForJigA = m_checkSaveResultImageForJigA.GetCheck();
	m_bSaveResultImageForJigB = m_checkSaveResultImageForJigB.GetCheck();

	CDialogEx::OnOK();
}
