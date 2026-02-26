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

#pragma once
#include <afxdockablepane.h>
#include "IWSDoc.h"
class CJobWeld;
class CPropertiesToolBar : public CMFCToolBar
{
public:
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CPropertiesWnd : public CDockablePane
{
	enum {
		PROP_WORK_NAME = 0,
		PROP_WORK_PENID,
		PROP_WORK_WELDING,
		PROP_WORK_REPEAT,
		PROP_WORK_OFFSETX,
		PROP_WORK_OFFSETY,
		PROP_WORK_CIRCLE_STARTANG,
		PROP_WORK_CIRCLE_OVERLAP,
		PROP_WORK_CIRCLE_DIA,
		PROB_WORK_CIRCLE_DIR,
		//PROP_WORK_POWER_RAMP,
		PROP_WORK_END
	};
// 생성입니다.
public:
	CPropertiesWnd() noexcept;

	void AdjustLayout();
	void ShowJobProperty(CJob *pJob);
	void ShowJobProperty(IWS_VECTOR_JOB *pJobs);
	void SetDocument(CIWSDoc *pDoc) { m_pDoc = pDoc; };
	void SetRptCntTemp(int nRptCnt); // sjyi 2022-06-07 Repeat No 저장용	
// 특성입니다.
public:
	void SetVSDotNetLook(BOOL bSet)
	{
		m_wndPropList.SetVSDotNetLook(bSet);
		m_wndPropList.SetGroupNameFullWidth(bSet);
	}

protected:
	CIWSDoc *m_pDoc;
	CJob *m_pJob;
	IWS_VECTOR_JOB m_pJobs;
	CFont m_fntPropList;
	CComboBox m_wndObjectCombo;
	CPropertiesToolBar m_wndToolBar;
	CMFCPropertyGridCtrl m_wndPropList;
	CMFCPropertyGridProperty* m_pItems[PROP_WORK_END];
	int m_nRptCntTemp; // sjyi 2022-06-07 현재 RptCount값을 확인하기 위한 변수
	//CMFCPropertyGridProperty* m_PowerRaping;
// 구현입니다.
public:
	virtual ~CPropertiesWnd();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSettingChange(UINT uFlags, LPCTSTR lpszSection);
	afx_msg LRESULT OnPropertyChanged(WPARAM wparam, LPARAM lparam);
	DECLARE_MESSAGE_MAP()

	void InitPropList();
	void SetPropListFont();

	int m_nComboHeight;
};


