// CDlgHairpinSet.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgHairpinSet.h"
#include "afxdialogex.h"


// CDlgHairpinSet 대화 상자

IMPLEMENT_DYNAMIC(CDlgHairpinSet, CDialogEx)

CDlgHairpinSet::CDlgHairpinSet(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HAIPPIN_SET, pParent)
	, m_nDivStartSlot(0)
	, m_nDivOffset(0)
	, m_bSortieEnable(FALSE) 
{
	m_bUseSortieRepeat = FALSE;
}

CDlgHairpinSet::~CDlgHairpinSet()
{
}

void CDlgHairpinSet::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

	DDX_Radio(pDX, IDC_RADIO_WELD, m_nWeldType);
	DDX_Radio(pDX, IDC_RADIO_SWITCH, m_nSwitchWelding);

	DDX_Radio(pDX, IDC_RADIO_Z_USE, m_bZCompUse);
	DDX_Control(pDX, IDC_EDIT_Z_THRESHOLD, m_editZThreshold);
	DDX_Control(pDX, IDC_EDIT_Z_CNT, m_editZnCnt);
	DDX_Control(pDX, IDC_EDIT_Z_PWR, m_editZPwr);

	DDX_Control(pDX, IDC_EDIT_Z1_MIN, m_editZMin[0]);
	DDX_Control(pDX, IDC_EDIT_Z1_MAX, m_editZMax[0]);
	DDX_Control(pDX, IDC_EDIT_Z1_CNT, m_editZCnt[0]);

	DDX_Control(pDX, IDC_EDIT_Z2_MIN, m_editZMin[1]);
	DDX_Control(pDX, IDC_EDIT_Z2_MAX, m_editZMax[1]);
	DDX_Control(pDX, IDC_EDIT_Z2_CNT, m_editZCnt[1]);

	DDX_Control(pDX, IDC_EDIT_Z3_MIN, m_editZMin[2]);
	DDX_Control(pDX, IDC_EDIT_Z3_MAX, m_editZMax[2]);
	DDX_Control(pDX, IDC_EDIT_Z3_CNT, m_editZCnt[2]);

	DDX_Control(pDX, IDC_EDIT_Z4_MIN, m_editZMin[3]);
	DDX_Control(pDX, IDC_EDIT_Z4_MAX, m_editZMax[3]);
	DDX_Control(pDX, IDC_EDIT_Z4_CNT, m_editZCnt[3]);

	DDX_Control(pDX, IDC_EDIT_Z5_MIN, m_editZMin[4]);
	DDX_Control(pDX, IDC_EDIT_Z5_MAX, m_editZMax[4]);
	DDX_Control(pDX, IDC_EDIT_Z5_CNT, m_editZCnt[4]);

	DDX_Control(pDX, IDC_EDIT_Z6_MIN, m_editZMin[5]);
	DDX_Control(pDX, IDC_EDIT_Z6_MAX, m_editZMax[5]);
	DDX_Control(pDX, IDC_EDIT_Z6_CNT, m_editZCnt[5]);

	DDX_Control(pDX, IDC_EDIT_Z7_MIN, m_editZMin[6]);
	DDX_Control(pDX, IDC_EDIT_Z7_MAX, m_editZMax[6]);
	DDX_Control(pDX, IDC_EDIT_Z7_CNT, m_editZCnt[6]);

	DDX_Control(pDX, IDC_EDIT_Z8_MIN, m_editZMin[7]);
	DDX_Control(pDX, IDC_EDIT_Z8_MAX, m_editZMax[7]);
	DDX_Control(pDX, IDC_EDIT_Z8_CNT, m_editZCnt[7]);

	DDX_Control(pDX, IDC_EDIT_Z9_MIN, m_editZMin[8]);
	DDX_Control(pDX, IDC_EDIT_Z9_MAX, m_editZMax[8]);
	DDX_Control(pDX, IDC_EDIT_Z9_CNT, m_editZCnt[8]);

	DDX_Control(pDX, IDC_EDIT_Z10_MIN, m_editZMin[9]);
	DDX_Control(pDX, IDC_EDIT_Z10_MAX, m_editZMax[9]);
	DDX_Control(pDX, IDC_EDIT_Z10_CNT, m_editZCnt[9]);

	DDX_Control(pDX, IDC_EDIT_REPAIR_POWER, m_editPowerRatio);
	DDX_Control(pDX, IDC_EDIT_REPAIR_SPEED, m_editSpeedRatio);

	DDX_Control(pDX, IDC_EDIT_LEGEND_ANG, m_editLegendAng);
	DDX_Control(pDX, IDC_EDIT_LEGEND_DIA, m_editLegendDia);
	DDX_Control(pDX, IDC_EDIT_LEGEND_DIR, m_editLegendDir);

	DDX_Control(pDX, IDC_EDIT_RCP1_SCALE, m_editRcpScale[0]);
	DDX_Control(pDX, IDC_EDIT_RCP1_ANGLE, m_editRcpAngle[0]);
	DDX_Control(pDX, IDC_EDIT_RCP1_OFFSET_X, m_editRcpOffsetX[0]);
	DDX_Control(pDX, IDC_EDIT_RCP1_OFFSET_Y, m_editRcpOffsetY[0]);

	DDX_Control(pDX, IDC_EDIT_RCP2_SCALE, m_editRcpScale[1]);
	DDX_Control(pDX, IDC_EDIT_RCP2_ANGLE, m_editRcpAngle[1]);
	DDX_Control(pDX, IDC_EDIT_RCP2_OFFSET_X, m_editRcpOffsetX[1]);
	DDX_Control(pDX, IDC_EDIT_RCP2_OFFSET_Y, m_editRcpOffsetY[1]);

	DDX_Control(pDX, IDC_EDIT_RCP3_SCALE, m_editRcpScale[2]);
	DDX_Control(pDX, IDC_EDIT_RCP3_ANGLE, m_editRcpAngle[2]);
	DDX_Control(pDX, IDC_EDIT_RCP3_OFFSET_X, m_editRcpOffsetX[2]);
	DDX_Control(pDX, IDC_EDIT_RCP3_OFFSET_Y, m_editRcpOffsetY[2]);

	DDX_Control(pDX, IDC_EDIT_RCP4_SCALE, m_editRcpScale[3]);
	DDX_Control(pDX, IDC_EDIT_RCP4_ANGLE, m_editRcpAngle[3]);
	DDX_Control(pDX, IDC_EDIT_RCP4_OFFSET_X, m_editRcpOffsetX[3]);
	DDX_Control(pDX, IDC_EDIT_RCP4_OFFSET_Y, m_editRcpOffsetY[3]);

	DDX_Control(pDX, IDC_EDIT_RCP5_SCALE, m_editRcpScale[4]);
	DDX_Control(pDX, IDC_EDIT_RCP5_ANGLE, m_editRcpAngle[4]);
	DDX_Control(pDX, IDC_EDIT_RCP5_OFFSET_X, m_editRcpOffsetX[4]);
	DDX_Control(pDX, IDC_EDIT_RCP5_OFFSET_Y, m_editRcpOffsetY[4]);

	DDX_Control(pDX, IDC_EDIT_RCP6_SCALE, m_editRcpScale[5]);
	DDX_Control(pDX, IDC_EDIT_RCP6_ANGLE, m_editRcpAngle[5]);
	DDX_Control(pDX, IDC_EDIT_RCP6_OFFSET_X, m_editRcpOffsetX[5]);
	DDX_Control(pDX, IDC_EDIT_RCP6_OFFSET_Y, m_editRcpOffsetY[5]);
//	DDX_Control(pDX, IDC_EDIT_Z_PWR, m_editZPwr);
	/*DDX_Control(pDX, IDC_EDIT_DEGREE12, m_edit12degree);
	DDX_Control(pDX, IDC_EDIT_DEGREE34, m_edit34degree);
	DDX_Control(pDX, IDC_EDIT_DEGREE56, m_edit56degree);
	DDX_Control(pDX, IDC_EDIT_DEGREE78, m_edit78degree);*/

	DDX_Text(pDX, IDC_EDIT_START_SLOT, m_nDivStartSlot);
	DDX_Text(pDX, IDC_EDIT_DIVISION_OFFSET, m_nDivOffset);

	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_JIGA, m_bUseAllwaysScanJigA);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_JIGB, m_bUseAllwaysScanJigB);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_REWELD_A, m_bUseAllwaysScanReweldA);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_REWELD_B, m_bUseAllwaysScanReweldB);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_WELD_A, m_bUseAllwaysScanWeldA);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_WELD_B, m_bUseAllwaysScanWeldB);

	DDX_Control(pDX, IDC_EDIT_SOTIE_CNT, m_editSortieCnt);
	DDX_Check(pDX, IDC_CHECK_SORTIE, m_bSortieEnable);
	DDX_Radio(pDX, IDC_RADIO_SORITE_DIR, m_nSortieDir);

}


BEGIN_MESSAGE_MAP(CDlgHairpinSet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgHairpinSet::OnBnClickedOk)
END_MESSAGE_MAP()


// CDlgHairpinSet 메시지 처리기
BOOL CDlgHairpinSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	m_editZThreshold.SetValue(m_dblZThreshold);
	m_editZnCnt.SetValue(m_nZCtn);
	m_editZPwr.SetValue(m_dblZPwr*100.0, L"%0.1f");// mspark 2022.04.20
	for (int i = 0; i < 10; i++)
	{
		m_editZMin[i].SetValue(m_zComp[i].dblMin, L"%.1f");
		m_editZMax[i].SetValue(m_zComp[i].dblMax, L"%.1f");
		m_editZCnt[i].SetValue(m_zComp[i].nCnt);
	}

	for (int i = 0; i < 6; i++)
	{
		m_editRcpScale[i].SetValue(m_dRcpScale[i], L"%.4f");
		m_editRcpAngle[i].SetValue(m_dRcpAngle[i], L"%.4f");
		m_editRcpOffsetX[i].SetValue(m_dRcpOffsetX[i], L"%.4f");
		m_editRcpOffsetY[i].SetValue(m_dRcpOffsetY[i], L"%.4f");
	}

	m_editPowerRatio.SetValue(m_dPowerRatio, L"%.1f");
	m_editSpeedRatio.SetValue(m_dSpeedRatio, L"%.1f");

	m_editLegendAng.SetValue(m_dLegendAng, L"%.1f");
	m_editLegendDia.SetValue(m_dLegendDia, L"%.1f");
	m_editLegendDir.SetValue(m_dLegendDir, L"%.1f");

	/*m_edit12degree.SetValue(m_dDegree12, L"%.2f");
	m_edit34degree.SetValue(m_dDegree34, L"%.2f");
	m_edit56degree.SetValue(m_dDegree56, L"%.2f");
	m_edit78degree.SetValue(m_dDegree78, L"%.2f");*/

	if (m_bUseSortieRepeat)
	{
		GetDlgItem(IDC_STATIC_SORTIE_ENABLE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CHECK_SORTIE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SORITE_CNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDIT_SOTIE_CNT)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_STATIC_SORTIE_DIR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_SORITE_DIR)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RADIO_SORITE_DIR2)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_STATIC_SORTIE_ENABLE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CHECK_SORTIE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SORITE_CNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EDIT_SOTIE_CNT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_SORTIE_DIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_SORITE_DIR)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RADIO_SORITE_DIR2)->ShowWindow(SW_HIDE);
	}

	// sjyi 2024.05.27 Sortie 반복 횟수 추가
	m_editSortieCnt.SetValue(m_nSortieCnt);

	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgHairpinSet::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	m_dblZThreshold = m_editZThreshold.GetValue();
	m_nZCtn			= m_editZnCnt.GetValue();
	m_dblZPwr = m_editZPwr.GetValue() / 100.0; // mspark 2022.04.20
	for (int i = 0; i < 10; i++)
	{
		m_zComp[i].dblMin = m_editZMin[i].GetValue();
		m_zComp[i].dblMax = m_editZMax[i].GetValue();
		m_zComp[i].nCnt = m_editZCnt[i].GetValue();
	}

	double dblScale, dblAngle, dblOffsetX, dblOffsetY;
	for (int i = 0; i < 6; i++)
	{
		dblScale = m_editRcpScale[i].GetValue();
		dblAngle = m_editRcpAngle[i].GetValue();
		dblOffsetX = m_editRcpOffsetX[i].GetValue();
		dblOffsetY = m_editRcpOffsetY[i].GetValue();

		CString msg;
		if (m_dRcpScale[i] != dblScale)
		{
			msg.Format(L"RCP%d Scale: %.4f -> %.4f", i + 1, m_dRcpScale[i], dblScale);
			_tracer(TRACE_NORMAL, 3000, msg);
			m_dRcpScale[i] = dblScale;
		}

		if (m_dRcpAngle[i] != dblAngle)
		{
			msg.Format(L"RCP%d Angle: %.4f -> %.4f", i + 1, m_dRcpAngle[i], dblAngle);
			_tracer(TRACE_NORMAL, 3000, msg);
			m_dRcpAngle[i] = dblAngle;
		}

		if (m_dRcpOffsetX[i] != dblOffsetX)
		{
			msg.Format(L"RCP%d OffsetX: %.4f -> %.4f", i + 1, m_dRcpOffsetX[i], dblOffsetX);
			_tracer(TRACE_NORMAL, 3000, msg);
			m_dRcpOffsetX[i] = dblOffsetX;
		}

		if (m_dRcpOffsetY[i] != dblOffsetY)
		{
			msg.Format(L"RCP%d OffsetY: %.4f -> %.4f", i + 1, m_dRcpOffsetY[i], dblOffsetY);
			_tracer(TRACE_NORMAL, 3000, msg);
			m_dRcpOffsetY[i] = dblOffsetY;
		}		
	}

	m_dPowerRatio = m_editPowerRatio.GetValue();
	m_dSpeedRatio = m_editSpeedRatio.GetValue();

	m_dLegendAng = m_editLegendAng.GetValue();
	m_dLegendDia = m_editLegendDia.GetValue();
	m_dLegendDir = m_editLegendDir.GetValue();

	/*m_dDegree12 = m_edit12degree.GetValue();
	m_dDegree34 = m_edit34degree.GetValue();
	m_dDegree56 = m_edit56degree.GetValue();
	m_dDegree78 = m_edit78degree.GetValue();*/

	// sjyi 2024.05.27 Sortie 반복 횟수 추가
	m_nSortieCnt = m_editSortieCnt.GetValue();

	UpdateData();

	CDialogEx::OnOK();
}

