// DlgBatchInspection.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgBatchInspection.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define AUTO_TIMER 999

// CDlgBatchInspection 대화 상자

IMPLEMENT_DYNAMIC(CDlgBatchInspection, CDialogEx)

CDlgBatchInspection::CDlgBatchInspection(CHairpin2DInspDlg* pParent /*=nullptr*/)
	: CDialogEx(IDD_DLG_BATCH_INSPECTION, pParent)
	, m_pParent(pParent)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
{

}

CDlgBatchInspection::~CDlgBatchInspection()
{
}

void CDlgBatchInspection::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_DATA, m_listData);
}


BEGIN_MESSAGE_MAP(CDlgBatchInspection, CDialogEx)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ADD_FILES, &CDlgBatchInspection::OnBnClickedBtnAddFiles)
	ON_BN_CLICKED(IDC_BTN_DEL_FILES, &CDlgBatchInspection::OnBnClickedBtnDelFiles)
	ON_BN_CLICKED(IDC_BTN_CLEAR_FILE_LIST, &CDlgBatchInspection::OnBnClickedBtnClearFileList)
	ON_BN_CLICKED(IDC_BTN_ALL_CHECK, &CDlgBatchInspection::OnBnClickedBtnAllCheck)
	ON_BN_CLICKED(IDC_BTN_ALL_UN_CHECK, &CDlgBatchInspection::OnBnClickedBtnAllUnCheck)
	ON_BN_CLICKED(IDC_BTN_AUTO_INSPECTION, &CDlgBatchInspection::OnBnClickedBtnAutoInspection)
	ON_BN_CLICKED(IDC_BTN_AUTO_INSPECTION_STOP, &CDlgBatchInspection::OnBnClickedBtnAutoInspectionStop)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgBatchInspection::OnBnClickedBtnInspection)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST_DATA, &CDlgBatchInspection::OnLvnItemchangedListData)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_LIST_DATA, &CDlgBatchInspection::OnCustomdrawList)
END_MESSAGE_MAP()


// CDlgBatchInspection 메시지 처리기

BOOL CDlgBatchInspection::OnInitDialog()
{
	CDialog::OnInitDialog();

	InitListControl();

	return TRUE;  // return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgBatchInspection::OnTimer(UINT_PTR nIDEvent)
{
	CString strTemp;

	if(nIDEvent == AUTO_TIMER)
	{
		KillTimer(AUTO_TIMER);

		if(m_nInspIndex < (long)m_vecsBatchInfo.size())
		{
			strTemp.Format(_T("%d) %s"), m_nInspIndex + 1, m_vecsBatchInfo[m_nInspIndex].m_strFilePath);

			GetDlgItem(IDC_STATIC_SELECTED)->SetWindowTextW(strTemp);

			m_pParent->ImageOpen2D(m_vecsBatchInfo[m_nInspIndex].m_strFilePath);

			m_pDetectionAlgorithm->DoInspection();

			m_pParent->ShowCriteriaOverlay_Original(TRUE, TRUE);

			int col = 3;

			m_vecsBatchInfo[m_nInspIndex].m_strTotalResult = m_pDetectionAlgorithm->m_strTotalResult;
			m_listData.SetItemText(m_nInspIndex, col, m_pDetectionAlgorithm->m_strTotalResult); col++;

			m_nInspIndex++;

			m_nSeletedIndex = m_nInspIndex;

			Sleep(1000);

			Invalidate(FALSE);

			SetTimer(AUTO_TIMER, 1, NULL);
		}
		else
		{
			::AfxMessageBox(_T("Inspection Finished"));
		}
	}

	CDialog::OnTimer(nIDEvent);
}

void CDlgBatchInspection::SetData()
{
	m_vecsBatchInfo.clear();
	SetListControl();

	InitListControl();
}

void CDlgBatchInspection::InitListControl()
{
	m_listData.DeleteAllItems();

	m_listData.SetExtendedStyle(m_listData.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT | LVS_EX_CHECKBOXES);

	long col = 0;
	m_listData.InsertColumn(col, _T(""), LVCFMT_CENTER, 25); col++;
	m_listData.InsertColumn(col, _T("No"), LVCFMT_CENTER, 40); col++;
	m_listData.InsertColumn(col, _T("File Path"), LVCFMT_LEFT, 700); col++;
	m_listData.InsertColumn(col, _T("Result"), LVCFMT_CENTER, 120); col++;

	Invalidate(FALSE);
}

void CDlgBatchInspection::SetListControl()
{
	CString strTemp;

	// List 에 있는 내용을 모두 지운다.
	m_listData.DeleteAllItems();

	for(long data = 0; data < (long)m_vecsBatchInfo.size(); data++)
	{
		m_listData.InsertItem(data, _T(""));

		long col = 1;
		strTemp.Format(_T("%d"), data + 1);
		m_listData.SetItemText(data, col, strTemp); col++;
		strTemp.Format(_T("%s"), m_vecsBatchInfo[data].m_strFilePath);
		m_listData.SetItemText(data, col, strTemp); col++;
		strTemp.Format(_T("%s"), m_pDetectionAlgorithm->m_strTotalResult);
		m_listData.SetItemText(data, col, strTemp); col++;
	}

	Invalidate(FALSE);
}

void CDlgBatchInspection::OnBnClickedBtnAddFiles()
{
	CFileDialog Dlg(TRUE, _T("bmp"), NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_ALLOWMULTISELECT, _T("Bmp File (*.bmp)|*.bmp||"), NULL, NULL, FALSE);

	int nMaxFileName = 4096000;
	TCHAR *pszFileNames = new TCHAR[nMaxFileName];
	ZeroMemory(pszFileNames, nMaxFileName);

	Dlg.m_ofn.lpstrFile = pszFileNames;
	Dlg.m_ofn.nMaxFile = nMaxFileName - 1;

	if(Dlg.DoModal() == IDOK)
	{
		POSITION pos = Dlg.GetStartPosition();

		while(pos)
		{
			SBatchInfo sBatchInfo;
			sBatchInfo.Init();
			sBatchInfo.m_strFilePath = Dlg.GetNextPathName(pos);

			m_vecsBatchInfo.push_back(sBatchInfo);
		}

		SetListControl();
	}

	delete[] pszFileNames;

	Invalidate(FALSE);
}


void CDlgBatchInspection::OnBnClickedBtnDelFiles()
{
	// 임시로 사용할 변수
	std::vector<SBatchInfo> m_vecsBatchInfo_Temp;

	// 체크가 안되어 있는 정보를 임시 변수에 저장(복사)한다.
	// 체크한(선택한) 정보는 필요없으므로 체크안된 정보만 임시 변수에 저장
	long nDataCount = 0;
	for(long data = 0; data < (long)m_vecsBatchInfo.size(); data++)
	{
		if(m_listData.GetCheck(data) == FALSE)
		{
			SBatchInfo sBatchInfo;
			sBatchInfo.Copy(m_vecsBatchInfo[data]);

			m_vecsBatchInfo_Temp.push_back(sBatchInfo);
		}
	}

	m_vecsBatchInfo.clear();
	for(long data = 0; data < (long)m_vecsBatchInfo_Temp.size(); data++)
	{
		SBatchInfo sBatchInfo;
		sBatchInfo.Copy(m_vecsBatchInfo_Temp[data]);

		m_vecsBatchInfo.push_back(sBatchInfo);
	}

	SetListControl();
}


void CDlgBatchInspection::OnBnClickedBtnClearFileList()
{
	m_vecsBatchInfo.clear();

	SetListControl();
}


void CDlgBatchInspection::OnBnClickedBtnAllCheck()
{
	for(long data = 0; data < (long)m_vecsBatchInfo.size(); data++)
	{
		m_listData.SetCheck(data);
	}

	Invalidate(FALSE);
}


void CDlgBatchInspection::OnBnClickedBtnAllUnCheck()
{
	for(long data = 0; data < (long)m_vecsBatchInfo.size(); data++)
	{
		m_listData.SetCheck(data, FALSE);
	}

	Invalidate(FALSE);
}


void CDlgBatchInspection::OnBnClickedBtnAutoInspection()
{
	m_nSeletedIndex = 0;
	m_nInspIndex = 0;

	SetTimer(AUTO_TIMER, 1, NULL);
}


void CDlgBatchInspection::OnBnClickedBtnAutoInspectionStop()
{
	KillTimer(AUTO_TIMER);

	Invalidate(FALSE);
}


void CDlgBatchInspection::OnBnClickedBtnInspection()
{
	m_pDetectionAlgorithm->DoInspection();

	m_pParent->ShowCriteriaOverlay_Original(TRUE, TRUE);

	int col = 3;

	if((long)m_vecsBatchInfo.size() <= m_nSeletedIndex)
	{
		return;
	}

	m_vecsBatchInfo[m_nSeletedIndex].m_strTotalResult = m_pDetectionAlgorithm->m_strTotalResult;
	m_listData.SetItemText(m_nSeletedIndex, col, m_pDetectionAlgorithm->m_strTotalResult); col++;

	Invalidate(FALSE);
}


void CDlgBatchInspection::OnLvnItemchangedListData(NMHDR *pNMHDR, LRESULT *pResult)
{
	CString strTemp;

	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);

	if((pNMLV->uNewState & LVIS_SELECTED) == LVIS_SELECTED)
	{
		m_nSeletedIndex = pNMLV->iItem;

		if((long)m_vecsBatchInfo.size() <= m_nSeletedIndex)
		{
			return;
		}

		strTemp.Format(_T("%d) %s"), m_nSeletedIndex + 1, m_vecsBatchInfo[m_nSeletedIndex].m_strFilePath);

		GetDlgItem(IDC_STATIC_SELECTED)->SetWindowTextW(strTemp);

		m_pParent->ImageOpen2D(m_vecsBatchInfo[m_nSeletedIndex].m_strFilePath);
	}

	*pResult = 0;
}

void CDlgBatchInspection::OnCustomdrawList(NMHDR* pNMHDR, LRESULT* pResult)
{
	NMLVCUSTOMDRAW* pLVCD = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);

	*pResult = 0;

	CString strType = m_listData.GetItemText((int)pLVCD->nmcd.dwItemSpec, 3);

	if(CDDS_PREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		*pResult = CDRF_NOTIFYITEMDRAW;
	}
	else if(CDDS_ITEMPREPAINT == pLVCD->nmcd.dwDrawStage)
	{
		if(strType == _T("NG"))
		{
			pLVCD->clrText = RGB(255, 0, 0);
		}
		else
		{
			pLVCD->clrText = RGB(0, 0, 0);
		}

		*pResult = CDRF_DODEFAULT;
	}
}