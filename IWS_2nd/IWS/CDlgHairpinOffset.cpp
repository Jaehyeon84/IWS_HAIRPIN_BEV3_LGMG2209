// CDlgHairpinOffset.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgHairpinOffset.h"
#include "afxdialogex.h"


// CDlgHairpinOffset 대화 상자

IMPLEMENT_DYNAMIC(CDlgHairpinOffset, CDialogEx)

CDlgHairpinOffset::CDlgHairpinOffset(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HAIRPIN_OFFSET, pParent)
	, m_dBaseR(0)
	, m_dBaseH(0)
	, m_dHRRatio(0)
	, m_dCenOffX(0)
	, m_dCenOffY(0)
{
}

CDlgHairpinOffset::~CDlgHairpinOffset()
{
}

void CDlgHairpinOffset::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RADIUS_BASE, m_dBaseR);
	DDX_Text(pDX, IDC_EDIT_RADIUS_HEIGHT, m_dBaseH);
	DDX_Text(pDX, IDC_EDIT_HR_RATIO, m_dHRRatio);
	DDX_Text(pDX, IDC_EDIT_CENTER_OFFSET_X, m_dCenOffX);
	DDX_Text(pDX, IDC_EDIT_CENTER_OFFSET_Y, m_dCenOffY);
}


BEGIN_MESSAGE_MAP(CDlgHairpinOffset, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgHairpinOffset::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgHairpinOffset 메시지 처리기

BOOL CDlgHairpinOffset::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_grid.m_nSlot = m_nSlot;
	m_grid.m_nLayer = m_nLayer;

	m_grid.AttachGrid(this, IDC_STATIC_GRID_OFFSET);
	m_grid.SetEventReciveHandle(this->GetSafeHwnd());
	
	InitGrid();
	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgHairpinOffset::InitGrid()
{
	/*for (int i = 0; i < m_nSlot * m_nLayer; i++)
	{
		for (int j = 0; j < m_nLayer; j++)
			m_grid.SetDouble(j, (long)i, (double)0);
	}*/

	for (int i = 0; i < m_nSlot * m_nLayer; i++)
	{
		m_grid.SetDouble(0, (long)i, m_hpOffset[i].ax);
		m_grid.SetDouble(1, (long)i, m_hpOffset[i].ay);
		m_grid.SetDouble(2, (long)i, m_hpOffset[i].bx);
		m_grid.SetDouble(3, (long)i, m_hpOffset[i].by);
	}
}

//void CDlgHairpinOffset::InitGrid()
//{
//	int max_rows = m_nSlot * m_nLayer;
//	int max_col = 4;
//	m_grid.SetNumberCols(max_col);
//	m_grid.SetNumberRows(max_rows);
//	m_grid.SetHighlightRow(TRUE);
//	m_grid.SetSH_NumberCols(2);
//	m_grid.SetSH_Width(100);
//	CUGCell cell;
//	m_grid.GetHeadingDefault(&cell);
//	//	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
//	// 	cell.SetBorder(0);
//	// 	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
//	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNRIGHT);
//	m_grid.SetGridDefault(&cell);
//	for (int i = 0; i < 5; i++)
//		m_grid.SetColDefault(i, &cell);
//
//	m_grid.SetVScrollMode(UG_SCROLLNORMAL);
//	m_grid.SetCurrentCellMode(2);
//	m_grid.EnableJoins(TRUE);
//
//	
//
//	m_grid.QuickSetText(-1, -1, _T("Info"));
//	m_grid.QuickSetText(0, -1, _T("aX"));
//	m_grid.QuickSetText(1, -1, _T("aY"));
//	m_grid.QuickSetText(2, -1, _T("bX"));
//	m_grid.QuickSetText(3, -1, _T("bY"));
//
//	//int nSlot = 96;
//	CString str;
//	int irow = 0;
//	int is;
//	for (int i = 0; i < m_nSlot; i++) 
//	{
//		is = irow;
//		for (int j = 0; j < m_nLayer; j++) 
//		{
//			str.Format(L"%d-%d", 2 * j + 1, 2 * j + 2);
//			m_grid.QuickSetText(-1, irow, str);
//			irow++;
//		}
//		m_grid.JoinCells(-2, is, -2, irow - 1);
//		str.Format(L"%d", i + 1);
//		m_grid.QuickSetText(-2, i * 4, str);
//	}
//}


void CDlgHairpinOffset::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CDialogEx::OnOK();
}



