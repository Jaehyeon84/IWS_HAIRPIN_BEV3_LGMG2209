#include "stdafx.h"
#include "IWS.h"
#include "IWSWork.h"
#include "IWSDoc.h"
#include "CDlgWaitWork.h"
#include "EntyPoint.h"
#include <vector>

#include "CRapidJasonWork.h" // sjyi 2025.09.29 Jason 사용을 위한 함수 추가

using namespace std;

DECLARE_USER_MESSAGE(UWM_CIWSWORK)

CIWSWork::CIWSWork()
{
	m_pWorkThread = NULL;
	m_bSimulation = FALSE;
	m_bStopThread = FALSE;
	m_hRcv = NULL;
	m_pScanner = NULL;
	m_pDlgWait = NULL;
	m_dwStart = 0;
	m_dwEnd = 0;
	m_nDefLayer = 8;
	m_bWeldPath = FALSE;
	m_bInnerWelding = FALSE;
}

CIWSWork::~CIWSWork()
{
	clear();
}

DWORD CIWSWork::GetTactTime()
{
	return m_dwTactTime;
}

BOOL CIWSWork::GetInspectjudgement()
{
	if (!m_resInspect.size()) 
		return FALSE;

	BOOL bRes = TRUE;
	int iFail = 0;
	int idx = 0;
	for (auto p : m_resInspect) 
	{	
		if (p < 0.1) {
			iFail++;
			bRes = FALSE;
			_tracer(TRACE_WARNING, 1, L"Check %d-pin !", idx);
		}
		idx++;
	}

	_tracer(TRACE_WARNING, 1, L"Number of fail count = %d ", iFail);
	return bRes;
}

void CIWSWork::GotoSelectedScannerPos()
{
	HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);
	// select all entities on current block
	HANDLE entitiy = lcBlockGetFirstSel(hBlock);
	if (!entitiy) 
		return;

	CJob* pJob;
	for (int i = 0; i < m_JobGroup.size(); i++)
	{
		pJob = m_JobGroup[i]->GetSelectEntity(entitiy);
		if (pJob)
		{
			CLaserPen *pOldPen = m_pScanner->GetLaserPen();
			m_pScanner->SetLaserPen(m_pPenMove);
			m_pScanner->SetLaserPenParam();
			m_pScanner->WaitMoving(TRUE);

			m_pScanner->Move(pJob->GetEntity()->m_cen.x, pJob->GetEntity()->m_cen.y);
			m_pScanner->WaitMoving(TRUE);

			m_pScanner->SetLaserPen(pOldPen);
			m_pScanner->SetLaserPenParam();
			return;
		}
	}
}

void CIWSWork::SetNextJob()
{
	HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);
	// select all entities on current block
	HANDLE entitiy = lcBlockGetFirstSel(hBlock);
	if (!entitiy) 
		return;

	CJob* pJob;
	BOOL isLast = FALSE;
	for (int i = 0; i < m_JobGroup.size(); i++)
	{
		isLast = FALSE;
		pJob = m_JobGroup[i]->GetNextEntity(entitiy, &isLast);

		if (isLast) // 그룹 내에 마지막 Job 이면 다음 그룹의 첫번 째 Job Load
		{
			if (i < m_JobGroup.size() - 1)
				pJob = m_JobGroup[i + 1]->m_jobs.front();
		}

		if (i == m_JobGroup.size() - 1 && !pJob) // 마지막까지 해서 없으면 맨 처음 엔티티 선택
			pJob = m_JobGroup.front()->m_jobs.front();

		if (pJob)
		{
			lcBlockUnselect(hBlock);
			lcBlockSelectEnt(hBlock, pJob->GetEntity()->m_hCadEntity, TRUE);
			lcWndRedraw(g_hLcWnd);
			return;
		}
	}
}

void CIWSWork::SetPrevJob()
{
	HANDLE hBlock = lcPropGetHandle(g_hLcWnd, LC_PROP_WND_BLOCK);
	// select all entities on current block
	HANDLE entitiy = lcBlockGetFirstSel(hBlock);
	if (!entitiy)
		return;

	CJob* pJob;
	BOOL isFirst;
	int nJobSize = 0; // ZigZag2 추가 여부 확인용, JobSize가 4개이면 ZigZag2
	nJobSize = m_JobGroup[0]->m_jobs.size();
	for (int i = 0; i < m_JobGroup.size(); i++)
	{
		isFirst = FALSE;
		pJob = m_JobGroup[i]->GetPrevEntity(entitiy, &isFirst);
		if (isFirst) // 그룹 내에 첫번째 Job 이면 이전 그룹의 마지막 Job Load 
		{
			// sjyi 2022-08-19 2차 패턴은 선택되지 않도록 수정 -->
			int nLastIdx = 1;
			if (m_pDoc->m_config.m_bUseHairpin) // 헤어핀의 경우, 2차패턴은 제외
			{
				if (nJobSize == 4)
				{
					nLastIdx = 3;
				}
				else
				{
					nLastIdx = 2;
				}
			}
				
			if (i == 0)
				pJob = m_JobGroup.back()->m_jobs[m_JobGroup.back()->m_jobs.size() - nLastIdx];
			else
				pJob = m_JobGroup[i - 1]->m_jobs[m_JobGroup.back()->m_jobs.size() - nLastIdx];
			// sjyi 2022-08-19 2차 패턴은 선택되지 않도록 수정 <--
		}

		if (pJob)
		{
			lcBlockUnselect(hBlock);
			lcBlockSelectEnt(hBlock, pJob->GetEntity()->m_hCadEntity, TRUE);
			lcWndRedraw(g_hLcWnd);
			return;
		}
	}
}

void CIWSWork::clear()
{
	for (auto job : m_JobGroup)
	{
		job->Clear();
		delete job;
	}

	m_JobGroup.clear();
}

void CIWSWork::SaveWork(CFile *pf, CIWSDoc *pDoc)
{
	
	int nGroup = (int)m_JobGroup.size();
	int nJob = 0;
	if(nGroup != 0)
		nJob = (int)m_JobGroup.front()->m_jobs.size();
	
	pf->Write(&m_pDoc->m_config.m_bUseHairpin, sizeof(BOOL));
	pf->Write(&nGroup, sizeof(int));
	pf->Write(&nJob, sizeof(int));
	pf->Write(&m_bSimulation, sizeof(BOOL));

	// sjyi 2022-06-03 comment 작업파일 저장 부분
	for (auto group : m_JobGroup)
	{
		pf->Write(&group->m_nIndex, sizeof(int));
		pf->Write(&group->m_nSlot, sizeof(int));
		pf->Write(&group->m_nLayer, sizeof(int));

		for (auto job : group->m_jobs) {
			int iType = job->GetJobType();
			pf->Write(&iType, sizeof(int));
			job->Save(pf);
		}
	}

	// save pen 
	CStringA strPens = "LaserPen";
	int nSize = strPens.GetLength();
	char *pchr = (strPens.GetBuffer());

	pf->Write(&nSize, sizeof(int));
	pf->Write((void*)pchr, nSize * sizeof(char));
	CArchive ar(pf, CArchive::store);
	int nPen = (int)(pDoc->m_pens.size());
	ar << nPen;

	for (int i = 0; i < nPen; i++) 
		pDoc->m_pens[i].Serialize(ar);

	ar.Close();

	// sjyi 2022-06-03 개별 파워에 대한 내용 추가
	if (pDoc->m_config.m_bUseHairpin)	// Hairpin일 경우에만 저장
	{
		CJobWeld* pJobWeld = NULL;
		for (auto group : m_JobGroup)
		{
			for (auto job : group->m_jobs)
			{
				pJobWeld = (CJobWeld*)job;
				pJobWeld->SaveIndividualPower(pf);
			}
		}
	}
}

BOOL CIWSWork::LoadWork(CFile *pf, CIWSDoc *pDoc)
{
	int nGroup;
	int nJob;
	BOOL bUseHairpin;

	pf->Read(&bUseHairpin, sizeof(BOOL));

	//if (!m_pDoc->m_config.m_bUseHairpin)
	if (bUseHairpin == FALSE && m_pDoc->m_config.m_bUseHairpin == TRUE)
	{
		AfxMessageBox(L"The saved file cannot be loaded because it was saved in Normal Mode.");
		return FALSE;
	}
	else if (bUseHairpin == TRUE && m_pDoc->m_config.m_bUseHairpin == FALSE)
	{
		AfxMessageBox(_T("The saved file cannot be loaded because it was saved in Hairpin Mode."));
		return FALSE;
	}

	pf->Read(&nGroup, sizeof(int));
	pf->Read(&nJob, sizeof(int));
	pf->Read(&m_bSimulation, sizeof(BOOL));

	int iType;
	for (int i = 0; i < nGroup; i++)

	{
		CJobGroup* group = new CJobGroup();
		pf->Read(&group->m_nIndex, sizeof(int));
		pf->Read(&group->m_nSlot, sizeof(int));
		pf->Read(&group->m_nLayer, sizeof(int));

		for (int j = 0; j < nJob; j++) 
		{
			pf->Read(&iType, sizeof(int));
			if (iType == JOB_TYPE_WELD) 
			{
				CJobWeld *pJob = new CJobWeld;

				pJob->m_pScan = pDoc->m_pScanner;
				pJob->m_pPens = &(pDoc->m_pens);
				if (!pJob->Load(pf)) 
				{
					delete pJob;
					AfxMessageBox(L"Work file loading fail");
					return FALSE;
				}

				group->AddJob(pJob);
			}
		}

		m_JobGroup.push_back(group);
	}

	int nSize = 0;
	char buff[MAX_PATH] = { "" };

	pf->Read(&nSize, sizeof(int));
	if (nSize>0) 
	{
		pf->Read(&buff, nSize * sizeof(char));
		CArchive ar(pf, CArchive::load);
		int nPen = 0;
		ar >> nPen;
		pDoc->m_pens.clear();

		for (int i = 0; i < nPen; i++) 
		{
			CLaserPen pen;
			pen.Serialize(ar);

			switch (pDoc->m_config.m_iLaser)
			{
			case IPG_CW_YLR2000SM:
			case TRUMPF_TRUDISK:
				pen.m_bUsePulseShape = FALSE;
				break;
			case IPG_CW_YLR6000QCW:
				pen.m_bUsePulseShape = FALSE;
				break;
			case IPG_PULSE_YLR6000QCW:
				pen.m_bUsePowerShape = FALSE;
				break;
			}
			pDoc->m_pens.push_back(pen);
		}
		ar.Close();
	}
	
	// sjyi 2022-06-03 개별 파워 설정 관련 Load -->
	// 여기에 개별설정 관련 Load 한 이유는 기존 작업파일과의 호환을 위해,
	// 파일 끝에 추가 설정(개별파워 설정) 관련 내용 Save 하였음.
	if (bUseHairpin)	// Hairpin일 경우에만 Load
	{
		CJobWeld* pJobWeld = NULL;
		for (auto group : m_JobGroup)
		{
			for (auto job : group->m_jobs) 
			{
				pJobWeld = (CJobWeld*)job;
				pJobWeld->LoadIndividualPower(pf);

			}
		}
	}
	// sjyi 2022-06-03 개별 파워 설정 관련 Load <--


	return TRUE;
}

void CIWSWork::DeleteJob(HANDLE entity)
{
	if (m_pDoc->m_config.m_bUseHairpin)
		return;

	IWS_VECTOR_JOB_GROUP::iterator it;
	CJobGroup *pEnt;
	for (it = m_JobGroup.begin(); it != m_JobGroup.end(); it++) 
	{
		if ((*it)->DeleteJob(entity)) 
		{
			if ((*it)->m_jobs.empty())
				delete *it;

			m_JobGroup.erase(it);
			break;
		}
	}
//	for (int i = 0; i < m_JobGroup.size(); i++)
//		if (m_JobGroup[i]->DeleteJob(entity))
//			break;
}

int CIWSWork::ChangeJob(HANDLE entity)
{
// 	if (m_pDoc->m_config.m_bUseHairpin)
// 		return FALSE;

	for (int i = 0; i < m_JobGroup.size(); i++)
		if (m_JobGroup[i]->ChangeJob(entity))
			return TRUE;

	return FALSE;
}


void CIWSWork::Swap(int nFirst, int nSecond)
{
	if (m_pDoc->m_config.m_bUseHairpin)
		return;

	CJob* pFirst = NULL;
	CJob* pScecond = NULL;
	int nFirstGp;
	int nSecondGp;

	for (int i = 0; i < m_JobGroup.size(); i++)
	{
		if (!pFirst)
		{
			pFirst = m_JobGroup[i]->GetJob(nFirst);
			if (pFirst)
				nFirstGp = i;
		}

		if (!pScecond)
		{
			pScecond = m_JobGroup[i]->GetJob(nSecond);
			if (pScecond)
				nSecondGp = i;
		}

		if (pFirst && pScecond)
			break;
	}

	if (pFirst && pScecond)
	{
		m_JobGroup[nFirstGp]->m_jobs.at(nFirst) = pScecond;
		m_JobGroup[nSecondGp]->m_jobs.at(nSecond) = pFirst;
	}
}

double CIWSWork::GetAverageLaserPower()
{	
	// �̻�� �Լ� ���� ���� �ּ� ����
	return 0;
	int nJob =0;
	double dSum=0;
	CJobWeld* pWeld = NULL;	

	for (auto group : m_JobGroup)
	{
		_tracer(TRACE_NORMAL, 1, L"m_JobGroup Size = [%d]", m_JobGroup.size());
		for (auto job : group->m_jobs) 		
		{			
			pWeld = (CJobWeld*)job;
			dSum += pWeld->GetLaserPower();
			nJob++;
		}
	}

	if (nJob == 0)
		return 0.f;

	return dSum / double(nJob);
}

CJob* CIWSWork::GetJob(int idx)
{
	if (m_JobGroup.empty())
		return NULL;

	if (m_pDoc->m_config.m_bUseHairpin)
		// sjyi 2022-05-17 수정		return m_JobGroup[idx / MAX_NUM_GRP]->m_jobs[idx % MAX_NUM_GRP];
		//		return m_JobGroup[idx / nNumGrp]->m_jobs[idx % nNumGrp];
	{
		// sjyi 2022-05-18 재수정 -->
		// 그룹별로 헤어핀 용접 작업 개수 변경 
		// 기존 MAX_NUM_GRP(3개) 사용
		// 변경되면서, 3개 또는 4개(지그재그 패턴) 사용되어야 함.
		// 그룹별 작업 개수 3개인지, 4개인지 확인하기 위해, 아래 내용 추가
		
		int nPairLayerCnt = m_nDefLayer / 2;
		int* nJobSizeOfLaer = new int[nPairLayerCnt];
		int nJobCountOfSlot = 0;

		// 한 슬롯별 Job 개수 계산(모든 슬롯이 같은 작업개수를 가지므로)
//		for (int i = 0; i < m_nDefLayer; i++)
		for (int i = 0; i < nPairLayerCnt; i++)
		{
			nJobSizeOfLaer[i] = m_JobGroup[i]->m_jobs.size();
			nJobCountOfSlot += nJobSizeOfLaer[i];
		}

		// Slot Index 계산
		int nSlotIndex = idx / nJobCountOfSlot;
		
		// Laywer Index 계산
		int nTemp = idx % nJobCountOfSlot;
		int nTempBoundary = 0;
		int nLayerIndex = 0;
//		for (int i = 0; i < m_nDefLayer; i++)
		for (int i = 0; i < nPairLayerCnt; i++)
		{
			nTempBoundary += nJobSizeOfLaer[i];
			if (nTemp < nTempBoundary)
			{
				nLayerIndex = i;
				break;
			}
		}

		// Job Index 계산
		int nJobID = 0;
		if (nLayerIndex)
		{
			nJobID = nTemp - (nTempBoundary - nJobSizeOfLaer[nLayerIndex]);
		}
		else
		{
			nJobID = nTemp;
		}

		delete [] nJobSizeOfLaer;

		return m_JobGroup[nSlotIndex * nPairLayerCnt + nLayerIndex ]->m_jobs[nJobID];
		
		// sjyi 2022-05-18 재수정 <--
		// sjyi 2022-05-17 수정 <--
	}
	else
	{
		return m_JobGroup[idx]->m_jobs.front();

		return NULL;
	}
}

CJob* CIWSWork::GetJob(HANDLE entity)
{
	CJob* pCjob;
	for (auto group : m_JobGroup)
	{
		pCjob = group->GetJob(entity);
		if (pCjob)
			return pCjob;
	}

	return NULL;
}

int CIWSWork::GetJobIdx(HANDLE entity)
{
	if (m_pDoc->m_config.m_bUseHairpin)
		return -1;

	int nIdx;
	for (auto group : m_JobGroup)
	{
		nIdx = group->GetJobIdx(entity);
		if (nIdx != -1)
			return nIdx;
	}

	return -1;
}

int CIWSWork::DoWork()
{
	if (m_pWorkThread) 
		return -1;

	if (!m_pDlgWait) 
	{
		m_pDlgWait = new CDlgWaitWork;
		m_pDlgWait->Create(IDD_DIALOG_WAIT_WORK);
	}

	m_pWorkThread = AfxBeginThread(StartWorkThread, this);
	return 0;
}

void CIWSWork::Stop()
{
	if (m_pWorkThread != NULL)
	{
		m_bStopThread = TRUE;
		HANDLE hThread = m_pWorkThread->m_hThread;
		m_pWorkThread = NULL;
		DWORD dw = WaitForSingleObject(hThread, 1000);
		if (dw == WAIT_TIMEOUT) 
		{
			::GetExitCodeThread(hThread, &dw);
			::TerminateThread(hThread, dw);
		}
	}
	m_pDlgWait->ShowWindow(SW_HIDE);
}

UINT CIWSWork::StartWorkThread(LPVOID pParam)
{
	CIWSWork *pMe = (CIWSWork*)pParam;
	pMe->WorkThread();
	return 0;
}

void CIWSWork::WorkThread()
{
	m_pDlgWait->AssignStopFlag(&m_bStopThread,&(m_pScanner->m_bAbortExe));
	m_pDlgWait->SetMessage(L"Process Welding");
	m_pDlgWait->CenterWindow();
	m_pDlgWait->ShowWindow(SW_SHOW);	

	TRACE("=============Start work thread=============\n");
	m_bStopThread = FALSE;	
	m_pDoc->m_plc.SetWeldRun(TRUE);
		
	if (!m_bSimulation) 
	{
		//m_bSimulation = m_pDoc->m_plc.GetSimulation();
		//m_bSimulation = FALSE;
		
		//C-Ring Division blower 개별 제어를 하기 위해 용접패턴 추가 - 21.10.25 PJM
		//if (m_pDoc->m_work.m_hp.nWeldType == 7)
		//{
		//	//Inner Airknife 먼저 작동 후 내경 용접 완료 후,
		//	m_pDoc->m_work.m_bInnerWelding = TRUE;
		//	m_pDoc->m_plc.SetInnerAirknife(TRUE);
		//	DoJob();
		//	m_pDoc->m_plc.SetInnerAirknife(FALSE);
		//	m_pDoc->m_work.m_bInnerWelding = FALSE;

		//	Sleep(m_pDoc->m_config.m_nInOutAirSleep);

		//	//Ouuter Airknife 작동 후 외경 용접 진행
		//	m_pDoc->m_plc.SetOutterAirknife(TRUE);
		//	DoJob();
		//	m_pDoc->m_plc.SetOutterAirknife(FALSE);						
		//}
		//else
			DoJob();
	}
	else 
	{
		m_pScanner->LaserEnable(FALSE, 0);
		while (!m_bStopThread)
		{
			DoJob();
			Sleep(300);
		}
	}
		
	/*m_pScanner->Move(0, 0);
	m_pScanner->WaitMoving();*/
		
	TRACE("=============End work thread=============\n");

	// sjyi 2024.02.28 안전을 위해 Power 값 "0"으로 설정
	_tracer(TRACE_NORMAL, 1, _T("Set Laser Power Zero(0)"));
	m_pScanner->SetPower(0, TRUE);

	m_pWorkThread = NULL;
	m_pDlgWait->ShowWindow(SW_HIDE);
	::PostMessage(m_hRcv, UWM_CIWSWORK, EVENT_WORK_WELDDONE, 0);

	if (!m_pDoc->m_config.m_bUseHairpin)
		return;

	// 2024.02.05 jh.kim 추가 - 용접 후 좌표 저장 유무
	if (m_pDoc->m_config.m_bUseWeldPosSave == TRUE)
	{
		// hslee ���� �Ϸ� �� ���� ��ũ ����
		TCHAR path[_MAX_PATH];
		GetModuleFileName(NULL, path, sizeof path);

		CString strPath = path;
		int i = strPath.ReverseFind('\\');//���� ���� �̸��� ����� ���ؼ� ���ʿ� �ִ� '/'�� ã�´�.
		strPath = strPath.Left(i);//�ڿ� �ִ� ���� ���� ���� �̸��� �����.

		CString fileName;// = _T("Work\\defaults.wrk");
		fileName.Format(_T("%s\\Work\\defaults.wrk"), strPath);
		m_pDoc->SaveFile(fileName, TRUE, FALSE);

		CFileFind find;
		BOOL bRet = find.FindFile(m_pDoc->m_strLastFilePath);
		if (bRet)
			m_pDoc->SaveFile(m_pDoc->m_strLastFilePath, TRUE, FALSE);
		//CString str = GetFileTitle();
	}

	// WeldResultSave
	SaveWeldResult();
}

void CIWSWork::SaveWeldResult()
{
	if (!m_pDoc->m_config.m_bUseHairpin)
		return;

	TCHAR path[_MAX_PATH];
	GetModuleFileName(NULL, path, sizeof path);

	CString strStartUpPath = path;
	int i = strStartUpPath.ReverseFind('\\'); //실행 파일 이름을 지우기 위해서 왼쪽에 있는 '/'를 찾는다.
	strStartUpPath = strStartUpPath.Left(i); //뒤에 있는 현재 실행 파일 이름을 지운다.
	strStartUpPath.Format(_T("%s\\WeldResult"), strStartUpPath);

	// CreateFolder
	if (GetFileAttributes((LPCTSTR)strStartUpPath) == 0xFFFFFFFF)
	{
		CString csPrefix(_T("")), csToken(_T(""));
		int nStart = 0, nEnd;
		while ((nEnd = strStartUpPath.Find('\\', nStart)) >= 0)
		{
			CString csToken = strStartUpPath.Mid(nStart, nEnd - nStart);
			CreateDirectory(csPrefix + csToken, NULL);

			csPrefix += csToken;
			csPrefix += _T("\\");
			nStart = nEnd + 1;
		}
		csToken = strStartUpPath.Mid(nStart);
		CreateDirectory(csPrefix + csToken, NULL);
	}

	CString strPath;
	if (m_pDoc->GetUseInterface())
		strPath.Format(_T("%s\\Online.csv"), strStartUpPath);
	else
		strPath.Format(_T("%s\\Offline.csv"), strStartUpPath);

	CFile file;
	if (!file.Open(strPath, CFile::modeCreate | CFile::modeReadWrite))
	{
		_tracer(TRACE_ERROR, 1, _T("File Save Fail IWeldResult"));
		return;
	}

	CString msg;
	CLaserPen *pFirstPen, *pSecondPen, *pThirdPen;

	IWS_VECTOR_JOB_GROUP *pJobGroup = m_pDoc->m_work.GetJobs();
	for (auto group : (*pJobGroup))
	{
		if (group->m_jobs.size() < MIN_HAIRPIN_CRP) continue;
		
		CJobWeld* pFirstJob = (CJobWeld*)group->m_jobs[0];
		CJobWeld* pSecondJob = (CJobWeld*)group->m_jobs[1];
		CJobWeld* pThirdJob = (CJobWeld*)group->m_jobs[2];
		
		pFirstPen = &(*pFirstJob->m_pPens)[pFirstJob->m_iPenID];
		pSecondPen = &(*pSecondJob->m_pPens)[pSecondJob->m_iPenID];
		pThirdPen = &(*pThirdJob->m_pPens)[pThirdJob->m_iPenID];

		if(pFirstJob->m_iRepeat > 0 || pFirstJob->m_nZRepeat > 0)
			group->m_nWeldCnt++;
				
		msg.Format(L"%.2f,%.2f,%d,%.2f,%.2f,%d,%.2f,%.2f,%d,%d\n",
			pFirstPen->m_dblPower, pFirstPen->m_dblMarkSpeed, pFirstJob->m_iRepeat + pFirstJob->m_nZRepeat,
			pSecondPen->m_dblPower, pSecondPen->m_dblMarkSpeed, pSecondJob->m_iRepeat + pSecondJob->m_nZRepeat,
			pThirdPen->m_dblPower, pThirdPen->m_dblMarkSpeed, pThirdJob->m_iRepeat + pThirdJob->m_nZRepeat, group->m_nWeldCnt);

		file.Write(msg, msg.GetLength() * sizeof(TCHAR));
	}

	file.Close();
}

void CIWSWork::DoJobProcessing(BOOL bNormal)
{
	m_WeldIdx.clear();
	if (!m_pDoc->m_config.m_bUseHairpin || bNormal)
	{
		for (int i = 0; i < m_JobGroup.size(); i++)	// Normal
			m_WeldIdx.push_back(i);

		return;
	}

	// sjyi 2022-05-19 아래 내용 수정 여부 확인 필요
	// MAX_NUM_GRP 개수 수정에 따른 수정
	int nMaxNumGrp = 0;
	nMaxNumGrp = m_JobGroup[0]->m_jobs.size();
	if (nMaxNumGrp < 3) {
		_tracer(TRACE_NORMAL, 1000, L"MODE : Hairpin mode, WORK FILE : Not Hairpin file");
		return;
	}

	// sjyi imsi nMaxNumGrp = 3;
//	if (m_pDoc->m_work.m_hp.nWeldType == 0)		// Division (In -> Out)
//	{
////		int nJump = m_JobGroup.size() / MAX_NUM_GRP;
//		int nJump = m_JobGroup.size() / nMaxNumGrp;
//		int iPos;
//		int idx;
//		for (int i = 0; i < m_JobGroup.size(); i++)
//		{
//			//iPos = (i) % MAX_NUM_GRP;
//			//idx = int(i / MAX_NUM_GRP);
//			iPos = (i) % nMaxNumGrp;
//			idx = int(i / nMaxNumGrp);
//			m_WeldIdx.push_back(idx + iPos * nJump);
//		}
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 1)		//Division (Out -> In)
//	{
//		//int nJump = m_JobGroup.size() / MAX_NUM_GRP;
//		int nJump = m_JobGroup.size() / nMaxNumGrp;
//		int iPos;
//		int idx;
//
//		
//		for (int i = m_JobGroup.size() -1; i > -1; i--)
//		{
//			//iPos = (i) % MAX_NUM_GRP;			
//			//idx = int(i / MAX_NUM_GRP);
//			iPos = (i) % nMaxNumGrp;
//			idx = int(i / nMaxNumGrp);
//			m_WeldIdx.push_back(idx + iPos * nJump);
//		}		
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 2)  // Spiral (In->Out)
//	{
//		// 8 == Pin Count
//		int nJump = (m_nDefLayer / 2);
//		for (int i = 0; i < nJump; i++)
//			for (int j = 0; j < m_JobGroup.size(); j += nJump)
//				m_WeldIdx.push_back(i + j);
//
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 3)  // Spiral (Out->In)
//	{
//		// 마지막 레이어부터 (Last Layer Start) 4,3,2,1
//		int nJump = (m_nDefLayer / 2);
//		for (int i = nJump - 1; i > -1; i--)
//			for (int j = 0; j < m_JobGroup.size(); j += nJump)
//				m_WeldIdx.push_back(i + j);
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 4)  // Normal
//	{
//		for (int i = 0; i < m_JobGroup.size(); i++)
//			m_WeldIdx.push_back(i);
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 5)  // Spiral2
//	{
//		int nJump = 8;
//		for (int i = 0; i < nJump; i++)
//			for (int j = 0; j < m_JobGroup.size(); j += nJump)
//				m_WeldIdx.push_back(i + j);
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 6)  // CRing
//	{
//		if (m_pDoc->m_work.m_nModel != 1)
//		{
//			AfxMessageBox(_T("This is a CRing-only pattern."));
//			return;
//		}
//
//		int nJump = 8;
//		for (int i = 0; i < nJump; i++)
//		{
//			int nStart = 0;
//			switch (i)
//			{
//			case 0: nStart = 1; break;
//			case 1: nStart = 7; break;
//			case 2: nStart = 3; break;
//			case 3: nStart = 5; break;
//			case 4: nStart = 0; break;
//			case 5: nStart = 6; break;
//			case 6: nStart = 2; break;
//			case 7: nStart = 4; break;
//			}
//
//			for (int k = nStart; k < m_JobGroup.size(); k += nJump)
//			{
//				m_WeldIdx.push_back(k);
//			}
//		}
//	}
//	else if (m_pDoc->m_work.m_hp.nWeldType == 7)		//CRing(In -> Out)
//	{
//		if (m_pDoc->m_work.m_nModel != 1)
//		{
//			AfxMessageBox(_T("This is a CRing-only pattern."));
//			return;
//		}
//		//C-Ring 내, 외경 Airknife 개별 제어를 위해 용접을 2번 나누어 진행해야하지만
//		//WeldPath에서는 보여줘야하기 때문에 다음 아래와 같이 1번 용접하는 것처럼 Index를 매김
//		//(아래와 같이 index를 매기면 한번에 용접진행이 되므로 실제 용접 시 사용해서는 안됨) - 21.10.25 PJM		
//		if (m_pDoc->m_work.m_bWeldPath)
//		{
//			int nJump = 8;
//			for (int i = 0; i < nJump; i++)
//				for (int j = 0; j < m_JobGroup.size(); j += nJump)
//					if (i % 2 == 0)
//						m_WeldIdx.push_back(i + j);
//
//			for (int i = 0; i < nJump; i++)
//				for (int k = 0; k < m_JobGroup.size(); k += nJump)
//					if (i % 2 != 0)
//						m_WeldIdx.push_back(i + k);
//		}
//		else
//		{
//			//실제 용접시에만 사용
//			int nJump = 8;
//			//if (m_pDoc->m_work.m_bInnerWelding)
//			//{
//				//내경 용접				
//				for (int i = 0; i < nJump; i++)
//					for (int j = 0; j < m_JobGroup.size(); j += nJump) {
//						if (i % 2 == 0)
//							m_WeldIdx.push_back(i + j);
//					}
//			//}
//			//else
//			//{
//				//외경 용접 진행
//				for (int i = 0; i < nJump; i++)
//					for (int k = 0; k < m_JobGroup.size(); k += nJump) {
//						if (i % 2 != 0)
//							m_WeldIdx.push_back(i + k);
//					}
//			//}
//		}
//	}
//	else
//	{
//		for (int i = 0; i < m_JobGroup.size(); i++)
//			m_WeldIdx.push_back(i);
//	}
	
	// sjyi 2023.06.15 Division 관련 알고리즘 수정 -->
	int nJump;
	int iPos;
	int idx;

	int nStartOffset = 0;
	int nWeldID;

	nStartOffset = (m_pDoc->m_work.m_hp.nDivStartSlot - 1) * m_pDoc->m_work.m_hp.nLayer / 2;
	switch (m_pDoc->m_work.m_hp.nWeldType)
	{
	case IWS_JOB_DIV_INOUT:
		{
			nJump = m_pDoc->m_work.m_hp.nDivJumpOffset * m_pDoc->m_work.m_hp.nLayer / 2;

			for (int i = 0; i < nJump; i++)
			{
				for (int j = 0; j < m_JobGroup.size(); j += nJump)
				{
					nWeldID = i + j;

					if (nWeldID >= m_JobGroup.size())
					{
						continue;
					}

					m_WeldIdx.push_back(nWeldID);
				}
			}
		}
		break;
	case IWS_JOB_DIV_OUTIN:
		{
			nJump = m_pDoc->m_work.m_hp.nDivJumpOffset * m_pDoc->m_work.m_hp.nLayer / 2;
			for (int i = nJump - 1; i > -1; i--)
			{
				for (int j = 0; j < m_JobGroup.size(); j += nJump)
				{
					nWeldID = i + j;

					if (nWeldID >= m_JobGroup.size())
					{
						continue;
					}

					m_WeldIdx.push_back(nWeldID);
				}
			}
		}
		break;
	case IWS_JOB_NORMAL:
		{
			nJump = m_pDoc->m_work.m_hp.nDivJumpOffset * m_pDoc->m_work.m_hp.nLayer / 2;
			for (int k = 0; k < m_pDoc->m_work.m_hp.nDivJumpOffset ; k++)
			{
				int nSlotOffset = m_pDoc->m_work.m_hp.nLayer / 2 * k;
				
				for (int j = nSlotOffset; j < m_JobGroup.size(); j += nJump)
				{
					for (int i = 0; i < m_pDoc->m_work.m_hp.nLayer / 2; i++)
					{
						nWeldID = j + i;

						if (nWeldID >= m_JobGroup.size())
						{
							continue;
						}

						m_WeldIdx.push_back(nWeldID);
					}
				}

			}
		}
		break;

	case IWS_JOB_SPIRAL_INOUT:
		{
			for (int nLayer = 0; nLayer < m_pDoc->m_work.m_hp.nLayer / 2; nLayer++)
			{
				nJump = m_pDoc->m_work.m_hp.nDivJumpOffset;

				for (int i = 0; i < nJump; i++)
				{
					for (int nSlot = i; nSlot < m_pDoc->m_work.m_hp.nSlot; nSlot += nJump)
					{
						nWeldID = nSlot * m_pDoc->m_work.m_hp.nLayer / 2 + nLayer;

						if (nWeldID >= m_JobGroup.size())
						{
							break;
						}

						m_WeldIdx.push_back(nWeldID);
					}
				}
			}
		}
		break;
	case IWS_JOB_SPIRAL_OUTIN:
		{
			for (int nLayer = m_pDoc->m_work.m_hp.nLayer / 2 - 1; nLayer >= 0; nLayer--)
			{
				nJump = m_pDoc->m_work.m_hp.nDivJumpOffset;

				for (int i = 0; i < nJump; i++)
				{
					for (int nSlot = i; nSlot < m_pDoc->m_work.m_hp.nSlot; nSlot += nJump)
					{
						nWeldID = nSlot * m_pDoc->m_work.m_hp.nLayer / 2 + nLayer;

						if (nWeldID >= m_JobGroup.size())
						{
							break;
						}

						m_WeldIdx.push_back(nWeldID);
					}
				}
			}
		}
		break;
	}

	// Shift to Start Offset
	for(int i=0; i<m_WeldIdx.size(); i++)
	{
		nWeldID = m_WeldIdx[i] + nStartOffset;
		if (nWeldID >= m_JobGroup.size())
		{
			nWeldID -= m_JobGroup.size();
		}

		m_WeldIdx[i] = nWeldID;
	}
	// sjyi 2023.06.15 Division 관련 알고리즘 수정 <--

}

void CIWSWork::DoJob()
{
	_tracer(TRACE_NORMAL, 1, L"Job start");
	DoJobProcessing();
	
	m_dwStart = timeGetTime();	

	BOOL bSimulation = TRUE;
	if (m_bSimulation) // 무한반복 시뮬모드
	{
		bSimulation = TRUE;
	}
	else
	{
		bSimulation = m_pDoc->m_plc.GetSimulation();
		if (bSimulation)
			_tracer(TRACE_NORMAL, 1, L"WORKMODE - Simulation Mode");
		else
			_tracer(TRACE_NORMAL, 1, L"WORKMODE - Weld Mode");
	}

	for (auto i : m_WeldIdx)
	{
		m_JobGroup[i]->DoJob(m_pDoc, bSimulation);
		if (m_bStopThread) 
			break;
	}

	m_dwEnd = timeGetTime();
	m_dwTactTime = m_dwEnd - m_dwStart;
	_tracer(TRACE_NORMAL, 1, L"Job end , tact time = %.3f sec", GetTactTime()/1000.0);

}

//void CIWSWork::DoJobOnce()
//{
//	// �Ϲݿ����� ��� ���� �� �����̷��� ���� �ʴ� �Ϲ��� ������ ����
//	_tracer(TRACE_NORMAL, 1, L"DoJobOnce start");
//	DoJobProcessing(TRUE);
//
//	m_dwStart = timeGetTime();
//	CLaserPen *pPen = NULL;
//	CJobWeld* pJob = NULL;
//	m_pScanner->WaitMoving();
//	m_pScanner->StartList();
//	unsigned long int nBuffCnt = 0;
//
//	for (auto i : m_WeldIdx) 
//	{
//		m_JobGroup[i]->DoJobOnce(m_pDoc, m_bSimulation, m_pScanner, &nBuffCnt);
//		if (m_bStopThread)
//			break;
//	}
//
//	m_pScanner->EndList();
//	m_pScanner->Execute();
//	m_pScanner->WaitMoving();
//	m_dwEnd = timeGetTime();
//	m_dwTactTime = m_dwEnd - m_dwStart;
//	_tracer(TRACE_NORMAL, 1, L"DoJobOnce end , InBuffCnt=%ld, tact time = %.3f sec", nBuffCnt, GetTactTime() / 1000.0);
//}


// sjyi 2025.09.29 Jason 사용을 위한 함수 추가 --->
BOOL CIWSWork::SaveWorkJson(CFile *pf, CIWSDoc *pDoc)
{
	CRapidJasonWork jsonWork;
	return jsonWork.SaveWork(pf, pDoc);
}

BOOL CIWSWork::OpenWorkJson(CString strFilePath, CIWSDoc *pDoc)
{
	CRapidJasonWork jsonWork;
	return jsonWork.LoadWork(strFilePath, pDoc);
}
// sjyi 2025.09.29 Jason 사용을 위한 함수 추가 <---
