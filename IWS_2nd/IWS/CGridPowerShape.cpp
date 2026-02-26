#include "stdafx.h"
#include "CGridPowerShape.h"

DECLARE_USER_MESSAGE(UWM_CGRIDPOWERSHAPE)
CGridPowerShape::CGridPowerShape()
{
	m_hEventRcv = NULL;
}


CGridPowerShape::~CGridPowerShape()
{
}

void CGridPowerShape::SetDouble(int col, long row, double dval)
{
	CString str;
	str.Format(L"%0.2f", dval);
	QuickSetText(col, row, str);
}

double CGridPowerShape::GetDouble(int col, long row)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	return cell.GetNumber();
}

void CGridPowerShape::OnSetup()
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
	QuickSetText(0, -1, _T("Length"));
	QuickSetText(1, -1, _T("Power(0~100)"));
	SetDefRowHeight(28);
	CRect rect;
	GetClientRect(rect);
	int nWidData = int(double(rect.Width()) * 0.4);
	int nWidIdx = int(double(rect.Width()) * 0.1);

	SetColWidth(-1, nWidIdx);
	SetColWidth(0, nWidData);
	SetColWidth(1, nWidData);
	GetColDefault(0, &cell);
	
	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNRIGHT);
	SetGridDefault(&cell);
	SetColDefault(0, &cell);
	SetColDefault(1, &cell);

}

COLORREF CGridPowerShape::OnGetDefBackColor(int section)
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

void CGridPowerShape::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
// 	if (!updn)
		StartEdit();
}

void CGridPowerShape::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{

}

void CGridPowerShape::OnKeyDown(UINT *vcKey, int processed)
{

}

int CGridPowerShape::OnEditStart(int col, long row, CWnd **edit)
{
	if (col < 0 && row <0)
		return FALSE;

	return TRUE;

}

int CGridPowerShape::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	if (m_hEventRcv) {
		::PostMessage(m_hEventRcv, UWM_CGRIDPOWERSHAPE, 0, row);
	}
	return TRUE;
}

int CGridPowerShape::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

BEGIN_MESSAGE_MAP(CGridPowerShape, CUGCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CGridPowerShape::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);

	CRect rect;
	GetClientRect(rect);
	int nWidData = int(double(rect.Width()) * 0.4);
	int nWidIdx =  int(double(rect.Width()) * 0.1);

	SetColWidth(-1, nWidIdx);
	SetColWidth(0, nWidData);
	SetColWidth(1, nWidData);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}
