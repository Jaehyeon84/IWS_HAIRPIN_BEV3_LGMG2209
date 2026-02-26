#pragma once
#include "./util/AutoEdit.h"

// CDlgLaserPrgNo 대화 상자

class CDlgLaserPrgNo : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgLaserPrgNo)

public:
	CDlgLaserPrgNo(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgLaserPrgNo();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_LASER_PRGNO };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()

public:
	int m_nValue;

public:
	CAutoEditInt m_editPrgNo;
	afx_msg void OnBnClickedButtonChange();
};
