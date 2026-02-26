#pragma once
#include <afxwin.h>
#include <vector>
#define UWM_STATICCAD_MSG _T("CStaticCad")

#define CAD_EVENT_MOUSEMOVE 1
#define CAD_EVENT_ENTSENECT 2
#define CAD_EVENT_ENTCHANGE 3
#define CAD_EVENT_LBDOWN    4
#define CAD_EVENT_SELECTCLEAR 5
typedef struct _cad_overlay
{
	int iType;
	double x0, x1, y0, y1;
	double r;
}cad_overlay;
class CStaticCad :
	public CStatic
{
public:
	CStaticCad();
	~CStaticCad();
	
	HANDLE m_lcWnd;
	HANDLE m_lcDrw;
	HANDLE	m_hDrwWS; // work space handle
	HANDLE m_hLayerImage;
	HANDLE m_hRefImg;
	HANDLE m_hCurrPos;
	std::vector<HANDLE> m_hEntity;
	std::vector<cad_overlay> m_overlay;
	vector_int m_idxSelect;
	double m_dCurrX, m_dCurrY;
	double m_posx, m_posy;

	void EnableGrip(BOOL bFlag);
	void clearOverlay();
	void AddOverlay(int itype, double x0, double y0, double r);
	void AddOverlay(int itype, double x0, double y0, double x1, double y1);
	void UpdateOverlay();
	void RedrawCad();
	HANDLE GetDrwHandle() { return m_lcDrw; };
	HANDLE GetBlockHandle();
	void SetImgSize(double x, double y) { 
		m_imgSizeX = x;
		m_imgSizeY = y;
	}
	void ClearImage();
	void AddImage(CString strPath, double sizex=-1, double sizey=-1, double scx = 0, double scy = 0);
	void Clear();
	void ClearSelect();
	void AddCirclePoint(double cx, double cy);
	void AddCircle(double cx, double cy, double radius);
	void AddLine(double x0, double y0, double x1, double y1);
	void Update() { 
		lcBlockUpdate(GetBlockHandle(), true, 0);
		lcWndRedraw(m_lcWnd);
		lcWndZoomRect(m_lcWnd, 0, 0, 0, 0);
	}
	void ChangeEntity(int idx, HANDLE handle);
	void ModifyPosition(int idx, double x, double y);
	void SetWorkSpace(double sx, double sy, double cx, double cy) {
		m_wsX = sx;
		m_wsY = sy;
		m_wsXc = cx;
		m_wsYc = cy;

	}
	void SetHairpinTxt(int nSlot, double dir, double dia, double startAng) {
		m_dTxtStartAng = startAng;
		m_dTxtPCD = dia;
		m_dTxtDir = dir;
		m_nSlot = nSlot;
		m_bDisplayhapinTxt = TRUE;
	}
	void UpdateHapinTxt(HANDLE hBlock);

	void GetWorkSpace(double &sx, double &sy, double &cx, double &cy) {
		sx = m_wsX ;
		sy = m_wsY ;
		cx = m_wsXc;
		cy = m_wsYc;

	}
	void AdjustDraw(double x0, double y0, double x1, double y1);

	void SetSelectEntity(int idx);

	void BackUpDrwLimit();
	void RestoreDrwLimt();
	// 
	void SetEventRcvHandle(HWND hwnd) { 
		m_hRcvWnd = hwnd; 
	};
	void OnMouseMove(HANDLE hEvent);
	void OnSelect(HANDLE hEvent);
	void OnPropChange(HANDLE hEvent);
	void OnLBDown(HANDLE hEvent);
	void OnKeydown(HANDLE hEvent);
protected:
	HWND m_hRcvWnd;
	double m_wsX; // workspace
	double m_wsY;
	double m_wsXc;
	double m_wsYc;
	double m_imgSizeX;
	double m_imgSizeY;

	double m_backupXmin;
	double m_backupYmin;
	double m_backupXmax;
	double m_backupYmax;

	double m_dTxtStartAng;
	double m_dTxtPCD;
	double m_dTxtDir;
	int m_nSlot;
	BOOL   m_bDisplayhapinTxt;
public:
	void InitStaticCad();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

