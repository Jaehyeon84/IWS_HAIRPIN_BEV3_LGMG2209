#pragma once
#include <afxdockablepane.h>
#include "IWSDoc.h"
#include "ListCtrlEx.h"

class CJobWeld;

class CJobListView :
	public CDockablePane
{
public:
	CJobListView();
	~CJobListView();
	void AdjustLayout();
	void InitCtrlJobList();
	void DeleteJob(int idx);
	void UpdateJobList(IWS_VECTOR_JOB_GROUP* pWork);
	void UpdateSelection(IWS_VECTOR_JOB_GROUP* pWork);
	void SetDocument(CIWSDoc *pDoc) { m_pDoc = pDoc; };

	void MoveUpAndDown(BOOL bUp);
	CListCtrlEx m_ctrlJoblist;
	CIWSDoc *m_pDoc;
	CMFCButton m_btnJobUp;
	CMFCButton m_btnJobDown;



	DECLARE_MESSAGE_MAP()

	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void ONMClickListListJobList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnKeydownListJobList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnGetdispinfoJoblist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnButtonJoblistDown();
	afx_msg void OnButtonJoblistUp();
	afx_msg void OnUpdateButtonJoblistDown(CCmdUI *pCmdUI);
	afx_msg void OnUpdateButtonJoblistUp(CCmdUI *pCmdUI);
};

