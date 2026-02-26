#pragma once


// CDlgVScanCalib_ConfGrid 대화 상자

#include ".\VRSaux\WorkImg.h"
#include "CImageMerging.h"
#include "CStaticChart.h"


class CDlgVScanCalib_ConfGrid : public CDialogEx
{
	DECLARE_DYNAMIC(CDlgVScanCalib_ConfGrid)

	//class CChart :
	//	public CChartViewer
	//{
	//public:
	//	CChart();
	//	~CChart();
	//	void DrawChart();
	//	void trackLineLabel(CD_NAMESPACE::XYChart *c, int mouseX, int mouseY);
	//	virtual void MouseMovePlotArea();
	//	virtual void ViewPortChanged();
	//	void SetAxisTitle(CString sX, CString sY, CString sLegend);
	//	double *m_pdata;
	//	double *m_px;
	//	int m_nData;
	//
	//	void SetData(double *px, double *py, int nLen);
	//protected:
	//	CStringA m_strTitleX;
	//	CStringA m_strTitleY;
	//	CStringA m_strLegend;
	//};


public:
	CDlgVScanCalib_ConfGrid(CWnd* pParent = nullptr);   // 표준 생성자입니다.
	virtual ~CDlgVScanCalib_ConfGrid();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_VSCANING_CALIB_CONF_GRID };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	CWorkImg* m_pImgWnd;
	Image* m_pImg;
	GridRectParam m_pGridParam;

	CComboBox m_cboTarget;
	CEdit m_eLeft;
	CEdit m_eTop;
	CEdit m_eWidth;
	CEdit m_eHeight;
	CStatic m_StDir;
	CEdit m_eStrength;

	void ExchangeParam(BOOL bLoad, GridLineParam* pLineParam);
	GridLineInfo FindGridLine(GridLineParam pLineParam);
	afx_msg void OnBnClickedButtonRoiApply();
	afx_msg void OnBnClickedButtonFindAll();
	
	afx_msg void OnCbnSelchangeComboTargetLine();


	StraightEdgeReport2* m_pResultOut;
	StraightEdgeReport2* m_pResultIn;
	virtual void OnOK();
	virtual void OnCancel();
	CComboBox m_cboSearchLine;
	afx_msg void OnCbnSelchangeComboSearchLine();
	CStaticChart m_stChart;


	afx_msg void OnBnClickedButtonZoomP();
	afx_msg void OnBnClickedButtonZoomFit();
	afx_msg void OnBnClickedButtonZoomN();
	float m_fZoomScale;
	float m_fZoomScaleUnit;
	Point m_fZoomCen;

};
