#pragma once
#include <afxdockablepane.h>
class CDlgWorkViewer;
class CIWSDoc;
class CWorkView :
	public CDockablePane
{
public:
	CWorkView();
	~CWorkView();

	CDlgWorkViewer *m_pWorkView;
	void SetCurrent(int idx);
	void InitPane(CIWSDoc *pDoc);
	void UpdateWork() ;
	void CadSelectEvent(HANDLE hEvent);
	void SetPropertyGrid(CMFCPropertyGridCtrl* pGrid);
	void UpdateCurrentSelect(CMFCPropertyGridProperty * pProp);
	vector_int *pGetSelectionList() ;
	void UpdateWorkImage(CString strImagefile, double dWidth, double dHeight);
	void ClearWorkImage();
	void SetWorkspace(double sx, double sy, double cx, double cy);
public:
	DECLARE_MESSAGE_MAP()
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

