#include "../Stdafx.h"
#include "Utility.h"

CUtility g_Util;

CUtility::CUtility()
{
}

CUtility::~CUtility()
{
}

void CUtility::GetImaqLastError(CString strdesc)
{
	char* err = imaqGetErrorText(imaqGetLastError());
	MessageBoxW((LPCTSTR)(LPSTR)err, strdesc, MB_OK);
	imaqDispose(err);
}

Image* CUtility::LoadNiImage(const CString& file, const CString& title)
{
	int wd, ht;
	if (!imaqGetFileInfo((CStringA)file, NULL, NULL, NULL, &wd, &ht, NULL)) {
		GetImaqLastError(L"File Reading error !");
		return NULL;
	}

	Image* loadImage = imaqCreateImage(g_ImgType, 2);

//	if( width < 0 || height < 0)
//		imaqSetImageSize(pImg, wd, ht);
	
	//-----------------------------------------------------------------------
	//  Read in the image.
	//-----------------------------------------------------------------------
	if (!imaqReadFile(loadImage, (CStringA)file, NULL, NULL)) {
		GetImaqLastError(L"Error Reading the File");

		imaqDispose(loadImage);
		return NULL;
	}

	return loadImage;
}


BOOL CUtility::Logging(CString strMsg)
{
	if (strMsg.IsEmpty()) {
		return FALSE;
	}
	else {
		CString strName, strPath;
		SYSTEMTIME st;
		GetLocalTime(&st);
		strMsg.Replace(L"\r\n", L"");

//		TCHAR curPath[100];
//		GetCurrentDirectoNo Image ry(100, curPath);

		CString logPath = g_strLogPath;
		logPath += L"\\";
		strName.Format(L"%04d%2d%2d.log", st.wYear, st.wMonth, st.wDay);
		strName = logPath + strName;


		CFile file;
		CreateDirectory(logPath, NULL);
		if (file.Open(strName, CFile::modeWrite | CFile::modeNoTruncate, NULL) == FALSE)
		{
			if (file.Open(strName, CFile::modeCreate | CFile::modeWrite | CFile::modeNoTruncate, NULL) == FALSE)
			{
				return FALSE;
			}
		}
		file.SeekToEnd();


		CString strTemp;
		strTemp.Format(L"[%02d:%02d:%02d:%03d] %s\r\n", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds, strMsg);

		char *buf = NULL;
		int nLen = WideCharToMultiByte(CP_ACP, 0, strTemp, -1, NULL, 0, NULL, NULL);
		buf = new char[nLen];
		WideCharToMultiByte(CP_ACP, 0, strTemp, -1, buf, nLen, NULL, NULL);
		file.Write(buf, nLen - 1);
		file.Close();
		delete[] buf;
	}

	return TRUE;
}

// IMAQ_RGB_TYPE은 4 channel image
// DC에 직접 그릴때는 3 channel만 사용
int CUtility::DisplayImgOnDC(int ctrlID, unsigned char* pImg, int nWd, int nHt, int nChnnel)
{
	CWnd* pWnd = GetDlgItem(ctrlID);
	if (!pWnd->IsWindowEnabled())
		return -1;

	CDC* pDcDest = pWnd->GetDC();
	CRect rect;
	pWnd->GetClientRect(&rect);

	// draw image on pDcDest here !
	CDC memDC;
	memDC.CreateCompatibleDC(pDcDest);

	CBitmap bitmap;
	bitmap.CreateDiscardableBitmap(pDcDest, nWd, nHt);
	CBitmap* pOldBitmap = (CBitmap*)memDC.SelectObject(&bitmap);

	//
	int widthbytes = WIDTHBYTES_VRS(nWd, 24);
	BYTE *lpvBits = new BYTE[nHt*widthbytes];

	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = nWd;
	bmi.bmiHeader.biHeight = -nHt;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = nHt*widthbytes;

	int i, j, d_jump, s_jump;
	if (nChnnel == IMG_GRAY_CHAN)
	{
		for (j = 0; j < nHt; j++) {
			s_jump = j * nWd;
			d_jump = j * widthbytes;
			for (i = 0; i < nWd; i++)
			{
				lpvBits[d_jump + 3 * i + 0] =
				lpvBits[d_jump + 3 * i + 1] =
				lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + i];
			}
		}
	}
	else if (nChnnel == IMG_RGB_CHAN)
	{
		for (j = 0; j < nHt; j++) {
			s_jump = j * nWd * nChnnel;
			d_jump = j * widthbytes;
			for (i = 0; i < nWd; i++)
			{
				lpvBits[d_jump + 3 * i + 0] = pImg[s_jump + nChnnel * i + 0];
				lpvBits[d_jump + 3 * i + 1] = pImg[s_jump + nChnnel * i + 1];
				lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + nChnnel * i + 2];
			}
		}
	}
	else
		Logging(L"No supported image depth !");

	SetDIBitsToDevice(
		memDC.m_hDC,     // handle of device context 
		0,     // x-coordinate of upper-left corner of dest. rect. 
		0,     // y-coordinate of upper-left corner of dest. rect. 
		nWd,     // source rectangle width 
		nHt,     // source rectangle height 
		0,     // x-coordinate of lower-left corner of source rect. 
		0,     // y-coordinate of lower-left corner of source rect. 
		0,     // first scan line in array 
		nHt,     // number of scan lines 
		(CONST VOID *)lpvBits,     // address of array with DIB bits 
		&bmi,     // address of structure with bitmap info. 
		DIB_RGB_COLORS      // RGB or palette indices 
	);

	delete[] lpvBits;
	//
	int rect_wd = rect.Width();
	int rect_ht = rect.Height();

	if (0)
		pDcDest->BitBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, SRCCOPY);
	else
	{
		int mode = pDcDest->SetStretchBltMode(HALFTONE);
		pDcDest->StretchBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, nWd, nHt, SRCCOPY);
		pDcDest->SetStretchBltMode(mode);
	}

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

	bitmap.DeleteObject();
	ReleaseDC(pDcDest);
	return 0;
}

int CUtility::DisplayImgOnDC(CDC* pDcDest, CRect rect, unsigned char* pImg,
	int nWd, int nHt, int nChnnel)
{
	if (pDcDest->m_hDC == NULL) return -1;

	// draw image on pDcDest here !
	CDC memDC;
	memDC.CreateCompatibleDC(pDcDest);

	CBitmap bitmap;
	bitmap.CreateDiscardableBitmap(pDcDest, nWd, nHt);
	CBitmap* pOldBitmap = (CBitmap*)memDC.SelectObject(&bitmap);

	//
	int widthbytes = WIDTHBYTES_VRS(nWd, 24);
	BYTE *lpvBits = new BYTE[nHt*widthbytes];

	BITMAPINFO bmi;
	ZeroMemory(&bmi, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biWidth = nWd;
	bmi.bmiHeader.biHeight = -nHt;
	bmi.bmiHeader.biBitCount = 24;
	bmi.bmiHeader.biCompression = BI_RGB;
	bmi.bmiHeader.biSizeImage = nHt*widthbytes;

	int i, j, d_jump, s_jump;
	if (nChnnel == IMG_GRAY_CHAN)
	{
		for (j = 0; j < nHt; j++) {
			s_jump = j * nWd;
			d_jump = j * widthbytes;
			for (i = 0; i < nWd; i++)
			{
				lpvBits[d_jump + 3 * i + 0] =
					lpvBits[d_jump + 3 * i + 1] =
					lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + i];
			}
		}
	}
	else if (nChnnel == IMG_RGB_CHAN)
	{
		for (j = 0; j < nHt; j++) {
			s_jump = j * nWd * nChnnel;
			d_jump = j * widthbytes;
			for (i = 0; i < nWd; i++)
			{
				lpvBits[d_jump + 3 * i + 0] = pImg[s_jump + nChnnel * i + 0];
				lpvBits[d_jump + 3 * i + 1] = pImg[s_jump + nChnnel * i + 1];
				lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + nChnnel * i + 2];
			}
		}
	}
	else
		Logging(L"No supported image depth !");

	SetDIBitsToDevice(
		memDC.m_hDC,     // handle of device context 
		0,     // x-coordinate of upper-left corner of dest. rect. 
		0,     // y-coordinate of upper-left corner of dest. rect. 
		nWd,     // source rectangle width 
		nHt,     // source rectangle height 
		0,     // x-coordinate of lower-left corner of source rect. 
		0,     // y-coordinate of lower-left corner of source rect. 
		0,     // first scan line in array 
		nHt,     // number of scan lines 
		(CONST VOID *)lpvBits,     // address of array with DIB bits 
		&bmi,     // address of structure with bitmap info. 
		DIB_RGB_COLORS      // RGB or palette indices 
	);

	delete[] lpvBits;
	//
	int rect_wd = rect.Width();
	int rect_ht = rect.Height();

	if (0)
		pDcDest->BitBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, SRCCOPY);
	else
	{
		int mode = pDcDest->SetStretchBltMode(HALFTONE);
		pDcDest->StretchBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, nWd, nHt, SRCCOPY);
		pDcDest->SetStretchBltMode(mode);
	}

	memDC.SelectObject(pOldBitmap);
	memDC.DeleteDC();

	bitmap.DeleteObject();
	return 0;
}

void CUtility::DrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);			//버튼의 dc구하기
	RECT rect = lpDrawItemStruct->rcItem;        //버튼영역 구하기
	
	UINT state = lpDrawItemStruct->itemState;    //버튼상태구하기
	if ((state & (ODS_SELECTED | ODS_DISABLED)))
	{
		//		dc.DrawEdge(&rect, EDGE_ETCHED, BF_RECT);
		dc.FillSolidRect(&rect, RED);             //버튼색상
	}
	else
	{
		dc.FillSolidRect(&rect, GRAY);             //버튼색상
		dc.Draw3dRect(&rect, WHITE, WHITE);		//버튼의 외각선 그리기
		//		dc.SetBkMode(TRANSPARENT);
	}
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(WHITE);                     //texttort
	TCHAR buffer[MAX_PATH];                            //버튼의 text를 얻기위한 임시버퍼
	ZeroMemory(buffer, MAX_PATH);                       //버퍼초기화
	::GetWindowText(lpDrawItemStruct->hwndItem, buffer, MAX_PATH); //버튼의 text얻기
	dc.DrawTextW(buffer, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE); //버튼의 text넣기
	dc.Detach();                                                  //버튼의 dc 풀어주기
}

HBRUSH CUtility::CtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor, HBRUSH hbrush)
{
	if (!hbrush)	return NULL;

	switch (nCtlColor)
	{
	case CTLCOLOR_EDIT:
//		pWnd->GetClientRect(&rc);
//		pDC->SetBkMode(OPAQUE);
//		pDC->FillSolidRect(&rc, BACK1);
//		pDC->SetBkColor(BACK1);
		pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(WHITE);
		return hbrush;// g_HbrEditCtrl;
	case CTLCOLOR_LISTBOX:
		pDC->SetTextColor(WHITE);
//		pDC->SetBkColor(BACK1);
		pDC->SetBkMode(TRANSPARENT);
		return hbrush;
	case CTLCOLOR_STATIC:
//		pDC->SetTextColor(BLACK);
		pDC->SetBkMode(TRANSPARENT);
//		pDC->SetBkColor(BLUE);
		pDC->SetTextColor(WHITE);
		return hbrush;
	case CTLCOLOR_MSGBOX:
		pDC->SetBkMode(TRANSPARENT);
		return hbrush;
	case CTLCOLOR_BTN:
	case CTLCOLOR_SCROLLBAR:
	case CTLCOLOR_DLG:
		return hbrush;
	default:
		return hbrush;
	}
}

void CUtility::DrawRectOverlay(const ParticleReport* report, int reportCnt, int minLimit, int maxLimit, RGBValue boxColor, int windowNumber)
{
	Point rect[10];
	Overlay* p_overlay;
	ContourID boxContour;
	ROI* p_overlayROI;

	//-----------------------------------------------------------------------
	//  Create the ROI we'll make the overlay from, then iterate over each
	//  match.
	//-----------------------------------------------------------------------
	p_overlayROI = imaqCreateROI();
	for (int j = 0; j < reportCnt; ++j)
	{
		if (report[j].area < minLimit || report[j].area > maxLimit)
			continue;

		//-------------------------------------------------------------------
		// Find the nearest pixel to each corner of the match.
		//-------------------------------------------------------------------

		rect[0].x = rect[1].x = report[j].boundingBox.left;
		rect[2].x = rect[3].x = report[j].boundingBox.left + report[j].boundingBox.width;

		rect[0].y = rect[3].y = report[j].boundingBox.top;
		rect[1].y = rect[2].y = report[j].boundingBox.top + report[j].boundingBox.height;

		rect[4] = rect[0];
		int delta;
		delta = (rect[0].x < rect[2].x) ? 1 : -1;
		rect[5].x = rect[0].x - delta;
		rect[7].x = rect[2].x + delta;
		delta = (rect[0].y < rect[2].y) ? 1 : -1;
		rect[5].y = rect[0].y - delta;
		rect[7].y = rect[2].y + delta;
		delta = (rect[1].x < rect[3].x) ? 1 : -1;
		rect[6].x = rect[1].x - delta;
		rect[8].x = rect[3].x + delta;
		delta = (rect[1].y < rect[3].y) ? 1 : -1;
		rect[6].y = rect[1].y - delta;
		rect[8].y = rect[3].y + delta;
		rect[9] = rect[5];
		//-------------------------------------------------------------------
		//  Add a contour for that box and make it the desired color.
		//-------------------------------------------------------------------
		boxContour = imaqAddClosedContour(p_overlayROI, rect, 10);
		imaqSetContourColor(p_overlayROI, boxContour, &boxColor);
	}
	//-----------------------------------------------------------------------
	//  Now that we have the ROI, make an overlay from it & attach that to
	//  the desired window.
	//-----------------------------------------------------------------------
	p_overlay = imaqCreateOverlayFromROI(p_overlayROI);
	imaqSetWindowOverlay(windowNumber, p_overlay);
	imaqDispose(p_overlayROI);
	imaqDispose(p_overlay);
}

void CUtility::DrawRectOverlay(const MeasureParticlesReport* report, int reportCnt, int minLimit, int maxLimit, RGBValue boxColor, int windowNumber)
{
	Point rect[10];
	Overlay* p_overlay;
	ContourID boxContour;
	ROI* p_overlayROI;

	//-----------------------------------------------------------------------
	//  Create the ROI we'll make the overlay from, then iterate over each
	//  match.
	//-----------------------------------------------------------------------
	p_overlayROI = imaqCreateROI();
	for (int j = 0; j < reportCnt; ++j)
	{
		int width = (int)report->pixelMeasurements[j][6] - (int)report->pixelMeasurements[j][4];
		int height = (int)report->pixelMeasurements[j][7] - (int)report->pixelMeasurements[j][5];
		
		// PIXEL AREA CRITERIA
//		if (report->pixelMeasurements[j][0] < minLimit*minLimit || 
//			report->pixelMeasurements[j][0] > maxLimit*maxLimit)
//			continue;

		// WIDTH or/and HEIGHT CRITERIA
//		if (width < minLimit || maxLimit < width || height < minLimit || maxLimit < height)
//			continue;

		//-------------------------------------------------------------------
		// Find the nearest pixel to each corner of the match.
		//-------------------------------------------------------------------
		rect[0].x = rect[1].x = (int)report->pixelMeasurements[j][4];	// left
		rect[2].x = rect[3].x = (int)report->pixelMeasurements[j][6];	// right

		rect[0].y = rect[3].y = (int)report->pixelMeasurements[j][5];	// top
		rect[1].y = rect[2].y = (int)report->pixelMeasurements[j][7];	// bottom

		rect[4] = rect[0];
		int delta;
		delta = (rect[0].x < rect[2].x) ? 1 : -1;
		rect[5].x = rect[0].x - delta;
		rect[7].x = rect[2].x + delta;
		delta = (rect[0].y < rect[2].y) ? 1 : -1;
		rect[5].y = rect[0].y - delta;
		rect[7].y = rect[2].y + delta;
		delta = (rect[1].x < rect[3].x) ? 1 : -1;
		rect[6].x = rect[1].x - delta;
		rect[8].x = rect[3].x + delta;
		delta = (rect[1].y < rect[3].y) ? 1 : -1;
		rect[6].y = rect[1].y - delta;
		rect[8].y = rect[3].y + delta;
		rect[9] = rect[5];
		//-------------------------------------------------------------------
		//  Add a contour for that box and make it the desired color.
		//-------------------------------------------------------------------
		boxContour = imaqAddClosedContour(p_overlayROI, rect, 10);
		imaqSetContourColor(p_overlayROI, boxContour, &boxColor);
	}
	//-----------------------------------------------------------------------
	//  Now that we have the ROI, make an overlay from it & attach that to
	//  the desired window.
	//-----------------------------------------------------------------------
	p_overlay = imaqCreateOverlayFromROI(p_overlayROI);
	imaqSetWindowOverlay(windowNumber, p_overlay);
	imaqDispose(p_overlayROI);
	imaqDispose(p_overlay);
}

void CUtility::DrawRectOverlay(const PatternMatch* report, int numMatches, RGBValue boxColor, int windowNumber)
{
	int i, j;
	Point approxCorner[4];
	Overlay* p_overlay;
	ContourID boxContour;
	ROI* p_overlayROI;

	//-----------------------------------------------------------------------
	//  Create the ROI we'll make the overlay from, then iterate over each
	//  match.
	//-----------------------------------------------------------------------
	p_overlayROI = imaqCreateROI();
	for (i = 0; i < numMatches; ++i) {
		//-------------------------------------------------------------------
		// Find the nearest pixel to each corner of the match.
		//-------------------------------------------------------------------
		for (j = 0; j < 4; ++j) {
			approxCorner[j].x = (int)(report[i].corner[j].x + 0.5);
			approxCorner[j].y = (int)(report[i].corner[j].y + 0.5);
		}
		//-------------------------------------------------------------------
		//  Add a contour for that box and make it the desired color.
		//-------------------------------------------------------------------
		boxContour = imaqAddClosedContour(p_overlayROI, approxCorner, 4);
		imaqSetContourColor(p_overlayROI, boxContour, &boxColor);
	}
	//-----------------------------------------------------------------------
	//  Now that we have the ROI, make an overlay from it & attach that to
	//  the desired window.
	//-----------------------------------------------------------------------
	p_overlay = imaqCreateOverlayFromROI(p_overlayROI);
	imaqSetWindowOverlay(windowNumber, p_overlay);
	imaqDispose(p_overlayROI);
	imaqDispose(p_overlay);
}

//bool CUtility::InsertJIthScandata2Mosdata(unsigned char* pImgDst, int nHtDst, int nWdDst,
//	int blockIdxJ, int blockIdxI, unsigned char* pImgSrc, int nHtSrc, int nWdSrc, int nCh)
//{
//	int ys = blockIdxJ * nHtSrc;
//	int ye = ys + nHtSrc;
//	int xs = blockIdxI * nWdSrc;
//	int xe = xs + nWdSrc;
//
//	//
//	// update Rect pixel coordindate on MosImg: m_pRectMosData
//	Rect* pRt = &(m_mosaicImgInfo.pRectOfBlock[blockIdxJ*m_scanInfo.nBlockX + blockIdxI]);
//	pRt->left = xs;			pRt->width = nWdSrc;
//	pRt->top = ys;			pRt->height = nHtSrc;
//	//
//	int pxlIdxByY;
//	int counter = 0;
//	for (int yi = ys; yi < ye; yi++)
//	{
//		pxlIdxByY = yi * nWdDst * nCh;
//		for (int xi = xs; xi < xe; xi++)
//		{
//			for (int ci = 0; ci < nCh; ci++)
//			{
//				pImgDst[pxlIdxByY + nCh*xi + ci] = pImgSrc[counter];
//				counter++;
//			}
//		}
//	}
//
//	return true;
//}

// Extract (j,i) th scan block data from stitched mos data
// pImgDst: scan data to extract
// pImgSrc: stitched mos data
//bool CUtility::GetJIthScandataFromMosdata(unsigned char* pImgDst, int nHtDst, int nWdDst,
//	int blockIdxJ, int blockIdxI,
//	unsigned char* pImgSrc, int nHtSrc, int nWdSrc, int nCh)
//{
//	Rect* pRt = &(m_mosaicImgInfo.pRectOfBlock[blockIdxJ*m_scanInfo.nBlockX + blockIdxI]);
//	int xs = pRt->left;		int xe = pRt->width;
//	int ys = pRt->top;		int ye = pRt->height;
//	
//	int pXLIdxByY;
//	int iCounter = 0;
//	for (int yi = ys; yi < ye; yi++)
//	{
//		pXLIdxByY = yi * nWdSrc * nCh;
//		for (int xi = xs; xi < xe; xi++)
//		{
//			for (int ci = 0; ci < nCh; ci++)
//			{
//				pImgDst[iCounter] = pImgSrc[pXLIdxByY + nCh*xi + ci];
//				iCounter++;
//			}
//		}
//	}
//	return true;
//}

void CUtility::DrawRectOverlay(Image* pImg, Rect rect, int windowNumber)
{
//	if (bClear)
//		imaqClearOverlay(pImg, NULL);

	const RGBValue rect_color = RGB_RED;

	// draw selected grid with different color
	imaqOverlayRect(pImg, rect, &rect_color, IMAQ_DRAW_VALUE, NULL);
}

void CUtility::DrawCrossOverlay(int width, int height, int crossSize, int windownumber)
{
	Point horiPt0[2];
	horiPt0[0].x = (width - crossSize) / 2;
	horiPt0[1].x = (width + crossSize) / 2;
	horiPt0[0].y = horiPt0[1].y = height / 2;

	Point horiPt1[2];
	horiPt1[0].x = horiPt0[0].x;		horiPt1[0].y = horiPt0[0].y - 1;
	horiPt1[1].x = horiPt0[1].x;		horiPt1[1].y = horiPt0[1].y - 1;

	Point vertPt0[2];
	vertPt0[0].y = (height - crossSize) / 2;
	vertPt0[1].y = (height + crossSize) / 2;
	vertPt0[0].x = vertPt0[1].x = width / 2;
	
	Point vertPt1[2];
	vertPt1[0].y = vertPt0[0].y;		vertPt1[0].x = vertPt0[0].x + 1;
	vertPt1[1].y = vertPt0[1].y;		vertPt1[1].x = vertPt0[1].x + 1;

	//-----------------------------------------------------------------------
	//  Create a ROI 
	//-----------------------------------------------------------------------
	ROI* _roi = imaqCreateROI();
	
	//-------------------------------------------------------------------
	//  Add a line conour & desired color.
	//-------------------------------------------------------------------
	RGBValue color = RGB_RED;
	ContourID contourid = imaqAddLineContour(_roi, horiPt0[0], horiPt0[1]);
	imaqSetContourColor(_roi, contourid, &color);
	//contourid = imaqAddLineContour(_roi, horiPt1[0], horiPt1[1]);
	//imaqSetContourColor(_roi, contourid, &color);
	contourid = imaqAddLineContour(_roi, vertPt0[0], vertPt0[1]);
	imaqSetContourColor(_roi, contourid, &color);
	//contourid = imaqAddLineContour(_roi, vertPt1[0], vertPt1[1]);
	//imaqSetContourColor(_roi, contourid, &color);

	//-------------------------------------------------------------------
	//  Draw rect around image edge
	//-------------------------------------------------------------------
	color = RGB_RED;
	Point rtPt[4];
	rtPt[0].x = rtPt[3].x = 0;
	rtPt[0].y = rtPt[1].y = 0;
	rtPt[1].x = rtPt[2].x = width - 1;
	rtPt[2].y = rtPt[3].y = height - 1;
	for (int j = 0; j < 4; j++)
	{
		contourid = imaqAddLineContour(_roi, rtPt[j], rtPt[(j+1)%4]);
		imaqSetContourColor(_roi, contourid, &color);
	}

	//-----------------------------------------------------------------------
	//  Now We have the ROI, make an overlay from it & attach that to
	//  the desired window.
	//-----------------------------------------------------------------------
	Overlay* _overlay = imaqCreateOverlayFromROI(_roi);
	imaqSetWindowOverlay(windownumber, _overlay);
	imaqDispose(_roi);
	imaqDispose(_overlay);
}

void CUtility::DrawCross(Point pos, ROI* roi, RGBValue color, int size)
{
	Point horiPt[2];
	horiPt[0].x = pos.x - size / 2;
	horiPt[1].x = pos.x + size / 2;
	horiPt[0].y = horiPt[1].y = pos.y;

	Point vertPt[2];
	vertPt[0].y = pos.y - size / 2;
	vertPt[1].y = pos.y + size / 2;
	vertPt[0].x = vertPt[1].x = pos.x;

	ContourID id1 = imaqAddLineContour(roi, horiPt[0], horiPt[1]);
	imaqSetContourColor(roi, id1, &color);
	ContourID id2 = imaqAddLineContour(roi, vertPt[0], vertPt[1]);
	imaqSetContourColor(roi, id2, &color);
}

void CUtility::DrawCross(Point pos, Image* pImg, RGBValue color, int size)
{
	//draw center with crocess 
	Point horiPt[2];
	horiPt[0].x = pos.x - size/2;
	horiPt[1].x = pos.x + size/2;
	horiPt[0].y = horiPt[1].y = pos.y;
	imaqOverlayLine(pImg, horiPt[0], horiPt[1], &color, NULL);

	Point vertPt[2];
	vertPt[0].y = pos.y - size/2;
	vertPt[1].y = pos.y + size/2;
	vertPt[0].x = vertPt[1].x = pos.x;
	imaqOverlayLine(pImg, vertPt[0], vertPt[1], &color, NULL);

}


void CUtility::DrawTextOverlay(Image* pImg, Point pos, CString strText, RGBValue color)
{
	RGBValue rgb = color;
	OverlayTextOptions options;
	options.angle = 0;
	options.backgroundColor = RGB_BLACK;
	options.bold = TRUE;
	options.italic = FALSE;
	options.strikeout = FALSE;
	options.underline = FALSE;

	options.horizontalTextAlignment = IMAQ_LEFT;
	options.verticalTextAlignment = IMAQ_TOP;
	options.fontName = (CStringA)(L"Arial");
	options.fontSize = 12;
	imaqOverlayText(pImg, pos, (CStringA)strText, &rgb, &options, NULL);
}

void CUtility::DrawTextOverlay(Image* pImg, Point pos, double dispRate, double acqRate, RGBValue color)
{
	CString strAvgFrmRate;
	strAvgFrmRate.Format(L" %d/%d f/s", int(dispRate), int(acqRate));

	RGBValue rgb = color; 
	OverlayTextOptions options;
	options.angle = 0;
//	options.backgroundColor = RGB_BLACK;
	options.bold = TRUE;
	options.italic = FALSE;
	options.strikeout = FALSE;
	options.underline = FALSE;

	options.horizontalTextAlignment = IMAQ_LEFT;
	options.verticalTextAlignment = IMAQ_TOP;
	options.fontName = (CStringA)(L"Arial");
	options.fontSize = 18;
	imaqOverlayText(pImg, pos, (CStringA)strAvgFrmRate, &rgb, &options, NULL);
}


//void CUtility::GenDeMosaicImage(int nWd_s, int nHt_s, Mat src_img, int nWd_t, int nHt_t, std::string strPath)
//{
//	int nX = nWd_s / nWd_t;
//	int nY = nHt_s / nHt_t;
//	int xs, ys;
//	std::string strYi, strXi;
//	Mat crop_img;
//
//	for (int yi = 0; yi < 7; yi++)
//	{
//		ys = yi * nHt_t;
//		strYi = std::to_string(yi);
//		for (int xi = 0; xi < 10; xi++)
//		{
//			xs = xi * nWd_t;
//			Rect rect(xs, ys, nWd_t, nHt_t);
//			crop_img = src_img(rect);
//			crop_img = src_img;
//
//			std::string strXi = std::to_string(xi);
//			std::string name = "/" + strYi + "_" + strXi + ".jpg";
//			std::string path = "../images-gray/";
//			name = strPath + name;
//			imwrite(name, crop_img);
//		}
//	}
//}

//void CUtility::DisplayImgOnDC(CDC* pDcDest, CRect rect, BYTE* pImg, int nWd, int nHt, int nCh)
//{
//	CRect roi;
////	COLORREF linecolor;
//
//	// draw image on pDcDest here !
//	CDC memDC;
//	memDC.CreateCompatibleDC(pDcDest);
//
//	CBitmap bitmap;
//	bitmap.CreateDiscardableBitmap(pDcDest, nWd, nHt);
//	CBitmap* pOldBitmap = (CBitmap*)memDC.SelectObject(&bitmap);
//
//	//
//	int widthbytes = WIDTHBYTES(nWd, 24);
//	BYTE *lpvBits = new BYTE[nHt*widthbytes];
//
//	BITMAPINFO bmi;
//	ZeroMemory(&bmi, sizeof(bmi));
//	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
//	bmi.bmiHeader.biPlanes = 1;
//	bmi.bmiHeader.biWidth = nWd;
//	bmi.bmiHeader.biHeight = -nHt;
//	bmi.bmiHeader.biBitCount = 24;
//	bmi.bmiHeader.biCompression = BI_RGB;
//	bmi.bmiHeader.biSizeImage = nHt*widthbytes;
//
//	int i, j, d_jump, s_jump;
//	if (NUM_IMG_CH == 1)
//	{
//		for (j = 0; j < nHt; j++) {
//			s_jump = j * nWd;
//			d_jump = j * widthbytes;
//			for (i = 0; i < nWd; i++)
//			{
//				lpvBits[d_jump + 3 * i + 0] =
//					lpvBits[d_jump + 3 * i + 1] =
//					lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + i];
//			}
//		}
//	}
//	else if (NUM_IMG_CH == 3)
//	{
//		for (j = 0; j < nHt; j++) {
//			s_jump = j * nWd * 3;
//			d_jump = j * widthbytes;
//			for (i = 0; i < nWd; i++)
//			{
//				lpvBits[d_jump + 3 * i + 0] = pImg[s_jump + 3 * i + 0];
//				lpvBits[d_jump + 3 * i + 1] = pImg[s_jump + 3 * i + 1];
//				lpvBits[d_jump + 3 * i + 2] = pImg[s_jump + 3 * i + 2];
//			}
//		}
//	}
//	else
//		g_Util.Logging(L"No supported image depth !");
//
//	SetDIBitsToDevice(
//		memDC.m_hDC,     // handle of device context 
//		0,     // x-coordinate of upper-left corner of dest. rect. 
//		0,     // y-coordinate of upper-left corner of dest. rect. 
//		nWd,     // source rectangle width 
//		nHt,     // source rectangle height 
//		0,     // x-coordinate of lower-left corner of source rect. 
//		0,     // y-coordinate of lower-left corner of source rect. 
//		0,     // first scan line in array 
//		nHt,     // number of scan lines 
//		(CONST VOID *)lpvBits,     // address of array with DIB bits 
//		&bmi,     // address of structure with bitmap info. 
//		DIB_RGB_COLORS      // RGB or palette indices 
//	);
//
//	delete[] lpvBits;
//	//
//	int rect_wd = rect.Width();
//	int rect_ht = rect.Height();
//
//	if (0)
//		pDcDest->BitBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, SRCCOPY);
//	else
//	{
//		int mode = pDcDest->SetStretchBltMode(HALFTONE);
//		pDcDest->StretchBlt(0, 0, rect_wd, rect_ht, &memDC, 0, 0, nWd, nHt, SRCCOPY);
//		pDcDest->SetStretchBltMode(mode);
//	}
//
//	memDC.SelectObject(pOldBitmap);
//	memDC.DeleteDC();
//
//	bitmap.DeleteObject();
//}

void CUtility::DispText(int size, BOOL isBold, CRect rect, COLORREF txtColor,
	CString text, BOOL isPos, CDC *dc)
{
	CFont font;
	CFont* oldFont;

	if (!isBold)
	{
		font.CreateFont(size, 0, 0, 0, FW_NORMAL, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
	}
	else
	{
		font.CreateFont(size, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Consolas");
	}

	oldFont = (CFont*)dc->SelectObject(&font);
	dc->SetTextColor(txtColor);
	dc->SetBkMode(TRANSPARENT);

	if (isPos == 0)
	{
		dc->DrawText(text, &rect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}
	else if (isPos == 1)
	{
		dc->DrawText(text, &rect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}
	else
	{
		dc->DrawText(text, &rect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
	}

	dc->SelectObject(oldFont);
	font.DeleteObject();
}

void CUtility::DrawLine(CDC* pDC, CPoint* pPtS, CPoint* pPtE, int PenStyle, int nWidth, COLORREF crcolor)
{
	CPen pen;
	pen.CreatePen(PenStyle, nWidth, crcolor);
	CPen* pOldPen = pDC->SelectObject(&pen);
	pDC->MoveTo(pPtS->x, pPtS->y);
	pDC->LineTo(pPtE->x, pPtE->y);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}


void CUtility::DrawRect(CDC* pDC, CRect* pRect, int PenStyle, int nWidth, COLORREF crcolor)
{
	CPen pen;
	pen.CreatePen(PenStyle, nWidth, crcolor);
	CPen* pOldPen = pDC->SelectObject(&pen);
	int s = 0;
	pDC->MoveTo(pRect->TopLeft().x + s, pRect->TopLeft().y + s);
	pDC->LineTo(pRect->BottomRight().x - s, pRect->TopLeft().y + s);
	pDC->LineTo(pRect->BottomRight().x - s, pRect->BottomRight().y - s);
	pDC->LineTo(pRect->TopLeft().x + s, pRect->BottomRight().y - s);
	pDC->LineTo(pRect->TopLeft().x + s, pRect->TopLeft().y + s);
	pDC->SelectObject(pOldPen);
	pen.DeleteObject();
}

std::string CUtility::CStringToString(CString cs)
{
	CT2CA pszConvertedAnsiString(cs);
	std::string s(pszConvertedAnsiString);
	return s;
}

int CUtility::sortSmall2Big(const void *a, const void *b)
{
	// refer: 		qsort(pHt, cnt, sizeof(int), sortSmall2Big);

	int num1 = *(int *)a; // void 포인터를 float 포인터로 변환한 뒤 역참조
	int num2 = *(int *)b; // void 포인터를 float 포인터로 변환한 뒤 역참조

	if (num1 < num2)		// a < b
		return -1;
	else if (num1 > num2)   // a > b
		return 1;
	else
		return 0;			// a == b
}




