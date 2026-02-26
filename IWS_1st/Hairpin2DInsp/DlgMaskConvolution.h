#pragma once


// CDlgMaskConvolution 대화 상자

class CDlgMaskConvolution : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgMaskConvolution)

public:
	CDlgMaskConvolution(BOOL bUseMaskConvolution, long nMaskWidth1, long nMaskHeight1, long nMaskWidth2, long nMaskHeight2, float fRef1, float fRef2, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgMaskConvolution();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_MASK_CONVOLUTION };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CButton m_chkUseMaskConvolution;
	BOOL m_bUseMaskConvolution;
	long m_nMaskWidth1;
	long m_nMaskWidth2;
	long m_nMaskHeight1;
	long m_nMaskHeight2;
	float m_fRef1;
	float m_fRef2;

	afx_msg void OnBnClickedOk();
};
