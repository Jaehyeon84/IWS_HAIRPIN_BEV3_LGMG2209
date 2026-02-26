#include "stdafx.h"
#include "CGridJob.h"
DECLARE_USER_MESSAGE(UWM_GRIDJOB)


CGridJob::CGridJob()
{
}


CGridJob::~CGridJob()
{
}

void CGridJob::OnSetup()
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

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(0, -1, _T("Work name"));


	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNLEFT);
	SetColDefault(0,&cell);

	CRect rc;
	GetClientRect(rc);
	int iw = int((double(rc.Width())*(1 - 0.1) / 1));
	int in = rc.Width() - iw * 1;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, int(double(iw)*0.9));
	this->SetRowHeight(-1, 20);
}

COLORREF CGridJob::OnGetDefBackColor(int section)
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
	return COLOR_WHITESMOKE;
}

void CGridJob::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{

}

void CGridJob::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void CGridJob::OnKeyDown(UINT *vcKey, int processed)
{

}

void CGridJob::AddWorklist(CString str)
{
	int nRow = GetNumberRows();
	SetNumberRows(nRow + 1, FALSE);
	this->SetRowHeight(nRow, 20);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(COLOR_WHITESMOKE);
	SetCell(0, nRow, &cell);
	QuickSetText(0, nRow, str);
	str.Format(L"%d", nRow);
	QuickSetText(-1, nRow, str);

}

void CGridJob::ClearWorklist()
{
	SetNumberRows(0);
	this->RedrawAll();
}

void CGridJob::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);

	CRect rc;
	GetClientRect(rc);
	int iw = int(double(cx) *(1 - 0.1) / 1);
	int in = cx - iw * 1;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, int(double(iw)*0.9));
	this->SetRowHeight(-1, 20);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
