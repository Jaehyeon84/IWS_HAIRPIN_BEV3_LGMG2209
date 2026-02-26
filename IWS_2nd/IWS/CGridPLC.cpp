#include "stdafx.h"
#include "CGridPLC.h"
#define MAX_GRID_PLC 12

CGridPLC::CGridPLC()
{
	m_strArrInputname.Add(L"Active");
	m_strArrInputname.Add(L"Req. Initialize");
	m_strArrInputname.Add(L"Req. TimeSync");
	m_strArrInputname.Add(L"Ready to Welding");
	m_strArrInputname.Add(L"Alarm");
	m_strArrInputname.Add(L"Auto");
	m_strArrInputname.Add(L"Manual");
	m_strArrInputname.Add(L"-");
	m_strArrInputname.Add(L"Weld Start");
	m_strArrInputname.Add(L"Weld Done");
	m_strArrInputname.Add(L"-");
	m_strArrInputname.Add(L"Inspection");
	//m_strArrInputname.Add(L"Inspection Done");

	m_strArrOutputname.Add(L"Active");
	m_strArrOutputname.Add(L"Rpy Initialize");
	m_strArrOutputname.Add(L"Rpy TimeSync");
	m_strArrOutputname.Add(L"Rdy. Welding");
	m_strArrOutputname.Add(L"Alarm");
	m_strArrOutputname.Add(L"Auto");
	m_strArrOutputname.Add(L"Manual");
	m_strArrOutputname.Add(L"-");
	m_strArrOutputname.Add(L"Weld Start");
	m_strArrOutputname.Add(L"Weld Done");
	m_strArrOutputname.Add(L"Weld Run");
	m_strArrOutputname.Add(L"Inspection");
	//m_strArrOutputname.Add(L"Inspection Done");
}


CGridPLC::~CGridPLC()
{
}

COLORREF CGridPLC::OnGetDefBackColor(int section)
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

void CGridPLC::OnSetup()
{
	SetNumberRows(MAX_GRID_PLC);
	SetNumberCols(2);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(1);
	this->HideCurrentCell();
	SetHighlightRow(TRUE);

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(0, -1, _T("Input"));
	QuickSetText(1, -1, _T("Output"));
	CString str;
	for (int i = 0; i < MAX_GRID_PLC; i++) {
		str.Format(L"%d", i);
		QuickSetText(-1, i, str);
		SetRowHeight(i, 20);
	}

	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	SetGridDefault(&cell);

	for (int i = 0; i < MAX_GRID_PLC; i++) {
		SetCell(0, i, &cell);
		SetCell(1, i, &cell);
		QuickSetText(0, i, m_strArrInputname[i]);
		QuickSetText(1, i, m_strArrOutputname[i]);
		SetRowHeight(i, 20);
	}

	CRect rc;
	GetClientRect(rc);
	int iw = int(double(rc.Width())*(1 - 0.1) / 2);// MAX_GRID_DIO);
	int in = rc.Width() - iw * 2;// MAX_GRID_DIO;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, iw);
	this->SetColWidth(1, iw);
}

void CGridPLC::OnDClicked(int col, long row, RECT * rect, POINT * point, BOOL processed)
{
}

void CGridPLC::OnLClicked(int col, long row, int updn, RECT * rect, POINT * point, int processed)
{
}

void CGridPLC::OnKeyDown(UINT * vcKey, int processed)
{
}

BEGIN_MESSAGE_MAP(CGridPLC, CUGCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CGridPLC::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);

	int iw = int(double(cx)*(1 - 0.02) / MAX_GRID_PLC);
	int in = cx - iw * MAX_GRID_PLC;
	this->SetColWidth(-1, in);
	for (int i = 0; i < MAX_GRID_PLC; i++)
		this->SetColWidth(i, iw);
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CGridPLC::SetInOut(int in, int out, int inCmd, int outCmd)
{
	CUGCell cellOn, cellOff;
	GetGridDefault(&cellOn);
	GetGridDefault(&cellOff);

	cellOn.SetTextColor(COLOR_OLIVEDRAB1);
	cellOff.SetTextColor(RGB(0, 128, 0));
	for (int i = 0; i < m_numDI; i++) {
		if (check_bit(in, i))
			SetCell(0, i, &cellOn);
		else
			SetCell(0, i, &cellOff);
	}
	for (int i = 0; i < m_numDO; i++) {
		if (check_bit(out, i))
			SetCell(1, i, &cellOn);
		else
			SetCell(1, i, &cellOff);
	}

	for (int i = 0; i < m_numDIOCmd; i++) {
		if (check_bit(inCmd, i))
			SetCell(0, i + m_numDI, &cellOn);
		else
			SetCell(0, i + m_numDI, &cellOff);
	}
	for (int i = 0; i < m_numDIOCmd; i++) {
		if (check_bit(outCmd, i))
			SetCell(1, i + m_numDO, &cellOn);
		else
			SetCell(1, i + m_numDO, &cellOff);
	}

	for (int i = 0; i < MAX_GRID_PLC; i++) {
		QuickSetText(0, i, m_strArrInputname[i]);
		QuickSetText(1, i, m_strArrOutputname[i]);
		SetRowHeight(i, 20);
	}
	this->RedrawAll();
}
