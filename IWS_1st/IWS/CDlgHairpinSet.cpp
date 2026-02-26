// CDlgHairpinSet.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgHairpinSet.h"
#include "afxdialogex.h"
#include "CDlgSortieMap.h"


// CDlgHairpinSet 대화 상자

IMPLEMENT_DYNAMIC(CDlgHairpinSet, CDialogEx)

CDlgHairpinSet::CDlgHairpinSet(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_HAIPPIN_SET, pParent)
	, m_nZAxisCompMove(0)
	, m_bUseNGChkJigA(FALSE)
	, m_bUseNGChkJigB(FALSE)
	, m_bSaveHResut(FALSE)
	, m_nDivStartSlot(0)
	, m_nDivOffset(0)
	, m_nModel(0)
	, m_nJig(0)
	, m_b3DUse(0)
	, m_bSortieEnable(FALSE)
	, m_strPosPathJigA(_T(""))
	, m_strPosPathJigB(_T(""))
{
	m_nSlotCnt = 0;
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
	DDX_Control(pDX, IDC_EDIT_Z_PWR, m_editZPwr);
	/*DDX_Control(pDX, IDC_EDIT_DEGREE12, m_edit12degree);
	DDX_Control(pDX, IDC_EDIT_DEGREE34, m_edit34degree);
	DDX_Control(pDX, IDC_EDIT_DEGREE56, m_edit56degree);
	DDX_Control(pDX, IDC_EDIT_DEGREE78, m_edit78degree);*/
	DDX_Radio(pDX, IDC_RADIO_ZAXIS_COMP, m_nZAxisCompMove);
	DDX_Control(pDX, IDC_EDIT_HCOMP_WD, m_editHCompWD);
	DDX_Control(pDX, IDC_EDIT_HCOMP_CEN_X, m_editHCompCenX);
	DDX_Control(pDX, IDC_EDIT_HCOMP_CEN_Y, m_editHCompCenY);
	DDX_Control(pDX, IDC_EDIT_HCOMP_REF, m_editHCompRef);
	DDX_Control(pDX, IDC_EDIT_HCOMP_TC_ANGLE, m_editHCompTeleAngle);
	DDX_Control(pDX, IDC_EDIT_HCOMP_WORK_R, m_editHCompWorkRadius);
	DDX_Check(pDX, IDC_CHECK_SAVE_HRESUT, m_bSaveHResut);
	DDX_Text(pDX, IDC_EDIT_START_SLOT, m_nDivStartSlot);
	DDX_Text(pDX, IDC_EDIT_DIVISION_OFFSET, m_nDivOffset);

	DDX_CBIndex(pDX, IDC_COMBO_MODEL, m_nModel);
	DDX_CBIndex(pDX, IDC_COMBO_JIG, m_nJig);

	DDX_Control(pDX, IDC_EDIT_SOTIE_CNT, m_editSortieCnt);
	DDX_Check(pDX, IDC_CHECK_SORTIE, m_bSortieEnable);
	DDX_Radio(pDX, IDC_RADIO_SORITE_DIR, m_nSortieDir);

	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_JIGA, m_bUseAllwaysScanJigA);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_JIGB, m_bUseAllwaysScanJigB);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_REWELD_A, m_bUseAllwaysScanReweldA);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_REWELD_B, m_bUseAllwaysScanReweldB);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_WELD_A, m_bUseAllwaysScanWeldA);
	DDX_Check(pDX, IDC_CHECK_SCAN_ENABLE_WELD_B, m_bUseAllwaysScanWeldB);

	DDX_Check(pDX, IDC_CHECK_USE_NGCHECK, m_bUseNGChkJigA);
	DDX_Check(pDX, IDC_CHECK_USE_NGCHECK2, m_bUseNGChkJigB);
	DDX_Control(pDX, IDC_EDIT_HCOMP_NGD, m_editNGDJigA);
	DDX_Control(pDX, IDC_EDIT_HCOMP_NGD2, m_editNGDJigB);

	DDX_Text(pDX, IDC_EDIT_POS_PATH_JIGA, m_strPosPathJigA);
	DDX_Text(pDX, IDC_EDIT_POS_PATH_JIGB, m_strPosPathJigB);

	DDX_Control(pDX, IDC_EDIT_MATRIX_DIV_CNT, m_editMatrixDivCnt);
}


BEGIN_MESSAGE_MAP(CDlgHairpinSet, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgHairpinSet::OnBnClickedOk)
	ON_BN_CLICKED(IDC_RADIO_ZAXIS_COMP, &CDlgHairpinSet::OnBnClickedRadioZaxisComp)
	ON_BN_CLICKED(IDC_CHECK_USE_NGCHECK, &CDlgHairpinSet::OnBnClickedCheckUseNgcheck)
	ON_BN_CLICKED(IDC_RADIO_ZAXIS_COMP2, &CDlgHairpinSet::OnBnClickedRadioZaxisComp)
	ON_CBN_SELCHANGE(IDC_COMBO_MODEL, &CDlgHairpinSet::OnCbnSelchangeComboModel)
	ON_CBN_SELCHANGE(IDC_COMBO_JIG, &CDlgHairpinSet::OnCbnSelchangeComboJig)
	ON_BN_CLICKED(IDC_BTN_SET_HREF, &CDlgHairpinSet::OnBnClickedBtnSetHref)
	ON_BN_CLICKED(IDCANCEL, &CDlgHairpinSet::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_RADIO_RCP_3D, &CDlgHairpinSet::OnClickedRadioRcp3d)
	ON_BN_CLICKED(IDC_RADIO_RCP_2D, &CDlgHairpinSet::OnClickedRadioRcp2d)
	ON_BN_CLICKED(IDC_BUTTON_RCP_ENABLE, &CDlgHairpinSet::OnBnClickedButtonRcpEnable)
	ON_BN_CLICKED(IDC_CHECK_SORTIE, &CDlgHairpinSet::OnClickedCheckSortie)
	ON_EN_KILLFOCUS(IDC_EDIT_SOTIE_CNT, &CDlgHairpinSet::OnKillfocusEditSotieCnt)
	ON_BN_CLICKED(IDC_BTN_POS_PATH_JIGA, &CDlgHairpinSet::OnBnClickedBtnPosPathJiga)
	ON_BN_CLICKED(IDC_BTN_POS_PATH_JIGB, &CDlgHairpinSet::OnBnClickedBtnPosPathJigb)
	ON_EN_KILLFOCUS(IDC_EDIT_MATRIX_DIV_CNT, &CDlgHairpinSet::OnKillfocusEditMatrixDivCnt)
	ON_BN_CLICKED(IDC_BTN_SORTIE_MAP, &CDlgHairpinSet::OnBnClickedBtnSortieMap)
END_MESSAGE_MAP()


// CDlgHairpinSet 메시지 처리기
BOOL CDlgHairpinSet::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_bEnable = FALSE;
	CString strEnable = _T("Enable");
	GetDlgItem(IDC_BUTTON_RCP_ENABLE)->SetWindowText(strEnable);
	
	GetDlgItem(IDC_RADIO_RCP_3D)->EnableWindow(FALSE);
	GetDlgItem(IDC_RADIO_RCP_2D)->EnableWindow(FALSE);


	m_editZThreshold.SetValue(m_dblZThreshold);
	m_editZnCnt.SetValue(m_nZCtn);
	m_editZPwr.SetValue(m_dblZPwr*100.0, L"%0.1f");// mspark 2022.04.20
	for (int i = 0; i < 10; i++)
	{
		m_editZMin[i].SetValue(m_zComp[i].dblMin, L"%.1f");
		m_editZMax[i].SetValue(m_zComp[i].dblMax, L"%.1f");
		m_editZCnt[i].SetValue(m_zComp[i].nCnt);
	}

	if (m_b3DUse == TRUE)
	{
		((CButton*)GetDlgItem(IDC_RADIO_RCP_3D))->SetCheck(TRUE);
		for (int i = 0; i < 6; i++)
		{
			m_editRcpScale[i].SetValue(m_dRcpScale[i], L"%.4f");
			m_editRcpAngle[i].SetValue(m_dRcpAngle[i], L"%.4f");
			m_editRcpOffsetX[i].SetValue(m_dRcpOffsetX[i], L"%.4f");
			m_editRcpOffsetY[i].SetValue(m_dRcpOffsetY[i], L"%.4f");
		}
	}
	else
	{
		((CButton*)GetDlgItem(IDC_RADIO_RCP_2D))->SetCheck(TRUE);
		for (int i = 0; i < 6; i++)
		{
			m_editRcpScale[i].SetValue(m_d2DRcpScale[i], L"%.4f");
			m_editRcpAngle[i].SetValue(m_d2DRcpAngle[i], L"%.4f");
			m_editRcpOffsetX[i].SetValue(m_d2DRcpOffsetX[i], L"%.4f");
			m_editRcpOffsetY[i].SetValue(m_d2DRcpOffsetY[i], L"%.4f");
		}
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

	m_editHCompWD.SetValue(m_dHCompWD, L"%.3f");
	m_editHCompRef.SetValue(m_dHCompRef[0], L"%.3f");
	m_editHCompCenX.SetValue(m_dHCompCenX, L"%.3f");
	m_editHCompCenY.SetValue(m_dHCompCenY, L"%.3f");


	m_editHCompTeleAngle.SetValue(m_dHCompTCMaxAngle, L"%.3f");
	m_editHCompWorkRadius.SetValue(m_dHCompWorkRadius, L"%.3f");

	CheckZAxisCompMove();

	// sjyi 2024.05.27 Sortie 반복 횟수 추가
	m_editSortieCnt.SetValue(m_nSortieCnt);


	m_editNGDJigA.SetValue(m_dNGDistJigA, L"%.3f");
	m_editNGDJigB.SetValue(m_dNGDistJigB, L"%.3f");

	m_editMatrixDivCnt.SetValue(m_nMatrixDivCnt);


	CheckSortieUI();

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

	m_b3DUse = ((CButton*)GetDlgItem(IDC_RADIO_RCP_3D))->GetCheck();
	
	double dblScale, dblAngle, dblOffsetX, dblOffsetY;
	for (int i = 0; i < 6; i++)
	{
		dblScale = m_editRcpScale[i].GetValue();
		dblAngle = m_editRcpAngle[i].GetValue();
		dblOffsetX = m_editRcpOffsetX[i].GetValue();
		dblOffsetY = m_editRcpOffsetY[i].GetValue();

		if (m_b3DUse == TRUE)
		{
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
		else
		{
			CString msg;
			if (m_d2DRcpScale[i] != dblScale)
			{
				msg.Format(L"RCP%d Scale 2D : %.4f -> %.4f", i + 1, m_d2DRcpScale[i], dblScale);
				_tracer(TRACE_NORMAL, 3000, msg);
				m_d2DRcpScale[i] = dblScale;
			}

			if (m_d2DRcpAngle[i] != dblAngle)
			{
				msg.Format(L"RCP%d Angle 2D : %.4f -> %.4f", i + 1, m_d2DRcpAngle[i], dblAngle);
				_tracer(TRACE_NORMAL, 3000, msg);
				m_d2DRcpAngle[i] = dblAngle;
			}

			if (m_d2DRcpOffsetX[i] != dblOffsetX)
			{
				msg.Format(L"RCP%d OffsetX 2D : %.4f -> %.4f", i + 1, m_d2DRcpOffsetX[i], dblOffsetX);
				_tracer(TRACE_NORMAL, 3000, msg);
				m_d2DRcpOffsetX[i] = dblOffsetX;
			}

			if (m_d2DRcpOffsetY[i] != dblOffsetY)
			{
				msg.Format(L"RCP%d OffsetY 2D : %.4f -> %.4f", i + 1, m_d2DRcpOffsetY[i], dblOffsetY);
				_tracer(TRACE_NORMAL, 3000, msg);
				m_d2DRcpOffsetY[i] = dblOffsetY;
			}
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

	m_dHCompWD = m_editHCompWD.GetValue();
	//m_dHCompRef = m_editHCompRef.GetValue();
	m_dHCompCenX = m_editHCompCenX.GetValue();
	m_dHCompCenY = m_editHCompCenY.GetValue();

	m_dHCompTCMaxAngle = m_editHCompTeleAngle.GetValue();
	m_dHCompWorkRadius = m_editHCompWorkRadius.GetValue();

	// sjyi 2024.05.27 Sortie 반복 횟수 추가
	m_nSortieCnt = m_editSortieCnt.GetValue();

	m_dNGDistJigA = m_editNGDJigA.GetValue();
	m_dNGDistJigB = m_editNGDJigB.GetValue();

	// sjyi 2024.10.20 Matrix Div Count 추가
	m_nMatrixDivCnt = m_editMatrixDivCnt.GetValue();

	UpdateData();

	// Check Division Value & Message
	CString msg;
	msg.Format(L"Division Value : (WeldOption : %d), (Start Slot : %d), (Jump Offset : %d)", m_nWeldType, m_nDivStartSlot, m_nDivOffset);
	_tracer(TRACE_NORMAL, 3000, msg);
	CheckDivParams();

	CDialogEx::OnOK();
}



void CDlgHairpinSet::OnBnClickedRadioZaxisComp()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CheckZAxisCompMove();
}

void CDlgHairpinSet::OnBnClickedCheckUseNgcheck()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
//	CheckZAxisCompNGCheck();
}


void CDlgHairpinSet::CheckZAxisCompMove()
{
	if (m_nZAxisCompMove)
	{
		GetDlgItem(IDC_EDIT_HCOMP_WD)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HCOMP_REF)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HCOMP_CEN_X)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HCOMP_CEN_Y)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HCOMP_TC_ANGLE)->EnableWindow(TRUE);
		GetDlgItem(IDC_EDIT_HCOMP_WORK_R)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHECK_SAVE_HRESUT)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_MODEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_COMBO_JIG)->EnableWindow(TRUE);
		GetDlgItem(IDC_BTN_SET_HREF)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_EDIT_HCOMP_WD)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HCOMP_REF)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HCOMP_CEN_X)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HCOMP_CEN_Y)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HCOMP_TC_ANGLE)->EnableWindow(FALSE);
		GetDlgItem(IDC_EDIT_HCOMP_WORK_R)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHECK_SAVE_HRESUT)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_MODEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_COMBO_JIG)->EnableWindow(FALSE);
		GetDlgItem(IDC_BTN_SET_HREF)->EnableWindow(FALSE);
	}
}

//void CDlgHairpinSet::CheckZAxisCompNGCheck()
//{
//	if (m_bUseHCompNGChk)
//	{
//		GetDlgItem(IDC_EDIT_HCOMP_NGD)->EnableWindow(TRUE);
//	}
//	else
//	{
//		GetDlgItem(IDC_EDIT_HCOMP_NGD)->EnableWindow(FALSE);
//	}
//}

void CDlgHairpinSet::CheckDivParams()
{
	CString msg;
	if (m_nDivStartSlot > m_nSlotCnt)
	{
		msg.Format(L"Division Start Slot Value is Over : (Start Slot : %d)", m_nDivStartSlot);
		_tracer(TRACE_NORMAL, 3000, msg);

		m_nDivStartSlot = m_nSlotCnt;
	}
	else if (m_nDivStartSlot < 1)
	{
		msg.Format(L"Division Start Slot Value is Under : (Start Slot : %d)", m_nDivStartSlot);
		_tracer(TRACE_NORMAL, 3000, msg);

		m_nDivStartSlot = 1;
	}

	if (m_nDivOffset >= m_nSlotCnt)
	{
		msg.Format(L"Division Jump Offset Value is Over : (Jump Offset : %d)", m_nDivOffset);
		_tracer(TRACE_NORMAL, 3000, msg);

		m_nDivOffset = m_nSlotCnt - 1;
	}
	else if (m_nDivOffset < 1)
	{
		msg.Format(L"Division Jump Offset Value is Under : (Jump Offset : %d)", m_nDivOffset);
		_tracer(TRACE_NORMAL, 3000, msg);

		m_nDivOffset = 1;
	}

	UpdateData(FALSE);
}

void CDlgHairpinSet::OnCbnSelchangeComboModel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetHCompRefHeight();
}


void CDlgHairpinSet::OnCbnSelchangeComboJig()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	SetHCompRefHeight();
}

void CDlgHairpinSet::SetHCompRefHeight()
{
	UpdateData(TRUE);

	int nRefIndex = m_nModel * 2 + m_nJig;

	m_editHCompRef.SetValue(m_dHCompRef[nRefIndex]);
}

void CDlgHairpinSet::OnBnClickedBtnSetHref()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);

	int nRefIndex = m_nModel * 2 + m_nJig;

	m_dHCompRef[nRefIndex] = m_editHCompRef.GetValue();
}


void CDlgHairpinSet::OnBnClickedCancel()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDialogEx::OnCancel();
}


void CDlgHairpinSet::OnClickedRadioRcp3d()
{
	m_b3DUse = TRUE;
	for (int i = 0; i < 6; i++)
	{
		m_editRcpScale[i].SetValue(m_dRcpScale[i], L"%.4f");
		m_editRcpAngle[i].SetValue(m_dRcpAngle[i], L"%.4f");
		m_editRcpOffsetX[i].SetValue(m_dRcpOffsetX[i], L"%.4f");
		m_editRcpOffsetY[i].SetValue(m_dRcpOffsetY[i], L"%.4f");
	}
}


void CDlgHairpinSet::OnClickedRadioRcp2d()
{
	m_b3DUse = FALSE;
	for (int i = 0; i < 6; i++)
	{
		m_editRcpScale[i].SetValue(m_d2DRcpScale[i], L"%.4f");
		m_editRcpAngle[i].SetValue(m_d2DRcpAngle[i], L"%.4f");
		m_editRcpOffsetX[i].SetValue(m_d2DRcpOffsetX[i], L"%.4f");
		m_editRcpOffsetY[i].SetValue(m_d2DRcpOffsetY[i], L"%.4f");
	}
}



void CDlgHairpinSet::OnBnClickedButtonRcpEnable()
{
	CString strEnable = _T("Enable");
	if (m_bEnable == FALSE)
	{
		m_bEnable = TRUE;

		strEnable = _T("Disable");
		GetDlgItem(IDC_BUTTON_RCP_ENABLE)->SetWindowText(strEnable);

		GetDlgItem(IDC_RADIO_RCP_3D)->EnableWindow(TRUE);
		GetDlgItem(IDC_RADIO_RCP_2D)->EnableWindow(TRUE);
	}
	else
	{
		m_bEnable = FALSE;

		GetDlgItem(IDC_BUTTON_RCP_ENABLE)->SetWindowText(strEnable);

		GetDlgItem(IDC_RADIO_RCP_3D)->EnableWindow(FALSE);
		GetDlgItem(IDC_RADIO_RCP_2D)->EnableWindow(FALSE);
	}
}


void CDlgHairpinSet::OnClickedCheckSortie()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	UpdateData(TRUE);
	CheckSortieUI();

	if (m_bSortieEnable)
	{
		_tracer(TRACE_ERROR, 1, L"Sortie Repeat has been Enabled.");

		//gseom_240426_언어변경_Stringtable 불러오기
		CString AfxMsg;
		AfxMsg.Format(_T("Sortie Repeat has been Enabled.\nPlease, Check out the 1st Pattern's Repeat"));
		AfxMessageBox(AfxMsg, MB_ICONWARNING);
	}
	else
	{
		_tracer(TRACE_ERROR, 1, L"Sortie Repeat has been Disabled.");
	}
}

void CDlgHairpinSet::OnKillfocusEditSotieCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_nSortieCnt != m_editSortieCnt.GetValue())
	{
		_tracer(TRACE_ERROR, 1, L"Sortie Repeat Count has been changed. [%d] -> [%d]", m_nSortieCnt, m_editSortieCnt.GetValue());
		//gseom_240426_언어변경_Stringtable 불러오기
		CString strMsg;
		strMsg.Format(_T("Sortie Repeat Count has been changed. [%d] -> [%d]\nPlease, Check out the 1st Pattern's Repeat"), m_nSortieCnt, m_editSortieCnt.GetValue());
		AfxMessageBox(strMsg, MB_ICONWARNING);

		m_nSortieCnt = m_editSortieCnt.GetValue();
	}
}

void CDlgHairpinSet::OnKillfocusEditMatrixDivCnt()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	if (m_editMatrixDivCnt.GetValue() > MATRIX_DIVISION_MAX) {
		CString strMsg;
		strMsg.Format(_T("Matrix Div Count cannot over %d.\nPlease, check the Matrix Cnt"), MATRIX_DIVISION_MAX);
		AfxMessageBox(strMsg, MB_ICONWARNING);

		return;
	}

	if (m_nMatrixDivCnt != m_editMatrixDivCnt.GetValue())
	{
		_tracer(TRACE_ERROR, 1, L"Matrix Div Count has been changed. [%d] -> [%d]", m_nMatrixDivCnt, m_editMatrixDivCnt.GetValue());

		CString strMsg;
		strMsg.Format(_T("Matrix Div Count has been changed. [%d] -> [%d]\nPlease, Restart IWS software"), m_nMatrixDivCnt, m_editMatrixDivCnt.GetValue());
		AfxMessageBox(strMsg, MB_ICONWARNING);

		m_nMatrixDivCnt = m_editMatrixDivCnt.GetValue();
	}
}

void CDlgHairpinSet::CheckSortieUI()
{
	m_editSortieCnt.EnableWindow(m_bSortieEnable);
	GetDlgItem(IDC_RADIO_SORITE_DIR)->EnableWindow(m_bSortieEnable);
	GetDlgItem(IDC_RADIO_SORITE_DIR2)->EnableWindow(m_bSortieEnable);
	GetDlgItem(IDC_BTN_SORTIE_MAP)->EnableWindow(m_bSortieEnable);
}

void CDlgHairpinSet::OnBnClickedBtnPosPathJiga()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strFilter;
	strFilter = _T("Position File(*.pos)|*.pos||");
	// 프로그램 실행 경로
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strDefPath = m_strPosPathJigA;
	int i = strDefPath.ReverseFind('\\');//실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strDefPath = strDefPath.Left(i + 1);//뒤에 있는 현재 실행 파일 이름을 지운다.
	CString strDefaultFilename = strDefPath;
	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.pos"),													//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;

	m_strPosPathJigA = dlgFile.GetPathName();

	UpdateData(FALSE);
}


void CDlgHairpinSet::OnBnClickedBtnPosPathJigb()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CString strFilter;
	strFilter = _T("Position File(*.pos)|*.pos||");
	// 프로그램 실행 경로
	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strDefPath = m_strPosPathJigB;
	int i = strDefPath.ReverseFind('\\');//실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strDefPath = strDefPath.Left(i + 1);//뒤에 있는 현재 실행 파일 이름을 지운다.
	CString strDefaultFilename = strDefPath;
	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.pos"),													//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;

	m_strPosPathJigB = dlgFile.GetPathName();

	UpdateData(FALSE);
}


void CDlgHairpinSet::OnBnClickedBtnSortieMap()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	CDlgSortieMap dlgSortieMap;
	dlgSortieMap.SetSortieRptMap(m_nSortieRptMap);

	if (dlgSortieMap.DoModal() == IDOK)
	{
		dlgSortieMap.GetSortieRptMap(m_nSortieRptMap);
	}
}
