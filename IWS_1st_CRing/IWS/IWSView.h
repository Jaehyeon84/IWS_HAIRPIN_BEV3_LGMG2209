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

// IWSView.h: CIWSView 클래스의 인터페이스
//

#pragma once
#define UWM_CIWSVIEW_MSG _T("CIWSView_message")
#define CIWSVIEWMSG_ENT_UPDATE 1
// Base class for all commands
//-----------------------------------------------
class CCmdBase {
	int    m_ID;              // command ID
	WCHAR  m_szName[32];      // command name
protected:
	bool   m_bNeedSelection;  // TRUE if a command requires selected objects
	HANDLE m_hCmd;            // command handle
	HANDLE m_hLcWnd;
public:
	CCmdBase(int Id, LPCWSTR szName, bool szNeedSelection);

	virtual void Start(int Prm) = 0;
	virtual void Finish() = 0;
	virtual void MouseDown(int Button, int Flags, int Xwin, int Ywin, double X, double Y) = 0;
	virtual void MouseUp(int Button, int Flags, int Xwin, int Ywin, double X, double Y) = 0;
	virtual void MouseMove(HDC hDC, int Flags, int Xwin, int Ywin, double X, double Y) = 0;

	bool Create(HANDLE hLcWnd);
	bool IsHandle(HANDLE hCmd) const;
};

class CIWSView : public CView
{
protected: // serialization에서만 만들어집니다.
	CIWSView() noexcept;
	DECLARE_DYNCREATE(CIWSView)

// 특성입니다.
public:
	CIWSDoc* GetDocument() const;
// 작업입니다.
	void SetWorkArea();
	void SetDrwNew();
	void SortLayerOrder();
	
	void UpdateHapinTxt(HANDLE hBlock);

public:
	HANDLE m_hRefImg;
	HANDLE m_hLayerImage;
	HANDLE m_hWorkArea;

	BOOL m_bModeAddSpiral;
	BOOL m_bDisableCadEvent;

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	void GotoScaner(double x, double y);
	int ShowRefImage();
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// 구현입니다.
public:
	virtual ~CIWSView();
	void Test();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	int m_alignLast;
// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnButtonCadLine();
	afx_msg void OnButtonCadPolyline();
	afx_msg void OnButtonCadCircle();
	afx_msg void OnButtonCadArc();
	afx_msg void OnButtonCadExtend();
	afx_msg void OnButtonCadTrim();
	afx_msg void OnButtonCadOffset();
	afx_msg void OnButtonCadRound();
	afx_msg void OnButtonZoomExtents();
	afx_msg void OnButtonCadUndo();
	afx_msg void OnButtonCadRedo();
	afx_msg void OnButtonZoomBox();
	afx_msg void OnButtonZoomPrev();
	afx_msg void OnButtonCadPoint();
	afx_msg void OnButtonLaserPens();
	afx_msg void OnButtonLoadScoutimg();
	afx_msg void OnButtonCadSpiral();
	afx_msg void OnButtonCadRect();
	virtual void OnInitialUpdate();
	afx_msg void OnButtonZoomFittowork();
	afx_msg void OnButtonEllipse();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnButtonCadHatch();
	afx_msg void OnButtonWeldpath();
//	afx_msg void OnButtonCadMerge();
	afx_msg void OnButtonCadJoin();
	afx_msg void OnButtonCadCopy();
	afx_msg void OnButtonCadSnap();
	afx_msg void OnButtonCopymenu();
	afx_msg void OnUpdateButtonCopymenu(CCmdUI *pCmdUI);
	afx_msg void OnButtonCadCopyCircle();
	afx_msg void OnButtonCadCopyRect();
	afx_msg void OnButtonCadAlignBottom();
	afx_msg void OnButtonCadAlignCenter();
	afx_msg void OnButtonCadAlignLeft();
	afx_msg void OnButtonCadAlignRight();
	afx_msg void OnButtonCadAlignTop();
	afx_msg void OnButtonCadKbmove();
	afx_msg void OnButtonCadAlignment();
	afx_msg void OnButtonHairpin();	
	afx_msg void OnFileHpConfig();
	afx_msg void OnButtonPattern();
	afx_msg void OnHairpinOffset();
	afx_msg void OnWeld();
protected:
	afx_msg LRESULT OnUwmCiwsview(WPARAM wParam, LPARAM lParam);
};

#ifndef _DEBUG  // IWSView.cpp의 디버그 버전
inline CIWSDoc* CIWSView::GetDocument() const
   { return reinterpret_cast<CIWSDoc*>(m_pDocument); }
#endif

