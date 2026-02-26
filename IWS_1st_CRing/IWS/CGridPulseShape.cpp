#include "stdafx.h"
#include "CGridPulseShape.h"

DECLARE_USER_MESSAGE(UWM_CGRIDPULSESHAPE)
CGridPulseShape::CGridPulseShape()
{
	m_hEventRcv = NULL;
}


CGridPulseShape::~CGridPulseShape()
{
}

void CGridPulseShape::SetDouble(int col, long row, double dval)
{
	CString str;
	str.Format(L"%0.2f", dval);
	QuickSetText(col, row, str);
}

double CGridPulseShape::GetDouble(int col, long row)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	return cell.GetNumber();
}

void CGridPulseShape::OnSetup()
{
	this->SetNumberCols(2);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);
	// 	this->HideCurrentCell();
	SetHighlightRow(TRUE);

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(0, -1, _T("Time(T)"));
	QuickSetText(1, -1, _T("Power(0~100)"));
	SetDefRowHeight(28);
	CRect rect;
	GetClientRect(rect);
	int nWidData = int(double(rect.Width()) * 0.4);
	int nWidIdx =  int(double(rect.Width()) * 0.1);

	SetColWidth(-1, nWidIdx);
	SetColWidth(0, nWidData);
	SetColWidth(1, nWidData);
	GetColDefault(0, &cell);
	
	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNRIGHT);
	SetGridDefault(&cell);
	SetColDefault(0, &cell);
	SetColDefault(1, &cell);

}

COLORREF CGridPulseShape::OnGetDefBackColor(int section)
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

void CGridPulseShape::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
// 	if (!updn)
		StartEdit();
}

void CGridPulseShape::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void CGridPulseShape::OnKeyDown(UINT *vcKey, int processed)
{

}

int CGridPulseShape::OnEditStart(int col, long row, CWnd **edit)
{
	if (col < 0 && row <0)
		return FALSE;

	return TRUE;

}

int CGridPulseShape::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	if (m_hEventRcv) {
		::PostMessage(m_hEventRcv, UWM_CGRIDPULSESHAPE, 0, row);
	}
	return TRUE;
}

int CGridPulseShape::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

BEGIN_MESSAGE_MAP(CGridPulseShape, CUGCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CGridPulseShape::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	int nWidData = int(double(rect.Width()) * 0.4);
	int nWidIdx = int(double(rect.Width()) * 0.1);

	SetColWidth(-1, nWidIdx);
	SetColWidth(0, nWidData);
	SetColWidth(1, nWidData);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
