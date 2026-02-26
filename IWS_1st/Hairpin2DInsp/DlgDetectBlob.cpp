// DlgAlign1.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgDetectBlob.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"
#include "DlgImageProcManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgDetectBlob 대화 상자

IMPLEMENT_DYNAMIC(CDlgDetectBlob, CDialog)

CDlgDetectBlob::CDlgDetectBlob(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialog(IDD_DLG_DETECT_BLOB, pParent)
	, m_pParent(pParent)
	, m_pImageDisplay(m_pParent->m_pImageDisplay)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_nMinBlobSize(0)
	, m_nMaxBlobCount(0)
	, m_nBlobMergeDist(0)
{

}

CDlgDetectBlob::~CDlgDetectBlob()
{
}

void CDlgDetectBlob::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_MIN_SIZE, m_nMinBlobSize);
	DDX_Text(pDX, IDC_EDIT_MAX_BLOB_COUNT, m_nMaxBlobCount);
	DDX_Text(pDX, IDC_EDIT_MERGEDISTANCE, m_nBlobMergeDist);
	DDX_Control(pDX, IDC_LIST_BLOBLIST, m_listBlobInfo);
	DDX_Control(pDX, IDC_CHECK_USEMERGEBLOB, m_checkBlobMerge);
}


BEGIN_MESSAGE_MAP(CDlgDetectBlob, CDialog)
	ON_BN_CLICKED(IDC_BTN_UPDATEBLOBLIST, &CDlgDetectBlob::OnBnClickedBtnUpdatebloblist)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgDetectBlob::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgDetectBlob::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_CHECK_USEMERGEBLOB, &CDlgDetectBlob::OnBnClickedCheckUsemergeblob)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE, &CDlgDetectBlob::OnBnClickedBtnOriginalImage)
	ON_BN_CLICKED(IDC_BTN_PROCEEDED_IMAGE, &CDlgDetectBlob::OnBnClickedBtnProceededImage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_BLOBLIST, &CDlgDetectBlob::OnLvnItemchangedListBloblist)
	ON_BN_CLICKED(IDC_BTN_THRESHOLD_IMAGE, &CDlgDetectBlob::OnBnClickedBtnThresholdImage)
	ON_BN_CLICKED(IDC_BTN_UNROLLED_IMAGE, &CDlgDetectBlob::OnBnClickedBtnUnrolledImage)
END_MESSAGE_MAP()


// CDlgDetectBlob 메시지 처리기

BOOL CDlgDetectBlob::OnInitDialog()
{
	CDialog::OnInitDialog();

	auto params = ::g_RecipeManager.m_sDetectBlobParams;

	m_listBlobInfo.SetExtendedStyle(m_listBlobInfo.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	m_listBlobInfo.InsertColumn(0, _T("No."), LVCFMT_CENTER, 40);
	m_listBlobInfo.InsertColumn(1, _T("Area(px)"), LVCFMT_CENTER, 80);
	m_listBlobInfo.InsertColumn(2, _T("Width(px)"), LVCFMT_CENTER, 80);
	m_listBlobInfo.InsertColumn(3, _T("Height(px)"), LVCFMT_CENTER, 80);
	m_listBlobInfo.InsertColumn(4, _T("AspectRatio"), LVCFMT_CENTER, 80);

	return TRUE;
}

void CDlgDetectBlob::UpdateUI()
{
	CString strTemp;
	auto params = ::g_RecipeManager.m_sDetectBlobParams;

	UpdateData(TRUE);

	m_nMinBlobSize = params.nMinBlobSize;
	m_nMaxBlobCount = params.nMaxBlobCount;
	m_checkBlobMerge.SetCheck(params.bBlobMerge);
	m_nBlobMergeDist = params.nBlobMergeDist;

	UpdateData(FALSE);
}

void CDlgDetectBlob::UpdateParameters()
{
	UpdateData(TRUE);

	auto &params = ::g_RecipeManager.m_sDetectBlobParams;

	params.nMinBlobSize = m_nMinBlobSize;
	params.nMaxBlobCount = m_nMaxBlobCount;
	params.bBlobMerge = m_checkBlobMerge.GetCheck();
	params.nBlobMergeDist = m_nBlobMergeDist;
}

void CDlgDetectBlob::OnBnClickedCheckUsemergeblob()
{
	auto &params = ::g_RecipeManager.m_sDetectBlobParams;

	params.bBlobMerge = m_checkBlobMerge.GetCheck();
}

void CDlgDetectBlob::OnBnClickedBtnOriginalImage()
{
	m_pParent->ShowOriginal(TRUE, TRUE);
}

void CDlgDetectBlob::OnBnClickedBtnUnrolledImage()
{
	m_pParent->ShowMakeUnrolled(FALSE);
}

void CDlgDetectBlob::OnBnClickedBtnProceededImage()
{
	m_pParent->ShowInterpolation(IMAGE::INTER_PROC, TRUE, TRUE);
}

void CDlgDetectBlob::OnBnClickedBtnThresholdImage()
{
	m_pParent->ShowDetectBlob(TRUE, TRUE);
}

void CDlgDetectBlob::OnBnClickedBtnApply()
{
	UpdateParameters();
}

void CDlgDetectBlob::OnBnClickedBtnUpdatebloblist()
{
	HUBSDK::ThreadedProgress progressX(_T("Processing now..."));

	CString strNo, strTemp;

	auto roiParams = ::g_RecipeManager.m_sROIParams;

	m_pImageDisplay->Overlay_RemoveAll();

	UpdateParameters();

	m_pDetectionAlgorithm->DoMakeSubOriginalImage();
	m_pDetectionAlgorithm->DoMakeUnrolledImage();
	m_pDetectionAlgorithm->DoSetDetectionROI();
	m_pDetectionAlgorithm->DoMakeOverlay();
	m_pDetectionAlgorithm->DoRemoveNoise();
	m_pDetectionAlgorithm->DoInterpolation();
	m_pDetectionAlgorithm->DoDetectBlob();

	// List 에 있는 내용을 모두 지운다.
	m_listBlobInfo.DeleteAllItems();

	// Blob List에 Blob 정보 보여주기
	long index(0);
	m_vecBlobInfoList.clear();
	for(long slot = 0; slot < roiParams.nSlotCount; slot++)
	{
		for(long layer = 0; layer < roiParams.nLayerCount / 2; layer++)
		{
			auto blobInfo = m_pDetectionAlgorithm->m_vecDetectBlobBlobInfoList[slot][layer];

			for(long blob = 0; blob < (long)blobInfo.size(); blob++)
			{
				auto info = blobInfo[blob];

				m_listBlobInfo.InsertItem(index, _T(""));

				strNo.Format(_T("%d"), index + 1);
				m_listBlobInfo.SetItemText(index, 0, strNo);
				strTemp.Format(_T("%d"), info.m_area);
				m_listBlobInfo.SetItemText(index, 1, strTemp);
				strTemp.Format(_T("%d"), info.m_roi.Width());
				m_listBlobInfo.SetItemText(index, 2, strTemp);
				strTemp.Format(_T("%d"), info.m_roi.Height());
				m_listBlobInfo.SetItemText(index, 3, strTemp);
				strTemp.Format(_T("%.3f"), (float)info.m_roi.Width() / (float)info.m_roi.Height());
				m_listBlobInfo.SetItemText(index, 4, strTemp);

				m_vecBlobInfoList.push_back(info);

				index++;
			}
		}
	}

	m_pParent->ShowROI(TRUE, TRUE);
	m_pParent->ShowInterpolation(IMAGE::INTER_THRESHOLD, FALSE, FALSE);
	m_pParent->ShowDetectBlob(FALSE, TRUE);
}

void CDlgDetectBlob::OnBnClickedBtnInspection()
{
	OnBnClickedBtnUpdatebloblist();
}


void CDlgDetectBlob::OnLvnItemchangedListBloblist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		int nItem = pNMLV->iItem;

		m_pParent->ShowROI(TRUE, TRUE);

		m_pImageDisplay->Overlay_AddRectangle(m_vecBlobInfoList[nItem].m_roi, RGB(255, 0, 0));

		m_pImageDisplay->Overlay_Show(TRUE);
	}

	*pResult = 0;
}

