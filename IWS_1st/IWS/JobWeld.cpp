
#include "stdafx.h"
#include "JobWeld.h"
#include "IWSDoc.h"
#include "EntyArc.h"
#include "EntyCircle.h"
#include "EntyEllipse.h"
#include "EntyLine.h"
#include "EntyPoint.h"
#include "EntyPolyline.h"
#include "EntitySpiral.h"
#include "EntityRect.h"
#include "EntityHatch.h"

CJobWeld::CJobWeld()
{
	m_entity = NULL;
	m_iRepeat = 1;
	m_nZRepeat = 0;
	m_iPenID = 0;
	m_nOriPenID = 0;
	m_strJobTypeName = L"WeldJob";
	m_bSimulation = TRUE;

	m_dZ = 0; 
	m_dX = 0;
	m_dY = 0;

	// sjyi 2022-06-02 개별 리피트 횟수별 파워 관련 변수 추가 -->
	m_bUseIndividualRptPower = FALSE;
	// sjyi 2022-06-02 개별 리피트 횟수별 파워 관련 변수 추가 <--

	// sjyi 2023.04.15 패턴 회전각도 추가
	m_dRotation = 0.0;
}

CJobWeld::~CJobWeld()
{
}

void CJobWeld::DisplayProperties(CMFCPropertyGridCtrl *pProp)
{
	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Job Info"));
	jobInfo->AddSubItem(new CMFCPropertyGridProperty(_T("Name"), (_variant_t)m_entity->m_strName, L"Job name", (DWORD_PTR)&(m_entity->m_strName)));
	jobInfo->AddSubItem(new CMFCPropertyGridProperty(_T("Pen ID"), (_variant_t)m_iPenID, L"Pen ID", (DWORD_PTR)&m_iPenID));
	jobInfo->AddSubItem(new CMFCPropertyGridProperty(_T("Welding"), (_variant_t)m_bDoJob, L"Welding", (DWORD_PTR)&m_bDoJob));
	jobInfo->AddSubItem(new CMFCPropertyGridProperty(_T("Repeat"), (_variant_t)m_iRepeat, L"Repeat", (DWORD_PTR)&m_iRepeat));
	
	pProp->AddProperty(jobInfo);
	m_entity->SetPen(&m_iPenID);
	
	CMFCPropertyGridProperty* patternSize = new CMFCPropertyGridProperty(_T("PatternSize"));
	patternSize->AddSubItem(new CMFCPropertyGridProperty(_T("1st Length"), (_variant_t)m_d1StLength, L"1st Length", (DWORD_PTR)&m_d1StLength));
	patternSize->AddSubItem(new CMFCPropertyGridProperty(_T("1st Width"), (_variant_t)m_d1StWidth, L"1st Width", (DWORD_PTR)&m_d1StWidth));
	patternSize->AddSubItem(new CMFCPropertyGridProperty(_T("2nd Length"), (_variant_t)m_d2NdWidth, L"2nd Length", (DWORD_PTR)&m_d2NdWidth));
	patternSize->AddSubItem(new CMFCPropertyGridProperty(_T("2nd OverLen"), (_variant_t)m_d2NdOver, L"2nd OverLen", (DWORD_PTR)&m_d2NdOver));

	pProp->AddProperty(patternSize);

	CMFCPropertyGridProperty* WeldParam = new CMFCPropertyGridProperty(_T("WeldParam"));
	WeldParam->AddSubItem(new CMFCPropertyGridProperty(_T("Power"), (_variant_t)(*m_pPens)[m_iPenID].m_dblPower, L"Power", (DWORD_PTR)&(*m_pPens)[m_iPenID].m_dblPower));
	WeldParam->AddSubItem(new CMFCPropertyGridProperty(_T("Jump Speed"), (_variant_t)(*m_pPens)[m_iPenID].m_dblJumpSpeed, L"Jump Speed", (DWORD_PTR)&(*m_pPens)[m_iPenID].m_dblJumpSpeed));
	WeldParam->AddSubItem(new CMFCPropertyGridProperty(_T("Weld Speed"), (_variant_t)(*m_pPens)[m_iPenID].m_dblMarkSpeed, L"Weld Speed", (DWORD_PTR)&(*m_pPens)[m_iPenID].m_dblMarkSpeed));
		
	pProp->AddProperty(WeldParam);	

	// sjyi 2022-06-03 리피트 파워 파라미터 관련 수정 -->
	CMFCPropertyGridProperty* RptParam = new CMFCPropertyGridProperty(_T("RepeatParam"));
	// 개별 파워 조정 여부
	//RptParam->AddSubItem(new CMFCPropertyGridProperty(_T("Use Individual Power"), (_variant_t)m_bUseIndividualRptPower, L"Use Individual Power", (DWORD_PTR)&m_bUseIndividualRptPower));	// 
	CString strRptParamName;
	for (int i = 0; i < m_RptPower.size(); i++)
	{
		strRptParamName.Format(_T("Rpt Power(%d)"), i + 1);
		RptParam->AddSubItem(new CMFCPropertyGridProperty(strRptParamName, (_variant_t)m_RptPower[i].dPower, strRptParamName, (DWORD_PTR)&m_RptPower[i].dPower));
	}
	pProp->AddProperty(RptParam);
	// sjyi 2022-06-03 리피트 파워 파라미터 관련 수정 <--

	m_entity->DisplayEntityProperties(pProp);
}


double CJobWeld::GetLaserPower()
{
	CLaserPen *pPen = &(*m_pPens)[m_iPenID];
	return pPen->m_dblPower;
	return 90.0;
}

BOOL CJobWeld::Save(CFile *pf)
{
	pf->Write(&m_iPenID, sizeof(int));
	pf->Write(&m_nOriPenID, sizeof(int));
	CString strVer; 
	strVer.Format(L"_%d", WELDJOB_VERC);
	int ipos = m_strJobTypeName.Find(L"_", 0);
	if (ipos > 0)
		m_strJobTypeName = m_strJobTypeName.Mid(0, ipos);

	CString strWeldJobVer =  m_strJobTypeName + strVer;
	int nSize = strWeldJobVer.GetLength();
	
	USES_CONVERSION;
	char *pchr = W2A(strWeldJobVer.GetBuffer());
	pf->Write(&nSize, sizeof(int));
	pf->Write((void*)pchr, nSize*sizeof(char));
	pf->Write(&m_iRepeat, sizeof(int));
	pf->Write(&m_bDoJob, sizeof(int));
	int iType = m_entity->GetEntityType();
	pf->Write(&iType, sizeof(int));

	pf->Write(&m_d1StLength, sizeof(double));
	pf->Write(&m_d1StWidth, sizeof(double));
	pf->Write(&m_d2NdWidth, sizeof(double));
	pf->Write(&m_d2NdOver, sizeof(double));

	pf->Write(&m_dRotation, sizeof(double));		// sjyi 2023.04.15 패턴 회전 추가 

	BOOL bRet = m_entity->Save(pf, WELDJOB_VERC);
	return bRet;
}

BOOL CJobWeld::Load(CFile *pf)
{
	TRACE("CJobWeld::Load\n");
  	pf->Read(&m_iPenID, sizeof(int));
	pf->Read(&m_nOriPenID, sizeof(int));	

	int nSize = 0;
	char buff[MAX_PATH] = { "" };
	pf->Read(&nSize, sizeof(int));
 	pf->Read(&buff, nSize*sizeof(char));
	m_strJobTypeName = CString(buff);
	int ipos = m_strJobTypeName.Find(L"_", 0);
	CString strVer;

	if (ipos > 0) 
	{
		strVer = m_strJobTypeName.Mid(ipos+1, m_strJobTypeName.GetLength() - ipos);
		m_iFileVer = _tstoi(strVer);
	}		
	else
	{
		m_iFileVer = 0;
	}

	pf->Read(&m_iRepeat, sizeof(int));
	pf->Read(&m_bDoJob, sizeof(int));
	int iType;
	pf->Read(&iType, sizeof(int));

	pf->Read(&m_d1StLength, sizeof(double));
	pf->Read(&m_d1StWidth, sizeof(double));
	pf->Read(&m_d2NdWidth, sizeof(double));
	pf->Read(&m_d2NdOver, sizeof(double));

	// sjyi 2023.04.15 헤어핀 회전 기능 추가
	switch (m_iFileVer)
	{
	case WELDJOB_VER105:
		pf->Read(&m_dRotation, sizeof(double));
		break;
	default:
		m_dRotation = 0.0;
		break;
	}

	switch (iType)
	{
		case LC_ENT_LINE://		101  // Line
		{
			m_entity = new CEntyLine;
			break;
		}
		case LC_ENT_POLYLINE://		102  // Polyline
		{
			TRACE("Polyline ");
			m_entity = new CEntyPolyline;
			break;
		}
		case LC_ENT_CIRCLE://		103  // Circle
		{
			TRACE("Circle ");
			m_entity = new CEntyCircle;
			break;
		}
		case LC_ENT_ARC://         104  // Arc
		{
			TRACE("Arc ");
			m_entity = new CEntyArc;
			break;
		}
		case LC_ENT_POINT://       107  // Point
		{
			TRACE("Point ");
			m_entity = new CEntyPoint;
			break;
		}
		case LC_ENT_ELLIPSE://     109  // Ellipse or elliptical arc
		{
			TRACE("ELLIPSE ");
			m_entity = new CEntyEllipse;
			break;
		}
		case LC_ENT_RECT://        118  // Rectangle
		{
			TRACE("RECT ");
			m_entity = new CEntityRect;

			break;
		}
		case LC_ENT_HATCH://       124  // Hatch
		{
			TRACE("HATCH ");
			m_entity = new CEntityHatch;
			break;
		}
		case LC_ENT_SPIRAL:
		{
			TRACE("Block");
			m_entity = new CEntitySpiral;
			break;
		}
		default:
			AfxMessageBox(L"Unknown Shape");
			return FALSE;
	}

	m_entity->SetPen(&m_iPenID);
	m_entity->Load(pf, m_iFileVer);
	TRACE("entity was added\r\n");

 	return TRUE;
}


//===============================================================================
//	DoJob()
//	<목적>	실제 용접 작업 진행
//	<인자>	CIWSDoc *pDoc		(CIWS2Doc 클래스 포인터)
//			BOOL bSimulation	(시뮬레이션 적용 여부)
//			int nRepeat			(작업 반복 횟수)
//			double *pRptProfile (반복 횟수별 용접 파워, NULL일 경우 모두 동일 파워로 용접)
//			BOOL bContinuousRtp (Sortie 반복 적용 여부)
//			int nSortieDir		(Sortie 반복 방향, 0 : One-way, 1 : Bi-directional)
//	<반환>	int					(0 : 정상종료, 그 외 : 이상종료)
//  <참고사항> 
//	sjyi 2022-08-31 [comment] Continuous Repeat Mode(Sortie 반복)에서는 파워 프로파일 적용 안됨.(적용하면 안됨.)
//					-> 사유) Sortie 반복은 일반 반복과 별도로 반복횟수 지정.(파워 프로파일 적용 불가)
//						     파워 프로파일은 인반 반복에 한함.
//  sjyi 2024.05.27 [comment] Sortie 반복일 경우, 파워 별도 지정할 수 있도록 인자 추가 필요.(향후, 수정 예정)
//===============================================================================
int CJobWeld::DoJob(CIWSDoc *pDoc, BOOL bSimulation, int nRepeat, double *pRptProfile, BOOL bContinuousRtp, int nSortieDir)
{
	// Entity Check
	if (m_entity == NULL) {
		return 0;
	}

	// Repeat Check
	if(nRepeat < 0)
		nRepeat = m_iRepeat;

	// Pen No 설정
	//TRACE("%s:Job(%s) start\n", m_strJobTypeName, m_entity->m_strName);	
	CLaserPen *pPen;// = &(*m_pPens)[m_iPenID];
	pPen = &(*m_pPens)[m_iPenID];
	
	// 변수 초기화
	//double dAddPower = 0;
	//double dAddSpeed = 0;

	// 펜 설정 가져오기
	if (bSimulation) // 시뮬레이션이면 시뮬레이션 팬 설정
	{
		pPen = &(*m_pPens)[PEN_SIMULATION]; //int isimPen = (int)m_pPens->size() - 2;
	}
	else // 시뮬레이션 아니면 정상 펜 설정
	{
		pPen = &(*m_pPens)[m_iPenID];
		/*CMainFrame *pFrm = (CMainFrame*)AfxGetMainWnd();
		CIWSDoc *pDoc = (CIWSDoc *)pFrm->GetActiveDocument();*/

		//if (m_iPenID < 10)
		//{
		//	dAddPower = pDoc->m_work.m_hp.m_dPenPower[m_iPenID];
		//	//dAddSpeed = pDoc->m_config.m_dPenSpeed[m_iPenID];
		//}
	}

	// 펜설정
	m_pScan->SetLaserPen(pPen);
	//m_pScan->SetLaserPenParam();

	// 스캐너 파라미터 설정
	m_pScan->SetScanParamList();
	//Sleep(10);

	/*if (dAddSpeed != 0)
		m_pScan->SetAddMarkSpeed(dAddSpeed);*/

	// 파워 쉐이핑 Check
	if (pPen->m_bUsePowerShape) 
	{
		DoJobPowerShaping(pPen, nRepeat);
		return 0;
	}

	// 펄스 쉐이핑 Check
	if (pPen->m_bUsePulseShape) 
	{
		DoJobPulseShapeing(pPen, nRepeat);
		return 0;
	}

	// mspark 2022.04.19 
	//m_pScan->SetPower(bSimulation ? 0 : (pPen->m_dblPower + dAddPower) * m_pScan->GetPowerRatio(), TRUE);
	
	// 워벨 사용여브 Check
	if (pPen->m_bUseWobbel)
		m_pScan->SetWobbel(pPen->m_dWbFreq, pPen->m_dWbLongitudinal, pPen->m_dWbTransbersal);
	else
		m_pScan->SetWobbel(0, 0, 0);

	// Hatch 사용 여부
	if (m_entity->GetEntityType() == LC_ENT_HATCH) 
	{
		DoHatch(nRepeat);

		return 0;
	}
	
	// Hatch 아닌 경우(정상인 경우)-------------------------------------------------------------
	// Point이면 Delay Manual On 사용
	if (m_entity->GetEntityType() == LC_ENT_POINT) 
	{
		m_pScan->LaserOnHold(((CEntyPoint*)m_entity)->delay);
	}

	// Path 개수 가져오기
	int nPath = (int)m_entity->m_path.size();
	double dPower = 0; // 변수 초기화
	BOOL bContinuousEnable = FALSE; // 연속 사용 여부, 즉 한 패턴 반복할 때, 끝에서 부터 다시 시작(bi-direction) 사용 여부

	if (nSortieDir == SORTIE_BIDIRECTION) {
		bContinuousEnable = TRUE;
	}
	else {
		bContinuousEnable = FALSE;
	}

	if (bContinuousRtp && bContinuousEnable) // sjyi 2022-08-31 [comment] Continuous Repeat Mode에서는 파워 프로파일 적용 안됨.
	{
		for (int itr = 0; itr < nRepeat; itr++)
		{
			// Sortie 반복일 경우, 모두 동일 파워로 용접
			dPower = (pPen->m_dblPower/* + dAddPower*/) * m_pScan->GetPowerRatio() * 1.0;

			// sjyi 2023.07.19 Percent / Wat 모드 구분을 위해 SetPower 함수로 변경
			//m_pScan->INT_AO_SetPower_Percent(bSimulation ? 0 : dPower, TRUE);
			m_pScan->SetPower(bSimulation ? 0 : dPower, TRUE);
			// ==================

			// 첫번째 위치로 이동(Jump)
			if (itr == 0) // 첫번째 작업의 첫번째 포인트에서만 위치 이동(Jump)
			{
				m_pScan->JumpList(m_entity->m_path[0].x, m_entity->m_path[0].y);
			}

			// 작업 리스트 만들기
			if (itr % 2 == 0) // 홀수 번 째는 정방향
			{
				for (int i = 1; i < nPath; i++) // 1 ~ nPath-1
					m_pScan->MarkList(m_entity->m_path[i].x, m_entity->m_path[i].y);
			}
			else // 짝수번 째는 역방향
			{
				for (int i = 2; i <= nPath; i++) // nPath - 2 ~ 0
					m_pScan->MarkList(m_entity->m_path[nPath - i].x, m_entity->m_path[nPath - i].y);
			}
		}
	}
	else
	{
		for (int itr = 0; itr < nRepeat; itr++)
		{
			// sjyi 2022-06-03 리피트 개별 파워 관련 내용 수정 -->
			//if (m_bUseIndividualRptPower) // 포인트별 개별 파워 사용 설정 여부(최우선)
			//{
			//	dPower = (pPen->m_dblPower + dAddPower) * m_pScan->GetPowerRatio() * m_RptPower[itr].dPower;
			//}
			//else if (pRptProfile) // 파워 프로파일 사용 설정
			if (pRptProfile) // 파워 프로파일 사용 설정
			{
				dPower = (pPen->m_dblPower/* + dAddPower*/) * m_pScan->GetPowerRatio() * pRptProfile[itr];
			}
			else // 파워 프로파일이 아닌 경우, 모두 동일 파워로 용접
			{
				dPower = (pPen->m_dblPower/* + dAddPower*/) * m_pScan->GetPowerRatio() * 1.0;
			}

			// 파워 설정
			m_pScan->SetPower(bSimulation ? 0 : dPower, TRUE);
			// ==================
			// sjyi 2022-06-03 리피트 개별 파워 관련 내용 수정 <--

			// Jump(패턴 처음으로 위치 이동)
			m_pScan->JumpList(m_entity->m_path[0].x, m_entity->m_path[0].y);

			// 작업 리스트 만들기
			for (int i = 1; i < nPath; i++)
				m_pScan->MarkList(m_entity->m_path[i].x, m_entity->m_path[i].y);
		}
	}

	return 0;
}

void CJobWeld::DoHatch(int nRepeat)
{
	CEntityHatch* pEnt = (CEntityHatch*)m_entity;
	int nPath = (int)pEnt->hatchPath->size();
	for (int itr = 0; itr < nRepeat; itr++) 
	{
		for (int i = 0; i < nPath; i++) 
		{
			m_pScan->JumpList(pEnt->hatchPath[0][i].x, pEnt->hatchPath[0][i].y);
			m_pScan->MarkList(pEnt->hatchPath[1][i].x, pEnt->hatchPath[1][i].y);
		}
	}
}

void CJobWeld::DoJobPowerShaping(CLaserPen *pPen, int nRepeat, double *pRptProfile)
{
	MakePathPowerShape(pPen);
	
	//m_pScan->WaitMoving();
	
	//m_pScan->StartList();
	//m_pScan->INT_AO_SetPower(bSimulation ? 0 : pPen->m_dblPower,TRUE);
	int nPath = (int)m_PathPwrShape.size();
	if (m_entity->GetEntityType() == LC_ENT_POINT) 
	{
		m_pScan->LaserOnHold(((CEntyPoint*)m_entity)->delay);
	}

	double dPower = 0;
	for (int itr = 0; itr < nRepeat; itr++) 
	{
		m_pScan->JumpList(m_PathPwrShape[0].x, m_PathPwrShape[0].y);

		// mspark 2022.04.19
		if (pRptProfile)
			dPower = (m_PathPwrShape[0].z) * pRptProfile[itr];
		else
			dPower = (m_PathPwrShape[0].z) * 1.0;

		dPower = dPower * m_pScan->GetPowerRatio(); // sjyi 2024.01.17 Power Shaping 모드에서도 Manual Power 비율 적용
		
		m_pScan->SetPower(dPower, TRUE);
		// ==================

		for (int i = 0; i < nPath; i++) 
		{
			//m_pScan->SetPower(m_PathPwrShape[i].z, TRUE);
			dPower = m_PathPwrShape[i].z * m_pScan->GetPowerRatio(); // sjyi 2024.01.17 Power Shaping 모드에서도 Manual Power 비율 적용
			m_pScan->SetPower(dPower, TRUE);

			m_pScan->MarkList(m_PathPwrShape[i].x, m_PathPwrShape[i].y);
		}
	}

	//m_pScan->EndList();
	//m_pScan->Execute();
	//m_pScan->WaitMoving();
}

void CJobWeld::DoJobPulseShapeing(CLaserPen *pPen, int nRepeat, double *pRptProfile)
{
	if(!MakePathPulseShape(pPen)) return;

	//m_pScan->WaitMoving();

	//m_pScan->StartList();
	//m_pScan->INT_AO_SetPower(bSimulation ? 0 : pPen->m_dblPower,TRUE);
	int nPath = (int)m_PathPwrShape.size();
	if (m_entity->GetEntityType() == LC_ENT_POINT) 
	{
		m_pScan->LaserOnHold(((CEntyPoint*)m_entity)->delay);
	}

	double dPower = 0;
	for (int itr = 0; itr < nRepeat; itr++)
	{
		m_pScan->JumpList(m_PathPwrShape[0].x, m_PathPwrShape[0].y);
		if (pRptProfile)
			dPower = (m_PathPwrShape[0].z) * pRptProfile[itr];
		else
			dPower = (m_PathPwrShape[0].z) * 1.0;

		dPower = dPower * m_pScan->GetPowerRatio(); // sjyi 2024.01.17 Power Shaping 모드에서도 Manual Power 비율 적용

		m_pScan->SetPower(dPower, TRUE);
		for (int i = 0; i < nPath; i++) 
		{
			//m_pScan->SetPower(m_PathPwrShape[i].z, TRUE);
			dPower = m_PathPwrShape[i].z * m_pScan->GetPowerRatio(); // sjyi 2024.01.17 Power Shaping 모드에서도 Manual Power 비율 적용
			m_pScan->SetPower(dPower, TRUE);

			m_pScan->MarkList(m_PathPwrShape[i].x, m_PathPwrShape[i].y);
		}
	}
	//m_pScan->EndList();
	//m_pScan->Execute();
	//m_pScan->WaitMoving();

}

void CJobWeld::MakePathPowerShape(CLaserPen *pPen)
{
	m_PathPwrShape.clear();
	Point3Dbl start_pt, end_pt;
	start_pt = m_entity->m_path[0];
	end_pt = m_entity->m_path[m_entity->m_path.size() - 1];
	double length = 0;

	std::vector<double> path_length;
	double dlen;
	for (int i = 1; i < m_entity->m_path.size(); i++)
	{
		dlen = (_hypot(m_entity->m_path[i - 1].x - m_entity->m_path[i].x, m_entity->m_path[i - 1].y - m_entity->m_path[i].y));
		length += dlen;
		path_length.push_back(dlen);
	}
	double min_divide_len = length / pPen->m_nResPowerShape;

	m_PathPwrShape.push_back(start_pt);
	for (int i = 0; i < path_length.size(); i++)
	{
		Point3Dbl pt0 = m_entity->m_path[i];
		Point3Dbl pt1 = m_entity->m_path[i + 1];
		if (shorterThen(pt0 - pt1, min_divide_len)) 
		{
			m_PathPwrShape.push_back(pt1);
		}
		else 
		{
			PathSubDivide(pt0, pt1, min_divide_len);
		}
	}

	if (TRUE) {
		int nShape =(int) m_PathPwrShape.size();

		double LEN = length;
		double len = 0;

		double x, y;
		double x0, y0;
		double dpower;
		m_PathPwrShape[0].z = pPen->m_pwrShape[0].y;
		x0 = m_PathPwrShape[0].x;
		y0 = m_PathPwrShape[0].y;
		int nPowerShap = (int)pPen->m_pwrShape.size();
		for (int i = 1; i < nShape - 1; i++) 
		{
			x = m_PathPwrShape[i].x;
			y = m_PathPwrShape[i].y;;
			len += _hypot(x - x0, y - y0);
			dpower = m_PathPwrShape[i - 1].z;
			for (int j = 0; j < pPen->m_pwrShape.size(); j++) 
			{
				if (pPen->m_pwrShape[j].x / 100.0 * LEN > len)
				{
					dpower = INTERPOLATE(len / LEN * 100, pPen->m_pwrShape[j - 1].x, pPen->m_pwrShape[j].x,
						pPen->m_pwrShape[j - 1].y, pPen->m_pwrShape[j].y);
					break;
				}
			}

			m_PathPwrShape[i].z = dpower;
			x0 = x;
			y0 = y;
		}
		m_PathPwrShape[nShape - 1].z = pPen->m_pwrShape[pPen->m_pwrShape.size() - 1].y;
	}
	_tracer(TRACE_NORMAL, 1, L"Number of seqment(Power shape mode) =%d", m_PathPwrShape.size());
// 	int idx = 0;
// 	for (auto p : m_PathPwrShape) {
// 		TRACE("%d, pos x= %f, y = %f, power = %f \n", idx, p.x, p.y, p.z);
// 		idx++;
// 	}
}

BOOL CJobWeld::MakePathPulseShape(CLaserPen * pPen)
{
	m_PulseShape.clear();
	m_PathPwrShape.clear();
	Point3Dbl start_pt, end_pt;
	start_pt = m_entity->m_path[0];
	end_pt = m_entity->m_path[m_entity->m_path.size() - 1];
	double length = 0;
	int nPath;
	std::vector<double> path_length;
	double dlen;

	for (int i = 1; i < m_entity->m_path.size(); i++)
	{
		dlen = (_hypot(m_entity->m_path[i - 1].x - m_entity->m_path[i].x, m_entity->m_path[i - 1].y - m_entity->m_path[i].y));
		length += dlen;
		path_length.push_back(dlen);
	}

	double min_divide_len = PULSE_RESOLUTION/1000.0f * pPen->m_dblMarkSpeed; // mm

	m_PathPwrShape.push_back(start_pt);
	for (int i = 0; i < path_length.size(); i++)
	{
		Point3Dbl pt0 = m_entity->m_path[i];
		Point3Dbl pt1 = m_entity->m_path[i + 1];
		if (shorterThen(pt0 - pt1, min_divide_len)) 
			m_PathPwrShape.push_back(pt1);
		else 
			PathSubDivide(pt0, pt1, min_divide_len);
	}

 	int nShape =(int) pPen->m_pulseShape.size();
	double dPulseWidth = pPen->m_dblPulseWidth/1000000.0f ; // sec
	double dPulseLenght = pPen->m_dblMarkSpeed*dPulseWidth; // mm
	int nPulseShape = int(dPulseWidth / PULSE_RESOLUTION *1000+ 0.5);

	for (int i = 0; i < nPulseShape; i++) 
	{
		double dpower = 0;
		for (int j = 0; j < pPen->m_pulseShape.size(); j++) 
		{
			if (pPen->m_pulseShape[j].x / 100.0 * nPulseShape > i)
			{
				dpower = INTERPOLATE(double(i)/double(nPulseShape)*100.f, pPen->m_pulseShape[j - 1].x, pPen->m_pulseShape[j].x,
					pPen->m_pulseShape[j - 1].y, pPen->m_pulseShape[j].y);

				break;
			}
		}
		m_PulseShape.push_back(dpower);
	}

	int nOff = (int)(double(nPulseShape) / PULSE_DUTY_CYCLE - nPulseShape);
	for(int i=0;i< nOff; i++)
		m_PulseShape.push_back(0);

	nPath =(int) m_PathPwrShape.size();
	int idx = 0;
	nPulseShape = (int)m_PulseShape.size();

	for (size_t i = 0; i < nPath; i++) 
	{
		idx = i % nPulseShape;
		double dpower = m_PulseShape[idx];
		m_PathPwrShape[i].z = dpower;
	}

	if (nPath > MAX_PULSE_SHAPE)
	{
		_tracer(TRACE_WARNING, 1, L"Pulse count is too small");
		return FALSE;
	}

	double tmin = dPulseWidth / PULSE_DUTY_CYCLE;
	double t = length / pPen->m_dblMarkSpeed;
	if (t < tmin) 
	{
		_tracer(TRACE_WARNING, 1, L"Too many pulses");
		return FALSE;
	}

	_tracer(TRACE_NORMAL, 1, L"Number of pulses output(pulse shape mode) : seg=%d, pulse=%d", nPath, nPath/ nPulseShape);
	return TRUE;
}

void CJobWeld::PathSubDivide(Point3Dbl p0, Point3Dbl p1, double division)
{
	double U = p1.x - p0.x; 
	double V = p1.y - p0.y;
	double UU = U * U;
	double VV = V * V;
	double D = sqrt(UU + VV);
	double u = U / D;
	double v = V / D;
	double d = division;

	Point3Dbl p;
	int ndiv = int(D / division+0.5)+1;
	for (int i = 0; i < ndiv; i++) 
	{
		p.x = u * d + p0.x;
		p.y = v * d + p0.y;
		if (D - d <=0.0 ) 
		{
			if (shorterThen(p - p1, 1e-3))
				m_PathPwrShape[m_PathPwrShape.size() - 1] = p1;
			else if(shorterThen(m_PathPwrShape[m_PathPwrShape.size() - 1]-p1, 1e-6) )
				m_PathPwrShape[m_PathPwrShape.size() - 1] = p1;
			else
				m_PathPwrShape.push_back(p1);
			//TRACE("PathSubDivide_end %d : %f,%f\n", i, p1.x, p1.y);
			break;
		}
		else
		{
			m_PathPwrShape.push_back(p);
		}

		d += division;

		//TRACE("PathSubDivide %d : %f,%f\n", i, p.x, p.y);
	}

}

BOOL CJobWeld::SaveIndividualPower(CFile *pf)
{
	// Save IndividualPower UseFlag
	pf->Write(&m_bUseIndividualRptPower, sizeof(BOOL));
	
	// Save IndividualPower 
	vector_RepeatPowerInfo::iterator it;

	for (it = m_RptPower.begin(); it != m_RptPower.end(); it++)
	{
		pf->Write(&it->dPower, sizeof(double));
	}

	return TRUE;
}

BOOL CJobWeld::LoadIndividualPower(CFile *pf)
{
	int nRtn = 0;
	
	// Load IndividualPower UseFlag
	nRtn = pf->Read(&m_bUseIndividualRptPower, sizeof(BOOL));

	// sjyi 2022-01-08 일부 Save Load과정에서 쓰레기 값 확인, TRUE 즉 1이 아닌 경우 FALSF로 변경
	if (m_bUseIndividualRptPower != 1)
	{
		m_bUseIndividualRptPower = 0;
	}

	// Load IndividualPower 
	IWS_HAIRPIN_REPEAT_INFO stRepeatInfo;
	double dPower = 0;
	for (int i =0; i<m_iRepeat; i++)
	{

		nRtn = pf->Read(&dPower, sizeof(double));
		stRepeatInfo.dPower = dPower;

		if (m_bUseIndividualRptPower == FALSE)
		{
			// sjyi 2022-01-08 일부 Save Load과정에서 쓰레기 값 확인, 개별 파워가 설정이 FALSE이고, 개별파워가 0이면 1.0(100%)로 변경
			if (dPower <= 0.01)
			{
				stRepeatInfo.dPower = 1.0;
			}
		}

		m_RptPower.push_back(stRepeatInfo);
	}


	return TRUE;
}

BOOL CJobWeld::GetUseIndividualPower()
{
	return m_bUseIndividualRptPower;
}

void CJobWeld::SetUseIndividualPower(BOOL bUse)
{
	m_bUseIndividualRptPower = bUse;
}

int CJobWeld::AddIndividualPower(double dPower)
{
	IWS_HAIRPIN_REPEAT_INFO stRepeatInfo;
	stRepeatInfo.dPower = dPower;

	m_RptPower.push_back(stRepeatInfo);

	return (int)m_RptPower.size();
}

void CJobWeld::SetIndividualPower(int nRepeatIndex, double dPower)
{
	if (nRepeatIndex > m_RptPower.size())	// Error : Size Over
	{
		return;
	}

	m_RptPower[nRepeatIndex].dPower = dPower;
}

void CJobWeld::ClearIndividualPower()
{
	m_RptPower.clear();
}

void CJobWeld::AddIndividualPowerAsLast()
{
	double dPower = 1.0;
	IWS_HAIRPIN_REPEAT_INFO stRepeatInfo;

	if (m_RptPower.size()) {
		dPower = m_RptPower[m_RptPower.size() - 1].dPower;
	}

	stRepeatInfo.dPower = dPower;

	m_RptPower.push_back(stRepeatInfo);
}

void CJobWeld::RevomeIndividualPowerLast()
{
	if (m_RptPower.size() != 0)
	{
		m_RptPower.pop_back();
	}
}

// sjyi 2023.04.15 헤어핀 회전 파라미터 추가
void CJobWeld::SetRotation(double dRotation)
{
	m_dRotation = dRotation;
}

// sjyi 2023.04.15 헤어핀 회전 파라미터 추가
double CJobWeld::GetRotation()
{
	return m_dRotation;
}

double CJobWeld::GetIndividualPower(int nRepeatID)
{
	if (nRepeatID < m_RptPower.size()) {
		return m_RptPower[nRepeatID].dPower;
	}
	else {
		return 1.0;
	}
}

int CJobWeld::GetIndividualPowerCnt()
{
	return m_RptPower.size();
}

void CJobWeld::SetPenID(int nID, BOOL bOrigin)
{
	m_iPenID = nID;

	if (bOrigin)
	{
		m_nOriPenID = nID;
	}
}