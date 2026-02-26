// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를 
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된 
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해 
// 추가적으로 제공되는 내용입니다.  
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.  
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은 
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// MainFrm.h: CMainFrame 클래스의 인터페이스
//

#pragma once

#include "PropertiesWnd.h"
#include "DrawPropertyView.h"
#include "DrawCmdView.h"
#include "JobListView.h"
#include "RibbonBarIWS.h"
#include "LiveCCDView.h"
#include "DlgWorkViewer.h"
#include "WorkView.h"

enum ViewDefine {
	DRAW_VIEW = 0,
	HOME_VIEW,
// 	HOME_VIEW = 0,
// 	DRAW_VIEW,
	VIEW_END
};
class CIWSDoc;
class CDispImgDlg;
class CMainFrame : public CFrameWndEx
{
	
protected: // serialization에서만 만들어집니다.
	CMainFrame() noexcept;
	DECLARE_DYNCREATE(CMainFrame)

// 특성입니다.
public:
// 작업입니다.
public:
	void InitViews();
	void SwitchView(UINT id);
	void InitializeIWS();
	void TerminateIWS();

	void ResetInterfaceFlag();
	CRibbonBarIWS *pGetRibbonBar() { return &m_wndRibbonBar; };
	void	CadMousePos(double x, double y);

	void LoadVSCanImg();
private:
// 재정의입니다.
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	CJobListView	m_wndJobListView;
	CPropertiesWnd	m_wndPropertiesView;
	CDispImgDlg*	m_pLiveCCDDlg;
	CWorkView		  m_wndWorkView;
	BOOL	m_bInitCadFrame;
	CRibbonBarIWS     m_wndRibbonBar;
	CView * m_pViews[VIEW_END];

protected:  // 컨트롤 모음이 포함된 멤버입니다.
	HACCEL  m_hAccelTable;
	UINT m_nCurView;

	CMFCRibbonApplicationButton m_MainButton;
	CMFCToolBarImages m_PanelImages;
	CMFCRibbonStatusBar  m_wndStatusBar;
	CMFCRibbonStatusBarPane* m_wndStatusBarPaneXY;
	//CDrawPropertyView	 m_wndDrawProp;
	CDrawCmdView      m_wndDrawCmdView;
	CIWSDoc *m_pDoc;
// 생성된 메시지 맵 함수
public:
	void LaserpenHiddern();
protected:
	void FitWorkAreaToFrame();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	DECLARE_MESSAGE_MAP()

	BOOL CreateDockingWindows();
	void SetDockingWindowIcons(BOOL bHiColorIcons);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
protected:
	afx_msg LRESULT OnUwmActiveRibbonTabChanged(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUwmCIWSwork(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnAccHomeview();
	afx_msg void OnAccDrawview();
protected:
	afx_msg LRESULT OnInterfaceThread(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnClose();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnAccViewHome();
	afx_msg void OnAccVscan();
	afx_msg void OnAccViewDrw();
	afx_msg void OnAccLaserpen();
	afx_msg void OnAccRefimgSetting();
	afx_msg void OnAccWeldonce();
	afx_msg void OnFileSetVscan();
};


