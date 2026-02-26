// DlgROI.cpp: 구현 파일
//

#include "pch.h"
#include "Hairpin2DInsp.h"
#include "Hairpin2DInspDlg.h"
#include "DlgROI.h"
#include "afxdialogex.h"

#include "DetectionAlgorithm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// CDlgROI 대화 상자

IMPLEMENT_DYNAMIC(CDlgROI, CDialog)

CDlgROI::CDlgROI(CHairpin2DInspDlg* pParent)
	: CDialog(IDD_DLG_ROI, pParent)
	, m_pParent(pParent)
	, m_pDetectionAlgorithm(m_pParent->m_pDetectionAlgorithm)
	, m_nSlotCount(96)
	, m_nLayerCount(8)
	, m_nLayerWidth(250)
	, m_nSlotHeight(150)
	, m_nSlotTopOffset(0)
	, m_nSlotBottomOffset(0)
	, m_nOffsetX(0)
	, m_nHairpinAreaROICenterToTopOffset(10)
	, m_nHairpinAreaROICenterToBottomOffset(10)
	, m_nHairpinAreaROICenterToLeftOffset(0)
	, m_nHairpinAreaROICenterToRightOffset(0)
	, m_nEachRoiOffsetXStartSlot(0)
	, m_nEachRoiOffsetXEndSlot(0)
	, m_nEachRoiOffsetXValue(0)
	, m_nSearchROIWidthIncreaseRatio(10)
	, m_nAutoROIProgressCount(1)
	, m_nStartSlotIndex(1)
{

}

CDlgROI::~CDlgROI()
{
}

void CDlgROI::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SLOT_COUNT, m_nSlotCount);
	DDX_Text(pDX, IDC_EDIT_LAYER_COUNT, m_nLayerCount);
	DDX_Text(pDX, IDC_EDIT_LAYER_WIDTH, m_nLayerWidth);
	DDX_Text(pDX, IDC_EDIT_SLOT_HEIGHT, m_nSlotHeight);
	DDX_Text(pDX, IDC_EDIT_OFFSET_X, m_nOffsetX);
	DDX_Text(pDX, IDC_EDIT_SLOT_TOP_OFFSET, m_nSlotTopOffset);
	DDX_Text(pDX, IDC_EDIT_SLOT_BOTTOM_OFFSET, m_nSlotBottomOffset);
	DDX_Text(pDX, IDC_EDIT_HAIRPIN_AREA_ROI_CENTER_TO_TOP_OFFSET, m_nHairpinAreaROICenterToTopOffset);
	DDX_Text(pDX, IDC_EDIT_HAIRPIN_AREA_ROI_CENTER_TO_BOTTOM_OFFSET, m_nHairpinAreaROICenterToBottomOffset);
	DDX_Control(pDX, IDC_GRID_EACH_ROI_OFFSET_X, m_gridEachROIOffsetX);
	DDX_Text(pDX, IDC_EDIT_HAIRPIN_AREA_ROI_CENTER_TO_LEFT_OFFSET, m_nHairpinAreaROICenterToLeftOffset);
	DDX_Text(pDX, IDC_EDIT_HAIRPIN_AREA_ROI_CENTER_TO_RIGHT_OFFSET, m_nHairpinAreaROICenterToRightOffset);
	DDX_Text(pDX, IDC_EDIT_EACH_ROI_OFFSET_X_START_SLOT, m_nEachRoiOffsetXStartSlot);
	DDX_Text(pDX, IDC_EDIT_EACH_ROI_OFFSET_X_END_SLOT, m_nEachRoiOffsetXEndSlot);
	DDX_Text(pDX, IDC_EDIT_EACH_ROI_OFFSET_X_VALUE, m_nEachRoiOffsetXValue);
	DDX_Text(pDX, IDC_EDIT_SEARCH_ROI_WIDTH_INCREASE_RATIO, m_nSearchROIWidthIncreaseRatio);
	DDX_Text(pDX, IDC_EDIT_AUTO_ROI_PROGRESS_COUNT, m_nAutoROIProgressCount);
	DDX_Control(pDX, IDC_CHECK_USE_AUTO_ROI, m_checkUseAutoROI);
	DDX_Text(pDX, IDC_EDIT_START_SLOT_INDEX, m_nStartSlotIndex);
}


BEGIN_MESSAGE_MAP(CDlgROI, CDialog)
	ON_BN_CLICKED(IDC_BTN_APPLY, &CDlgROI::OnBnClickedBtnApply)
	ON_BN_CLICKED(IDC_BTN_ORIGINAL_IMAGE, &CDlgROI::OnBnClickedBtnOriginalImage)
	ON_BN_CLICKED(IDC_BTN_INSPECTION, &CDlgROI::OnBnClickedBtnInspection)
	ON_BN_CLICKED(IDC_BTN_INSPECTION4, &CDlgROI::OnBnClickedBtnInspection4)
	ON_NOTIFY(GVN_ENDLABELEDIT, IDC_GRID_EACH_ROI_OFFSET_X, OnGridEditEnd)
	ON_NOTIFY(GVN_STATECHANGED, IDC_GRID_EACH_ROI_OFFSET_X, OnGridEditEnd)
	ON_BN_CLICKED(IDC_BTN_EACH_ROI_OFFSET_X_SET, &CDlgROI::OnBnClickedBtnEachRoiOffsetXSet)
	ON_BN_CLICKED(IDC_CHECK_USE_AUTO_ROI, &CDlgROI::OnBnClickedCheckUseAutoRoi)
END_MESSAGE_MAP()


// CDlgROI 메시지 처리기

BOOL CDlgROI::OnInitDialog()
{
	CDialog::OnInitDialog();

	return TRUE;
}


void CDlgROI::UpdateUI()
{
	CString strTemp;
	auto params = ::g_RecipeManager.m_sROIParams;

	UpdateData(TRUE);

	m_nSlotCount = params.nSlotCount;
	m_nLayerCount = params.nLayerCount;
	m_nLayerWidth = params.nLayerWidth;
	m_nSlotHeight = params.nSlotHeight;
	m_nSlotTopOffset = params.nSlotTopOffset;
	m_nSlotBottomOffset = params.nSlotBottomOffset;
	m_nOffsetX = params.nOffsetX;
	m_nHairpinAreaROICenterToLeftOffset = params.nHairpinAreaROICenterToLeftOffset;
	m_nHairpinAreaROICenterToTopOffset = params.nHairpinAreaROICenterToTopOffset;
	m_nHairpinAreaROICenterToRightOffset = params.nHairpinAreaROICenterToRightOffset;
	m_nHairpinAreaROICenterToBottomOffset = params.nHairpinAreaROICenterToBottomOffset;
	m_nStartSlotIndex = params.nStartSlotIndex;

	// Each Offset X 관련 GridCtrl
	m_gridEachROIOffsetX.GetDefaultCell(FALSE, TRUE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_gridEachROIOffsetX.GetDefaultCell(TRUE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_gridEachROIOffsetX.GetDefaultCell(FALSE, FALSE)->SetFormat(DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	m_gridEachROIOffsetX.SetGridLineColor(RGB(0, 0, 0));
	m_gridEachROIOffsetX.SetDefCellMargin(0);
	m_gridEachROIOffsetX.SetFixedColumnSelection(FALSE);
	m_gridEachROIOffsetX.SetFixedRowSelection(FALSE);
	m_gridEachROIOffsetX.SetSingleColSelection(FALSE);
	m_gridEachROIOffsetX.SetSingleRowSelection(FALSE);
	m_gridEachROIOffsetX.SetTrackFocusCell(FALSE);
	m_gridEachROIOffsetX.SetRowResize(FALSE);
	m_gridEachROIOffsetX.SetColumnResize(FALSE);

	// Auto ROI 관련
	m_checkUseAutoROI.SetCheck(params.bUseAutoROI);
	m_nSearchROIWidthIncreaseRatio = params.nSearchROIWidthIncreaseRatio;
	m_nAutoROIProgressCount = params.nAutoROIProgressCount;

	m_gridEachROIOffsetX.DeleteAllItems();

	// Items + Header
	m_gridEachROIOffsetX.SetRowCount(m_nSlotCount + 1);
	m_gridEachROIOffsetX.SetColumnCount(2);
	m_gridEachROIOffsetX.SetFixedRowCount(1);
	m_gridEachROIOffsetX.SetFixedColumnCount(1);
	m_gridEachROIOffsetX.SetColumnWidth(0, 50);
	m_gridEachROIOffsetX.SetColumnWidth(1, 100);

	m_gridEachROIOffsetX.SetItemText(0, 0, _T("Slot No."));
	m_gridEachROIOffsetX.SetItemText(0, 1, _T("Offset X"));

	for(long idx = 0; idx < m_nSlotCount; idx++)
	{
		m_gridEachROIOffsetX.SetItemTextFmt(idx + 1, 0, _T("%d"), idx + 1);
		m_gridEachROIOffsetX.SetItemTextFmt(idx + 1, 1, _T("%d"), params.pnEachROIOffsetX[idx]);
	}

	UpdateData(FALSE);
}

void CDlgROI::UpdateParameters()
{
	UpdateData(TRUE);

	auto &params = ::g_RecipeManager.m_sROIParams;

	params.nSlotCount = m_nSlotCount;
	params.nLayerCount = m_nLayerCount;
	params.nLayerWidth = m_nLayerWidth;
	params.nSlotHeight = m_nSlotHeight;
	params.nSlotTopOffset = m_nSlotTopOffset;
	params.nSlotBottomOffset = m_nSlotBottomOffset;
	params.nOffsetX = m_nOffsetX;
	params.nHairpinAreaROICenterToLeftOffset = m_nHairpinAreaROICenterToLeftOffset;
	params.nHairpinAreaROICenterToTopOffset = m_nHairpinAreaROICenterToTopOffset;
	params.nHairpinAreaROICenterToRightOffset = m_nHairpinAreaROICenterToRightOffset;
	params.nHairpinAreaROICenterToBottomOffset = m_nHairpinAreaROICenterToBottomOffset;
	params.bUseAutoROI = m_checkUseAutoROI.GetCheck();
	params.nSearchROIWidthIncreaseRatio = m_nSearchROIWidthIncreaseRatio;
	params.nAutoROIProgressCount = m_nAutoROIProgressCount;
	params.nStartSlotIndex = m_nStartSlotIndex;
}


void CDlgROI::OnBnClickedBtnApply()
{
	UpdateParameters();
}


void CDlgROI::OnBnClickedBtnOriginalImage()
{
	m_pParent->ShowOriginal(TRUE, TRUE);
}


void CDlgROI::OnBnClickedBtnInspection()
{
	UpdateParameters();

	m_pDetectionAlgorithm->DoMakeSubOriginalImage();
	m_pDetectionAlgorithm->DoMakeUnrolledImage();
	m_pParent->ShowMakeUnrolled(TRUE);

	m_pDetectionAlgorithm->DoSetDetectionROI();

	m_pParent->ShowROI(TRUE, TRUE);
}


void CDlgROI::OnBnClickedBtnInspection4()
{
	m_pParent->ShowMakeUnrolled(TRUE);
}


void CDlgROI::OnGridEditEnd(NMHDR* pNotifyStruct, LRESULT* result)
{
	NM_GRIDVIEW* pNotify = (NM_GRIDVIEW*)pNotifyStruct;
	int row = pNotify->iRow;
	int col = pNotify->iColumn;

	auto &params = ::g_RecipeManager.m_sROIParams;

	if(row < 0 || row > params.nSlotCount || col < 0 || col >= 2)
	{
		return;
	}

	CStringA strParam(m_gridEachROIOffsetX.GetItemText(row, col));
	long value = (long)_ttol((CString)strParam);

	params.pnEachROIOffsetX[row - 1] = value;

	Invalidate(FALSE);

	*result = 0;
}


void CDlgROI::OnBnClickedBtnEachRoiOffsetXSet()
{
	auto &params = ::g_RecipeManager.m_sROIParams;

	UpdateData(TRUE);

	if(m_nEachRoiOffsetXStartSlot < 1 || m_nEachRoiOffsetXStartSlot > m_nSlotCount || m_nEachRoiOffsetXEndSlot < 1 || m_nEachRoiOffsetXEndSlot > m_nSlotCount)
	{
		return;
	}

	for(long idx = m_nEachRoiOffsetXStartSlot; idx <= m_nEachRoiOffsetXEndSlot; idx++)
	{
		params.pnEachROIOffsetX[idx - 1] = m_nEachRoiOffsetXValue;
		m_gridEachROIOffsetX.SetItemTextFmt(idx, 1, _T("%d"), params.pnEachROIOffsetX[idx - 1]);
	}

	Invalidate(FALSE);
}


void CDlgROI::OnBnClickedCheckUseAutoRoi()
{
	auto &params = ::g_RecipeManager.m_sROIParams;

	params.bUseAutoROI = m_checkUseAutoROI.GetCheck();
}
