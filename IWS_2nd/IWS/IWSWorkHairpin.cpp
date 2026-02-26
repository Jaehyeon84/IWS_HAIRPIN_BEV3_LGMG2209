#include "stdafx.h"
#include "IWSWorkHairpin.h"
#include "EntyEllipse.h"
#include "EntyLine.h"
#include "EntyPolyline.h"
#include "IWSDoc.h"
#include "IWS.h"
#include "VRSaux/GrabImgRunner.h"
//#include "VRSaux/GrabImgDlg.h"
#include "CDlgWaitWork.h"
#include <queue>
#include "VRSaux/NIVision/include/nivision.h"

DECLARE_USER_MESSAGE(UWM_CIWSWORK)

CIWSWorkHairpin::CIWSWorkHairpin()
{
	m_bRunVScan = FALSE;
	m_iWorkMode = WORK_MODE_NONE;
	m_nModel = 0;
	m_pFitLine = NULL;
	m_pFitCircle = NULL;

	CIWSWork::CIWSWork();
}

CIWSWorkHairpin::~CIWSWorkHairpin()
{
	if (m_pFitCircle)	delete[] m_pFitCircle;
	if (m_pFitLine) delete[] m_pFitLine;

	CIWSWork::~CIWSWork();
}

void CIWSWorkHairpin::clear()
{
	CIWSWork::clear();
}

void CIWSWorkHairpin::OffsetToCurrent()
{
	CEntity *Ent;
	for (auto group : m_JobGroup)
	{
		for (auto p : group->m_jobs)
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD)
				continue;

			Ent = p->GetEntity();
			Ent->ChangedEntity(Ent->m_hCadEntity);
		}
	}
}

void CIWSWorkHairpin::SaveWork(CFile * pf, CIWSDoc * pDoc)
{
	CStringA strName = STRINGIFY(CIWSWorkHairpin);
	CStringA strVer;
	strVer.Format("_%d", WORK_VERC);
	strName = strName + strVer;
	int nStr = strName.GetLength();

	pf->Write(&nStr, sizeof(int));
	pf->Write(&m_nModel, sizeof(int));
	pf->Write(strName.GetBuffer(), sizeof(char)*nStr);
	pf->Write(&m_hp, sizeof(IWS_HAIRPIN_INFO));
	pf->Write(&m_zComp, sizeof(IWS_HAIRPIN_Z_COMP));
	pf->Write(&m_nDefLayer, sizeof(int));

	CIWSWork::SaveWork(pf, pDoc);
}

BOOL CIWSWorkHairpin::LoadWork(CFile * pf, CIWSDoc * pDoc)
{
	int nStr;
	char tmp[100] = "";
	pf->Read(&nStr, sizeof(int));
	if (nStr > 100)
	{
		CIWSWork::LoadWork(pf, pDoc);
		_tracer(TRACE_ERROR, 1, L"Work file load error ( Not Hairpin work )");
		return FALSE;
	}

	pf->Read(&m_nModel, sizeof(int));
	pf->Read(&tmp, sizeof(char)*nStr);
	CStringA strVer = CStringA(tmp);

	int ipos = strVer.Find("_", 0);

	int iFileVer;
	if (ipos > 0)
	{
		strVer = strVer.Mid(ipos + 1, strVer.GetLength() - ipos);
		iFileVer = atoi(strVer);
	}
	else
	{
		iFileVer = 0;
	}

	// mspark 2022.04.19 
	// WORK_VER103이하버전 더이상 지원 하지 않음.
	//if (iFileVer < WORK_VER103)
	if (iFileVer < WORK_VER103)
	{
		_tracer(TRACE_ERROR, 1, L"Work file version is too old : v%d", iFileVer);
		AfxMessageBox(L"Work file version is too old");
		return FALSE;
	}
	else if(iFileVer == WORK_VER103)
	{
		ReadHairpinInfoV103(pf);
		ReadHairpinZCompV103(pf);
		//pf->Read(&m_zComp, sizeof(IWS_HAIRPIN_Z_COMP));
	}
	else if (iFileVer == WORK_VER104)
	{
		ReadHairpinInfoV104(pf);
		pf->Read(&m_zComp, sizeof(IWS_HAIRPIN_Z_COMP));
	}
	else
	{
		pf->Read(&m_hp, sizeof(IWS_HAIRPIN_INFO));
		pf->Read(&m_zComp, sizeof(IWS_HAIRPIN_Z_COMP));
	}

	//pf->Read(&m_zComp, sizeof(IWS_HAIRPIN_Z_COMP));
	pf->Read(&m_nDefLayer, sizeof(int));

	CStringA str(tmp);
	int ret = str.Compare(STRINGIFY(CIWSWorkHairpin));
	if (ret)
	{
		if (pDoc->m_config.m_bUseHairpin)
			theApp.m_iwsMode = CIWSApp::IWSMODE_HAIRPIN;
		else
			theApp.m_iwsMode = CIWSApp::IWSMODE_NORMAL;

		CIWSWork::LoadWork(pf, pDoc);
		_tracer(TRACE_ERROR, 1, L"Work file load as Normal works");

		return TRUE;
	}

	BOOL bret = CIWSWork::LoadWork(pf, pDoc);
	if (pDoc->m_config.m_bUseHairpin)
		theApp.m_iwsMode = CIWSApp::IWSMODE_HAIRPIN;
	else
		theApp.m_iwsMode = CIWSApp::IWSMODE_NORMAL;

	return bret;
}

void CIWSWorkHairpin::ReadHairpinInfoV104(CFile * pf)
{
	IWS_HAIRPIN_INFO_V104   hp;
	pf->Read(&hp, sizeof(IWS_HAIRPIN_INFO_V104));

	m_hp.nSlot = hp.nSlot;
	m_hp.nDrawSlot = hp.nDrawSlot;
	m_hp.nLayer = hp.nLayer;
	m_hp.startAng = hp.startAng;
	m_hp.HpSize = hp.HpSize;
	m_hp.LayerDia[0] = hp.LayerDia[0];
	m_hp.LayerDia[1] = hp.LayerDia[1];
	m_hp.LayerDia[2] = hp.LayerDia[2];
	m_hp.LayerDia[3] = hp.LayerDia[3];
	m_hp.LayerDia[4] = hp.LayerDia[4];
	m_hp.LayerDia[5] = hp.LayerDia[5];
	m_hp.LayerDia[6] = hp.LayerDia[6];
	m_hp.LayerDia[7] = hp.LayerDia[7];
	m_hp.LayerDia[8] = hp.LayerDia[8];
	m_hp.LayerDia[9] = hp.LayerDia[9];
	m_hp.HpOverLen = hp.HpOverLen;

	m_hp.nCntP1 = hp.nCntP1;
	m_hp.nPenP1 = hp.nPenP1;
	m_hp.nWeldShapeP1 = hp.nWeldShapeP1;
	m_hp.nWeldZigZagFreqP1 = hp.nWeldZigZagFreqP1;

	m_hp.nCntP2 = hp.nCntP2;
	m_hp.nPenP2 = hp.nPenP2;
	m_hp.nWeldShapeP2 = hp.nWeldShapeP2;
	m_hp.nWeldZigZagFreqP2 = hp.nWeldZigZagFreqP2;

	m_hp.HpSize2nd = hp.HpSize2nd;
	m_hp.HpSizeW = hp.HpSizeW;

	m_hp.nSwitchWelding = hp.nSwitchWelding;
	m_hp.nWeldType = hp.nWeldType;

	m_hp.dblPowerRatio = hp.dblPowerRatio;
	m_hp.dblSpeedRatio = hp.dblSpeedRatio;

	copy(hp.hpOffset, hp.hpOffset + 512, m_hp.hpOffset);

	memcpy(m_hp.m_dPenPower, hp.m_dPenPower, sizeof(double) * 10);
	memcpy(m_hp.m_dPenSpeed, hp.m_dPenSpeed, sizeof(double) * 10);
	m_hp.bUseRptPowerProfile_1st = hp.bUseRptPowerProfile_1st;
	m_hp.bUseRptPowerProfile_2nd = hp.bUseRptPowerProfile_2nd;
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++)
	{
		m_hp.dRptPowerProfile_1st[i] = hp.dRptPowerProfile_1st[i];
		m_hp.dRptPowerProfile_2nd[i] = hp.dRptPowerProfile_2nd[i];
	}

	// sjyi 2024.03.30 헤어핀 회전 추가
	m_hp.dPtnRotationAngle = 0.0;
	m_hp.dPtnRotationAngle2 = 0.0;

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
	m_hp.bReversal = FALSE;

}

void CIWSWorkHairpin::ReadHairpinInfoV103(CFile * pf)
{
	IWS_HAIRPIN_INFO_V103   hp;
	pf->Read(&hp, sizeof(IWS_HAIRPIN_INFO_V103));

	m_hp.nSlot = hp.nSlot;
	m_hp.nDrawSlot = hp.nDrawSlot;
	m_hp.nLayer = hp.nLayer;
	m_hp.startAng = hp.startAng;
	m_hp.HpSize = hp.HpSize;
	m_hp.LayerDia[0] = hp.LayerDia[0];
	m_hp.LayerDia[1] = hp.LayerDia[1];
	m_hp.LayerDia[2] = hp.LayerDia[2];
	m_hp.LayerDia[3] = hp.LayerDia[3];
	m_hp.LayerDia[4] = hp.LayerDia[4];
	m_hp.LayerDia[5] = hp.LayerDia[5];
	m_hp.LayerDia[6] = hp.LayerDia[6];
	m_hp.LayerDia[7] = hp.LayerDia[7];
	m_hp.LayerDia[8] = hp.LayerDia[8];
	m_hp.LayerDia[9] = hp.LayerDia[9];
	m_hp.HpOverLen = hp.HpOverLen;

	m_hp.nCntP1 = hp.nCntP1;
	m_hp.nPenP1 = hp.nPenP1;
	m_hp.nWeldShapeP1 = hp.nWeldShapeP1;
	m_hp.nWeldZigZagFreqP1 = hp.nWeldZigZagFreqP1;

	m_hp.nCntP2 = hp.nCntP2;
	m_hp.nPenP2 = hp.nPenP2;
	m_hp.nWeldShapeP2 = hp.nWeldShapeP2;
	m_hp.nWeldZigZagFreqP2 = hp.nWeldZigZagFreqP2;

	m_hp.HpSize2nd = hp.HpSize2nd;
	m_hp.HpSizeW = hp.HpSizeW;

	m_hp.nSwitchWelding = hp.nSwitchWelding;
	m_hp.nWeldType = hp.nWeldType;

	m_hp.dblPowerRatio = hp.dblPowerRatio;
	m_hp.dblSpeedRatio = hp.dblSpeedRatio;

	copy(hp.hpOffset, hp.hpOffset + 512, m_hp.hpOffset);

	memcpy(m_hp.m_dPenPower, hp.m_dPenPower, sizeof(double) * 10);
	memcpy(m_hp.m_dPenSpeed, hp.m_dPenSpeed, sizeof(double) * 10);
	m_hp.bUseRptPowerProfile_1st = FALSE;
	m_hp.bUseRptPowerProfile_2nd = FALSE;
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++) 
	{
		m_hp.dRptPowerProfile_1st[i] = 1.0;
		m_hp.dRptPowerProfile_2nd[i] = 1.0;
	}

	// sjyi 2024.03.30 헤어핀 회전 추가
	m_hp.dPtnRotationAngle = 0.0;
	m_hp.dPtnRotationAngle2 = 0.0;

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
	m_hp.bReversal = FALSE;

}

// mspark 2022.04.19
void CIWSWorkHairpin::ReadHairpinZCompV103(CFile* pf)
{
	IWS_HAIRPIN_Z_COMP_V103 zComp;
	pf->Read(&zComp, sizeof(IWS_HAIRPIN_Z_COMP_V103));

	m_zComp.bUse = zComp.bUse;
	m_zComp.dblThreshold = zComp.dblThreshold;
	m_zComp.nCnt = zComp.nCnt;
	m_zComp.dblPower = 1.0;
	for (int i = 0; i < 10; i++) {
		m_zComp.zHpArr[i].dblMax = zComp.zHpArr[i].dblMax;
		m_zComp.zHpArr[i].dblMin = zComp.zHpArr[i].dblMin;
		m_zComp.zHpArr[i].nCnt = zComp.zHpArr[i].nCnt;
		m_zComp.zHpArr[i].dblPower = 1.0;
	}
}




//nPattern : 1 = 1차 패턴, 2 = 2차 패턴
//nShape = Ellipse = 0, Line = 1, Zigzag = 2
double CIWSWorkHairpin::GetWeldingEnergy(int nPattern, int nShape, double nWidth, double nLength, int nFreq, int Repeat, double dPower, double dSpeed)
{
//#define LASER_MAX_POWER 5000.0
	double dEnergy = 0.0f; // kWh
	CLaserPen *pPen = &(m_pDoc->m_pens)[m_hp.nPenP1];
	//double dPower = pPen->m_dblPower*m_pDoc->m_config.m_nLaserMax;
	double dTmpPower = dPower * m_pDoc->m_config.m_nLaserMax/100.0;
	//double dSpeed = pPen->m_dblMarkSpeed;	
		
	//1차 패턴
	if (nPattern == PATTERN_TYPE_1ST)
	{
		switch (nShape)
		{
		case 0:
		{
			// 타원의 둘레 
			double a = nLength*0.5;
			double b = nWidth *0.5;
			double P = LC_PI * sqrt(2.0 * (a *a + b * b));
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
		}
		break;
		case 1:
		{
			// line
			double P = nLength;
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
		}
		break;
		case 2:
		{
			//Zigzag
			int nx = nFreq * 2 + 1;
			double dx = nLength / double(nx);
			double dy = nWidth * 0.5;
			double dxdy = sqrt(dx*dx + dy*dy);
			double P = dxdy * double(nx);
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
		}
		break;
		default:
			break;
		}
	}
	//2차 패턴
	else if (nPattern == PATTERN_TYPE_2ND)
	{
		CJob* p0 = (GetJob(0));
		CJob* p1 = (GetJob(1));
		// sjyi 2022-05-18 Add 작업 클래스 포인터 널인경우, 예외처리, -->
		if (p0 == NULL || p1 == NULL)
		{
			_tracer(TRACE_ERROR, 1, L"There is no jobs");
			return 0;
		}
		// sjyi 2022-05-18 Add <--
		POINTF points[2];
		points[0].x = FLOAT(p0->GetEntity()->m_cen.x + p0->GetEntity()->m_offset.x);
		points[0].y = FLOAT(p0->GetEntity()->m_cen.y + p0->GetEntity()->m_offset.y);
		points[1].x = FLOAT(p1->GetEntity()->m_cen.x + p1->GetEntity()->m_offset.x);
		points[1].y = FLOAT(p1->GetEntity()->m_cen.y + p1->GetEntity()->m_offset.y);
				
		switch (nShape)
		{
		case 0:
		{
			double dx = points[0].x - points[1].x;
			double dy = points[0].y - points[1].y;
			// 타원의 둘레 
			double a = sqrt(dx*dx + dy*dy);
			a = a*0.5 + nLength;
			double b = nWidth*0.5;
			double P = LC_PI * sqrt(2.0 * (a *a + b * b));
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
		}
		break;
		case 1:
		{
			// line
			double dx = points[0].x - points[1].x;
			double dy = points[0].y - points[1].y;
			// 타원의 둘레 
			double P = sqrt(dx*dx + dy * dy);
			P = P + nLength *2;
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
		}
		break;
		case 2:
		{
			// sjyi 2022-05-23 Zigzag 살리고, Zigzag2 신규 추가에 따라, 기존 Zigzag 함수내용 다시 살림 --->
			// sjyi 2022-05-19 패턴 변경에 따른, 용접 에너지 관련 계산식 수정 -->
			//Zigzag
			double dx = points[0].x - points[1].x;
			double dy = points[0].y - points[1].y;
			double l = sqrt(dx*dx + dy * dy);
			l = l + nLength * 2;
			int nx = nFreq * 2 + 1;
			dx =  l / double(nx);
			dy = nWidth * 0.5;
			double dxdy = sqrt(dx*dx + dy * dy);
			double P = dxdy * double(nx);
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;

			//int nx = nFreq * 2 + 1;
			//double dx = nLength / double(nx);
			//double dy = nWidth * 0.5;
			//double dxdy = sqrt(dx*dx + dy * dy);
			//double P = dxdy * double(nx);
			//dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
			// sjyi 2022-05-19 패턴 변경에 따른, 용접 에너지 관련 계산식 수정 <--
			// sjyi 2022-05-23 Zigzag 살리고, Zigzag2 신규 추가에 따라, 기존 Zigzag 함수내용 다시 살림 <---
		}
		break;
		// sjyi 2022-05-23 Zigzag2 신규 추가 -->
		case 3: // Zigzag2
		{
			int nx = nFreq * 2 + 1;
			double dx = nLength / double(nx);
			double dy = nWidth * 0.5;
			double dxdy = sqrt(dx*dx + dy * dy);
			double P = dxdy * double(nx);
			dEnergy = dTmpPower * P / dSpeed * 3600.0 / 1000.0;
		}
		break;
		// sjyi 2022-05-23 Zigzag2 신규 추가 <--
		default:
			break;
		}
	}


	return dEnergy * Repeat;
}

int CIWSWorkHairpin::DoWork()
{
	return CIWSWork::DoWork();
}

void CIWSWorkHairpin::DoVScan()
{	
	if (m_bRunVScan)
		return;

	_tracer(TRACE_NORMAL, 9999, L"DoVScan, GetDoEmission = [%d]", m_pScanner->GetDOEmission());
	//if (!m_pScanner->GetDO(m_pScanner->GetDOEmission()))
	if (!m_pScanner->GetLaserOn())
	{		
		switch (m_pDoc->m_config.m_iLaser)
		{
		case TRUMPF_TRUDISK:
			m_pScanner->LaserPowerON(TRUE);
			//m_pScanner->LaserEnable(!m_pScanner->GetDOEmission(), 0);
			m_pScanner->LaserEnable(TRUE, 0);
			break;
		case COHERENT_ROFIN:
			m_pScanner->SetPower(0);
			m_pScanner->LaserOn(FALSE);
			break;
		}

		m_pScanner->SetGuideOn(FALSE);
	}

	/*if (m_pDoc->m_work.m_bSimulation)
	{
		CScanner *pScan = m_pDoc->m_pScanner;
		pScan->LaserPowerON(TRUE);
		pScan->LaserEnable(!pScan->INT_BE_EmissionEnable(), 0);
		m_pDoc->m_work.m_bSimulation = FALSE;
	}*/

	m_pDoc->m_pScanner->SetLaserPen(&(m_pDoc->m_pens[14]));
	m_pDoc->m_pScanner->SetLaserPenParam();// add mspark 2020.06.04	
	if (!m_pDlgWait)
	{
		m_pDlgWait = new CDlgWaitWork;
		m_pDlgWait->Create(IDD_DIALOG_WAIT_WORK);
	}
	m_iWorkMode = WORK_MODE_VSCAN;
	AfxBeginThread(StartVScanThread, this);
}

BOOL CIWSWorkHairpin::DoInspection()
{
	if (m_bRunVScan)
		return FALSE;

	if (!m_pDlgWait)
	{
		m_pDlgWait = new CDlgWaitWork;
		m_pDlgWait->Create(IDD_DIALOG_WAIT_WORK);
	}
	
	switch (m_pDoc->m_config.m_iLaser)
	{
	case TRUMPF_TRUDISK: 
		if (m_pScanner->GetDO(m_pScanner->GetDOEmission()))
		{
			m_pScanner->LaserPowerON(TRUE);
			m_pScanner->LaserEnable(!m_pScanner->GetDOEmission(), 0);
		}
		break;
	case COHERENT_ROFIN:
		if (m_pScanner->GetLaserOn())
		{
			m_pScanner->SetPower(0);
			m_pScanner->LaserOn(FALSE);
			m_pScanner->SetGuideOn(FALSE);
		}
		break;
	}	

	Sleep(500);

	m_iWorkMode = WORK_MODE_INSPECTION;
	AfxBeginThread(StartVScanThread, this);
	return TRUE;
}

UINT CIWSWorkHairpin::StartVScanThread(LPVOID pParam)
{
	CIWSWorkHairpin *pMe = (CIWSWorkHairpin*)pParam;

	pMe->m_bRunVScan = TRUE;
	switch (pMe->m_iWorkMode)
	{
	case WORK_MODE_VSCAN:
		pMe->StartVScanThreadBody();
		break;
	case WORK_MODE_INSPECTION:
		pMe->StartInspectionBody();
		break;
	}

	return 0;
}

UINT CIWSWorkHairpin::StartVScanThreadBody()
{	
	m_pDlgWait->AssignStopFlag(NULL);
	m_pDlgWait->SetMessage(L"Image Scanning....");
	m_pDlgWait->CenterWindow();
	m_pDlgWait->ShowWindow(SW_SHOW);

	g_pGrabImgRunner->SetFlagStitchImageDone(FALSE);

	CString strStitchImg = m_pDoc->m_strImgPath + STR_REFIMG_FILE;
	g_pGrabImgRunner->ScanAndSave(strStitchImg, TRUE, &m_nRefImgPixW, &m_nRefImgPixH, &m_fRefImgSizeW, &m_fRefImgSizeH);

	while (m_bRunVScan)
	{
		if (g_pGrabImgRunner->IsDoneStitchImage())
			break;

		Sleep(10);
	}

	if (m_hRcv)
		::PostMessage(m_hRcv, UWM_CIWSWORK, EVENT_WORK_VSCAN, 0);

	m_bRunVScan = FALSE;
	m_pDlgWait->ShowWindow(SW_HIDE);

	return 0;
}

UINT CIWSWorkHairpin::StartInspectionBody()
{
	_tracer(TRACE_NORMAL, 1, L"Start Inspection");
	m_dwStart = timeGetTime();
	m_pDlgWait->AssignStopFlag(NULL);
	m_pDlgWait->SetMessage(L"Pin Inspecting....");
	m_pDlgWait->CenterWindow();
	m_pDlgWait->ShowWindow(SW_SHOW);

	CLaserPen *pOldPen = m_pScanner->GetLaserPen();
	m_pScanner->SetLaserPen(&(m_pDoc->m_pens[14]));
	m_pScanner->SetLaserPenParam();
	m_pScanner->WaitMoving(TRUE);

	//IWS_VECTOR_JOB *pJobs = GetJobs();
	g_pGrabImgRunner->SetFlagStitchImageDone(FALSE);

	CString strStitchImg = m_pDoc->m_strImgPath + STR_REFIMG_FILE;
	g_pGrabImgRunner->ScanAndSave(strStitchImg, TRUE, &m_nRefImgPixW, &m_nRefImgPixH, &m_fRefImgSizeW, &m_fRefImgSizeH);

	while (m_bRunVScan)
	{
		if (g_pGrabImgRunner->IsDoneStitchImage())
			break;

		Sleep(10);
	}

	if (m_hRcv)
		::PostMessage(m_hRcv, UWM_CIWSWORK, EVENT_WORK_INSPECT, 0);

	m_bRunVScan = FALSE;
	m_pDlgWait->ShowWindow(SW_HIDE);
	m_dwEnd = timeGetTime();
	m_dwTactTime = m_dwEnd - m_dwStart;
	_tracer(TRACE_NORMAL, 1, L"End Inspection");
	return 0;
}

void CIWSWorkHairpin::ModifyHairpin(int st1, int nd2, BOOL bUpdate, double dRotateAngle, double dRotateAngle2, BOOL bKeepRotation, BOOL bReversal)
{
	// 0 = All, 1~4 = Layer

	//int st1 = 0;
	//int nd2 = 0;

	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	//HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);
	lcBlockUnselect(hBlock);
	lcEventsEnable(TRUE);

	/*IWS_VECTOR_JOB *pJobs = GetJobs();*/
	IWS_VECTOR_JOB_GROUP *pJobGroup = GetJobs();
	int idx = 0;
	int nJob = GetNumberOfJobs();
	double hp_cx = 0, hp_cy = 0;
	GetHairpinCenter(hp_cx, hp_cy);
	// int layer = (GetIndex() % 4) + 1;

	// sjyi 2023.04.13 패턴 회전 각도
	double dRotateAngleRAD = dRotateAngle * LC_DEG_TO_RAD; // 참고사항 "LC_DEG_TO_RAD" 값과 "1 / (180 * M_PI)" 값은 같음.
	double dRotateAngleRAD2 = dRotateAngle2 * LC_DEG_TO_RAD; // 참고사항 "LC_DEG_TO_RAD" 값과 "1 / (180 * M_PI)" 값은 같음.

	// sjyi 2024.03.01 Reverse 패턴 회전 각도 추가
	double dRevRotateAngle = dRotateAngle + 180; // Reverse Rotation Angle(Reverse 활성화시, 짝수열에 적용)
	if (dRevRotateAngle >= 360) dRevRotateAngle -= 360;
	double dRevRotationAngleRAD = dRevRotateAngle * LC_DEG_TO_RAD;

	// sjyi 2022-05-18 comment points 객체에, 각 Job의 중심점 입력
	POINTF *points = new POINTF[nJob];
	for (auto group : (*pJobGroup))
	{
		for (int i = 0; i < group->m_jobs.size(); i++)
		{
			if (group->m_jobs[i]->GetJobType() != JobType::JOB_TYPE_WELD)
				continue;

			points[idx].x = FLOAT(group->m_jobs[i]->GetEntity()->m_cen.x + group->m_jobs[i]->GetEntity()->m_offset.x);
			points[idx].y = FLOAT(group->m_jobs[i]->GetEntity()->m_cen.y + group->m_jobs[i]->GetEntity()->m_offset.y);
			idx++;
		}
	}

	ASSERT(idx == nJob);
	double tmpx, tmpy;
	double theta;
	double r1, r2, cx, cy;
	//double ol = m_hp.HpOverLen;
	idx = 0;
	CEntity *entity;
	CJobWeld *jobWeld;

	g_bEventEnable = FALSE;
	HANDLE handle = NULL;
	CString strEntName;
	int *pPenID;
	int nLayer = 0;

	// sjyi 2022-05-18 Comment
	// 현재, 해어핀 패턴에서 특정 Pair 레이어만 패턴을 바꾸고 싶을 때, 본 함수 사용.
	// st1는 1st 패턴, nd2는 2nd 패턴으로
	// 값이 0일 때는 모두 변경,
	// 1~4까지는 1~4 pair 레이어(즉, 1-2, 3-4, 5-6, 7-8 레이어)만 변경
	// 5일 경우에는 모두 변경 안함.
	for (auto group : (*pJobGroup))
	{
		for (int i = 0; i < group->m_jobs.size(); i++)
		{
			nLayer = (group->m_nIndex % (m_nDefLayer / 2)) + 1;
			if (i < 2 && st1 != 0)		// 1st pattern
			{
				if (nLayer != st1 || st1 == 5)
				{
					idx++;
					continue;
				}

			}
			// sjyi 2022-05-18 Modify 2nd 지그재그 패턴 변경으로, 그룹내 작업개수 4개까지 확장-->
//			else if (i == 2 && nd2 != 0)// 2nd pattern
			else if (i >= 2 && nd2 != 0)// 2nd pattern
			// sjyi 2022-05-18 Modify <--
			{
				if (nLayer != nd2 || nd2 == 5)
				{
					idx++;
					continue;
				}
			}

			if (group->m_jobs[i]->GetJobType() != JobType::JOB_TYPE_WELD)
				continue;

			jobWeld = ((CJobWeld*)(group->m_jobs[i]));
			entity = jobWeld->m_entity;

			strEntName = entity->m_strName;
			//pPenID = entity->m_pColorID;

			lcEntErase(entity->m_hCadEntity, TRUE);
			lcBlockPurge(hBlock);
			lcBlockUpdate(hBlock, TRUE, 0);
			delete jobWeld->m_entity;


			// sjyi 2022-05-18 Max_Num_Group의 수 변경
//			if (!((idx + 1) % MAX_NUM_GRP)) // 2nd pattern
			// sjyi 2022-05-19 사용 안함 int nMaxGrpNum = group->m_jobs.size();	// 3 or 4(zigzag)
			if (i >= 2) // 2nd pattern
			{
				if (bUpdate)
				{
					jobWeld->m_iPenID = m_hp.nPenP2;
					jobWeld->m_d2NdWidth = m_hp.HpSize2nd;
					jobWeld->m_d2NdOver = m_hp.HpOverLen;
				}

				double ol = jobWeld->m_d2NdOver;

				switch (m_hp.nWeldShapeP2)
				{
				case 0:	// Ellipse
				{
					entity = new CEntyEllipse;
					cx = double(points[idx - 2].x + points[idx - 1].x) * 0.5;
					cy = double(points[idx - 2].y + points[idx - 1].y) * 0.5;

					tmpx = double(points[idx - 1].x - points[idx - 2].x);
					tmpy = double(points[idx - 1].y - points[idx - 2].y);
					theta = atan2(tmpy, tmpx);
					r1 = _hypot(tmpx, tmpy) / 2;
					//r2 = m_hp.HpSize2nd * 0.5;
					r2 = jobWeld->m_d2NdWidth * 0.5;
					handle = lcBlockAddEllipse(hBlock, cx, cy, r1 + ol, r2, theta, 0, 0);
				} break;
				case 1: // Line
				{
					double nx, ny, mag, HpSize;
					entity = new CEntyLine;
					cx = (points[idx - 2].x + points[idx - 1].x) * 0.5;
					cy = (points[idx - 2].y + points[idx - 1].y) * 0.5;
					mag = _hypot(cx, cy);
					tmpx = (points[idx - 1].x - points[idx - 2].x);
					tmpy = (points[idx - 1].y - points[idx - 2].y);
					HpSize = _hypot(tmpx, tmpy) / 2;

					nx = cx / mag;
					ny = cy / mag;
					handle = lcBlockAddLine(hBlock
						, (cx + nx * (HpSize + ol))	// X1
						, (cy + ny * (HpSize + ol))	// Y1
						, (cx - nx * (HpSize + ol))	// X2
						, (cy - ny * (HpSize + ol)));	// Y2
				} break;
				case 2: // Zigzag
				{
					entity = new CEntyPolyline;
					double HpSize;
					
					double HpSizeW = jobWeld->m_d2NdWidth;
					cx = (points[idx - 2].x + points[idx - 1].x) * 0.5;
					cy = (points[idx - 2].y + points[idx - 1].y) * 0.5;
					tmpx = (points[idx - 1].x - points[idx - 2].x);
					tmpy = (points[idx - 1].y - points[idx - 2].y);
					theta = atan2(tmpy, tmpx);
					HpSize = _hypot(tmpx, tmpy);
					handle = GenZigZagPatrn(hBlock, cx, cy, theta, HpSize + ol * 2.0, HpSizeW, m_hp.nWeldZigZagFreqP2);
				} break;
				// sjyi 2022-05-23 Zigzag2 신규 추가 -->
				case 3: // Zigzag2
				{
					entity = new CEntyPolyline;
					double HpSize;

					HpSize = jobWeld->m_d2NdOver;
					double HpSizeW = jobWeld->m_d2NdWidth;
					cx = points[idx].x - hp_cx;
					cy = points[idx].y - hp_cy;
					theta = atan2(cy, cx) + M_PI / 2;
					handle = GenZigZagPatrn(hBlock, cx + hp_cx, cy + hp_cy, 0/*theta*/, HpSize, HpSizeW, m_hp.nWeldZigZagFreqP2);

					// sjyi 2023.04.13 ZigZag2 2차 패턴 회전, 1차 패턴과 동일하게 회전
					if (bKeepRotation == TRUE) // 기존 헤어핀 회전 정보 그대로 유지
					{
						dRotateAngleRAD2 = jobWeld->GetRotation() * LC_DEG_TO_RAD;
						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD2);
					}
					else // 새로운 각도로 회전
					{
						dRotateAngleRAD2 = dRotateAngle2 * LC_DEG_TO_RAD;
						jobWeld->SetRotation(dRotateAngle2);
						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD2);
					}
				} break;
				// sjyi 2022-05-23 Zigzag2 신규 추가 <--
				}
			}
			else // 1st pattern
			{
				if (bUpdate)
				{
					jobWeld->m_iPenID = m_hp.nPenP1;
					jobWeld->m_d1StWidth = m_hp.HpSizeW;
					jobWeld->m_d1StLength = m_hp.HpSize;
				}

				switch (m_hp.nWeldShapeP1)
				{
				case 0: // Ellipse
				{
					entity = new CEntyEllipse;
					cx = points[idx].x - hp_cx;
					cy = points[idx].y - hp_cy;
					/*r1 = m_hp.HpSize * 0.5;
					r2 = m_hp.HpSizeW * 0.5;*/
					r1 = jobWeld->m_d1StLength * 0.5;
					r2 = jobWeld->m_d1StWidth * 0.5;
					theta = atan2(cy, cx) + M_PI / 2;
					handle = lcBlockAddEllipse(hBlock, cx + hp_cx, cy + hp_cy, r1, r2, /*theta*/0, 0, 0);

					// sjyi 2023.04.13 1차 패턴 회전
					if (bKeepRotation == TRUE) // 기존 헤어핀 회전 정보 그대로 유지
					{
						dRotateAngleRAD = jobWeld->GetRotation() * LC_DEG_TO_RAD;
						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
					else // 새로운 정보로 회전
					{
						dRotateAngleRAD = dRotateAngle * LC_DEG_TO_RAD;
						jobWeld->SetRotation(dRotateAngle);

						// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
						if (bReversal) {
							if (i % 2) {// 외경만 회전
								jobWeld->SetRotation(dRevRotateAngle);
								dRotateAngleRAD = dRevRotationAngleRAD;
							}
						}

						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}

				} break;
				case 1: // Line
				{
					double nx, ny, mag, HpSize;
					//HpSize = m_hp.HpSize*0.5;
					HpSize = jobWeld->m_d1StLength * 0.5;
					entity = new CEntyLine;
					cx = points[idx].x - hp_cx;
					cy = points[idx].y - hp_cy;
					mag = _hypot(cx, cy);
					nx = -cy / mag;
					ny = cx / mag;
					handle = lcBlockAddLine(hBlock
						//, (cx + nx * HpSize) + hp_cx
						//, (cy + ny * HpSize) + hp_cy
						//, (cx - nx * HpSize) + hp_cx
						//, (cy - ny * HpSize) + hp_cy);
						, (cx + /*nx * */HpSize) + hp_cx
						, (cy /*+ nx * HpSize*/) + hp_cy
						, (cx - /*nx * */HpSize) + hp_cx
						, (cy /*- nx * HpSize*/) + hp_cy);

					// sjyi 2023.04.13 1차 패턴 회전
					if (bKeepRotation == TRUE) // 기존 헤어핀 회전 정보 그대로 유지
					{
						dRotateAngleRAD = jobWeld->GetRotation() * LC_DEG_TO_RAD;
						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
					else // 새로운 정보로 회전
					{
						dRotateAngleRAD = dRotateAngle * LC_DEG_TO_RAD;
						jobWeld->SetRotation(dRotateAngle);

						// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
						if (bReversal) {
							if (i % 2) {// 외경만 회전
								jobWeld->SetRotation(dRevRotateAngle);
								dRotateAngleRAD = dRevRotationAngleRAD;
							}
						}

						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
				} break;
				case 2: // Zigzag
				{
					entity = new CEntyPolyline;
					/*double HpSize = m_hp.HpSize;
					double HpSizeW = m_hp.HpSizeW;*/
					double HpSize = jobWeld->m_d1StLength;
					double HpSizeW = jobWeld->m_d1StWidth;
					cx = points[idx].x - hp_cx;
					cy = points[idx].y - hp_cy;
					theta = atan2(cy, cx) + M_PI / 2;
					handle = GenZigZagPatrn(hBlock, cx + hp_cx, cy + hp_cy, 0/*theta*/, HpSize, HpSizeW, m_hp.nWeldZigZagFreqP1);

					// sjyi 2023.04.13 1차 패턴 회전
					if (bKeepRotation == TRUE) // 기존 헤어핀 회전 정보 그대로 유지
					{
						dRotateAngleRAD = jobWeld->GetRotation() * LC_DEG_TO_RAD;

						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
					else // 새로운 정보로 회전
					{
						dRotateAngleRAD = dRotateAngle * LC_DEG_TO_RAD;
						jobWeld->SetRotation(dRotateAngle);

						// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
						if (bReversal) {
							if (i % 2) {// 외경만 회전
								jobWeld->SetRotation(dRevRotateAngle);
								dRotateAngleRAD = dRevRotationAngleRAD;
							}
						}

						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
				} break;
				case 3: // Octagon
				{
					entity = new CEntyPolyline;
					double HpSize = jobWeld->m_d1StLength;
					double HpSizeW = jobWeld->m_d1StWidth;
					cx = points[idx].x - hp_cx;
					cy = points[idx].y - hp_cy;
					theta = atan2(cy, cx) + M_PI / 2;
					handle = GenPolygon(hBlock, cx + hp_cx, cy + hp_cy, HpSize, HpSizeW, theta);

					// sjyi 2023.04.13 1차 패턴 회전
					if (bKeepRotation == TRUE) // 기존 헤어핀 회전 정보 그대로 유지
					{
						dRotateAngleRAD = jobWeld->GetRotation() * LC_DEG_TO_RAD;
						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
					else // 새로운 정보로 회전
					{
						dRotateAngleRAD = dRotateAngle * LC_DEG_TO_RAD;
						jobWeld->SetRotation(dRotateAngle);

						// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 사용여부 추가
						if (bReversal) {
							if (i % 2) {// 외경만 회전
								jobWeld->SetRotation(dRevRotateAngle);
								dRotateAngleRAD = dRevRotationAngleRAD;
							}
						}

						lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
					}
				} break;
				}
			}

			entity->SetDrawEntity(handle);
			pPenID = &jobWeld->m_iPenID;
			jobWeld->SetOriPen();
			entity->SetPen(pPenID);
			entity->m_strName = strEntName;
			((CJobWeld*)(group->m_jobs[i]))->m_entity = entity;
			entity->UpdateEntity(NULL, COleVariant::COleVariant(0.0));

			idx++;
		}
	}
	lcBlockPurge(hBlock);
	lcBlockUpdate(hBlock, TRUE, 0);
	lcWndRedraw(g_hLcWnd);
	delete[] points;
	g_bEventEnable = TRUE;
}

void CIWSWorkHairpin::AlignHairpin(HANDLE handle)
{
	//_tracer(TRACE_NORMAL, 1, );

	//IWS_VECTOR_JOB *pJobs = GetJobs();
	IWS_VECTOR_JOB_GROUP *pJobGroup = GetJobs();
	int idx = 0;
	int nJob = GetNumberOfJobs();

	POINTF *points = new POINTF[nJob];

	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs)
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD)
				continue;

			points[idx].x = FLOAT(p->GetEntity()->m_cen.x + p->GetEntity()->m_offset.x);
			points[idx].y = FLOAT(p->GetEntity()->m_cen.y + p->GetEntity()->m_offset.y);
			idx++;
		}
	}


	ASSERT(idx == nJob);
	idx = 0;
	int nMaxNumGrp = 0; // sjyi 2022-05-19 Add Job그룹 내 MAX 작업개수 확인용.
	for (auto group : (*pJobGroup))
	{
		for (auto p : group->m_jobs)
		{
			if (p->GetJobType() != JobType::JOB_TYPE_WELD) continue;
			// sjyi 2022-05-19 2nd Pattern Check -->
//			if (!((idx + 1) % MAX_NUM_GRP)) 
			nMaxNumGrp =  group->m_jobs.size();
			if (nMaxNumGrp == 3) // sjyi 2nd 패턴이 zigzag2가 아님 기존과 동일
			{
				if (!((idx + 1) % nMaxNumGrp))
				{
					// 2차 패턴
					CEntity *Ent = p->GetEntity();
					switch (Ent->GetEntityType())
					{
					case LC_ENT_ELLIPSE: AlignEntityEllipse2nd(Ent, points, idx, &((CJobWeld*)p)->m_iPenID, ((CJobWeld*)p)->m_d2NdOver); break;
					case LC_ENT_LINE: AlignEntityLine2nd(Ent, points, idx, &((CJobWeld*)p)->m_iPenID, ((CJobWeld*)p)->m_d2NdOver); break;
					case LC_ENT_POLYLINE: AlignEntityZigZag2nd(Ent, points, idx, &((CJobWeld*)p)->m_iPenID, ((CJobWeld*)p)->m_d2NdWidth, ((CJobWeld*)p)->m_d2NdOver); break;
					}

				}
				else
				{ // 1차패턴
					CEntity *Ent = p->GetEntity();
					switch (Ent->GetEntityType())
					{
					case LC_ENT_ELLIPSE: AlignEntityEllipse1st(Ent, &((CJobWeld*)p)->m_iPenID); break;
					case LC_ENT_LINE: AlignEntityLine1st(Ent, &((CJobWeld*)p)->m_iPenID); break;
					case LC_ENT_POLYLINE: AlignEntityZigZag1st(Ent, &((CJobWeld*)p)->m_iPenID); break;
					}

					// sjyi 2023.04.13 헤어핀 회전
					if (Ent->GetEntityType() != LC_ENT_POLYLINE) // ZigZag는 별도 회전
					{
						RotateHairpin(Ent, ((CJobWeld*)p)->GetRotation());
					}
				}
			}
			else  // sjyi 2nd 패턴이 ZigZag2
			{
				int nSubIndexCheck = (idx) % nMaxNumGrp; 
				// sjyi 2022-08-19 ZigZag2에 대한 Align 작업 관련 내용 수정 -->
				if (nSubIndexCheck >= 2 ) // 2nd
				{
					// 2차패턴
					CEntity *Ent1st = group->m_jobs[nSubIndexCheck - 2]->GetEntity();
					CEntity *Ent = p->GetEntity();
					AlignEntityZigZag2_2nd(Ent1st, Ent, &((CJobWeld*)p)->m_iPenID);
				}
				else
				{ // 1차패턴
					CEntity *Ent = p->GetEntity();
					switch (Ent->GetEntityType())
					{
					case LC_ENT_ELLIPSE: AlignEntityEllipse1st(Ent, &((CJobWeld*)p)->m_iPenID); break;
					case LC_ENT_LINE: AlignEntityLine1st(Ent, &((CJobWeld*)p)->m_iPenID); break;
					case LC_ENT_POLYLINE: AlignEntityZigZag1st(Ent, &((CJobWeld*)p)->m_iPenID); break;
					}

					// sjyi 2023.04.13 헤어핀 회전
					if (Ent->GetEntityType() != LC_ENT_POLYLINE) // ZigZag는 별도 회전
					{
						RotateHairpin(Ent, ((CJobWeld*)p)->GetRotation());
					}
				}
				// sjyi 2022-08-19 ZigZag2에 대한 Align 작업 관련 내용 수정 <--
			}
			idx++;
		}
	}

	lcWndRedraw(g_hLcWnd);

	delete[] points;
}

void CIWSWorkHairpin::AlignEntityEllipse2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double dOverLen)
{
	double tmpx, tmpy;
	double radius;
	double theta;
	double r1, r2, cx, cy;
	double hp_cx, hp_cy;
	/*double ol = m_hp.HpOverLen;*/
	double ol = dOverLen;
	GetHairpinCenter(hp_cx, hp_cy);

	CEntyEllipse *e = (CEntyEllipse*)pEnt;
	e->SetPen(penID);	// hslee
	tmpx = (points[idx - 2].x + points[idx - 1].x) * 0.5 - hp_cx;
	tmpy = (points[idx - 2].y + points[idx - 1].y) * 0.5 - hp_cy;
	radius = _hypot(tmpx, tmpy);
	cx = tmpx;// radius * cos(theta);
	cy = tmpy;// radius * sin(theta);

	tmpx = (points[idx - 1].x - points[idx - 2].x);
	tmpy = (points[idx - 1].y - points[idx - 2].y);
	theta = atan2(tmpy, tmpx);
	r1 = sqrt(tmpx*tmpx + tmpy * tmpy);
	r2 = m_hp.HpSize2nd * 0.5;
	//cx = tmpx;// radius * cos(theta);
	//cy = tmpy;// radius * sin(theta);
	e->Rmajor = r1 * 0.5 + ol;
	e->Rminor = r2;
	e->RotAngle = theta * LC_RAD_TO_DEG;
	e->center.x = cx + hp_cx;
	e->center.y = cy + hp_cy;
	e->m_cen.x = cx + hp_cx;
	e->m_cen.y = cy + hp_cy;
	e->m_offset.x = 0;
	e->m_offset.y = 0;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CIWSWorkHairpin::AlignEntityLine2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double dOverLen)
{
	double tmpx, tmpy;
	double r1, cx, cy;
	double nx, ny;
	double mag;
	double hp_cx, hp_cy;
	/*double ol = m_hp.HpOverLen;*/
	double ol = dOverLen;

	GetHairpinCenter(hp_cx, hp_cy);
	CEntyLine *e = (CEntyLine*)pEnt;
	e->SetPen(penID);	// hslee
	cx = (points[idx - 2].x + points[idx - 1].x) * 0.5;
	cy = (points[idx - 2].y + points[idx - 1].y) * 0.5;
	tmpx = (points[idx - 1].x - points[idx - 2].x);
	tmpy = (points[idx - 1].y - points[idx - 2].y);
	mag = _hypot(tmpx, tmpy);
	nx = tmpx / mag;
	ny = tmpy / mag;
	r1 = (mag + ol * 2.0) / 2.0;
	e->length = r1 * 2.0;
	e->point0.x = cx + nx * r1;
	e->point0.y = cy + ny * r1;
	e->point1.x = cx - nx * r1;
	e->point1.y = cy - ny * r1;
	e->m_cen.x = cx;
	e->m_cen.y = cy;
	// 	e->m_offset.x = 0;
	// 	e->m_offset.y = 0;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CIWSWorkHairpin::AlignEntityEllipse1st(CEntity *pEnt, int *penID)
{
	CEntyEllipse *e = (CEntyEllipse*)pEnt;
	double hp_cx, hp_cy;
	GetHairpinCenter(hp_cx, hp_cy);
	e->SetPen(penID);	// hslee
	// imsi sjyi e->RotAngle = atan2(e->center.y - hp_cy, e->center.x - hp_cx) * LC_RAD_TO_DEG + 90.0f;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CIWSWorkHairpin::AlignEntityLine1st(CEntity *pEnt, int *penID)
{
	CEntyLine *e = (CEntyLine*)pEnt;
	double hp_cx, hp_cy;
	GetHairpinCenter(hp_cx, hp_cy);
	e->SetPen(penID);
	double x = e->m_cen.x - e->m_offset.x - hp_cx;
	double y = e->m_cen.y - e->m_offset.y - hp_cy;
	double mag = sqrt(x*x + y * y);
	double nx = -y / mag;
	double ny = x / mag;
	Point3Dbl d = e->point0 - e->point1;
	double len = _hypot(d.x, d.y);
	e->point0.x = x + nx * len*0.5 + e->m_offset.x + hp_cx;
	e->point0.y = y + ny * len*0.5 + e->m_offset.y + hp_cy;
	e->point1.x = x - nx * len*0.5 + e->m_offset.x + hp_cx;
	e->point1.y = y - ny * len*0.5 + e->m_offset.y + hp_cy;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CIWSWorkHairpin::AlignEntityZigZag2nd(CEntity *pEnt, POINTF* points, int idx, int *penID, double d2ndWidth, double dOverLen)
{
	g_bEventEnable = FALSE;
	CEntyPolyline *e = (CEntyPolyline*)pEnt;
	e->SetPen(penID);

	double x = (points[idx - 1].x - points[idx - 2].x);
	double y = (points[idx - 1].y - points[idx - 2].y);

	double cx = (points[idx - 2].x + points[idx - 1].x) * 0.5;
	double cy = (points[idx - 2].y + points[idx - 1].y) * 0.5;

	double x0 = (double)points[idx - 1].x;
	double y0 = (double)points[idx - 1].y;
	double x1 = (double)points[idx - 2].x;
	double y1 = (double)points[idx - 2].y;

	/*double ol = m_hp.HpOverLen;
	double wid = m_hp.HpSize2nd;*/

	double wid = d2ndWidth;
	double ol = dOverLen;

	double theta = atan2(y, x);
	double len = _hypot(x, y) + ol * 2.0;
	int n = m_hp.nWeldZigZagFreqP2;
	int nx = n * 2 + 1;
	double dx = len / double(nx);
	double dy = wid * 0.5;

	x = 0;
	y = 0;
	HANDLE hPline = e->m_hCadEntity;
	lcPlineDeleteVer(hPline, NULL);
	for (int i = 0; i <= nx; i++) {
		y = (i % 2) ? dy : -dy;
		lcPlineAddVer(hPline, 0, x, y);
		x += dx;
	}

	lcPlineEnd(hPline);
	lcEntMove(hPline, -len * 0.5, 0);
	// imsi sjyi	lcEntRotate(hPline, 0, 0, theta);
	lcEntMove(hPline, cx, cy);
	lcEntUpdate(hPline);

	g_bEventEnable = TRUE;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CIWSWorkHairpin::AlignEntityZigZag1st(CEntity *pEnt, int *penID, double dRotateAngle)
{
	g_bEventEnable = FALSE;
	CEntyPolyline *e = (CEntyPolyline*)pEnt;
	double hp_cx, hp_cy;
	GetHairpinCenter(hp_cx, hp_cy);

	e->SetPen(penID);
	double x = e->m_cen.x - e->m_offset.x - hp_cx;
	double y = e->m_cen.y - e->m_offset.y - hp_cy;
	double theta = atan2(y, x);

	size_t nSize = e->m_points.size();
	Point3Dbl pt;
	pt.x = e->m_points[nSize - 2].x - e->m_points[0].x;
	pt.y = e->m_points[nSize - 2].y - e->m_points[0].y;

	double mag = _hypot(pt.x, pt.y);
	double nnx = pt.x / mag;
	double nny = pt.y / mag;
	double mag1 = _hypot(x, y);
	double nx = -y / mag1;
	double ny = x / mag1;
	double angCur = atan2(nny, nnx) * LC_RAD_TO_DEG; // 패턴의 각도
	// sjyi 2023.04.14 패턴 회전 관련 수정.
	angCur -= dRotateAngle; // 패턴의 각도 - Rotation 각도(원래 위치에서 Rotation 했으므로 돌린 정도만큼 빼야 현재 각도 나옴.
	angCur = Trunc360(angCur);

	double angTarg = atan2(ny, nx) * LC_RAD_TO_DEG; // 중심위치로 부터의 포인트에 대한 각도
	theta = angTarg - angCur;
	theta = Trunc360(theta);

	TRACE("ang current (%f), target(%f), del = %f\n", angCur, angTarg, angTarg - angCur);
	theta = angTarg - angCur;

	if (fabs(theta) < 1)
	{
		g_bEventEnable = TRUE;
		return;
	}

	// imsi sjyi	lcEntRotate(e->m_hCadEntity, x + e->m_offset.x + hp_cx, y + e->m_offset.y + hp_cy, theta*LC_DEG_TO_RAD);
	lcEntUpdate(e->m_hCadEntity);
	e->ChangedEntity(e->m_hCadEntity);

	//lcEntUpdate(e->m_hCadEntity);

	g_bEventEnable = TRUE;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

void CIWSWorkHairpin::GetHairpinCenter(double &cx, double &cy)
{
	switch (m_nModel)
	{
	case 0:
		cx = m_pDoc->m_config.m_cp.dModel1_X;
		cy = m_pDoc->m_config.m_cp.dModel1_Y;
		break;
	case 1:
		cx = m_pDoc->m_config.m_cp.dModel2_X;
		cy = m_pDoc->m_config.m_cp.dModel2_Y;
		break;
	case 2:
		cx = m_pDoc->m_config.m_cp.dModel3_X;
		cy = m_pDoc->m_config.m_cp.dModel3_Y;
		break;
	case 3:
		cx = m_pDoc->m_config.m_cp.dModel4_X;
		cy = m_pDoc->m_config.m_cp.dModel4_Y;
		break;
	}
}

void CIWSWorkHairpin::CalcHairpinInfo()
{
	//int nLayers = 4;//m_hp.nLayer;
	//int nSlots = 24;//m_hp.nSlot;	
	int nLayers = m_hp.nLayer;
	int nSlots = m_hp.nSlot;
	int nSizePin = nLayers * nSlots;
	if (nSizePin <= 0) return;
	//Point3Dbl *pHairpin = new Point3Dbl[nSizePin];
	Point3Dbl *pHairpin = new Point3Dbl[nSizePin];
	CEntity *Ent;
	int idx = 0;
	for (auto group : m_JobGroup)
	{
		Ent = ((CJobWeld*)group->m_jobs[0])->GetEntity();
		pHairpin[idx].x = Ent->m_cen.x + Ent->m_offset.y;
		pHairpin[idx].y = Ent->m_cen.y + Ent->m_offset.y;
		idx++;

		Ent = ((CJobWeld*)group->m_jobs[1])->GetEntity();
		pHairpin[idx].x = Ent->m_cen.x + Ent->m_offset.y;
		pHairpin[idx].y = Ent->m_cen.y + Ent->m_offset.y;
		idx++;
	}

	FitCircleOptions options;
	options.maxIterations = 1000;
	options.minScore = 800;
	options.pixelRadius = 50;
	options.rejectOutliers = 10;
	PointFloat *points = new PointFloat[nSlots];  // Layer 蹂꾨줈 CircleFitting???ъ슜??Sample Points
	if (m_pFitCircle) delete[] m_pFitCircle;
	m_pFitCircle = new CircleEquation[nLayers];
	for (int i = 0; i < nLayers; i++) {
		for (int j = 0; j < nSlots; j++) {
			points[j].x = pHairpin[i + j * nLayers].x;
			points[j].y = pHairpin[i + j * nLayers].y;
		}

		BestCircle2 *fitRes = imaqFitCircle2(points, nSlots, &options);
		int cx = (int)fitRes->center.x;
		int cy = (int)fitRes->center.y;
		int irad = (int)fitRes->radius;
		//imaqOverlayOval(m_pImgAW, imaqMakeRect(cy - irad, cx - irad, irad * 2, irad * 2), &rgb, IMAQ_DRAW_VALUE, (char*)FITTING_MARK);
		m_pFitCircle[i].cx = fitRes->center.x;
		m_pFitCircle[i].cy = fitRes->center.y;
		m_pFitCircle[i].r = fitRes->radius;
		TRACE("layer%d, cx=%f, cy=%f, r=%f\n", i, m_pFitCircle[i].cx, m_pFitCircle[i].cy, m_pFitCircle[i].r);
		imaqDispose(fitRes);
	}
	if (points) {
		delete[] points;
		points = NULL;
	}

	// Line Fitting
	FitLineOptions options_l;
	options_l.minScore = 800;
	options_l.numRefinements = 1;
	options_l.pixelRadius = 100;
	points = new PointFloat[nLayers];
	int ivalid;
	if (m_pFitLine) delete[] m_pFitLine;
	m_pFitLine = new CIWSWorkHairpin::LineEquation[nSlots];
	//float x, y;
	for (int i = 0; i < nSlots; i++) {

		ivalid = 0;
		for (int j = 0; j < nLayers; j++) {
			points[j].x = pHairpin[j + i * nLayers].x;
			points[j].y = pHairpin[j + i * nLayers].y;
		}
		BestLine *fitRes = imaqFitLine(points, nLayers, &options_l);
		m_pFitLine[i].a = fitRes->equation.a;
		m_pFitLine[i].b = fitRes->equation.b;
		m_pFitLine[i].c = fitRes->equation.c;
		m_pFitLine[i].start_x = double(fitRes->start.x);
		m_pFitLine[i].start_y = double(fitRes->start.y);
		m_pFitLine[i].end_x = double(fitRes->end.x);
		m_pFitLine[i].end_y = double(fitRes->end.y);

		// 		Point st = PointFloat2Point;
		// 		Point ed = PointFloat2Point(fitRes->end);
		imaqDispose(fitRes);

		//imaqOverlayLine(m_pImgAW, st, ed, &rgb, FITTING_MARK);
	}
	if (points) {
		delete[] points;
		points = NULL;
	}


	delete[] pHairpin;
	UpdateHairpinInfoLayer();
}

void CIWSWorkHairpin::NewCalcHairpinInfo()
{
	if (m_pDoc->m_config.m_bUseCircleFit != TRUE)
		return;

	int nLayers = m_hp.nLayer;
	int nSlots = m_pDoc->m_work.m_hp.nDrawSlot;
	int nSizePin = nLayers * nSlots;
	if (nSizePin <= 0) return;
	//Point3Dbl *pHairpin = new Point3Dbl[nSizePin];
	Point3Dbl *pHairpin = new Point3Dbl[nSizePin];
	CEntity *Ent;
	int idx = 0;
	for (auto group : m_JobGroup)
	{
		Ent = ((CJobWeld*)group->m_jobs[0])->GetEntity();
		pHairpin[idx].x = Ent->m_cen.x + Ent->m_offset.y;
		pHairpin[idx].y = Ent->m_cen.y + Ent->m_offset.y;
		idx++;

		Ent = ((CJobWeld*)group->m_jobs[1])->GetEntity();
		pHairpin[idx].x = Ent->m_cen.x + Ent->m_offset.y;
		pHairpin[idx].y = Ent->m_cen.y + Ent->m_offset.y;
		idx++;
	}

	FitCircleOptions options;
	options.maxIterations = 1000;
	options.minScore = 800;
	options.pixelRadius = 50;
	options.rejectOutliers = 10;
	PointFloat *points = new PointFloat[nSlots];  // Layer 蹂꾨줈 CircleFitting???ъ슜??Sample Points
	if (m_pFitCircle) delete[] m_pFitCircle;
	m_pFitCircle = new CircleEquation[nLayers];
	for (int i = 0; i < nLayers; i++) {
		for (int j = 0; j < nSlots; j++) {
			points[j].x = pHairpin[i + j * nLayers].x;
			points[j].y = pHairpin[i + j * nLayers].y;
		}




		BestCircle2 *fitRes = imaqFitCircle2(points, nSlots, &options);
		int cx = (int)fitRes->center.x;
		int cy = (int)fitRes->center.y;
		int irad = (int)fitRes->radius;
		//imaqOverlayOval(m_pImgAW, imaqMakeRect(cy - irad, cx - irad, irad * 2, irad * 2), &rgb, IMAQ_DRAW_VALUE, (char*)FITTING_MARK);
		m_pFitCircle[i].cx = fitRes->center.x;
		m_pFitCircle[i].cy = fitRes->center.y;
		m_pFitCircle[i].r = fitRes->radius;
		TRACE("layer%d, cx=%f, cy=%f, r=%f\n", i, m_pFitCircle[i].cx, m_pFitCircle[i].cy, m_pFitCircle[i].r);
		imaqDispose(fitRes);
	}
	if (points) {
		delete[] points;
		points = NULL;
	}

	// Line Fitting
	FitLineOptions options_l;
	options_l.minScore = 800;
	options_l.numRefinements = 1;
	options_l.pixelRadius = 100;
	points = new PointFloat[nLayers];
	int ivalid;
	if (m_pFitLine) delete[] m_pFitLine;
	m_pFitLine = new CIWSWorkHairpin::LineEquation[nSlots];
	//float x, y;
	for (int i = 0; i < nSlots; i++) {

		ivalid = 0;
		for (int j = 0; j < nLayers; j++) {
			points[j].x = pHairpin[j + i * nLayers].x;
			points[j].y = pHairpin[j + i * nLayers].y;
		}
		BestLine *fitRes = imaqFitLine(points, nLayers, &options_l);
		m_pFitLine[i].a = fitRes->equation.a;
		m_pFitLine[i].b = fitRes->equation.b;
		m_pFitLine[i].c = fitRes->equation.c;
		m_pFitLine[i].start_x = double(fitRes->start.x);
		m_pFitLine[i].start_y = double(fitRes->start.y);
		m_pFitLine[i].end_x = double(fitRes->end.x);
		m_pFitLine[i].end_y = double(fitRes->end.y);

		// 		Point st = PointFloat2Point;
		// 		Point ed = PointFloat2Point(fitRes->end);
		imaqDispose(fitRes);

		//imaqOverlayLine(m_pImgAW, st, ed, &rgb, FITTING_MARK);
	}
	if (points) {
		delete[] points;
		points = NULL;
	}


	delete[] pHairpin;
	UpdateHairpinInfoLayer();
}

void CIWSWorkHairpin::UpdateHairpinInfoLayer()
{
	if (!m_pFitCircle) return;
	if (!m_pFitLine) return;
	int nLayers = m_hp.nLayer;
	int nSlots = m_hp.nSlot;

	HANDLE hLayer = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_INFO);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	if (!hLayer)
		hLayer = lcDrwAddLayer(g_hLcDrw, STR_LAYER_INFO, L"100,100,100", 0, LC_LWEIGHT_DEFAULT);
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLayer);  // set active layer
	lcPropPutBool(g_hLcDrw, LC_PROP_LAYER_LOCKED, FALSE);
	lcEventsEnable(FALSE);
	{
		for (auto ent : m_entInfo) {
			lcEntErase(ent, true);
		}
		lcBlockPurge(hBlock);

		m_entInfo.clear();

		// layer display
		for (int i = 0; i < nLayers; i++) {
			HANDLE handle;
			handle = lcBlockAddCircle(hBlock, m_pFitCircle[i].cx, m_pFitCircle[i].cy, m_pFitCircle[i].r, FALSE);
			m_entInfo.push_back(handle);
			lcPropPutInt(handle, LC_PROP_ENT_COLOR, COLOR_GREY40);
		}
		// slot display
		for (int i = 0; i < nSlots; i++) {
			HANDLE handle;
			handle = lcBlockAddLine(hBlock, m_pFitLine[i].start_x, m_pFitLine[i].start_y,
				m_pFitLine[i].end_x, m_pFitLine[i].end_y);
			m_entInfo.push_back(handle);
			lcPropPutInt(handle, LC_PROP_ENT_COLOR, COLOR_GREY40);

		}
	}

	lcPropPutBool(hLayer, LC_PROP_LAYER_LOCKED, TRUE);
	HANDLE hLayer0 = lcDrwGetObjectByName(g_hLcDrw, LC_OBJ_LAYER, STR_LAYER_0);
	lcPropPutHandle(g_hLcDrw, LC_PROP_DRW_LAYER, hLayer0);  // set active layer

	lcEventsEnable(TRUE);

}

BOOL CIWSWorkHairpin::GetSelectedJob()
{
	for (auto group : m_JobGroup)
		for (auto job : group->m_jobs)
			if (job->m_bSelected)
				return TRUE;

	return FALSE;
}

void CIWSWorkHairpin::ModifyHairpinWeldRepeat(int nP1, int nP2, int st1, int nd2)
{
	IWS_VECTOR_JOB_GROUP *pGroup = GetJobs();
	int idx = 0;

	int nLayer = 0;
	CJobWeld* pJob = NULL;	// sjyi 2022-06-03 추가
	for (auto group : (*pGroup))
	{
		for (int i = 0; i < group->m_jobs.size(); i++)
		{
			nLayer = (group->m_nIndex % (m_nDefLayer / 2)) + 1;
			if (i < 2 && st1 != 0)		// 1st pattern
			{
				if (nLayer != st1 || st1 == 5)
					continue;
			}
//			else if (i == 2 && nd2 != 0)// 2nd pattern
			else if (i >= 2 && nd2 != 0)// 2nd pattern
			{
				if (nLayer != nd2 || nd2 == 5)
					continue;
			}

			// sjyi 2022-06-03 일부 오류 및 개별 파워 조정 입력 관련 수정--->	
			pJob = (CJobWeld*)group->m_jobs[i];
			if (i >= 2)	// 2nd Pattern
			{
				// ((CJobWeld*)group->m_jobs[i])->m_iRepeat = nP2;
				pJob->m_iRepeat = nP2;		// 리피트 회수 저장

				// 개별 파워 사용 여부 클리어
				pJob->SetUseIndividualPower(FALSE);

				// 개별파워 클리어
				pJob->ClearIndividualPower();

				// 리피트 회수 만큼 개별 파워 저장
				for (int i = 0; i < pJob->m_iRepeat; i++)
				{
					if (m_hp.bUseRptPowerProfile_2nd)
					{
						pJob->AddIndividualPower(m_hp.dRptPowerProfile_2nd[i]);
					}
					else
					{
						pJob->AddIndividualPower(1.00);
					}
				}
			}
			else // 1st Pattern
			{
				// ((CJobWeld*)group->m_jobs[i])->m_iRepeat = nP1;
				pJob->m_iRepeat = nP1;	// 리피트 회수 저장

				// 개별 파워 사용 여부 클리어
				pJob->SetUseIndividualPower(FALSE);

				// 개별파워 수정
				pJob->ClearIndividualPower();

				// 리피트 회수 만큼 개별 파워 저장
				for (int i = 0; i < pJob->m_iRepeat; i++)
				{
					if (m_hp.bUseRptPowerProfile_1st)
					{
						pJob->AddIndividualPower(m_hp.dRptPowerProfile_1st[i]);
					}
					else
					{
						pJob->AddIndividualPower(1.00);
					}
				}
			}
			// sjyi 2022-06-03 일부 오류 및 개별 파워 조정 입력 관련 수정<---
		}

		//((CJobWeld*)group->m_jobs[0])->m_iRepeat = nP1;
		//((CJobWeld*)group->m_jobs[1])->m_iRepeat = nP1;
		//((CJobWeld*)group->m_jobs[2])->m_iRepeat = nP2;
	}
}

void CIWSWorkHairpin::GenHairPin(int nSlot, int nLayer, double startAng,
	double HpSize, double HpSizeW, double HpSize2nd, double *pLayerDia,
	int nCntP1, int nCntP2, int nPenP1, int nPenP2, int nModel, int nDrawSlot, double dOverLen,
	double dRotateAngle, BOOL bReversal)
{
	double cx, cy;
	double delAng = 360.0 / double(nSlot);
	double radius;
	double theta;
	double nx, ny;// : direction vector
	double mag;
	double ol = m_hp.HpOverLen;
	m_pDoc->OnNewDocument();

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	double r1, r2;
	//lcEventsEnable(FALSE);
	g_bEventEnable = FALSE;
	CJobWeld *inner = NULL;
	CJobWeld *outer = NULL;
	BOOL isInner = true;
	// sjyi 2022-05-17 Add 지그재그에 대한 2nd 패턴 변경에 따른 변수 추가 -->
	CJobWeld *inner2nd = NULL;
	CJobWeld *outer2nd = NULL;
	BOOL isInner2nd = true;
	// sjyi 2022-05-17 Add <--

	// sjyi 2023.04.13 패턴 회전 각도
	double dRotateAngleRAD = dRotateAngle * LC_DEG_TO_RAD; // 참고사항 "LC_DEG_TO_RAD" 값과 "1 / (180 * M_PI)" 값은 같음.

	// sjyi 2024.02.28 패턴 회전 각도(반전 각도)
	double dRevRotateAng = dRotateAngle + 180;
	if (dRevRotateAng >= 360) dRevRotateAng -= 360;
	double dRevRotateAngleRAD = dRevRotateAng * LC_DEG_TO_RAD;

	int layer = 0;
	int nIndex = 0;

	m_nModel = nModel;
	double hp_cx, hp_cy;
	GetHairpinCenter(hp_cx, hp_cy);
	for (int i = 0; i < nSlot; i++)
	{
		if (i >= nDrawSlot)
			continue;

		theta = (i * delAng + startAng) / 180 * M_PI;
		for (int j = 0; j < nLayer; j++)
		{
			lcEventsEnable(FALSE);

			// 1st Pattern
			CJobWeld *pJob = new CJobWeld;

			CEntity *entity = NULL;
			switch (m_hp.nWeldShapeP1)
			{
			case 0:
				entity = new CEntyEllipse;
				break;
			case 1:
				entity = new CEntyLine;
				break;
			case 2:
				entity = new CEntyPolyline;
				break;
			case 3:
				entity = new CEntyPolyline;
				break;
			default:
				entity = new CEntyEllipse;
			}

			pJob->m_entity = entity;
			pJob->m_pScan = m_pScanner;
			pJob->m_pPens = &m_pDoc->m_pens;
			pJob->m_iRepeat = nCntP1;
			pJob->m_iPenID = nPenP1;

			pJob->m_d1StLength = HpSize;
			pJob->m_d1StWidth = HpSizeW;

			radius = pLayerDia[j] * 0.5;
			r1 = HpSize * 0.5;
			r2 = HpSizeW * 0.5;
			cx = radius * cos(theta);
			cy = radius * sin(theta);
			HANDLE handle = NULL;
			switch (m_hp.nWeldShapeP1)
			{
			case 0:
				handle = lcBlockAddEllipse(hBlock, cx + hp_cx, cy + hp_cy, r1, r2, theta + M_PI / 2, 0, 0);
				break;
			case 1:
				mag = _hypot(cx, cy);
				nx = -cy / mag;
				ny = cx / mag;
				handle = lcBlockAddLine(hBlock
					//, (cx + nx * r1 + hp_cx)	// x1
					//, (cy + ny * r1 + hp_cy)	// y1
					//, (cx - nx * r1 + hp_cx)	// x2
					//, (cy - ny * r1 + hp_cy));	// y2
					, (hp_cx + cx + r1)	// x1
					, (hp_cy + cy)		// y1
					, (hp_cx + cx - r1)	// x2
					, (hp_cy + cy));	// y2
					break;
			case 2:
				handle = GenZigZagPatrn(hBlock
					, cx + hp_cx
					, cy + hp_cy
					, /*theta + M_PI / 2*/ 0, HpSize, HpSizeW, m_hp.nWeldZigZagFreqP1);
				break;
			case 3:
				handle = GenPolygon(hBlock, cx + hp_cx, cy + hp_cy, HpSize, HpSizeW, theta + M_PI / 2);
				break;
			default:
				handle = lcBlockAddEllipse(hBlock, cx, cy, r1, r2, theta + M_PI / 2, 0, 0);
			}

			// sjyi 2023.04.13 1차 패턴 회전
			pJob->SetRotation(dRotateAngle);

			// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전 기능 으로 내용 수정
						//lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
						// 반전 기능 사용 안할 경우, 회전 각도로 회전
						// 반전 기능 사용할 경우, 외경(짝수열)일 경우, 반전각도(회전각도 + 180)으로 회전
			if (bReversal && j % 2)
			{
				pJob->SetRotation(dRevRotateAng);
				lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRevRotateAngleRAD);
			}
			else
			{
				lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);
			}

			pJob->SetOriPen();
			entity->SetPen(&pJob->m_iPenID); // hslee
			entity->SetDrawEntity(handle);
			layer = (j / 2) + 1;
			entity->m_strName.Format(L"Slot: %d, Layer: %d - 1 PT", i + 1, layer);
			/*entity->m_strName.Format(L"Slot: %d, Layer: %d", i, j);*/

			TRACE("cx = %f,cy = %f\n", cx, cy);
			if (isInner)
			{
				inner = pJob;
				isInner = false;
			}
			else
			{
				outer = pJob;
				isInner = true;
			}

			// sjyi 2022-06-07 개별 파워 조정 입력 관련 수정 --->
			// 개별 파워 사용 여부 클리어
			pJob->SetUseIndividualPower(FALSE);

			// 개별파워 변수 초기화
			pJob->ClearIndividualPower();

			// 리피트 회수 만큼 개별 파워 저장
			for (int i = 0; i < pJob->m_iRepeat; i++)
			{
				pJob->AddIndividualPower(1.00);
			}
			// sjyi 2022-06-07 개별 파워 조정 입력 관련 수정<---

			// 2nd Pattern		
			// sjyi 2022-05-17 Add 지그재그에 대한 2nd 패턴 변경 -->
			// sjyi 2022-05-23 수정-->
			//if (m_hp.nWeldShapeP2 == 2)
			if (m_hp.nWeldShapeP2 == 3) // Zigzag2
			// sjyi 2022-05-23 수정<--
			{
				CJobWeld *pJob2 = new CJobWeld;
				CEntity *entity = NULL;
				entity = new CEntyPolyline;

				pJob2->m_entity = entity;
				pJob2->m_pScan = m_pScanner;
				pJob2->m_pPens = &m_pDoc->m_pens;
				pJob2->m_iRepeat = nCntP2;
				pJob2->m_iPenID = nPenP2;

				pJob2->m_d2NdWidth = HpSize2nd;
				pJob2->m_d2NdOver = dOverLen;

				radius = pLayerDia[j] * 0.5;
				cx = radius * cos(theta);
				cy = radius * sin(theta);
				HANDLE handle = NULL;

				handle = GenZigZagPatrn(hBlock
					, cx + hp_cx
					, cy + hp_cy
					, /*theta + M_PI / 2*/ 0, dOverLen, HpSize2nd, m_hp.nWeldZigZagFreqP2);

				// sjyi 2023.04.13 ZigZag2 2차 패턴 회전, 1차 패턴과 동일하게 회전
				pJob2->SetRotation(dRotateAngle);
				lcEntRotate(handle, cx + hp_cx, cy + hp_cy, dRotateAngleRAD);

				pJob2->SetOriPen();
				entity->SetPen(&pJob2->m_iPenID);  // hslee
				entity->SetDrawEntity(handle);

				entity->m_strName.Format(L"Slot: %d, Layer: %d - 2 PT", i + 1, layer);

				// sjyi 2022-06-07 개별 파워 조정 입력 관련 수정 --->
				// 개별 파워 사용 여부 클리어
				pJob2->SetUseIndividualPower(FALSE);

				// 개별파워 변수 초기화
				pJob2->ClearIndividualPower();

				// 리피트 회수 만큼 개별 파워 저장
				for (int i = 0; i < pJob2->m_iRepeat; i++)
				{
					pJob2->AddIndividualPower(1.00);
				}
				// sjyi 2022-06-07 개별 파워 조정 입력 관련 수정<---

				if (isInner2nd)
				{
					inner2nd = pJob2;
					isInner2nd = false;
				}
				else
				{
					outer2nd = pJob2;
					isInner2nd = true;
					AddJob(inner, outer, inner2nd, outer2nd, nIndex, i, layer);
					nIndex++;
				}
			}
			else
			{
			// sjyi 2022-05-17 Add 지그재그에 대한 2nd 패턴 변경 <--
				if (!((j + 1) % 2))
				{
					CJobWeld *pJob12 = new CJobWeld;
					//CEntyEllipse *entity = new CEntyEllipse;
					CEntity *entity = NULL;
					switch (m_hp.nWeldShapeP2)
					{
					case 0: entity = new CEntyEllipse; break;
					case 1: entity = new CEntyLine; break;
					case 2: entity = new CEntyPolyline; break;
					default: entity = new CEntyEllipse;
					}

					pJob12->m_entity = entity;
					pJob12->m_pScan = m_pScanner;
					pJob12->m_pPens = &m_pDoc->m_pens;
					pJob12->m_iRepeat = nCntP2;
					pJob12->m_iPenID = nPenP2;

					pJob12->m_d2NdWidth = HpSize2nd;
					pJob12->m_d2NdOver = dOverLen;

					radius = (pLayerDia[j - 1] + pLayerDia[j]) * 0.25; 
					r1 = fabs((pLayerDia[j] + pLayerDia[j - 1])*0.25);
					r2 = HpSize2nd * 0.5;
					cx = radius * cos(theta);
					cy = radius * sin(theta);
					HANDLE handle = NULL;
					switch (m_hp.nWeldShapeP2)
					{
					case 0:	// Ellipse
						handle = lcBlockAddEllipse(hBlock, cx + hp_cx, cy + hp_cy, r1 + dOverLen, r2, theta, 0, 0);
						break;
					case 1: // line
						mag = sqrt(cx*cx + cy * cy);
						nx = cx / mag;
						ny = cy / mag;
						handle = lcBlockAddLine(hBlock
							, cx + nx * (r1 + ol) + hp_cx
							, cy + ny * (r1 + ol) + hp_cy
							, cx - nx * (r1 + ol) + hp_cx
							, cy - ny * (r1 + ol) + hp_cy);
						break;
					case 2: // Zigzag
						handle = GenZigZagPatrn(hBlock
							, cx + hp_cx
							, cy + hp_cy
							, theta, (r1 + ol) * 2, HpSize2nd, m_hp.nWeldZigZagFreqP2);
						break;
					default: handle = lcBlockAddEllipse(hBlock, cx, cy, r1, r2, theta, 0, 0);
					}
					pJob12->SetOriPen();
					entity->SetPen(&pJob12->m_iPenID);  // hslee
					entity->SetDrawEntity(handle);

					entity->m_strName.Format(L"Slot: %d, Layer: %d - 2 PT", i + 1, layer);
					//entity->m_strName.Format(L"Slot: %d, Layer: %d-%d", i, j-1, j);

					// sjyi 2022-06-07 개별 파워 조정 입력 관련 수정 --->
					// 개별 파워 사용 여부 클리어
					pJob12->SetUseIndividualPower(FALSE);

					// 개별파워 변수 초기화
					pJob12->ClearIndividualPower();

					// 리피트 회수 만큼 개별 파워 저장
					for (int i = 0; i < pJob12->m_iRepeat; i++)
					{
						pJob12->AddIndividualPower(1.00);
					}
					// sjyi 2022-06-07 개별 파워 조정 입력 관련 수정<---

					AddJob(inner, outer, pJob12, nIndex, i, layer);
					nIndex++;
				}
			}
		}
	}

	//lcEventsEnable(TRUE);
	g_bEventEnable = TRUE;

	//m_work.SetNumSlotLayer(nSlot, nLayer);
	lcBlockUpdate(hBlock, TRUE, 0);
	lcWndRedraw(g_hLcWnd);
	lcWndExeCommand(g_hLcWnd, LC_CMD_ZOOM_EXT, 0);
}

HANDLE CIWSWorkHairpin::GenZigZagPatrn(HANDLE hBlock, double cx, double cy, double theta, double l, double w, int n)
{
	int nx = n * 2 + 1;
	double dx = l / double(nx);
	double dy = w * 0.5;
	HANDLE hPline = lcBlockAddPolyline(hBlock, 0, FALSE, FALSE);

	double x = 0;
	double y = 0;
	for (int i = 0; i <= nx; i++)
	{
		y = (i % 2) ? dy : -dy;
		lcPlineAddVer(hPline, 0, x, y);
		x += dx;
	}

	lcPlineEnd(hPline);
	lcEntMove(hPline, -l * 0.5, 0);
	lcEntRotate(hPline, 0, 0, theta);
	lcEntMove(hPline, cx, cy);
	lcEntUpdate(hPline);
	//lcBlockUpdate(hBlock, FALSE, hPline);
	return hPline;
}


void CIWSWorkHairpin::AlignEntityZigZag2_2nd(CEntity* pEnt1st, CEntity *pEnt, int *penID, double dRotateAngle)
{
	g_bEventEnable = FALSE;
	CEntyPolyline *e = (CEntyPolyline*)pEnt;
	CEntyPolyline *e1st = (CEntyPolyline*)pEnt1st;
	double hp_cx, hp_cy;

	// Get Center Position
	GetHairpinCenter(hp_cx, hp_cy);

	// Set Pen
	e->SetPen(penID);

	// Calc Center Position
	double x = e1st->m_cen.x - e1st->m_offset.x - hp_cx;
	double y = e1st->m_cen.y - e1st->m_offset.y - hp_cy;
	double dX2 = pEnt->m_cen.x - pEnt->m_offset.x - hp_cx;
	double dY2 = pEnt->m_cen.y - pEnt->m_offset.y - hp_cy;

	// Calc Theta
	double theta = atan2(y, x);
	double theta2 = atan2(dY2, dX2);

	size_t nSize = e->m_points.size();
	Point3Dbl pt;
	pt.x = e->m_points[nSize - 2].x - e->m_points[0].x;
	pt.y = e->m_points[nSize - 2].y - e->m_points[0].y;

	// Get PolyLine 
	double mag = _hypot(pt.x, pt.y);
	double nnx = pt.x / mag;
	double nny = pt.y / mag;
	double mag1 = _hypot(x, y);
	double nx = -y / mag1;
	double ny = x / mag1;
	double angCur = atan2(nny, nnx) * LC_RAD_TO_DEG;
	// sjyi 2023.04.14 패턴 회전 관련 수정.
	angCur -= dRotateAngle; // 패턴의 각도 - Rotation 각도(원래 위치에서 Rotation 했으므로 돌린 정도만큼 빼야 현재 각도 나옴.
	angCur = Trunc360(angCur);

	double angTarg = atan2(ny, nx) * LC_RAD_TO_DEG;
	angTarg = Trunc360(angTarg);
	TRACE("ang current (%f), target(%f), del = %f\n", angCur, angTarg, angTarg - angCur);

	//double angCur = theta2 * LC_RAD_TO_DEG;
	//double angTarg = theta * LC_RAD_TO_DEG;
	//TRACE("ang current (%f), target(%f), del = %f\n", angCur, angTarg, angTarg - angCur);
	theta = angTarg - angCur;

	//if (fabs(theta) < 1)
	//{
	//	g_bEventEnable = TRUE;
	//	return;
	//}

	double dMoveX, dMoveY;
	dMoveX = x - dX2;
	dMoveY = y - dY2;

	lcEntMove(e->m_hCadEntity, dMoveX, dMoveY);
	// imsi sjyi	lcEntRotate(e->m_hCadEntity, x + e1st->m_offset.x + hp_cx, y + e1st->m_offset.y + hp_cy, theta*LC_DEG_TO_RAD);
	lcEntUpdate(e->m_hCadEntity);
	e->ChangedEntity(e->m_hCadEntity);

	//lcEntUpdate(e->m_hCadEntity);

	g_bEventEnable = TRUE;
	e->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
}

HANDLE CIWSWorkHairpin::GenPolygon(HANDLE hBlock, double cx, double cy, double dLength, double dWidth, double dtheta)
{
	HANDLE hPline = lcBlockAddPolyline(hBlock, 0, FALSE, FALSE);

	//Length == Height
	double dL05 = dLength * 0.5;
	double dL25 = dLength * 0.25;
	double dW05 = dWidth * 0.5;
	double dW25 = dWidth * 0.25;

	//Hexagon(팔각형)
	//    0 	7
	//	1	       6
	//  2          5    
	//    3     4  	
	lcPlineAddVer(hPline, 0, -dL25, dW05);		//0
	lcPlineAddVer(hPline, 0, -dL05, dW25);		//1
	lcPlineAddVer(hPline, 0, -dL05, -dW25);		//2
	lcPlineAddVer(hPline, 0, -dL25, -dW05);		//3
	lcPlineAddVer(hPline, 0, dL25, -dW05);		//4
	lcPlineAddVer(hPline, 0, dL05, -dW25);		//5
	lcPlineAddVer(hPline, 0, dL05, dW25);		//6
	lcPlineAddVer(hPline, 0, dL25, dW05);		//7
	lcPlineAddVer(hPline, 0, -dL25, dW05);		//0

	lcPlineEnd(hPline);
	lcEntRotate(hPline, 0, 0, dtheta);
	lcEntMove(hPline, cx, cy);
	lcEntUpdate(hPline);

	return hPline;
}

void CIWSWorkHairpin::RotateHairpin(CEntity* pEnt, double dRotateAngle)
{
	double hp_cx, hp_cy;
	GetHairpinCenter(hp_cx, hp_cy);
	double x = pEnt->m_cen.x - pEnt->m_offset.x - hp_cx;
	double y = pEnt->m_cen.y - pEnt->m_offset.y - hp_cy;
	lcEntRotate(pEnt->m_hCadEntity, x, y, dRotateAngle*LC_DEG_TO_RAD);
}

double CIWSWorkHairpin::Trunc360(double dDegree)
{
	double dRtnDeg = dDegree;
	if (dDegree < 0)
	{
		dRtnDeg = dDegree + 360.0;
	}
	else if (dDegree >= 360.0)
	{
		dRtnDeg = dDegree - 360.0;
	}

	return dRtnDeg;
}