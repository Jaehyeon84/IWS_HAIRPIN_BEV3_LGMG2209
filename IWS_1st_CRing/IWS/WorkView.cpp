#include "stdafx.h"
#include "IWS.h"
#include "WorkView.h"
#include "DlgWorkViewer.h"

CWorkView::CWorkView()
{
	m_pWorkView = NULL;
}

CWorkView::~CWorkView()
{	
}

BEGIN_MESSAGE_MAP(CWorkView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CWorkView::SetCurrent(int idx)
{
	//m_pWorkView->m_grid.EnableEvent(FALSE);
	//m_pWorkView->m_grid.ClearSelections();
	//if(idx>=0)
	//	m_pWorkView->m_grid.GotoRow(idx);
	//m_pWorkView->m_grid.HideCurrentCell();
	//m_pWorkView->m_grid.EnableEvent(TRUE);
}

void CWorkView::InitPane(CIWSDoc *pDoc)
{
	m_pWorkView = new CDlgWorkViewer;
	m_pWorkView->SetDocument(pDoc);
	m_pWorkView->Create(IDD_DIALOG_WORK_STAGE, this);
	m_pWorkView->ShowWindow(SW_SHOW);
}

void CWorkView::UpdateWork()
{
	m_pWorkView->UpdateWork();
}

void CWorkView::CadSelectEvent(HANDLE hEvent)
{
	m_pWorkView->m_cadDisplay.OnSelect(hEvent);
}

void CWorkView::SetPropertyGrid(CMFCPropertyGridCtrl* pGrid)
{

	m_pWorkView->SetPropertyGrid(pGrid);
}

void CWorkView::UpdateCurrentSelect(CMFCPropertyGridProperty * pProp)
{
	m_pWorkView->UpdateCurrentSelect(pProp);
}

vector_int * CWorkView::pGetSelectionList()
{
	 return m_pWorkView->pGetSelectionList(); 
}

void CWorkView::UpdateWorkImage(CString strImagefile, double dWidth, double dHeight)
{
	m_pWorkView->UpdateWorkImage(strImagefile, dWidth, dHeight);
}

void CWorkView::ClearWorkImage()
{
	m_pWorkView->ClearImage();
}

void CWorkView::SetWorkspace(double sx, double sy, double cx, double cy)
{
	m_pWorkView->m_cadDisplay.SetWorkSpace(sx, sy, cx, cy);
	UpdateWork();
}

int CWorkView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  ���⿡ Ư��ȭ�� �ۼ� �ڵ带 �߰��մϴ�.
	DWORD dwStyle = GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE);
	SetControlBarStyle(dwStyle);
	return 0;
}


void CWorkView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	if (m_pWorkView) {
		m_pWorkView->MoveWindow(0, 0, cx, cy);
	}
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


BOOL CWorkView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	
	return CDockablePane::PreTranslateMessage(pMsg);
}
