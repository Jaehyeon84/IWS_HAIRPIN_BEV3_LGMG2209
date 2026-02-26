#pragma once

#include "CGridHairpinOffset.h"
// CDlgHairpinOffset 대화 상자

class CDlgHairpinOffset : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgHairpinOffset)

public:
	CDlgHairpinOffset(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgHairpinOffset();

	void InitGrid();

public:
	CGridHairpinOffset m_grid;
	IWS_HAIRPIN_OFFSET m_hpOffset[512];
	IWS_HAIRPIN_OFFSET m_hpOffset2D[512];

	BOOL m_b3DUse;

	int m_nSlot, m_nLayer;
	
// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_HAIRPIN_OFFSET };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	double m_dBaseR;
	double m_dBaseH;
	double m_dHRRatio;
	double m_dCenOffX;
	double m_dCenOffY;

	afx_msg void OnClickedRadioRcp3d();
	afx_msg void OnClickedRadioRcp2d();

	afx_msg void OnBnClickedButtonAllSet();
	double m_dEditAllNum;

	BOOL m_bEnable;
	afx_msg void OnBnClickedButtonRcpEnable();
};
