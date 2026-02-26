#pragma once
//#include <vector>
//#include "IWSDoc.h"

//#define MAX_NUM_GRP 3
#define MAX_NUM_GRP 4	// sjyi 2022-05-17 2nd 패턴 변경을 위해, 잡 최대 개수 4개로 수정

#define MIN_HAIRPIN_CRP	3
#define ZIGZAG2_PINS	4

class CJob;
class CJobWeld;
class CScanner;
class CIWSDoc;

class CJobGroup
{
public:
	CJobGroup();
	~CJobGroup();

	void SetIndex(int nIdx);
	void SetIndex(int nIdx, int nSlot, int nLayer);

	void Clear();

	CJob* GetSelectEntity(HANDLE entity, int *idx=NULL);
	CJob* GetNextEntity(HANDLE entity, BOOL *isLast);
	CJob* GetPrevEntity(HANDLE entity, BOOL *isFirst);
	CJob* GetJob(HANDLE entity);
	int	  GetJobIdx(HANDLE entity);

	BOOL DeleteJob(HANDLE entity);
	BOOL ChangeJob(HANDLE entity);

	//void DoJob(CIWSDoc *pDoc, BOOL bSimulation);
	void DoJob(CIWSDoc *pDoc, BOOL bSimulation, BOOL bIndividualExe = TRUE);
	//void DoJobOnce(CIWSDoc *pDoc, BOOL bSimulation, CScanner *pScanner, unsigned long int* nBuffCnt);

	void ProcZComp(CIWSDoc *pDoc, BOOL bSimulation, BOOL bIndividualExe = TRUE);
	
	int AddJob(CJob *job) {
		if (!job) 
			return (int)m_jobs.size();
		m_jobs.push_back(job);
		return (int)m_jobs.size();
	}

	CJob* GetJob(int idx) {
		if (m_jobs.empty())
			return NULL;

		if (idx = m_nIndex)
			return m_jobs[0];

		return NULL;
	};

	void SetJobType();
	
private:
	//void DoJobWeld(CIWSDoc *pDoc, CJobWeld* pWeldJob, BOOL bSimulation, int nRepeat, double *pRptProfile = NULL);	
	BOOL DoJobWeld(CIWSDoc *pDoc, CJobWeld* pWeldJob, BOOL bSimulation, int nRepeat, double *pRptProfile = NULL, BOOL bContinuousRpt = FALSE, int nSortieDir = 0, BOOL bExecute = TRUE);
	void DoJobOnceWeld(CJobWeld* pWeldJob, CIWSDoc *pDoc, BOOL bSimulation, CScanner *pScanner,
		unsigned long int* nBuffCnt, int nRepeat);
	
public:
	std::vector<CJob*>    m_jobs;
	int m_nIndex;
	int m_nSlot; // 0 Base
	int m_nLayer; // 1 Base

	//21.05.29 MES 최종 보고 시 각 Slot Layer에 대한 용접 횟수
	int m_nWeldCnt;
	double m_d3H, m_dC, m_dJump;
};