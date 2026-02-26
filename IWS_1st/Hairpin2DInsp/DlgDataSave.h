#pragma once


// CDlgDataSave 대화 상자

class CDlgDataSave : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgDataSave)

public:
	CDlgDataSave(BOOL bSaveRawImage, CString strRawImageSavePath, BOOL bSaveResultData, CString strResultDataSavePath, BOOL bSaveResultImage, CString strResultImageSavePath, BOOL bSaveResultImageForJigA, CString strResultImageSavePathForJigA, BOOL bSaveResultImageForJigB, CString strResultImageSavePathForJigB, CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgDataSave();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DLG_DATA_SAVE };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()

public:
	BOOL m_bSaveRawImage;
	CButton m_checkSaveResultImage;
	CString m_strResultImageSavePath;
	BOOL m_bSaveResultData;
	CButton m_checkSaveResultData;
	CString m_strResultDataSavePath;
	BOOL m_bSaveResultImage;
	CButton m_checkSaveRawImage;
	CString m_strRawImageSavePath;
	BOOL m_bSaveResultImageForJigA;
	CButton m_checkSaveResultImageForJigA;
	CString m_strResultImageSavePathForJigA;
	BOOL m_bSaveResultImageForJigB;
	CButton m_checkSaveResultImageForJigB;
	CString m_strResultImageSavePathForJigB;
	
	afx_msg void OnBnClickedBtnSaveRawImage();
	afx_msg void OnBnClickedBtnSaveResultData();
	afx_msg void OnBnClickedBtnSaveResultImage();
	afx_msg void OnBnClickedOk();
};
