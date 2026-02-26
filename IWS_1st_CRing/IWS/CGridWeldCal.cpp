#include "stdafx.h"
#include "CGridWeldCal.h"

DECLARE_USER_MESSAGE(UWM_GRIDWELDCAL)

CGridWeldCal::CGridWeldCal()
{
}


CGridWeldCal::~CGridWeldCal()
{
}

void CGridWeldCal::OnSetup()
{
	SetNumberRows(25);
	SetNumberCols(4);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);
	SetHighlightRow(TRUE);

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(2, -1, _T("X"));
	QuickSetText(3, -1, _T("Y"));
	QuickSetText(0, -1, _T("SCREEN X"));
	QuickSetText(1, -1, _T("SCREEN Y"));
 	CString str;
	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));

	SetGridDefault(&cell);

//  	for (int i = 0; i < 25; i++) {
//  		str.Format(L"%d", i);
//  		QuickSetText(-1, i, str);
//  		SetRowHeight(i, 20);
//  	}

	
// 	for (int i = 0; i < MAX_GRID_DIO; i++) {
// 		SetCell(0, i, &cell);
// 		SetCell(1, i, &cell);
// 		QuickSetText(0, i, m_strArrInputname[i]);
// 		QuickSetText(1, i, m_strArrOutputname[i]);
// 		SetRowHeight(i, 20);
// 	}

	CRect rc;
	GetClientRect(rc);
	int iw = int(double((rc.Width())*(1 - 0.1) / 5));// MAX_GRID_DIO);
	int in = rc.Width() - iw * 4;// MAX_GRID_DIO;

	this->SetColWidth(0, int(double(iw)*1.1));
	this->SetColWidth(1, int(double(iw)*1.1));
	this->SetColWidth(2, iw);
	this->SetColWidth(3, iw);
}

void CGridWeldCal::SetData(int iRow, double x, double y, double sx, double sy)
{
	CUGCell cell;
	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	cell.SetAlignment(UG_ALIGNRIGHT);
	CString str;
	str.Format(L"%d", iRow);
	QuickSetText(-1, iRow, str);
	str.Format(L"%.1f",x);
	 SetCell(0, iRow, &cell); QuickSetText(0, iRow, str);
	str.Format(L"%.1f",y);
	SetCell(1, iRow, &cell); QuickSetText(1, iRow, str);
	str.Format(L"%.1f", sx);
	SetCell(2, iRow, &cell); QuickSetText(2, iRow, str);
	str.Format(L"%.1f", sy);
	SetCell(3, iRow, &cell); QuickSetText(3, iRow, str);
}

int CGridWeldCal::SetCurrentData(double sx, double sy)
{
	int iRow = this->GetCurrentRow();
	if (iRow < 0) return -1;
	CUGCell cell;
	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	cell.SetAlignment(UG_ALIGNRIGHT);
	CString str;
	str.Format(L"%.1f", sx);
	SetCell(0, iRow, &cell); QuickSetText(0, iRow, str);
	str.Format(L"%.1f", sy);
	SetCell(1, iRow, &cell); QuickSetText(1, iRow, str);
	return iRow;
}

COLORREF CGridWeldCal::OnGetDefBackColor(int section)
{
	switch (section)
	{
	case UG_TOPHEADING:
		return RGB(0x38, 0x38, 0x38);
		break;
	case UG_SIDEHEADING:
		return RGB(0x38, 0x38, 0x38);
		break;
	case UG_GRID:
		return RGB(0x38, 0x38, 0x38);
		break;
	}
	return RGB(0x00, 0x00, 0x00);
}

void CGridWeldCal::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void CGridWeldCal::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	if (m_hRcvHandle)
		::PostMessage(m_hRcvHandle, UWM_GRIDWELDCAL, 0, row);
}

void CGridWeldCal::OnKeyDown(UINT *vcKey, int processed)
{

}

