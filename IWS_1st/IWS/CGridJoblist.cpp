#include "stdafx.h"
#include "CGridJoblist.h"

DECLARE_USER_MESSAGE(UWM_GRIDJOBLIST)

CGridJoblist::CGridJoblist()
{
	m_hRcvEventHandle = NULL;
	m_bEnableEvent = TRUE;
}


CGridJoblist::~CGridJoblist()
{
}

void CGridJoblist::OnSetup()
{
	SetNumberCols(1);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);
	SetHighlightRow(TRUE);
	SetMultiSelectMode(TRUE);
	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(0, -1, _T("Job name"));


	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	cell.SetAlignment(UG_ALIGNLEFT);
	int fontID = AddFont(L"Arial", 25, 2);
	SetDefFont(fontID);
	SetColDefault(0,&cell);

	CRect rc;
	GetClientRect(rc);
	int iw = int(double(rc.Width())*(1 - 0.1) / 1);
	int in = rc.Width() - iw * 1;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, int(double(iw)*0.9));
	this->SetRowHeight(-1, 20);
	
}

COLORREF CGridJoblist::OnGetDefBackColor(int section)
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

void CGridJoblist::OnDClicked(int col, long row, RECT * rect, POINT * point, BOOL processed)
{
}

void CGridJoblist::OnLClicked(int col, long row, int updn, RECT * rect, POINT * point, int processed)
{
}

void CGridJoblist::OnKeyDown(UINT * vcKey, int processed)
{
}

void CGridJoblist::OnRowChange(long oldrow, long newrow)
{
	
}

void CGridJoblist::OnSelectionChanged(int startCol, long startRow, int endCol, long endRow, int blockNum)
{
	if (!m_bEnableEvent) return;
	if (m_hRcvEventHandle)
		::PostMessage(m_hRcvEventHandle, UWM_GRIDJOBLIST, MSG_GRID_CHANGEROW, endRow);
}

void CGridJoblist::AddJoblist(CString str, BOOL bDoJob)
{
	int nRow = GetNumberRows();
	SetNumberRows(nRow + 1, FALSE);
	this->SetRowHeight(nRow, 20);
	CUGCell cell;
	GetHeadingDefault(&cell);	

	// 12 = (Layer / 2) * 3
	if (((nRow) / 12)%2)
		cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	else
		cell.SetBackColor(RGB(0x2D, 0x2D, 0x2D));

	cell.SetBorder(0);
	if(bDoJob)
		cell.SetTextColor(COLOR_LIMEGREEN);
	else
		cell.SetTextColor(COLOR_WHITESMOKE);
	SetCell(0, nRow, &cell);
	QuickSetText(0, nRow, str);
	str.Format(L"%d", nRow+1);
	QuickSetText(-1, nRow, str);
	QuickSetAlignment(0, nRow, UG_ALIGNLEFT| UG_ALIGNVCENTER);
}

void CGridJoblist::ClearJoblist()
{
	SetNumberRows(0);
	this->RedrawAll();
}

void CGridJoblist::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);

	int iw = int(double(cx)*(1 - 0.1) / 1);
	int in = cx - iw * 1;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, int(double(iw)*0.9));
	this->SetRowHeight(-1, 20);
}
