#include "stdafx.h"
#include "ColorComboBox.h"


CColorComboBox::CColorComboBox()
{
}


CColorComboBox::~CColorComboBox()
{
}

COLORREF CColorComboBox::GetSelectedColourValue()
{
	return (COLORREF)GetItemData(GetCurSel());
}

void CColorComboBox::AddStringC(CString str, COLORREF clr)
{
	int iIndex = AddString(str);

	if (iIndex >= 0)
	{
		SetItemData(iIndex, clr);
	}
}


void CColorComboBox::PreSubclassWindow()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

	CComboBox::PreSubclassWindow();
	Initialise();
}

void CColorComboBox::Initialise()
{
	// add colours.
// 	int iNumColours = sizeof(g_arrColours) / sizeof(g_arrColours[0]);
// 
// 	for (int iNum = 0; iNum < iNumColours; iNum++)
// 	{
// 		Colour& colour = g_arrColours[iNum];
// 		AddColour(colour.m_strName, colour.m_crColour);
// 	}
// 
// 	// add a custom item on the end.
// 	AddColour("Custom...", RGB(255, 255, 255));
// 
// 	SetCurSel(0);
}


void CColorComboBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{

	CString strColour;
	CDC dcContext;
	CRect rItemRect(lpDrawItemStruct->rcItem);
	CRect rBlockRect(rItemRect);
	CRect rTextRect(rBlockRect);
	CBrush brFrameBrush;
	int iFourthWidth = 0;
	int iItem = lpDrawItemStruct->itemID;
	int iAction = lpDrawItemStruct->itemAction;
	int iState = lpDrawItemStruct->itemState;
	COLORREF crColour = NULL;
	COLORREF crNormal = GetSysColor(COLOR_WINDOW);
	COLORREF crSelected = GetSysColor(COLOR_HIGHLIGHT);
	COLORREF crText = GetSysColor(COLOR_WINDOWTEXT);

	if (!dcContext.Attach(lpDrawItemStruct->hDC))
	{
		return;
	}

	iFourthWidth = (rBlockRect.Width() / 4);
	brFrameBrush.CreateStockObject(BLACK_BRUSH);

	if (iState & ODS_SELECTED)
	{
		dcContext.SetTextColor((0x00FFFFFF & ~(crText)));
		dcContext.SetBkColor(crSelected);
		dcContext.FillSolidRect(&rBlockRect, crSelected);
	}
	else
	{
		dcContext.SetTextColor(crText);
		dcContext.SetBkColor(crNormal);
		dcContext.FillSolidRect(&rBlockRect, crNormal);
	}

	if (iState & ODS_FOCUS)
	{
		dcContext.DrawFocusRect(&rItemRect);
	}

	// calculate text area.
	rTextRect.left += (iFourthWidth + 2);
	rTextRect.top += 0;

	// calculate colour block area.
	rBlockRect.DeflateRect(CSize(2, 2));
	rBlockRect.right = iFourthWidth;

	// draw colour text and block.
	if (iItem != -1)
	{
		GetLBText(iItem, strColour);

		if (iState & ODS_DISABLED)
		{
			crColour = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
			dcContext.SetTextColor(crColour);
		}
		else
		{
			crColour = (COLORREF)GetItemData(iItem);
		}

		dcContext.SetBkMode(TRANSPARENT);
		dcContext.TextOut(rTextRect.left+2, rTextRect.top, strColour);

		dcContext.FillSolidRect(&rBlockRect, crColour);

		dcContext.FrameRect(&rBlockRect, &brFrameBrush);
	}

	dcContext.Detach();
}
BEGIN_MESSAGE_MAP(CColorComboBox, CComboBox)
	//ON_CONTROL_REFLECT(CBN_SELCHANGE, &CColorComboBox::OnCbnSelchange)
END_MESSAGE_MAP()

// 
// void CColorComboBox::OnCbnSelchange()
// {
// 
// 	CString strText;
// 	GetLBText(GetCurSel(), strText);
// 
// 	if (strText == "Custom...")
// 	{
// 		CColorDialog dlg;
// 
// 		if (dlg.DoModal() == IDOK)
// 		{
// 			SetItemData(GetCurSel(), dlg.GetColor());
// 		}
// 	}
// }
