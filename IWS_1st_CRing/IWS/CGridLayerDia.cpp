#include "stdafx.h"
#include "CGridLayerDia.h"


CGridLayerDia::CGridLayerDia()
{
}


CGridLayerDia::~CGridLayerDia()
{
}

void CGridLayerDia::SetLayerDia(int iLayer, double dval)
{
	CString str;
	str.Format(L"%f", dval);
	QuickSetText(0, iLayer, str);
}

void CGridLayerDia::SetNumberOfLayers(int nLayer)
{

	SetNumberRows(nLayer);
	CString str;

	for (int i = 0; i < nLayer; i++) {
		str.Format(L"Layer%d", i);
		QuickSetText(-1, i, str);
	}
	RedrawAll();
}

void CGridLayerDia::OnSetup()
{
	SetNumberCols(1);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);
	this->HideCurrentCell();
	SetHighlightRow(TRUE);

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(0, -1, _T("Diameter"));
	
	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	SetColDefault(0,&cell);

	CRect rc;
	GetClientRect(rc);
	int iw = int(double(rc.Width())*(1 - 0.1) / 2);// MAX_GRID_DIO);
	int in = rc.Width() - iw * 1;// MAX_GRID_DIO;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, iw);
}

COLORREF CGridLayerDia::OnGetDefBackColor(int section)
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

void CGridLayerDia::OnDClicked(int col, long row, RECT * rect, POINT * point, BOOL processed)
{
}

void CGridLayerDia::OnLClicked(int col, long row, int updn, RECT * rect, POINT * point, int processed)
{
}

void CGridLayerDia::OnKeyDown(UINT * vcKey, int processed)
{
	if (*vcKey >= 96 && *vcKey <= 105)
		*vcKey -= 48;

	StartEdit(*vcKey);
}

int CGridLayerDia::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	return 1;
}
