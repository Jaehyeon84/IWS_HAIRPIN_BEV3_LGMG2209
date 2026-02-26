#pragma once

#include "GridCtrl/GridCtrl.h"


// CDlgSortieMap 대화 상자

class CDlgSortieMap : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgSortieMap)

// 멤버 변수 --------------------------------------------------------
protected:
	CGridCtrl m_gridSortieMap;
	int m_nSortieRptMap[MAX_NUM_REPEAT_WELD];	// sjyi 2025.09.10 Sortie Rpt Map 추가

public:
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_SORTIE_MAP };
#endif


// 멤버 함수 --------------------------------------------------------
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);

	void InitGrid();
public:
	CDlgSortieMap(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgSortieMap();
	virtual BOOL OnInitDialog();

	void SetSortieRptMap(int* pMap);
	void GetSortieRptMap(int* pMap);

};
