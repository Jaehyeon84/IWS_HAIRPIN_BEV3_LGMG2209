#pragma once
#include <afxwin.h>
#include <vector>
#include "Job.h"
#define UWM_CSTATICCADCCDVIEW_MSG _T("CStaticCadCCDView")

#define CAD_EVENT_MOUSEMOVE 1
#define CAD_EVENT_ENTSENECT 2
#define CAD_EVENT_ENTCHANGE 3
#define CAD_EVENT_LBDOWN    4
#define CAD_EVENT_SELECTCLEAR 5
typedef std::vector<HANDLE> VECTOR_HANDLE;

class CStaticCadPathView :
	public CStatic
{
public:
	CRITICAL_SECTION m_csImage;
	CStaticCadPathView();
	~CStaticCadPathView();
	void SetImageU8(BYTE* pImg, int w, int h);
	void SetImageRGB(BYTE* pImg, int w, int h);
	void OnPaint(HANDLE hEvent);
	void OnMouseMove(HANDLE hEvent);
	void OnLButtonDown(HANDLE hEvent);
	bool Init();
	void Update();
	void CreatePointBuff(HANDLE hBlock);
	void CreatePointBuffV();
	void DrawPointBuff(HANDLE hEvent);
	void AddEntity(HANDLE hEnt);
	void SetWorkSize(double x, double y, double w, double h);
	void CreatePointBuffFromJobs();

	std::vector<CJob*>* m_pJobs;

private:
	HWND m_hRcvWnd;
	double m_dCurrX, m_dCurrY;
	HANDLE      m_hLcWnd;          // Design window
	HANDLE	m_hImage;
	HANDLE  m_hLcDrw;
	VECTOR_HANDLE m_entities;
	int m_imgSizeW, m_imgSizeH;


	// 용접 패스 시뮬레이션용
	HANDLE	m_hPtbuf;
	int m_ColorBack;
	int m_ColorFore;
	int m_ColorCurs;
	double _Dist = 0.0;
	double _Time = 0.0;    // milliseconds since start
	DWORD  _TickPrev = 0;
	double    m_dPenX0;
	double    m_dPenY0;
	double    m_dPenSpeed;
	double    m_dPenWidth;

public:
	virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);
	DECLARE_MESSAGE_MAP()
	virtual void PreSubclassWindow();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

