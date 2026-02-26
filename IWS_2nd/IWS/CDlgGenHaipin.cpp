// CDlgGenHaipin.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgGenHaipin.h"
#include "afxdialogex.h"


// CDlgGenHaipin 대화 상자

IMPLEMENT_DYNAMIC(CDlgGenHaipin, CDialogEx)

CDlgGenHaipin::CDlgGenHaipin(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HAIRPIN_GEN, pParent)
{
	memset(m_dLayerDia, 0, sizeof(double)*MAX_HAIRPIN_LAYER);
	m_dLayerDia[0] = 160;
	m_dLayerDia[1] = 165;
	m_dLayerDia[2] = 175;
	m_dLayerDia[3] = 180;
	m_dLayerDia[4] = 190;
	m_dLayerDia[5] = 195;
	m_dLayerDia[6] = 205;
	m_dLayerDia[7] = 210;
	m_dLayerDia[8] = 220;
	m_dLayerDia[9] = 225;
	m_nModel = 0;
}

CDlgGenHaipin::~CDlgGenHaipin()
{
}

void CDlgGenHaipin::SetNumberOfLayer(int iLayer)
{
	if (iLayer <= 0) return;
	m_gridLayerDiameter.SetNumberOfLayers(iLayer);
}

void CDlgGenHaipin::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT_HAIRPIN_SIZE, m_editHairpinSize);
	DDX_Control(pDX, IDC_EDIT_SLOT_STARTANG, m_editHairpinStartAngle);
	DDX_Control(pDX, IDC_EDIT_NUM_SLOT, m_editNumSlot);
	DDX_Control(pDX, IDC_EDIT_DRAW_SLOT, m_editDrawSlot);
	DDX_Control(pDX, IDC_EDIT_NUM_LAYER, m_editNumLayer);
	DDX_Control(pDX, IDC_EDIT_HAIRPIN_SIZE_W, m_editHairpinSizeW);
	DDX_Control(pDX, IDC_EDIT_HAIRPIN_SIZE_WD, m_edit2ndweldSizeW);

	DDX_Control(pDX, IDC_EDIT_WELD_REPEAT_P1, m_editWeldRepeatP1);
	DDX_Control(pDX, IDC_EDIT_WELD_PEN_P1, m_editWeldPenP1);
	DDX_Radio(pDX, IDC_RADIO_Ell_P1, m_nWeldShapeP1);
	DDX_Control(pDX, IDC_EDIT_PREQ_P1, m_editWeldZigZagFreqP1);

	DDX_Control(pDX, IDC_EDIT_WELD_REPEAT_P2, m_editWeldRepeatP2);
	DDX_Control(pDX, IDC_EDIT_WELD_PEN_P2, m_editWeldPenP2);
	DDX_Radio(pDX, IDC_RADIO_Ell_P2, m_nWeldShapeP2);
	DDX_Control(pDX, IDC_EDIT_PREQ_P2, m_editWeldZigZagFreqP2);

	DDX_Radio(pDX, IDC_RADIO_MODEL_1, m_nModel);
	DDX_Control(pDX, IDC_EDIT_OVERLEN, m_editOverLen);

	DDX_Control(pDX, IDC_EDIT_PTN_ROTATION_ANGLE, m_editPtnRotationAng);
	DDX_Check(pDX, IDC_CHECK_USE_REVERSAL, m_bUseReversal); // sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
}


BEGIN_MESSAGE_MAP(CDlgGenHaipin, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgGenHaipin::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT_NUM_LAYER, &CDlgGenHaipin::OnEnChangeEditNumLayer)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_Ell_P2, IDC_RADIO_Zigzag2_P2, &CDlgGenHaipin::OnBnClickedRadio2ND) // sjyi 2022-05-19 Add
END_MESSAGE_MAP()


// CDlgGenHaipin 메시지 처리기


BOOL CDlgGenHaipin::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_gridLayerDiameter.AttachGrid(this, IDC_STATIC_LAYER_DIAMETER);

	m_editNumLayer.SetValue(m_nLayer);
	m_editNumSlot.SetValue(m_nSlot);
	m_editDrawSlot.SetValue(m_nDrawSlot);
	m_editHairpinStartAngle.SetValue(m_dStartAng);

	m_editHairpinSize.SetValue(m_dHairpinSize);
	m_editHairpinSizeW.SetValue(m_dHairpinSizeW);
	m_edit2ndweldSizeW.SetValue(m_d2ndWeldSizeW);

	m_editWeldRepeatP1.SetValue(m_nWeldRepeatP1);
	m_editWeldPenP1.SetValue(m_nWeldPenP1);	
	m_editWeldZigZagFreqP1.SetValue(m_nWeldZigZagFreqP1);

	m_editWeldRepeatP2.SetValue(m_nWeldRepeatP2);
	m_editWeldPenP2.SetValue(m_nWeldPenP2);	
	m_editWeldZigZagFreqP2.SetValue(m_nWeldZigZagFreqP2);
	m_editOverLen.SetValue(m_dOverLen, L"%.2f");

	// sjyi 2023.04.14 헤어핀 패턴 회전 각도
	m_editPtnRotationAng.SetValue(m_dPtnRotationAng);


	for (int i = 0; i < m_nLayer; i++) {
		m_gridLayerDiameter.SetLayerDia(i, m_dLayerDia[i]);
	}

	m_gridLayerDiameter.RedrawAll();

	UpdateStatic2nd(); // sjyi 2022-05-19 Add

	UpdateData(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgGenHaipin::OnEnChangeEditNumLayer()
{
	// TODO:  RICHEDIT 컨트롤인 경우, 이 컨트롤은
	// CDialogEx::OnInitDialog() 함수를 재지정 
	//하고 마스크에 OR 연산하여 설정된 ENM_CHANGE 플래그를 지정하여 CRichEditCtrl().SetEventMask()를 호출하지 않으면
	// 이 알림 메시지를 보내지 않습니다.
	int nLayer = m_editNumLayer.GetValue();
	SetNumberOfLayer(nLayer);
	// TODO:  여기에 컨트롤 알림 처리기 코드를 추가합니다.
}


void CDlgGenHaipin::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	m_nLayer = m_editNumLayer.GetValue();
	for (int i = 0; i < m_nLayer; i++) {
		CString strVal = m_gridLayerDiameter.QuickGetText(0, i);
		m_dLayerDia[i] = _tstof(strVal);
	}
	m_nSlot = m_editNumSlot.GetValue();
	m_nDrawSlot = m_editDrawSlot.GetValue();
	m_dStartAng = m_editHairpinStartAngle.GetValue();
	m_dHairpinSize = m_editHairpinSize.GetValue();
	m_dHairpinSizeW = m_editHairpinSizeW.GetValue();
	m_d2ndWeldSizeW = m_edit2ndweldSizeW.GetValue();

	m_nWeldRepeatP1 = m_editWeldRepeatP1.GetValue();
	m_nWeldPenP1 = m_editWeldPenP1.GetValue();
	m_nWeldZigZagFreqP1 = m_editWeldZigZagFreqP1.GetValue();

	m_nWeldRepeatP2 = m_editWeldRepeatP2.GetValue();
	m_nWeldPenP2 = m_editWeldPenP2.GetValue();
	m_nWeldZigZagFreqP2 = m_editWeldZigZagFreqP2.GetValue();
	m_dOverLen = m_editOverLen.GetValue();

	// sjyi 2023.04.14 헤어핀 패턴 회전 각도
	m_dPtnRotationAng = m_editPtnRotationAng.GetValue();

	UpdateData();
	
	CDialogEx::OnOK();
}

// sjyi 2022-05-19 Add  2nd 패턴에 따라 Static Text 변경하기 위해 함수 추가 -->
void CDlgGenHaipin::OnBnClickedRadio2ND(UINT uiD)
{
	UpdateData();

	UpdateStatic2nd();
}

void CDlgGenHaipin::UpdateStatic2nd()
{
	switch (m_nWeldShapeP2)
	{
	case 0: // Eclipse
	case 1: // line
	case 2: // ZigZag
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Over Length"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH, _T("Weld Width"));
		break;
	case 3: // ZigZag2 sjyi 2022-05-24 ZigZag2 패턴 추가
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Pin size(L)"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH, _T("Pin size(W)"));
		break;
	default:
		SetDlgItemText(IDC_STATIC_OVERLENGTH, _T("Over Length"));
		SetDlgItemText(IDC_STATIC_2ND_WIDTH, _T("Weld Width"));
		break;
	}
}
// sjyi 2022-05-19 Add <--