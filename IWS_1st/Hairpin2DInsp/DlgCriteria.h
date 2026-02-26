#pragma once

class CHairpin2DInspDlg;
class CDetectionAlgorithm;

// CDlgCriteria 대화 상자

class CDlgCriteria : public CDialog
{
	DECLARE_DYNAMIC(CDlgCriteria)

public:
	CDlgCriteria(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgCriteria();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_CRITERIA };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	CHairpin2DInspDlg* m_pParent;
	CImageDisplay* m_pImageDisplay;
	CDetectionAlgorithm* m_pDetectionAlgorithm;

	float m_fMin_C;
	float m_fMax_C;
	float m_fMin_T;
	float m_fMax_T;
	float m_fMin_J;
	float m_fMax_J;
	float m_fMin_H12;
	float m_fMax_H12;
	float m_fMax_H3;
	CListCtrl m_listResult;
	CButton m_checkReplace3DResult;
	BOOL m_bOriginalImageNow;
	BOOL m_bUnrolledImageNow;

	void UpdateUI();
	void UpdateParameters();
	void Inspection();
	void SetOverlay(const long slot, const long layer, const long layerCount, const BOOL bDBClick);

	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnBnClickedBtnOriginalImage2();
	afx_msg void OnBnClickedBtnUnrolledImage2();
	afx_msg void OnBnClickedBtnUpdateResult();
	afx_msg void OnLvnItemchangedListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedCheckReplace3dResult();
	afx_msg void OnNMDblclkListResult(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnWholeImage();
};
