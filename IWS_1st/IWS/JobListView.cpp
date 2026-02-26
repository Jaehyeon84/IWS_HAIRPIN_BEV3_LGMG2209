#include "stdafx.h"
#include "IWS.h"
#include "JobListView.h"
#include "MainFrm.h"

CJobListView::CJobListView()
{
	m_pDoc = NULL;
}


CJobListView::~CJobListView()
{

}

void CJobListView::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);
 	int btnHeight = 30;
 	int btnWid = rectClient.Width()/2;
 	CPoint pos;
	pos.x = 0;
 	pos.y = btnHeight;
	int h = rectClient.Height() - pos.y;
 	m_btnJobDown.MoveWindow(pos.x, 0, btnWid, btnHeight);
	m_btnJobUp.MoveWindow(pos.x + btnWid, 0, btnWid, btnHeight);
	m_ctrlJoblist.MoveWindow(0, pos.y, rectClient.Width(), h);
	//m_gridJob.MoveWindow(0, pos.y + h / 2, rectClient.Width(), h / 2);
	//m_ctrlJoblist.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), rectClient.Height(), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CJobListView::InitCtrlJobList()
{
	LV_COLUMN add_column;
	add_column.mask = LVCF_TEXT | LVCF_WIDTH;
	add_column.pszText = L"ID";
	add_column.cx = 30;
	m_ctrlJoblist.InsertColumn(0, &add_column);

	// 두번재 컬럼의 이름을 설정한다.
	add_column.pszText = L"Job Name";
	add_column.cx = 100;
	// 두번째 컬럼을 추가한다.
	int iret = m_ctrlJoblist.InsertColumn(1, &add_column);

	// 두번재 컬럼의 이름을 설정한다.
	add_column.pszText = L"Pen No.";
	add_column.cx = 100;
	// 두번째 컬럼을 추가한다.
	m_ctrlJoblist.InsertColumn(2, &add_column);

	// 두번재 컬럼의 이름을 설정한다.
	add_column.pszText = L"Mark.";
	add_column.cx = 80;
	// 두번째 컬럼을 추가한다.
	m_ctrlJoblist.InsertColumn(3, &add_column);

}

void CJobListView::DeleteJob(int idx)
{
	if (idx < 0) return;
	int nCnt = m_ctrlJoblist.GetItemCount();
	if (idx > nCnt) return;
	m_ctrlJoblist.DeleteItem(idx);
}

void CJobListView::UpdateJobList(IWS_VECTOR_JOB_GROUP* pWork)
{
//	m_ctrlJoblist.DeleteAllItems();
//	if (!pWork) return;
//	int nWorks = (int)pWork->size();
//	for (int i = 0; i < nWorks;i++) {
//		AddJob((*pWork)[i]);
//	}
	if (!pWork) {
		m_ctrlJoblist.DeleteAllItems();
		return;
	}

	int nSize = 0;
	for (auto group : (*pWork))
		nSize += (int)group->m_jobs.size();

	m_ctrlJoblist.SetItemCount(nSize);
}

void CJobListView::UpdateSelection(IWS_VECTOR_JOB_GROUP* pWork)
{
	if (!pWork) 
		return;

	int nWork = 0;
	for (auto group : (*pWork))
		nWork += (int)group->m_jobs.size();

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = lcBlockGetFirstSel(hBlock);
	m_ctrlJoblist.SetItemState(-1, 0, LVIS_SELECTED);
	int nIdx = 0;
	int subIdx = 0;
	while (hEnt)
	{
		nIdx = 0;
		for (auto group : (*pWork))
		{
			subIdx = 0;
			if (group->GetSelectEntity(hEnt, &subIdx))
			{
				nIdx += subIdx;

				m_ctrlJoblist.SetItemState(nIdx, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);
				m_ctrlJoblist.EnsureVisible(nIdx, TRUE);

				break;
			}

			nIdx += subIdx;
		}
		hEnt = lcBlockGetNextSel(hBlock);
	}
	
}

BEGIN_MESSAGE_MAP(CJobListView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(NM_CLICK, IDC_LIST_JOBLIST, &CJobListView::ONMClickListListJobList)
	ON_NOTIFY(LVN_KEYDOWN, IDC_LIST_JOBLIST, &CJobListView::OnLvnKeydownListJobList)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LIST_JOBLIST, &CJobListView::OnGetdispinfoJoblist)
	ON_COMMAND(ID_BUTTON_JOBLIST_DOWN, &CJobListView::OnButtonJoblistDown)
	ON_COMMAND(ID_BUTTON_JOBLIST_UP, &CJobListView::OnButtonJoblistUp)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_JOBLIST_DOWN, &CJobListView::OnUpdateButtonJoblistDown)
	ON_UPDATE_COMMAND_UI(ID_BUTTON_JOBLIST_UP, &CJobListView::OnUpdateButtonJoblistUp)
END_MESSAGE_MAP()

int CJobListView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE);
	SetControlBarStyle(dwStyle);
	CRect rectDummy;
	rectDummy.SetRectEmpty();
	const DWORD dwViewStyle = LVS_OWNERDATA | WS_CHILD | WS_VISIBLE | LVS_REPORT | LVS_EDITLABELS | LVS_SHOWSELALWAYS;
	m_ctrlJoblist.Create(dwViewStyle, rectDummy, this, IDC_LIST_JOBLIST);
	m_ctrlJoblist.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
	m_ctrlJoblist.SetShowSelectedItemUnFocus(TRUE);
	m_ctrlJoblist.SetPenStyle(PS_DOT);
	m_ctrlJoblist.SetUnFocusColors(RGB(0, 250, 0), RGB(255, 0, 255));
	m_ctrlJoblist.ShowWindow(TRUE);

	m_btnJobUp.Create(_T("Up"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rectDummy, this, ID_BUTTON_JOBLIST_UP);
	m_btnJobDown.Create(_T("Down"), WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, rectDummy, this, ID_BUTTON_JOBLIST_DOWN);
	m_btnJobUp.ShowWindow(SW_SHOW);
	m_btnJobDown.ShowWindow(SW_SHOW);
	m_btnJobUp.EnableWindow(TRUE);
	m_btnJobDown.EnableWindow(TRUE);

	AdjustLayout();
	InitCtrlJobList();
	return 0;
}


void CJobListView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
	m_btnJobUp.EnableWindow(TRUE);
	m_btnJobDown.EnableWindow(TRUE);
}

void CJobListView::ONMClickListListJobList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	if (!m_pDoc) {
		CMainFrame *pFrm = (CMainFrame*)this->GetParent();
		m_pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	}
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEntity = NULL;
	int nSelect = m_ctrlJoblist.GetSelectedCount();
	lcEventsEnable(FALSE);
	lcBlockUnselect(hBlock);

	int nItem = m_ctrlJoblist.GetItemCount();
	BOOL bSelect;
	CJob *pJob;
	CPropertiesWnd *pwndProp = &(m_pDoc->m_pMainFrm->m_wndPropertiesView);
	IWS_VECTOR_JOB sel;
	for (int i = 0; i < nItem; i++) {
		bSelect = m_ctrlJoblist.GetItemState(i, LVIS_SELECTED);
		pJob = m_pDoc->m_work.GetJob(i);
		if (pJob->GetJobType() != JOB_TYPE_WELD) continue;
		if (bSelect) {
			sel.push_back(pJob);
		}
// 		if (bSelect)
// 			pwndProp->ShowJobProperty((CJobWeld*)pJob);
 		hEntity = (CJobWeld*)pJob->GetEntity()->m_hCadEntity;
		lcBlockSelectEnt(hBlock, hEntity, bSelect);
	}
	int nSel = (int)sel.size();
	if (nSel == 1)
		pwndProp->ShowJobProperty(sel[0]);
	else if (nSel > 1)
		pwndProp->ShowJobProperty(&sel);
	lcEventsEnable(TRUE);
	lcWndRedraw(g_hLcWnd);
}

void CJobListView::OnLvnKeydownListJobList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLVKEYDOWN pLVKeyDow = reinterpret_cast<LPNMLVKEYDOWN>(pNMHDR);
	if (!m_pDoc) {
		CMainFrame *pFrm = (CMainFrame*)this->GetParent();
		m_pDoc = (CIWSDoc *)pFrm->GetActiveDocument();
	}
	switch (pLVKeyDow->wVKey)
	{
	case VK_DELETE:
	{
		if (theApp.m_iwsMode == CIWSApp::IWSMODE_HAIRPIN) 
			break;

		POSITION    pos = NULL;
		pos = m_ctrlJoblist.GetFirstSelectedItemPosition();
		int nItemidx = -1;
		std::vector<HANDLE> del_list;
		CJob *pJob;
		while (pos)
		{
			nItemidx = m_ctrlJoblist.GetNextSelectedItem(pos);
			pJob = m_pDoc->m_work.GetJob(nItemidx);
			if(pJob->GetJobType() != JobType::JOB_TYPE_WELD) continue;

			del_list.push_back(pJob->GetEntity()->m_hCadEntity);
		}

		int nDelList = (int)del_list.size();
		if (!nDelList) return;

		lcEventsEnable(FALSE);
		HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
		BOOL bDelete = FALSE;

		for (auto del : del_list) {
			bDelete = FALSE;
			if (lcPropGetInt(del, LC_PROP_ENT_TYPE) == LC_ENT_BLOCKREF) {
				HANDLE hRef = lcPropGetHandle(del, LC_PROP_BLKREF_BLOCK);
				lcEntErase(del, TRUE);
// 				lcBlockClear(hRef, NULL);
// 				lcBlockPurge(hRef);
				lcDrwDeleteObject(g_hLcDrw, hRef);
			}
			else {
				lcEntErase(del, TRUE);
			}
			
			m_pDoc->m_work.DeleteJob(del);
		}
		lcBlockUnselect(hBlock);
		lcBlockPurge(hBlock);
		UpdateJobList(m_pDoc->m_work.GetJobs());
		lcEventsEnable(TRUE);
		lcWndRedraw(g_hLcWnd);
	}
	}
}


void CJobListView::OnGetdispinfoJoblist(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	LV_ITEM* pItem = &(pDispInfo)->item;

	//Which item number?
	int itemid = pItem->iItem;

	//Do the list need text information?
	if (pItem->mask & LVIF_TEXT)
	{
		CString text;
		CJob *pJob = m_pDoc->m_work.GetJob(itemid);
		if (!pJob) return ;
		//Which column?
		switch (pItem->iSubItem)
		{
		case 0:
			text.Format(L"%d", itemid);
			break;
		case 1:
			text = pJob->GetJobName();
			break;
		case 2:
			ASSERT(pJob->GetJobType() == JobType::JOB_TYPE_WELD);
			text.Format(L"Pen%d", ((CJobWeld*)pJob)->m_iPenID);
			break;
		case 3:
			if (pJob->GetDoJob())
				text = L"True";
			else
				text = L"False";
		}
		
		//Copy the text to the LV_ITEM structure
		//Maximum number of characters is in pItem->cchTextMax
		lstrcpyn(pItem->pszText, text, pItem->cchTextMax);
	}


	*pResult = 0;
}

void CJobListView::OnButtonJoblistDown()
{
	MoveUpAndDown(FALSE);
}


void CJobListView::OnButtonJoblistUp()
{
	MoveUpAndDown(TRUE);
}

void CJobListView::MoveUpAndDown(BOOL bUp)
{
	// hslee 
	if (m_ctrlJoblist.GetSelectedCount() != 1)
		return;

	int nCellNo = m_ctrlJoblist.GetNextItem(-1, LVNI_SELECTED);
	int nNextCellNo = bUp ? nCellNo - 1 : nCellNo + 1;
	if (nNextCellNo < 0 || nNextCellNo >= m_ctrlJoblist.GetItemCount())
		return;

	/*CString strID, strJobName, strPenNo, strMark;
	strID = m_ctrlJoblist.GetItemText(nCellNo, 0);
	strJobName = m_ctrlJoblist.GetItemText(nCellNo, 1);
	strPenNo = m_ctrlJoblist.GetItemText(nCellNo, 2);
	strMark = m_ctrlJoblist.GetItemText(nCellNo, 3);

	m_ctrlJoblist.DeleteItem(nCellNo);
	m_ctrlJoblist.InsertItem(nNextCellNo, strID);
	m_ctrlJoblist.SetItemText(nNextCellNo, 1, strJobName);
	m_ctrlJoblist.SetItemText(nNextCellNo, 2, strPenNo);
	m_ctrlJoblist.SetItemText(nNextCellNo, 3, strMark);
	m_ctrlJoblist.SetItemState(nNextCellNo, LVNI_SELECTED, LVIS_SELECTED);*/
	
	m_pDoc->m_work.Swap(nCellNo, nNextCellNo);
	m_ctrlJoblist.SetItemState(nCellNo, 0,LVIS_SELECTED);
	m_ctrlJoblist.SetItemState(nNextCellNo, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED);

	UpdateJobList(m_pDoc->m_work.GetJobs());
}

void CJobListView::OnUpdateButtonJoblistDown(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}


void CJobListView::OnUpdateButtonJoblistUp(CCmdUI *pCmdUI)
{
	// TODO: 여기에 명령 업데이트 UI 처리기 코드를 추가합니다.
}
