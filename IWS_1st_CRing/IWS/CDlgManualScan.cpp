// CDlgManualScan.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgManualScan.h"
#include "afxdialogex.h"
#include "IWSDoc.h"

// CDlgManualScan 대화 상자
IMPLEMENT_DYNAMIC(CDlgManualScan, CDialogEx)

CDlgManualScan::CDlgManualScan(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_MANUAL_SCAN, pParent)
{
	m_pScan = NULL;
	m_pPenOld = NULL;
}

CDlgManualScan::~CDlgManualScan()
{
}

void CDlgManualScan::SetDocument(CIWSDoc *pDoc)
{
	m_pDoc = pDoc;
	m_pScan = m_pDoc->m_pScanner;
	m_pPens = &(m_pDoc->m_pens);
	
}

double CDlgManualScan::CalcFrequency(double pulseWidth, double dutyCycle)
{
	return 1.0 / (pulseWidth / dutyCycle);
}

double CDlgManualScan::CalcPulseWidth(double frequency, double dutyCycle)
{
	return (1.0 / frequency * dutyCycle);
}

void CDlgManualScan::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_FREQUENCY, m_editFrequency);
	DDX_Control(pDX, IDC_PULSE_WIDTH, m_editPulseWidth);
	DDX_Control(pDX, IDC_POWER, m_editPower);
	DDX_Control(pDX, IDC_SCAN_SPEED, m_editScanSpeed);
	DDX_Control(pDX, IDC_LASER_ENABLE, m_chkLaserOn);
	DDX_Control(pDX, IDC_LASER_ON_TIME, m_editLaserOnTime);
	DDX_Control(pDX, IDC_GRID_SIZE, m_editGridSize);
	DDX_Control(pDX, IDC_GRID_CNT, m_editGridCnt);
	DDX_Control(pDX, IDC_SCAN_MOVEX, m_editMoveX);
	DDX_Control(pDX, IDC_SCAN_MOVEY, m_editMoveY);
	DDX_Control(pDX, IDC_COMBO_LASERPENS, m_cbLaserPens);
	DDX_Control(pDX, IDC_RATIO_PWR, m_editRatioPwr);
	DDX_Control(pDX, IDC_RATIO_SPD, m_editRatioSpd);
}


BEGIN_MESSAGE_MAP(CDlgManualScan, CDialogEx)
	ON_BN_CLICKED(IDOK, &CDlgManualScan::OnBnClickedOk)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_LASER_ENABLE, &CDlgManualScan::OnBnClickedLaserEnable)
	ON_CBN_SELCHANGE(IDC_COMBO_LASERPENS, &CDlgManualScan::OnCbnSelchangeComboLaserpens)
	ON_BN_CLICKED(IDC_SCAN_MOVE, &CDlgManualScan::OnBnClickedScanMove)
	ON_BN_CLICKED(IDC_BUTTION_SAVEPEN, &CDlgManualScan::OnBnClickedButtionSavepen)
	ON_BN_CLICKED(IDC_LASER_EMISSION, &CDlgManualScan::OnBnClickedLaserEmission)
	ON_BN_CLICKED(IDC_LASER_ON, &CDlgManualScan::OnBnClickedLaserOn)
	ON_BN_CLICKED(IDC_GRID_MARK, &CDlgManualScan::OnBnClickedGridMark)
	ON_BN_CLICKED(IDC_DIR_MARK, &CDlgManualScan::OnBnClickedDirMark)
	ON_WM_SHOWWINDOW()
END_MESSAGE_MAP()


// CDlgManualScan 메시지 처리기

void CDlgManualScan::OnBnClickedOk()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	this->ShowWindow(SW_HIDE);
	// 가이드 레이저 셋으로 변경
	// &(*m_pPens)[0]
	m_pScan->SetLaserPen(&(*m_pPens)[14]);
	m_pScan->SetLaserPenParam();

	//if (m_pPenOld) {
	//	m_pScan->SetLaserPen(m_pPenOld);
	//	m_pScan->SetLaserPenParam();
	//}
}

void CDlgManualScan::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete this;
	CDialogEx::PostNcDestroy();
}


BOOL CDlgManualScan::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	//CString str;
	//int nPens = (int)m_pPens->size();
	//for (int i = 0; i < nPens; i++)
	//{
	//	str.Format(L"Laser Pen %d", (*m_pPens)[i].m_iPenID);
	//	m_cbLaserPens.AddString(str);
	//}
	//m_cbLaserPens.SetCurSel(0);
	//CLaserPen *pPen = &(*m_pPens)[0];
	//switch (m_pDoc->m_config.m_iLaser)
	//{
	//case IPG_CW_YLR2000SM:
	//case IPG_CW_YLR6000QCW:
	//	m_editPulseWidth.EnableWindow(FALSE);
	//	m_editFrequency.EnableWindow(FALSE);
	//	break;
	//case IPG_PULSE_YLR6000QCW:
	//	m_editFrequency.EnableWindow(FALSE);
	//	
	//	m_editFrequency.SetValue(CalcFrequency(pPen->m_dblPulseWidth, PULSE_DUTY_CYCLE) * 1000000.0, _T("%.3f"));
	//	m_editPulseWidth.SetValue(pPen->m_dblPulseWidth, _T("%0.3f"));
	//	m_editPulseWidth.SetMinMax(200, 50000);
	//	break;
	//}
	
	m_editPulseWidth.EnableWindow(FALSE);
	/*m_editPower.SetValue(pPen->m_dblPower, _T("%0.1f"));
	m_editScanSpeed.SetValue(pPen->m_dblMarkSpeed, _T("%0.1f"));
	m_pen = *pPen;*/

	m_editPower.SetValue(3);
	m_editScanSpeed.SetValue(2000);
	m_editLaserOnTime.SetValue(100);

	m_editMoveX.SetValue(0);
	m_editMoveY.SetValue(0);

	CString section = L"Manual Scan";
	CString strVal;
	strVal.Format(L"%f", 100.0);
	int iLaserOnTime = theApp.GetProfileInt(section, _T("Laser On Time"), 10);
	strVal.Format(L"%f", 10.0);
	strVal = theApp.GetProfileString(section, _T("Grid Size"), strVal);
	double dGridSize = _tstof(strVal); m_editGridSize.SetValue(dGridSize, _T("%.1f"));
	int iGridCnt = theApp.GetProfileInt(section, _T("Grid Count"), 3);
	m_editGridCnt.SetValue(iGridCnt);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지az는 FALSE를 반환해야 합니다.
}


void CDlgManualScan::OnBnClickedLaserEnable()
{
	BOOL bChk = m_chkLaserOn.GetCheck();
	double dPower = m_editPower.GetValue();
	m_pScan->LaserEnable(bChk, 0);
}


void CDlgManualScan::OnCbnSelchangeComboLaserpens()
{
	int idx = m_cbLaserPens.GetCurSel();
	CLaserPen *pPen = &(*m_pPens)[idx];

	switch (m_pScan->GetLaser())
	{
	case IPG_CW_YLR2000SM:
	case IPG_CW_YLR6000QCW:
		m_editPulseWidth.EnableWindow(FALSE);
		m_editFrequency.EnableWindow(FALSE);
		break;
	case IPG_PULSE_YLR6000QCW:
		m_editFrequency.EnableWindow(FALSE);

		m_editFrequency.SetValue(CalcFrequency(pPen->m_dblPulseWidth/1000000.0, PULSE_DUTY_CYCLE) / 1000.0, _T("%.3f"));
		m_editPulseWidth.SetValue(pPen->m_dblPulseWidth, _T("%0.3f"));
		break;
	}
	
	m_editPower.SetValue(pPen->m_dblPower, _T("%0.1f"));
	m_editScanSpeed.SetValue(pPen->m_dblMarkSpeed, _T("%0.1f"));
	m_pen = *pPen;
}


void CDlgManualScan::OnBnClickedScanMove()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	double dPosX, dPosY;
	dPosX = m_editMoveX.GetValue();
	dPosY = m_editMoveY.GetValue();
	m_pScan->Move(dPosX, dPosY);
}


void CDlgManualScan::OnBnClickedButtionSavepen()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int idx = m_cbLaserPens.GetCurSel();
	switch (m_pScan->GetLaser())
	{
	case IPG_CW_YLR2000SM:
	case IPG_CW_YLR6000QCW:
		m_editPulseWidth.EnableWindow(FALSE);
		m_editFrequency.EnableWindow(FALSE);
		break;
	case IPG_PULSE_YLR6000QCW:
		m_editFrequency.EnableWindow(FALSE);
		m_editFrequency.SetValue(CalcFrequency(m_editPulseWidth.GetValue() / 1000000.0, PULSE_DUTY_CYCLE) / 1000.0, _T("%.3f"));
		break;
	}

	m_pen.m_dblPower = m_editPower.GetValue();
	m_pen.m_dblFrequency = m_editFrequency.GetValue();
	m_pen.m_dblPulseWidth = m_editPulseWidth.GetValue();
	m_pen.m_dblMarkSpeed = m_editScanSpeed.GetValue();

	(*m_pPens)[idx] = m_pen;
	m_pDoc->SaveLaserPen(m_pDoc->m_strFileLaserPen);
}


void CDlgManualScan::OnBnClickedLaserEmission()
{
	double dPower = m_editPower.GetValue();
	double dRatioPwr = 1;//m_editRatioPwr.GetValue();
	double dRatioSpd = 1;//m_editRatioSpd.GetValue();

	m_pScan->SetRatio(dRatioPwr, dRatioSpd);
	m_pScan->SetPower(dPower);

	m_pen.m_dblFrequency = m_editFrequency.GetValue();
	m_pen.m_dblPulseWidth = m_editPulseWidth.GetValue();
	m_pen.m_dblMarkSpeed = m_editScanSpeed.GetValue();
	m_pen.m_dblJumpSpeed = m_editScanSpeed.GetValue();

	int iLaserOnTime = m_editLaserOnTime.GetValue();

	m_pScan->SetLaserPen(&m_pen);
	m_pScan->SetLaserPenParam();
	m_pScan->WaitMoving(TRUE);

	m_pScan->LaserOn(TRUE);
	Pause(iLaserOnTime);
	m_pScan->LaserOn(FALSE);
	m_pScan->SetPower(0);
	CString section = L"Manual Scan";
	theApp.WriteProfileInt(section, _T("Laser On Time"), iLaserOnTime);
}


void CDlgManualScan::OnBnClickedLaserOn()
{
	BOOL bLaserOn = m_chkLaserOn.GetCheck();
	CString caption;
	double dPower = m_editPower.GetValue();
	if (bLaserOn) {
		m_pScan->SetPower(dPower);
		m_pen.m_dblFrequency = m_editFrequency.GetValue();
		m_pen.m_dblPulseWidth = m_editPulseWidth.GetValue();
		m_pen.m_dblMarkSpeed = m_editScanSpeed.GetValue();
		m_pen.m_dblJumpSpeed = m_editScanSpeed.GetValue();

		m_pScan->SetLaserPen(&m_pen);
		m_pScan->SetLaserPenParam();
		m_pScan->WaitMoving(TRUE);

		caption = _T("출사 정지");
		m_pScan->LaserOn(TRUE);
	}
	else {
		caption = _T("연속 출사");
		m_pScan->LaserOn(FALSE);
		m_pScan->SetPower(0);
	}
	m_chkLaserOn.SetWindowText(caption);
}


void CDlgManualScan::OnBnClickedGridMark()
{
	double dPower = m_editPower.GetValue();
	m_pScan->SetPower(dPower);
	m_pen.m_dblFrequency = m_editFrequency.GetValue();
	m_pen.m_dblPulseWidth = m_editPulseWidth.GetValue();
	m_pen.m_dblMarkSpeed = m_editScanSpeed.GetValue();
	m_pen.m_dblJumpSpeed = m_editScanSpeed.GetValue();

	m_pScan->SetLaserPen(&m_pen);
	m_pScan->SetLaserPenParam();
	m_pScan->WaitMoving(TRUE);
	double size = m_editGridSize.GetValue();
	int    cnt = m_editGridCnt.GetValue();
	double spos = -size / 2.0;
	double tpos = size / 2.0;
	double gap = cnt > 0 ? size/(cnt) : size;
	double pos;

	//m_pScan->CircularQueueMode(FALSE);
	m_pScan->StartList();
	TRACE("First\n");
	pos = spos;// +gap;
	for (int i = 0; i < cnt + 1; i++) 
	{
		m_pScan->JumpList(pos + gap * i, spos - 3);
		m_pScan->MarkList(pos + gap * i, tpos + 3);
	}
	pos = spos;// +gap;
	for (int i = 0; i < cnt + 1; i++) 
	{
		m_pScan->JumpList(spos - 3, pos + gap * i);
		m_pScan->MarkList(tpos + 3, pos + gap * i);
	}

	//m_pScan->JumpList(spos, spos);
	//m_pScan->MarkList(spos, tpos);
	//m_pScan->MarkList(tpos, tpos);
	//m_pScan->MarkList(tpos, spos);
	//m_pScan->MarkList(spos, spos);
	m_pScan->EndList();

	m_pScan->Execute();
	m_pScan->WaitMoving(TRUE);
	m_pScan->SetPower(0);
	m_pScan->Move(0, 0);

	CString section = L"Manual Scan";
	theApp.WriteProfileInt(section,  _T("Grid Count"), cnt);
	
	CString strVal;	
	strVal.Format(L"%f", size);
	theApp.WriteProfileStringW(section, _T("Grid Size"), strVal);
}


void CDlgManualScan::OnBnClickedDirMark()
{
	double dPower = m_editPower.GetValue();
	m_pScan->SetPower(dPower);
	m_pen.m_dblFrequency = m_editFrequency.GetValue();
	m_pen.m_dblPulseWidth = m_editPulseWidth.GetValue();
	m_pen.m_dblMarkSpeed = m_editScanSpeed.GetValue();
	m_pen.m_dblJumpSpeed = m_editScanSpeed.GetValue();

	m_pScan->SetLaserPen(&m_pen);
	m_pScan->SetLaserPenParam();
	m_pScan->WaitMoving(TRUE);

	double size = m_editGridSize.GetValue();
	int    cnt = m_editGridCnt.GetValue();
	double spos = -size / 2.0;
	double tpos = size / 2.0;
	double gap = size / (cnt - 1);

	m_pScan->CircularQueueMode(FALSE);
	m_pScan->StartList();
	m_pScan->JumpList(0, 0);
	m_pScan->MarkList(0, tpos);
	m_pScan->JumpList(-2, (tpos + 4));
	m_pScan->MarkList(0, (tpos + 4) - 2);
	m_pScan->JumpList(2, (tpos + 4));
	m_pScan->MarkList(-2, (tpos + 4) - 4);

	m_pScan->JumpList(0, 0);
	m_pScan->MarkList(tpos, 0);
	m_pScan->JumpList((tpos + 4) - 2, -2);
	m_pScan->MarkList((tpos + 4) + 2, 2);
	m_pScan->JumpList((tpos + 4) - 2, 2);
	m_pScan->MarkList((tpos + 4) + 2, -2);
	m_pScan->EndList();

	m_pScan->Execute();
	m_pScan->WaitMoving(TRUE);
	m_pScan->SetPower(0);
	m_pScan->Move(0, 0);

	CString section = L"Manual Scan";
	theApp.WriteProfileInt(section, _T("Grid Count"), cnt);

	CString strVal;
	strVal.Format(L"%f", size);
	theApp.WriteProfileStringW(section, _T("Grid Size"), strVal);
}


void CDlgManualScan::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);

	if (bShow) {
		if (m_pScan) {
			//m_pPenOld = m_pScan->GetLaserPen();
			m_pScan->SetLaserPen(&m_pen);
			m_pScan->SetLaserPenParam();
			//m_editRatioPwr.SetValue(m_pScan->GetPowerRatio(), L"%.3f");
			//m_editRatioSpd.SetValue(m_pScan->GetSpeedRatio(), L"%.3f");
		}			
	}
	else {
		
	}
}