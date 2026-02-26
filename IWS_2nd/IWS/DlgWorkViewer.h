#pragma once

#include "CStaticCad.h"
#include "CGridJoblist.h"

// CDlgWorkViewer 대화 상자

class CIWSDoc;
class CDlgWorkViewer : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgWorkViewer)
	enum {
		PROP_WORK_NAME = 0,
		PROP_WORK_PENID,
		PROP_WORK_WELDING,
		PROP_WORK_REPEAT,
		PROP_WORK_X,
		PROP_WORK_Y,
		PROP_WORK_OFFSETX,
		PROP_WORK_OFFSETY,
		PROP_WORK_POWER,
		PROP_WROK_JUMP_SPEED,
		PROP_WROK_MARK_SPEED,
		//PROP_WORK_POWER_RAMPING,
		PROP_WORK_END
	};
public:
	void GotoScaner(double x, double y);
	void SetEnableEvent(BOOL bEnable) { m_bEnableEvent = bEnable; };
	CDlgWorkViewer(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgWorkViewer();
	//void SetWorkSpace(double sx, double sy, double cx, double cy);
	void UpdateWorkImage(CString strImagefile, double dWidth, double dHeight);
	CJob* GetJob(int idx);
	void UpdateWork();
	void UpdateCurrentSelect(CMFCPropertyGridProperty * pProp);
	void SetDocument(CIWSDoc *pDoc) { m_pDoc = pDoc; };
	void SetPropertyGrid(CMFCPropertyGridCtrl *pGrid) {
		m_pGridEntityParam = pGrid;
	}
	vector_int* pGetSelectionList();
	void DisplayProperties();
	void ClearImage();
	virtual void OnOK() {};
	virtual void OnCancel() {};
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_WORK_STAGE };
#endif
protected:
	CString m_strLastRefImg;
	double	 m_nLastImgWidth;
	double	 m_nLastImgHeight;
	CIWSDoc *m_pDoc;
	BOOL m_bEnableEvent;
	CMFCPropertyGridCtrl *m_pGridEntityParam;
	
	CMFCPropertyGridProperty* m_pItems[PROP_WORK_END];
	CMFCPropertyGridProperty* m_PowerRaping;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	virtual void PostNcDestroy();
public:
	afx_msg void OnClose();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	CStaticCad m_cadDisplay;
	//CGridJoblist m_grid;
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	afx_msg LRESULT OnUwmStaticcad(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnUwmGridjoblist(WPARAM wParam, LPARAM lParam);
};
