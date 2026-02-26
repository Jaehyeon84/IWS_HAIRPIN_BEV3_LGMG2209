#pragma once
#include "vrs.h"

class CUtility:public CWnd
{
public:
	CUtility();
	~CUtility();

	void GetImaqLastError(CString strdesc = L"NI error");
	void DrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	HBRUSH CtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor, HBRUSH hbrush);
	BOOL Logging(CString strMsg);
	int DisplayImgOnDC(int ctrlID, unsigned char* pImg, int nWd, int nHt, int nChnnel);
	int DisplayImgOnDC(CDC* pDcDest, CRect rect, unsigned char* pImg, int nWd, int nHt, int nChnnel);

	void DrawRectOverlay(const PatternMatch* report, int numMatches, RGBValue boxColor, int windowNumber);
	void DrawRectOverlay(const ParticleReport* report, int reportCnt, int minLimit, int maxLimit, RGBValue boxColor, int windowNumber);
	void DrawRectOverlay(const MeasureParticlesReport* report, int reportCnt, int minLimit, int maxLimit, RGBValue boxColor, int windowNumber);
	void DrawRectOverlay(Image* pImg, Rect rect, int windowNumber);
	void DrawCrossOverlay(int width, int height, int crossSize, int windownumber, RGBValue color);
	void DrawCross(Point pos, Image* pImg, RGBValue color, int size);
	void DrawCross(Point pos, ROI* roi, RGBValue color, int size);
	void DrawCrossLines(Image* pImg, int width, int height, int crossSize, RGBValue color);
	void DrawTextOverlay(Image* pImg, Point pos, CString strText, RGBValue color);
	void DrawTextOverlay(Image* pImg, Point pos, double dispRate, double acqRate, RGBValue color);
	void DrawTextOverlay2(Image* pImg, Point pos, double dispRate, double acqRate, RGBValue color);

	//	void GenDeMosaicImage(int nWd_s, int nHt_s, Mat src_img, int nWd_t, int nHt_t, std::string strPath);

//	void DisplayImgOnDC(CDC* pDcDest, CRect rect, BYTE* pImg, int nWd, int nHt, int nCh);
	void DispText(int size, BOOL isBold, CRect rect, COLORREF txtColor, CString text, BOOL isPos, CDC *dc);
	void DrawLine(CDC* pDC, CPoint* pPtS, CPoint* pPtE, int PenStyle, int nWidth, COLORREF crcolor);
	void DrawRect(CDC* pDC, CRect* pRect, int PenStyle, int nWidth, COLORREF crcolor);
	std::string CStringToString(CString cs);

//  bool GetJIthScandataFromMosdata(unsigned char* pImgDst, int nHtDst, int nWdDst,	int blockIdxJ, int blockIdxI,
//			unsigned char* pImgSrc, int nHtSrc, int nWdSrc, int nCh);
//	bool CUtility::InsertJIthScandata2Mosdata(unsigned char* pImgDst, int nHtDst, int nWdDst,
//			int blockIdxJ, int blockIdxI, unsigned char* pImgSrc, int nHtSrc, int nWdSrc, int nCh);
	Image* LoadNiImage(const CString& file, const CString& title);

	static int sortSmall2Big(const void *a, const void *b);	// 오름 차순 sorting

};

extern CUtility g_Util;
