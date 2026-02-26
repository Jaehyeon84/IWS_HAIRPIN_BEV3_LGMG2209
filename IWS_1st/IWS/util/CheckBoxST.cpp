// CheckBoxST.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "CheckBoxST.h"


// CCheckBoxST

IMPLEMENT_DYNAMIC(CCheckBoxST, CButton)

CCheckBoxST::CCheckBoxST()
{
	m_dwHighLight = RGB(0, 0, 0);
	m_dwCheckColour = COLOR_OLIVEDRAB1;

	m_bHover = 0;
	m_nCheckState = 0;

	m_b3State = 0;
	m_bRadioButton = 0; // 1:라디오 버튼
	m_bCheckButton = 0; // 1:체크 박스

	m_bPushed = 0;

	m_himl = NULL;
	m_pTipRcvWnd = NULL;
}

CCheckBoxST::~CCheckBoxST()
{
	// 사용한 이미지 리스트를 제거한다.
	if (m_himl != NULL) {
		::ImageList_Destroy(m_himl);
		m_himl = NULL;
	}
}


BEGIN_MESSAGE_MAP(CCheckBoxST, CButton)
	ON_WM_ERASEBKGND()

	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()

	ON_MESSAGE(WM_MOUSEHOVER, OnMouseHover)
	ON_MESSAGE(WM_MOUSELEAVE, OnMouseLeave)
END_MESSAGE_MAP()


void CCheckBoxST::PreSubclassWindow()
{
	unsigned int  dwStyle = 0;
	unsigned long dwRemove = 0;
	unsigned long dwNewStyle = 0;

	dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);

	// WS_DISABLED 속성이 설정되어 있지 않은 상태에서 함수를 수행한다..
	if ((dwStyle & WS_DISABLED) != WS_DISABLED)
	{
		// Check Box 컨트롤인 경우, 3STATE 설정을 확인함.
		m_bCheckButton = m_b3State = (((dwStyle & BS_AUTO3STATE) == BS_AUTO3STATE) ||
			((dwStyle & BS_3STATE) == BS_3STATE)) ? 1 : 0;

		switch (dwStyle & 0x0F) // 버튼 속성.
		{
		case BS_CHECKBOX:
		case BS_AUTOCHECKBOX:
			m_bCheckButton = 1;
			break;

		case BS_RADIOBUTTON:
		case BS_AUTORADIOBUTTON:
			m_bRadioButton = 1;
			break;
		}

		if (m_bRadioButton || m_bCheckButton) // 체크 박스 또는 라디오 버튼인 경우에만 수행.
		{
			// BS_AUTO3STATE | BS_3STATE 상태를 제거한다.
			dwRemove = BS_AUTO3STATE | BS_3STATE;

			// BS_OWNERDRAW 속성을 설정한다.
			dwNewStyle = (dwStyle & ~dwRemove) | BS_OWNERDRAW;
			::SetWindowLong(m_hWnd, GWL_STYLE, dwNewStyle);
		}
	}
	SetDefaultColors();
	CButton::PreSubclassWindow();
}

void CCheckBoxST::SetDefaultColors(BOOL bRepaint /*= TRUE*/)
{
	m_crColors[CMB_COLOR_BK_IN] = COLOR_GREY26;
	m_crColors[CMB_COLOR_FG_IN] = COLOR_WHITE;
	m_crColors[CMB_COLOR_BK_OUT] = COLOR_GREY41;
	m_crColors[CMB_COLOR_FG_OUT] = COLOR_GREY41;
	m_crColors[CMB_COLOR_BK_FOCUS] = COLOR_OLIVEDRAB1;
	m_crColors[CMB_COLOR_FG_FOCUS] = COLOR_OLIVEDRAB1;

	if (bRepaint)	Invalidate();
}


BOOL CCheckBoxST::OnEraseBkgnd(CDC* /*pDC*/)
{
	// TODO: Add your message handler code here and/or call default
	return 1;
}

void CCheckBoxST::OnMouseMove(UINT nFlags, CPoint point)
{
	// HOVER, LEAVE 이벤트 추가
	TRACKMOUSEEVENT trackmouseevent;
	trackmouseevent.cbSize = sizeof(trackmouseevent);
	trackmouseevent.dwFlags = TME_HOVER | TME_LEAVE;
	trackmouseevent.hwndTrack = GetSafeHwnd();
	trackmouseevent.dwHoverTime = 0x00000001;

	_TrackMouseEvent(&trackmouseevent);

	CButton::OnMouseMove(nFlags, point);
}

LRESULT CCheckBoxST::OnMouseHover(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	if (!m_bHover)
		if (m_pTipRcvWnd) m_pTipRcvWnd->SendMessage(UWM_CHECKST_TIP, (WPARAM)&m_bmToolTip, (LPARAM)&m_strToolTip);

	m_bHover = 1;
	::InvalidateRect(m_hWnd, NULL, FALSE);

	return 0L;
}

LRESULT CCheckBoxST::OnMouseLeave(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
	m_bHover = 0;
	::InvalidateRect(m_hWnd, NULL, FALSE);
	if (m_pTipRcvWnd) m_pTipRcvWnd->SendMessage(UWM_CHECKST_TIP, 0L, 0L);
	return 0L;
}

void CCheckBoxST::OnLButtonDown(UINT nFlags, CPoint point)
{
	CButton::OnLButtonDown(nFlags, point);

	// 버튼 영역에서 마우스가 눌리면, "1"로 설정한다.
	m_bPushed = 1;
}

void CCheckBoxST::OnLButtonUp(UINT nFlags, CPoint point)
{
	RECT rc = { 0 };
	// 먼저 버튼의 유효한 영역에서 마우스 버튼이 눌린 다음에...
	if (m_bPushed == 1) {
		::GetClientRect(m_hWnd, &rc);
		// 유효 영역에서, 해제되면 버튼의 상태값을 변경한다.
		if (::PtInRect(&rc, (POINT)point))
			SetChangeStates();
	}
	// 버튼의 눌림이 해제되면, "0"으로 초기화한다.
	m_bPushed = 0;

	CButton::OnLButtonUp(nFlags, point);
}

void CCheckBoxST::SetBitmapFile(const TCHAR* lpszBmpFile, unsigned int nMaskColour)
{
	// 비트맵 리소스에서 이미지 리스트를 생성한다. (m_himl)
	if (m_himl != NULL)
		::ImageList_Destroy(m_himl);

	m_himl = ::ImageList_LoadImage(AfxGetResourceHandle(), lpszBmpFile,
		EACH_BITMAP_SIZE, 6, nMaskColour, IMAGE_BITMAP,
		LR_CREATEDIBSECTION | LR_LOADFROMFILE);
}

void CCheckBoxST::SetBitmapResource(unsigned int nResID, unsigned int nMaskColour)
{
	// 비트맵 리소스에서 이미지 리스트를 생성한다. (m_himl)
	if (m_himl != NULL)
		::ImageList_Destroy(m_himl);

	m_himl = ::ImageList_LoadImage(AfxGetResourceHandle(), MAKEINTRESOURCE(nResID),
		EACH_BITMAP_SIZE, 6, nMaskColour, IMAGE_BITMAP,
		LR_CREATEDIBSECTION);
}

void CCheckBoxST::SetChangeStates()
{
	if (m_bRadioButton) {
		m_nCheckState = 1;
		UnCheckOtherRadio();
		return;
	}

	if (m_b3State) // BS_AUTO3STATE, BS_O3STATE 속성이면...
		m_nCheckState = (m_nCheckState + 1) % 3;
	else
		m_nCheckState = (m_nCheckState + 1) % 2;
}

void CCheckBoxST::SetCheck(int nCheck)
{
	m_nCheckState = nCheck;

	if (m_bRadioButton && nCheck)
		UnCheckOtherRadio();
}

int CCheckBoxST::GetCheck() const
{
	return m_nCheckState;
}

int CCheckBoxST::IsRadioButton()
{
	return m_bRadioButton;
}

int CCheckBoxST::IsCheckBox()
{
	return m_bCheckButton;
}

void CCheckBoxST::DrawOrangeBoarder(HDC hDC, LPCRECT lpRect)
{
	RECT rc = { 0 };
	int i = 0;
	unsigned long clrOver = 0;

	::CopyRect(&rc, lpRect);
	for (i = 0; i < 2; ++i)	{
		clrOver = RGB(255, 193 + (i + 10), 111 + (i + 10));
		Draw3dRect(hDC, lpRect, clrOver, clrOver);
		rc.left += 1;
		rc.top += 1;
		rc.right -= 1;
		rc.bottom -= 1;
	}
	::InflateRect(&rc, 2, 2);

	::SetPixel(hDC, rc.left, rc.top, 0x00FFFFFE);

	for (i = 1; i <= 9; ++i) {
		::SetPixel(hDC, rc.left, rc.top + i, 0x00D9F3FF);
		::SetPixel(hDC, rc.left + i, rc.top, 0x00D9F3FF);

		if (i > 1) {
			::SetPixel(hDC, rc.right - i, rc.bottom - 2, 0x008BDCFE);
			::SetPixel(hDC, rc.right - 2, rc.bottom - i, 0x008BDCFE);
		}
		::SetPixel(hDC, rc.left + i, rc.top + 1, 0x00B9DCFE);
		::SetPixel(hDC, rc.left + 1, rc.top + i, 0x00B9DCFE);
	}
}

void CCheckBoxST::Draw3dRect(HDC hDC, LPCRECT lpRect, unsigned long clrTopLeft, unsigned long clrBottomRight)
{
	ASSERT(hDC != NULL);

	RECT rc3d = { 0 };

	::SetBkColor(hDC, clrTopLeft);

	rc3d.left = lpRect->left;
	rc3d.top = lpRect->top;
	rc3d.right = lpRect->right - 1;
	rc3d.bottom = lpRect->top + 1;
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc3d, NULL, 0, NULL);

	rc3d.right = lpRect->left + 1;
	rc3d.bottom = lpRect->bottom - 1;
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc3d, NULL, 0, NULL);

	::SetBkColor(hDC, clrBottomRight);

	rc3d.left = lpRect->right;
	rc3d.top = lpRect->top;
	rc3d.right = lpRect->right - 1;
	rc3d.bottom = lpRect->bottom;
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc3d, NULL, 0, NULL);

	rc3d.left = lpRect->left;
	rc3d.top = lpRect->bottom;
	rc3d.right = lpRect->right;
	rc3d.bottom = lpRect->bottom - 1;
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc3d, NULL, 0, NULL);
}

void CCheckBoxST::DrawCheck(HDC hDC, LPCRECT lpRect)
{
	int iMediaAltura = (lpRect->bottom - lpRect->top) / 2 - 2;
	RECT rc = { 0 };

	HPEN hNewPen = ::CreatePen(PS_SOLID, 1, m_dwCheckColour);
	HPEN hOldPen = (HPEN)::SelectObject(hDC, (HGDIOBJ)hNewPen);

	::CopyRect(&rc, lpRect);
	rc.left += 1;
	rc.top += 1;
	rc.right -= 1;
	rc.bottom -= 1;

	::MoveToEx(hDC, rc.left + 1, rc.top + iMediaAltura + 2, NULL);
	::LineTo(hDC, rc.left + 3, rc.bottom - 2);
	::MoveToEx(hDC, rc.left + 3, rc.bottom - 2, NULL);
	::LineTo(hDC, rc.right - 1, rc.top + 2);

	::MoveToEx(hDC, rc.left + 1, rc.top + iMediaAltura + 1, NULL);
	::LineTo(hDC, rc.left + 3, rc.bottom - 3);
	::MoveToEx(hDC, rc.left + 3, rc.bottom - 3, NULL);
	::LineTo(hDC, rc.right - 1, rc.top + 1);

	::MoveToEx(hDC, rc.left + 1, rc.top + iMediaAltura, NULL);
	::LineTo(hDC, rc.left + 3, rc.bottom - 4);
	::MoveToEx(hDC, rc.left + 3, rc.bottom - 4, NULL);
	::LineTo(hDC, rc.right - 1, rc.top);

	::SelectObject(hDC, (HGDIOBJ)hOldPen);
	::DeleteObject((HGDIOBJ)hNewPen);
}


void CCheckBoxST::DrawIndeterminateRect(HDC hDC, LPCRECT lpRect, long dwColour)
{
	RECT rc = { 0 };

	::CopyRect(&rc, lpRect);
	rc.bottom -= 1;
	rc.top += 1;
	rc.left += 1;
	rc.right -= 1;

	if (dwColour == -1)
		::SetBkColor(hDC, m_dwCheckColour);
	else
		::SetBkColor(hDC, dwColour);
	::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
}

void CCheckBoxST::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	RECT		rcTemp = { 0 };  // 체크 박스 영역
	RECT		rcText = { 0 };  // Caption 문자열 영역

	HDC		hDC = NULL;	// pDC

	int     nHeight = 0;			// 버튼의 높이
	int		nLen = 0;			// 버튼 Caption 문자열의 길이
	TCHAR    *pszCaption = NULL; // 버튼 Caption 문자열 
	unsigned int dwCapionStyle = 0; // 
	unsigned int nButtonStyle = 0;
	int     nImageIdx = 0;  // 이미지 리스트의 순서

	// 버튼의 Caption 문자열 크기를 가져온다.
	nLen = ::GetWindowTextLength(m_hWnd);
	if (nLen > 0) {
		pszCaption = new TCHAR[nLen + 1];
		::GetWindowText(m_hWnd, pszCaption, nLen + 1);
	}

	// 버튼의 영역을 복사한다.
	::CopyRect(&rcTemp, &lpDrawItemStruct->rcItem);
	::CopyRect(&rcText, &lpDrawItemStruct->rcItem);

	// 버튼의 높이를 구한다.
	nHeight = rcTemp.bottom - rcTemp.top;
	if (nHeight > EACH_BITMAP_SIZE) {
		// 높이가 지정된 크기보다 크면, 지정된 크기로 조정한다.
		rcTemp.top = (nHeight - EACH_BITMAP_SIZE) / 2;
		rcTemp.bottom = rcTemp.top + EACH_BITMAP_SIZE;
	}

	nButtonStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	// Caption 문자열의 배치 속성이 체크 박스 왼쪽으로 설정된 경우, 
	if (nButtonStyle & BS_LEFTTEXT) {
		rcTemp.left = rcTemp.right - EACH_BITMAP_SIZE; // 체크 상자의 왼쪽 좌표를 변경한다.
	}
	else 	{
		rcTemp.right = rcTemp.left + EACH_BITMAP_SIZE;  // 체크 상자의 오른쪽 좌표를 변경한다.
		rcText.left = rcTemp.right + 5; // Caption 문자열 영역의 왼쪽 좌표를 변경한다.
	}

	// DC 복사
	hDC = lpDrawItemStruct->hDC;
	::SetBkMode(hDC, TRANSPARENT);//OPAQUE);

	// Caption 문자열의 속성
	dwCapionStyle = DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS;
	if (nButtonStyle & BS_CENTER) dwCapionStyle |= DT_CENTER;
	else if (nButtonStyle & BS_RIGHT) dwCapionStyle |= DT_RIGHT;

	// 컨트롤의 문자열을 표시한다.
	::DrawText(hDC, pszCaption, nLen, &rcText, dwCapionStyle);
	if (pszCaption) { delete[] pszCaption; pszCaption = NULL; }

	// 비트맵 이미지를 사용하지 않을 경우, 코드에서 직접 그린다.
	if (m_himl == NULL) {
		// 체크 상자를 그린다.
		Draw3dRect(hDC, &rcTemp, m_dwHighLight, m_dwHighLight);

		rcTemp.left += 1;
		rcTemp.top += 1;
		rcTemp.right -= 1;
		rcTemp.bottom -= 1;

		if (m_bHover == 1) // Hover 상태이면
		{
			//DrawOrangeBoarder(hDC,&rcTemp); // 내부에 Orange 색상으로 사각형 테두리를 그린다.
			Draw3dRect(hDC, &rcTemp, m_crColors[CMB_COLOR_FG_FOCUS], m_crColors[CMB_COLOR_FG_FOCUS]);//m_dwHighLight,m_dwHighLight);
		}
		else
		{
			//::FillRect(hDC,&rcTemp,::GetSysColorBrush(COLOR_3DHILIGHT));
			Draw3dRect(hDC, &rcTemp, m_crColors[CMB_COLOR_FG_OUT], m_crColors[CMB_COLOR_FG_OUT]);//m_dwHighLight,m_dwHighLight);
		}

		// 버튼의 선택 상태 값를 가져온다.
		switch (m_nCheckState)
		{
		case BST_CHECKED:
			if (m_bRadioButton == 1)
				DrawIndeterminateRect(hDC, &rcTemp);
			else
				DrawCheck(hDC, &rcTemp); // 체크 표시를 그린다.
			break;
		case BST_INDETERMINATE:
			// 사각형을 그린다.
			DrawIndeterminateRect(hDC, &rcTemp);
			break;
		case BST_UNCHECKED:
			// 사각형 내부를 지운다.
			DrawIndeterminateRect(hDC, &rcTemp, m_crColors[CMB_COLOR_BK_IN]);//::GetSysColor(COLOR_3DHILIGHT));
			break;
		}
	}
	// 비트맵 이미지를 사용하여, 컨트롤을 표시하는 경우......
	else {
		if (m_bRadioButton) { // 라디오 버튼의 처음 이미지
			nImageIdx += FIRST_RADIO_BMP;
		}
		else /*if (m_bCheckButton)*/{ // 체크 버튼의 처음 이미지
			nImageIdx = FIRST_CHECK_BMP;
		}

		// 이미지를 선택한다.
		nImageIdx += (m_nCheckState * 2 + m_bHover);

		// 이미지 리스트에서 해당 비트맵을 그린다.
		::ImageList_Draw(m_himl, nImageIdx, lpDrawItemStruct->hDC,
			rcTemp.left, rcTemp.top, ILD_TRANSPARENT);
	}
}

void CCheckBoxST::UnCheckOtherRadio()
{
	int   bRadio = 0;

	CWnd *pWndParent = GetParent();
	CWnd *pWnd = pWndParent->GetNextDlgGroupItem(this);
	while (pWnd && (pWnd != this))
	{
		if (pWnd->IsKindOf(RUNTIME_CLASS(CCheckBoxST)))
		{
			bRadio = ((CCheckBoxST*)pWnd)->IsRadioButton();

			if (bRadio) {
				((CCheckBoxST *)pWnd)->SetCheck(0);
				pWnd->Invalidate();
			}
		}
		pWnd = pWndParent->GetNextDlgGroupItem(pWnd);
	}
}

void CCheckBoxST::SetTipData(CWnd *pWnd, CString strDsc, UINT nIDBitmap /*= NULL*/)
{
	m_pTipRcvWnd = pWnd;
	m_strToolTip = strDsc;
	if (nIDBitmap)
		m_bmToolTip.LoadBitmap(nIDBitmap);
}

// CCheckBoxST 메시지 처리기입니다.


