// CDlgSortieMap.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "IWSDoc.h"
#include "CDlgSortieMap.h"
#include "afxdialogex.h"


// CDlgSortieMap 대화 상자

IMPLEMENT_DYNAMIC(CDlgSortieMap, CDialogEx)

CDlgSortieMap::CDlgSortieMap(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_SORTIE_MAP, pParent)
{
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++)
	{
		m_nSortieRptMap[i] =0;
	}
}

CDlgSortieMap::~CDlgSortieMap()
{
}

void CDlgSortieMap::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_GRID_SORTIE_MAP, m_gridSortieMap);

}


BEGIN_MESSAGE_MAP(CDlgSortieMap, CDialogEx)

ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_SORTIE_MAP, OnGridEndEdit)

END_MESSAGE_MAP()


// CDlgSortieMap 메시지 처리기


BOOL CDlgSortieMap::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	InitGrid();

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgSortieMap::SetSortieRptMap(int* pMap)
{
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++)
	{
		m_nSortieRptMap[i] = pMap[i];
	}
}

void CDlgSortieMap::GetSortieRptMap(int* pMap)
{
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++)
	{
		pMap[i] = m_nSortieRptMap[i];
	}
}

void CDlgSortieMap::InitGrid()
{
	// Delete All Items
	m_gridSortieMap.DeleteAllItems();

	m_gridSortieMap.SetEditable(TRUE);	// 편집 가능 여부
	m_gridSortieMap.SetListMode(FALSE);	// 리스트 선택 기능(한 줄 선택)
	m_gridSortieMap.EnableDragAndDrop(TRUE);	// Drag & Drop 사용 여부
	m_gridSortieMap.EnableDragRowMode(FALSE); // 열 전체 Drag 모드(현재, 오류 있음. 무조건 비활성화 - FALSE)
	m_gridSortieMap.SetTextBkColor(RGB(0xFF, 0xFF, 0xE0));	// 텍스트 백 그라운드 색
	m_gridSortieMap.EnableScrollBar(ESB_ENABLE_BOTH, TRUE);	// 스크롤바 사용 여부

	m_gridSortieMap.SetRowCount(MAX_NUM_REPEAT_WELD + 1); // 열 개수
	m_gridSortieMap.SetColumnCount(2);		// 행 개수

	m_gridSortieMap.SetFixedRowCount(1);	// 고정 열 개수
	m_gridSortieMap.SetFixedColumnCount(1);	// 고정 행 개수

	m_gridSortieMap.SetColumnWidth(0, 100);	// Column 폭 설정
	m_gridSortieMap.SetColumnWidth(1, 100);	// Column 폭 설정

	// Text 스타일
	DWORD dwTextStyle = DT_CENTER | DT_VCENTER | DT_SINGLELINE;	 // 가로 가운데 정열, 세로 가운데 정열, 한줄

	// 행열 Text 설정
	int nWordIndex = 0;

	for (int col = 0; col < m_gridSortieMap.GetColumnCount(); col++)
	{

		for (int row = 0; row < m_gridSortieMap.GetRowCount(); row++)
		{
			// Cell Item 구조체 선언
			GV_ITEM Item;
			Item.mask = GVIF_TEXT | GVIF_FORMAT;	// Mask 설정
			Item.row = row;	// 열 설정
			Item.col = col;	// 행 설정

			// 시작 주소 표시 -----
			if (row == 0 && col == 0) // 0,0
			{
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("Repeat NO"));
			}
			// 고정 행 설정(행 이름으로 표기)
			else if (row < 1)
			{
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("Sortie Repeat"));
			}
			// 고정 열 설정(열 이름으로 표기)
			else if (col < 1)
			{
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("%02d"), row);

			}
			// 그 외 데이터 표시
			else
			{
				Item.nFormat = dwTextStyle;
				Item.strText.Format(_T("%d"), m_nSortieRptMap[row-1]);
			}

			// Cell 설정
			m_gridSortieMap.SetItem(&Item);
		}
	}

	m_gridSortieMap.Refresh();
}



// 그리드 내용 변경 이벤트.
// GVN_ENDLABELEDIT
void CDlgSortieMap::OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult)
{
	NM_GRIDVIEW* pItem = (NM_GRIDVIEW*)pNotifyStruct;
	TRACE(_T("End Edit on row %d, col %d, value %s \n"), pItem->iRow, pItem->iColumn, m_gridSortieMap.GetItemText(pItem->iRow, pItem->iColumn));

	// 워드 인덱스 계산
	int nIndex = (pItem->iRow - 1);

	// 수신된 내용을 메모리에 반영
	m_nSortieRptMap[nIndex] = _tstoi(m_gridSortieMap.GetItemText(pItem->iRow, pItem->iColumn));
}
