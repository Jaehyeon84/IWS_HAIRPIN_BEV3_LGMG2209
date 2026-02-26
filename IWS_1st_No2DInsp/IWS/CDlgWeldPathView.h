#pragma once

#include "CStaticCadPathView.h"

// CDlgWeldPathView 대화 상자

class CDlgWeldPathView : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWeldPathView)

public:
	CDlgWeldPathView(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgWeldPathView();
	void SetCadBlock(HANDLE hBlock) { m_hBlock = hBlock; };
	void AddEntity(HANDLE hEnt);
	void SetJobs(IWS_VECTOR_JOB* pJobs);
	void SetWndSize(double w, double h) {
		m_wndSizeW = w;
		m_wndSizeH = h;
	};
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WELDPATH };
#endif

protected:
	
	double m_wndSizeW, m_wndSizeH;
	HANDLE m_hBlock;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CStaticCadPathView m_cadPathView;
	virtual BOOL OnInitDialog();
};
