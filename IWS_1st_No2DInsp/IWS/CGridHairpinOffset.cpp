#include "stdafx.h"
#include "CGridHairpinOffset.h"

DECLARE_USER_MESSAGE(UWM_CGRIDHAIRPINOFFSET)
CGridHairpinOffset::CGridHairpinOffset()
{
	m_hEventRcv = NULL;
}


CGridHairpinOffset::~CGridHairpinOffset()
{
}

void CGridHairpinOffset::SetDouble(int col, long row, double dval)
{
	CString str;
	str.Format(L"%0.2f", dval);
	QuickSetText(col, row, str);
}

double CGridHairpinOffset::GetDouble(int col, long row)
{
	CUGCell cell;
	GetCell(col, row, &cell);
	return cell.GetNumber();
}

void CGridHairpinOffset::OnSetup()
{
	int max_rows = m_nSlot * m_nLayer;
	int max_col = 4;

	SetNumberCols(max_col);
	SetNumberRows(max_rows);
	SetSH_NumberCols(2);
	SetSH_Width(100);

	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(2);
	EnableJoins(TRUE);
	
	SetHighlightRow(FALSE);
	cell.SetAlignment(UG_ALIGNCENTER);
	SetHeadingDefault(&cell);
	SetGridDefault(&cell);
	/*for (int i = 0; i < 4; i++)
		SetColDefault(i, &cell);*/

	QuickSetText(-1, -1, _T("Info"));
	QuickSetText(0, -1, _T("aX"));
	QuickSetText(1, -1, _T("aY"));
	QuickSetText(2, -1, _T("bX"));
	QuickSetText(3, -1, _T("bY"));

	//int nSlot = 96;
	CString str;
	int irow = 0;
	int is;
	for (int i = 0; i < m_nSlot; i++)
	{
		is = irow;
		for (int j = 0; j < m_nLayer; j++)
		{
			str.Format(L"%d-%d", 2 * j + 1, 2 * j + 2);
			QuickSetText(-1, irow, str);
			irow++;
		}
		JoinCells(-2, is, -2, irow - 1);
		str.Format(L"%d", i + 1);
		/*QuickSetText(-2, i * 4, str);*/
		QuickSetText(-2, i * m_nLayer, str);
	}
}

COLORREF CGridHairpinOffset::OnGetDefBackColor(int section)
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

void CGridHairpinOffset::OnDClicked(int col, long row, RECT *rect, POINT *point, BOOL processed)
{
	// 	if (!updn)
	StartEdit();
}

void CGridHairpinOffset::OnLClicked(int col, long row, int updn, RECT *rect, POINT *point, int processed)
{
	//StartEdit();
}

BOOL CGridHairpinOffset::OnKeyUp(int col, long row, UINT *vcKey)
{
	if (*vcKey == VK_UP)
		MoveCurrentRow(UG_LINEUP);
	else if (*vcKey == VK_DOWN)
		MoveCurrentRow(UG_LINEDOWN);
	else if (*vcKey == VK_LEFT)
		MoveCurrentCol(UG_COLLEFT);
	else if (*vcKey == VK_RIGHT)
		MoveCurrentCol(UG_COLRIGHT);

	return TRUE;
}


void CGridHairpinOffset::OnKeyDown(UINT *vcKey, int processed)
{
	/*if (*vcKey == VK_RETURN)
		StartEdit(VK_RETURN);	*/
	
	// sjyi 2022-05-25 NumPad 이상 수정 -->
	// Numpad로 숫자 입력 시, 이상 문자 입력
	// ex) Numpad 0 키 -> 작은 따옴표
	// Numpad 숫자 입력을 키보드 숫자 입력으로 변환
	int nKeyValue = *vcKey;			// 키 입력값을 변경하기 위한 변수

	if (nKeyValue >= 0x60 && nKeyValue <= 0x69) // Numpad 0 ~ Numpad 9
	{
		nKeyValue = nKeyValue - 0x30; // 숫자 키보드로 변경
	}
		
	//if if (*vcKey >= 48 && *vcKey <= 98) // 숫자 키보드 0 ~ Numpad 2
	if (*vcKey >= 0x30 && *vcKey <= 0x39 || *vcKey >= 0x60 && *vcKey <= 0x69) // 숫자키보드 0 ~ 숫자키보드 9, Numpad 0 ~ Numpad 9
		StartEdit(nKeyValue);
	// sjyi 2022-05-25 NumPad 이상 수정 <--
	else if (*vcKey == VK_RETURN)
		MoveCurrentRow(UG_LINEDOWN);
	else if (*vcKey == VK_TAB)
		MoveCurrentCol(UG_COLRIGHT);
}

void CGridHairpinOffset::OnMouseMove(int col, long row, POINT *point, UINT nFlags, BOOL processed)
{
	/*if (nFlags == MK_LBUTTON)
	{
		SetMultiSelectMode(true);

	}*/
}

int CGridHairpinOffset::OnEditStart(int col, long row, CWnd **edit)
{
	if (col < 0 && row < 0)
		return FALSE;

	return TRUE;

}

int CGridHairpinOffset::OnEditFinish(int col, long row, CWnd *edit, LPCTSTR string, BOOL cancelFlag)
{
	if (m_hEventRcv) {
		::PostMessage(m_hEventRcv, UWM_CGRIDHAIRPINOFFSET, 0, row);
	}
	return TRUE;
}

int CGridHairpinOffset::OnEditContinue(int oldcol, long oldrow, int* newcol, long* newrow)
{
	return FALSE;
}

BEGIN_MESSAGE_MAP(CGridHairpinOffset, CUGCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CGridHairpinOffset::OnSize(UINT nType, int cx, int cy)
{
	/*CUGCtrl::OnSize(nType, cx, cy);
	CRect rect;
	GetClientRect(rect);
	int nWidData = int(double(rect.Width()) * 0.4);
	int nWidIdx = int(double(rect.Width()) * 0.1);

	SetColWidth(-1, nWidIdx);
	SetColWidth(0, nWidData);
	SetColWidth(1, nWidData);*/
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

