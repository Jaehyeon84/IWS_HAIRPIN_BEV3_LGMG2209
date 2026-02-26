// CDlgReworkJob.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgReworkJob.h"
#include "afxdialogex.h"


// CDlgReworkJob 대화 상자

IMPLEMENT_DYNAMIC(CDlgReworkJob, CDialogEx)

CDlgReworkJob::CDlgReworkJob(IWS_HAIRPIN_INFO hpInfo, CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_REWORK_LIST, pParent)
{
	m_hpInfo = hpInfo;
}

CDlgReworkJob::~CDlgReworkJob()
{
}

void CDlgReworkJob::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_JOB, m_ctrlJobList);
	DDX_Control(pDX, IDC_COMBO_SLOT_NO, m_ctrlSlotNo);
	DDX_Control(pDX, IDC_COMBO_LAYER_NO, m_ctrlLayerNo);
	DDX_Control(pDX, IDC_COMBO_PTN_NO, m_ctrlPtnNo);
}


BEGIN_MESSAGE_MAP(CDlgReworkJob, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ADD, &CDlgReworkJob::OnBnClickedButtonAdd)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &CDlgReworkJob::OnBnClickedButtonDelete)
	ON_BN_CLICKED(IDC_BUTTON_DELETE2, &CDlgReworkJob::OnBnClickedButtonDelete2)
END_MESSAGE_MAP()


// CDlgReworkJob 메시지 처리기


void CDlgReworkJob::OnBnClickedButtonAdd()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	// Get Slot No, Layer No
	int nSlotNo = m_ctrlSlotNo.GetCurSel();
	int nLayerNo = m_ctrlLayerNo.GetCurSel();
	int nPtnNo = m_ctrlPtnNo.GetCurSel();

	AddReworkJob(nSlotNo, nLayerNo, nPtnNo);

	UpdateReworkJobListCtrl(TRUE);
}


void CDlgReworkJob::OnBnClickedButtonDelete()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	// Get Slot No, Layer No
	int nSlotNo = m_ctrlSlotNo.GetCurSel();
	int nLayerNo = m_ctrlLayerNo.GetCurSel();

	DeleteReworkJob(nSlotNo, nLayerNo);

	UpdateReworkJobListCtrl(TRUE);
}


BOOL CDlgReworkJob::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	InitCtrlJobList();

	InitComboBox();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgReworkJob::InitCtrlJobList()
{
	LV_COLUMN add_column;
	add_column.mask = LVCF_TEXT | LVCF_WIDTH;
	add_column.pszText = L"Slot No";
	add_column.cx = 70;
	m_ctrlJobList.InsertColumn(0, &add_column);
	m_ctrlJobList.SetExtendedStyle(LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);

	// 두번재 컬럼의 이름을 설정한다.
	add_column.pszText = L"Layer No";
	add_column.cx = 70;
	// 두번째 컬럼을 추가한다.
	int iret = m_ctrlJobList.InsertColumn(1, &add_column);

	// 세번재 컬럼의 이름을 설정한다.
	add_column.pszText = L"Pattern ID";
	add_column.cx = 70;
	// 세번재 컬럼을 추가한다.
	iret = m_ctrlJobList.InsertColumn(2, &add_column);
}

void CDlgReworkJob::InitComboBox()
{
	m_ctrlSlotNo.AddString(_T("All"));
	CString strSlotNo;
	for (int i = 0; i < m_hpInfo.nSlot; i++)
	{
		strSlotNo.Format(_T("%d"), i + 1);
		m_ctrlSlotNo.AddString(strSlotNo);
	}

	m_ctrlLayerNo.AddString(_T("All"));
	CString strLayerNo;
	int nRealLayer = m_hpInfo.nLayer / 2;
	for (int i = 0; i < nRealLayer; i++)
	{
		strLayerNo.Format(_T("%d"), i + 1);
		m_ctrlLayerNo.AddString(strLayerNo);
	}

	m_ctrlPtnNo.AddString(_T("All"));
	CString strPtnNo;
	for (int i = 0; i < 2; i++)
	{
		strPtnNo.Format(_T("%d"), i + 1);
		m_ctrlPtnNo.AddString(strPtnNo);
	}
}

void CDlgReworkJob::GetReworkJob(IWS_VECTOR_REWORK_JOB &vecReworkJob)
{
	vecReworkJob = m_vecReworkJob;
}

void CDlgReworkJob::AddReworkJob(int nSlotNo, int nLayerNo, int nPtnNo)
{
	// Delete First(If No Exist Seletect Job, do Nothing)
	DeleteReworkJob(nSlotNo, nLayerNo);

	if (nSlotNo == 0 && nLayerNo == 0 && nPtnNo == 0) // All Slot & All Layer && All Ptn
	{
		// Add All Job
		ST_REWORK_JOB stReworkJob;
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++)
		{
			for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++)
			{
				for (int nPtnID = 0; nPtnID < 2; nPtnID++)
				{
					stReworkJob.nSlotNo = nSlotIndex + 1;
					stReworkJob.nLayerNo = nLayerIndex + 1;
					stReworkJob.nPtnNo = nPtnID + 1;
					m_vecReworkJob.push_back(stReworkJob);
				}
				
			}
		}
	}
	else if (nSlotNo == 0 && nLayerNo == 0 && nPtnNo != 0) // All Slot & All Layer && Partial Ptn
	{
		// Add All Job
		ST_REWORK_JOB stReworkJob;
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++)
		{
			for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++)
			{
				stReworkJob.nSlotNo = nSlotIndex + 1;
				stReworkJob.nLayerNo = nLayerIndex + 1;
				stReworkJob.nPtnNo = nPtnNo;
				m_vecReworkJob.push_back(stReworkJob);
			}
		}
	}
	else if (nSlotNo == 0 && nLayerNo != 0 && nPtnNo == 0) // All Slot, but Partial Layer & All Ptn
	{
		// Add All Selected Layer
		ST_REWORK_JOB stReworkJob;
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++) 		// All Slot
		{
			for (int nPtnID = 0; nPtnID < 2; nPtnID++)
			{
				stReworkJob.nSlotNo = nSlotIndex + 1;
				stReworkJob.nLayerNo = nLayerNo;
				stReworkJob.nPtnNo = nPtnID + 1;
				m_vecReworkJob.push_back(stReworkJob);
			}
		}
	}
	else if (nSlotNo == 0 && nLayerNo != 0 && nPtnNo != 0) // All Slot, but Partial Layer & Partial Ptn
	{
		// Add All Selected Layer
		ST_REWORK_JOB stReworkJob;
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++) 		// All Slot
		{
			stReworkJob.nSlotNo = nSlotIndex + 1;
			stReworkJob.nLayerNo = nLayerNo;
			stReworkJob.nPtnNo = nPtnNo;
			m_vecReworkJob.push_back(stReworkJob);
		}
	}
	else if (nSlotNo != 0 && nLayerNo == 0 && nPtnNo == 0) // Partial Slot, All Layer & All Ptn
	{
		// Add All Selected Slot
		ST_REWORK_JOB stReworkJob;
		for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++) 		// All Layer
		{
			for (int nPtnID = 0; nPtnID < 2; nPtnID++)
			{
				stReworkJob.nSlotNo = nSlotNo;
				stReworkJob.nLayerNo = nLayerIndex + 1;
				stReworkJob.nPtnNo = nPtnID + 1;
				m_vecReworkJob.push_back(stReworkJob);
			}
		}
	}
	else if (nSlotNo != 0 && nLayerNo == 0 && nPtnNo != 0) // Partial Slot, All Layer & Partial Ptn
	{
		// Add All Selected Slot
		ST_REWORK_JOB stReworkJob;
		for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++) 		// All Layer
		{
			stReworkJob.nSlotNo = nSlotNo;
			stReworkJob.nLayerNo = nLayerIndex + 1;
			stReworkJob.nPtnNo = nPtnNo;
			m_vecReworkJob.push_back(stReworkJob);
		}
	}
	else if(nSlotNo != 0 && nLayerNo != 0 && nPtnNo == 0) // Partial Slot, Partial Layer && All Ptn
	{
		// Add Seletect Slot & Layer's Job
		ST_REWORK_JOB stReworkJob;
		for (int nPtnID = 0; nPtnID < 2; nPtnID++)
		{
			stReworkJob.nSlotNo = nSlotNo;
			stReworkJob.nLayerNo = nLayerNo;
			stReworkJob.nPtnNo = nPtnID + 1;
			m_vecReworkJob.push_back(stReworkJob);
		}
	}
	else // Partial Slot, Partial Layer, Partial Ptn
	{
		// Add Seletect Slot & Layer's Job
		ST_REWORK_JOB stReworkJob;
		stReworkJob.nSlotNo = nSlotNo;
		stReworkJob.nLayerNo = nLayerNo;
		stReworkJob.nPtnNo = nPtnNo;
		m_vecReworkJob.push_back(stReworkJob);
	}
}

BOOL CDlgReworkJob::IsExistReworkJob(int nSlotNo, int nLayerNo, int nPtnNo)
{
	BOOL bRtn = FALSE;
	for (auto reJob : m_vecReworkJob)
	{
		if (reJob.nSlotNo == nSlotNo && reJob.nLayerNo == nLayerNo && reJob.nPtnNo == nPtnNo)
		{
			bRtn = TRUE;
			break;
		}
	}
	return bRtn;
}

void CDlgReworkJob::DeleteReworkJob(int nSlotNo, int nLayerNo, int nPtnNo)
{
	if (nSlotNo == 0 && nLayerNo == 0 && nPtnNo == 0) // All Slot & All Layer
	{
		// Delete All Reworkjob
		if (m_vecReworkJob.size() != 0)
		{
			m_vecReworkJob.clear();
		}
	}
	else if (nSlotNo == 0 && nLayerNo == 0 && nPtnNo != 0) // All Slot, All Layer, but Partial Ptn
	{
		// Delete All Slot & All Layer & Selected Ptn
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++) // All Slot
		{
			for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++) // All Layer
			{
				DeleteSelectedReworkJob(nSlotIndex + 1, nLayerIndex + 1, nPtnNo);
			}
		}
	}
	else if (nSlotNo == 0 && nLayerNo != 0 && nPtnNo == 0) // All Slot, Partial Layer, All Ptn
	{
		// Delete All Slot & Selected Layer
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++) // All Slot
		{
			for (int nPtnID = 0; nPtnID < 2; nPtnID++) // All Ptn
			{
				DeleteSelectedReworkJob(nSlotIndex + 1, nLayerNo, nPtnID + 1);
			}
		}
	}
	else if (nSlotNo == 0 && nLayerNo != 0 && nPtnNo != 0) // All Slot, Partial Layer, Partial Ptn
	{
		// Delete All Slot & Selected Layer
		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++) // All Slot
		{
			DeleteSelectedReworkJob(nSlotIndex + 1, nLayerNo, nPtnNo);
		}
	}
	else if (nSlotNo != 0 && nLayerNo == 0 && nPtnNo == 0) // Partial Slot, All Layer, All Ptn
	{
		// Delete Selected Slot & All Layer 
		for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++) // All Layer
		{
			for (int nPtnID = 0; nPtnID < 2; nPtnID++) // All Ptn
			{
				DeleteSelectedReworkJob(nSlotNo, nLayerIndex + 1, nPtnID+1);
			}
		}
	}
	else if (nSlotNo != 0 && nLayerNo == 0 && nPtnNo != 0) // Partial Slot, All Layer, Partial Ptn
	{
		// Delete Selected Slot & All Layer 
		for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++) // All Layer
		{
			DeleteSelectedReworkJob(nSlotNo, nLayerIndex + 1, nPtnNo);
		}
	}
	else if (nSlotNo != 0 && nLayerNo != 0 && nPtnNo == 0) // Partial Slot, Partial Layer, All Ptn
	{
		for (int nPtnID = 0; nPtnID < 2; nPtnID++) // All Ptn
		{
			DeleteSelectedReworkJob(nSlotNo, nLayerNo, nPtnID + 1);
		}
	}
	else // Partial Slot, Partial Layer, ParTialPtn
	{
		DeleteSelectedReworkJob(nSlotNo, nLayerNo, nPtnNo);
	}
}

void CDlgReworkJob::DeleteSelectedReworkJob(int nSlotNo, int nLayerNo, int nPtnNo)
{
	if (nSlotNo < 1 || nSlotNo > m_hpInfo.nSlot ||
		nLayerNo < 1 || nLayerNo > m_hpInfo.nLayer / 2 ||
		nPtnNo < 1 || nPtnNo > 2)
	{
		return;
	}

	IWS_VECTOR_REWORK_JOB::iterator iterJob;
	for (iterJob = m_vecReworkJob.begin(); iterJob != m_vecReworkJob.end(); iterJob++)
	{
		if (iterJob->nSlotNo == nSlotNo && iterJob->nLayerNo == nLayerNo && iterJob->nPtnNo == nPtnNo)
		{
			m_vecReworkJob.erase(iterJob);
			break;
		}
	}
}

void CDlgReworkJob::UpdateReworkJobListCtrl(BOOL bSort)
{
	// Update false
	m_ctrlJobList.UpdateData(FALSE);

	// Delete List Control All
	m_ctrlJobList.DeleteAllItems();

	if (bSort == FALSE)
	{
		// Add Job to List Ctrl
		int nListIndex = 0;
		TCHAR szItem[100] = { 0 };
		for (auto reJob : m_vecReworkJob)
		{
			// Slot No
			_stprintf_s(szItem, 100, _T("%d"), reJob.nSlotNo);
			m_ctrlJobList.InsertItem(nListIndex, szItem);

			// Layer No
			_stprintf_s(szItem, 100, _T("%d"), reJob.nLayerNo);
			m_ctrlJobList.SetItemText(nListIndex, 1, szItem);

			nListIndex++;
		}
	}
	else
	{
		int nListIndex = 0;
		TCHAR szItem[100] = { 0 };

		for (int nSlotIndex = 0; nSlotIndex < m_hpInfo.nSlot; nSlotIndex++) // All Slot
		{
			// 
			for (int nLayerIndex = 0; nLayerIndex < m_hpInfo.nLayer / 2; nLayerIndex++) // All Layer
			{
				for (int nPtnID = 0; nPtnID < 2; nPtnID++)
				{
					if (IsExistReworkJob(nSlotIndex + 1, nLayerIndex + 1, nPtnID +1))
					{
						// Slot No
						_stprintf_s(szItem, 100, _T("%d"), nSlotIndex + 1);
						m_ctrlJobList.InsertItem(nListIndex, szItem);

						// Layer No
						_stprintf_s(szItem, 100, _T("%d"), nLayerIndex + 1);
						m_ctrlJobList.SetItemText(nListIndex, 1, szItem);

						// Ptn No
						_stprintf_s(szItem, 100, _T("%d"), nPtnID + 1);
						m_ctrlJobList.SetItemText(nListIndex, 2, szItem);

						nListIndex++;
					}
				}
			}
		}
	}

	
	// Update
	m_ctrlJobList.UpdateData(TRUE);
}

void CDlgReworkJob::OnBnClickedButtonDelete2()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	for (int i = 0; i < m_ctrlJobList.GetItemCount(); i++)
	{
		if (m_ctrlJobList.GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
		{
			CString strTemp;

			int nSlotNo;
			strTemp = m_ctrlJobList.GetItemText(i, 0);
			nSlotNo = _tstoi(strTemp.GetBuffer());

			int nLayerNo;
			strTemp = m_ctrlJobList.GetItemText(i, 1);
			nLayerNo = _tstoi(strTemp.GetBuffer());

			int nPtnNo;
			strTemp = m_ctrlJobList.GetItemText(i, 2);
			nPtnNo = _tstoi(strTemp.GetBuffer());

			// Delete 
			DeleteSelectedReworkJob(nSlotNo, nLayerNo, nPtnNo);
		}
	}

	UpdateReworkJobListCtrl(TRUE);
}
