#pragma once

class CHairpin2DInspDlg;
class CDetectionAlgorithm;

// CDlgMakeUnrolled 대화 상자

class CDlgMakeUnrolled : public CDialog
{
	DECLARE_DYNAMIC(CDlgMakeUnrolled)

public:
	CDlgMakeUnrolled(CHairpin2DInspDlg* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMakeUnrolled();

// 대화 상자 데이터입니다.
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MAKE_UNROLLED };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	CHairpin2DInspDlg* m_pParent;
	CDetectionAlgorithm* m_pDetectionAlgorithm;

	long m_nCenterPosCorrectionX;
	long m_nCenterPosCorrectionY;
	float m_fUnrolledStartAngle;
	long m_nUnrolledStartOffsetY;
	long m_nUnrolledLength;

	void UpdateUI();
	void UpdateParameters();

	afx_msg void OnBnClickedBtnApply();
	afx_msg void OnBnClickedBtnInspection();
	afx_msg void OnBnClickedBtnOriginalImage();
	afx_msg void OnBnClickedBtnUnrolledImage();
	float m_fSubOriginalWidth_mm;
	float m_fSubOriginalHeight_mm;
};
