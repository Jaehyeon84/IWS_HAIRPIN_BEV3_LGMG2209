#pragma once
#include "CJobGroup.h"

#define UWM_CIWSWORK_MSG _T("CIWSWork")
#define EVENT_WORK_CURRENT 100
#define EVENT_WORK_VSCAN   200
#define EVENT_WORK_INSPECT 300
#define EVENT_WORK_WELDDONE	   400

class CIWSDoc;
class CDlgWaitWork;

class CIWSWork
{
public:
	CIWSWork();
	~CIWSWork();
		
	IWS_VECTOR_JOB_GROUP m_JobGroup;
	
	CScanner *m_pScanner;
	CLaserPen *m_pPenMove;
	vector_int m_resInspect;

	vector_int m_WeldIdx;
	BOOL m_bSimulation;
	BOOL m_bStopThread;
	//BOOL m_bWeldOnce;

	int  m_nDefLayer;

	//C-Ring(Division blower) 내,외경 Airknife 용접 사용 시 Weldpath에서 보여주기 위해서만 사용
	BOOL m_bWeldPath;

	//CRing Division Blower 용접 시 내경 먼저 용접 할 것인지에 대한 flag
	BOOL m_bInnerWelding;

public:	
	DWORD GetTactTime();
	BOOL GetInspectjudgement();
	//void SetWeldOnce(BOOL bFlag) { m_bWeldOnce = bFlag; };
	void SetMovePen(CLaserPen *pPen) { m_pPenMove = pPen; };
	void GotoSelectedScannerPos();
	void SetNextJob();
	void SetPrevJob();
	void SetScanner(CScanner *pScan) { m_pScanner = pScan; }
	void SetReceiveHandle(HWND hWnd) { m_hRcv = hWnd; };
	void SetDocument(CIWSDoc *pDoc) { m_pDoc = pDoc; };
	void virtual clear();
	void virtual SaveWork(CFile *pf, CIWSDoc *pDoc);
	BOOL virtual LoadWork(CFile *pf, CIWSDoc *pDoc);

	IWS_VECTOR_JOB_GROUP*   GetJobs() { return &m_JobGroup; };

	int AddJob(CJob *job)
	{
		if (!job)
			return FALSE;

		CJobGroup* jobGroup = new CJobGroup();
		jobGroup->m_jobs.push_back(job);
		jobGroup->SetIndex((int)m_JobGroup.size() + 1);

		m_JobGroup.push_back(jobGroup);
		return (int)jobGroup->m_jobs.size();
	}

	int AddJob(CJob *inner, CJob *outer, CJob *center, int nIndex, int nSlot, int nLayer)
	{
		if (!inner || !outer || !center)
			return (int)m_JobGroup.size();

		CJobGroup* jobGroup = new CJobGroup();
		jobGroup->m_jobs.push_back(inner);
		jobGroup->m_jobs.push_back(outer);
		jobGroup->m_jobs.push_back(center);
		jobGroup->SetIndex(nIndex, nSlot, nLayer);
		m_JobGroup.push_back(jobGroup);

		jobGroup->SetJobType();

		return (int)m_JobGroup.size();
	}
// sjyi 2022-05-18 Add 2nd 패턴 변경에 따라 작업 그룹별 작업개수 변경되어 4개 입력 가능한 함수 신규 추가
	int AddJob(CJob *inner, CJob *outer, CJob *inner2nd, CJob* outer2nd, int nIndex, int nSlot, int nLayer)
	{
		if (!inner || !outer || !inner2nd || !outer2nd)
			return (int)m_JobGroup.size();

		CJobGroup* jobGroup = new CJobGroup();
		jobGroup->m_jobs.push_back(inner);
		jobGroup->m_jobs.push_back(outer);
		jobGroup->m_jobs.push_back(inner2nd);
		jobGroup->m_jobs.push_back(outer2nd);
		jobGroup->SetIndex(nIndex, nSlot, nLayer);
		m_JobGroup.push_back(jobGroup);

		jobGroup->SetJobType();

		return (int)m_JobGroup.size();
	}

	void DeleteJob(HANDLE entity);
	int  ChangeJob(HANDLE entity);
	void Swap(int nFirst, int nSecond);
	double GetAverageLaserPower();

	CJob* GetLastJob() 
	{ 
		return m_JobGroup.back()->m_jobs.back();
	}

	CJob* GetJob(int idx);
	CJob* GetJob(HANDLE entity);
	int	  GetJobIdx(HANDLE entity);

	inline int   GetNumberOfJobs() 
	{ 
		int nSize = 0;
		for (auto group : m_JobGroup)
			nSize += (int)group->m_jobs.size();

		return nSize;
	};
	/*IWS_VECTOR_JOB*   GetJobs() { return &m_jobs; };
	IWS_VECTOR_JOB_GROUP* GetHpJobs() { return &m_JobGroup; };*/

	int virtual DoWork();
	void Stop();

	static UINT StartWorkThread(LPVOID pParam);
	void WorkThread();
	void DoJobProcessing(BOOL bNormal = FALSE);
	void DoJob();
	//void DoJobOnce();
	BOOL virtual IsWorkOn() {
		return (m_pWorkThread ? TRUE : FALSE);
	};

	void SaveWeldResult();

	// 2025.09.29 Jason 사용을 위한 함수 추가 --->
	BOOL OpenWorkJson(CString strFilePath, CIWSDoc *pDoc);
	BOOL SaveWorkJson(CFile *pf, CIWSDoc *pDoc);
	// 2025.09.29 Jason 사용을 위한 함수 추가 <---

protected:
	CIWSDoc *m_pDoc;
	DWORD m_dwStart, m_dwEnd;
	DWORD m_dwTactTime;
	CDlgWaitWork *m_pDlgWait;
	CWinThread* m_pWorkThread;
	HWND m_hRcv;
};

