#pragma once
#include "Entity.h"
#include "Scanner.h"
#include "Job.h"

class CLaserPen;

typedef std::vector<IWS_HAIRPIN_REPEAT_INFO> vector_RepeatPowerInfo;	// 2022-06-03 개별파워 조정을 위한 변수 추가

class CJobWeld : public CJob
{
public:
	CJobWeld();
	~CJobWeld();

	int m_iFileVer;
	double GetLaserPower();
	virtual BOOL Save(CFile *pf);
	virtual BOOL Load(CFile *pf);	

	// sjyi 2024.05.26 Sortie 반복 추가
	//virtual int DoJob(CIWSDoc *pDoc, BOOL bSimulation, int nRepeat, double *pRptProfile = NULL);
	virtual int DoJob(CIWSDoc *pDoc, BOOL bSimulation, int nRepeat = -1, double *pRptProfile = NULL, BOOL bContinuousRtp = FALSE, int nSortieDir = SORTIE_ONEWAY);

	void		DoJobPowerShaping(CLaserPen *pPen, int nRepeat, double *pRptProfile = NULL);
	void		DoJobPulseShapeing(CLaserPen *pPen, int nRepeat, double *pRptProfile = NULL);
	void		MakePathPowerShape(CLaserPen *pPen);
	BOOL		MakePathPulseShape(CLaserPen *pPen);
	void		PathSubDivide(Point3Dbl p0, Point3Dbl p1, double division);
	virtual int GetJobType() { return JobType::JOB_TYPE_WELD; };
	virtual CString GetJobTypeName() { return CString(STR_JOBTYPE_WELD); };

	virtual CString GetJobName() { return m_entity->m_strName; }
	virtual void SetEntity(CEntity *pEnt) {
		m_entity = pEnt; 
		m_entity->SetPen(&m_iPenID);
	};
	virtual CEntity *GetEntity() { return m_entity; };
	virtual void DisplayProperties(CMFCPropertyGridCtrl *pProp);

	virtual void SetDoJob(BOOL bDoJob)
	{
		m_bDoJob = bDoJob;
		if (bDoJob)
		{
			if (m_nOriPenID != 15)
				m_iPenID = m_nOriPenID;
		}
		else
		{
			if (m_iPenID != 15)
				m_nOriPenID = m_iPenID;

			m_iPenID = 15;

		}
	}

	void SetOriPen()
	{
		if (m_iPenID != 15)
			m_nOriPenID = m_iPenID;

		//m_nOriPenID = m_iPenID;
	}

	// sjyi 2022-06-03 Individual Power 에 대한 Read /Write 추가 -->
	BOOL SaveIndividualPower(CFile *pf);
	BOOL LoadIndividualPower(CFile *pf);
	// sjyi 2022-06-03 Individual Power 에 대한 Read /Write 추가 <--

	// sjyi 2022-06-03 Individual Power에 대한 파라미터 관련 함수 추가 -->
	BOOL GetUseIndividualPower();
	void SetUseIndividualPower(BOOL bUse);

	void SetIndividualPower(int nRepeatIndex, double dPower);
	int AddIndividualPower(double dPower);
	void ClearIndividualPower();
	double GetIndividualPower(int nRepeatID);
	int GetIndividualPowerCnt();
	// sjyi 2022-06-03 Individual Power에 대한 파라미터 관련 함수 추가 <--

	// sjyi 2022-06-07 프로퍼티 윈도우에서 리피트 횟수 수정에 따른 함수 추가 -->
	void AddIndividualPowerAsLast();
	void RevomeIndividualPowerLast();
	// sjyi 2022-06-07 프로퍼티 윈도우에서 리피트 횟수 수정에 따른 함수 추가 <--

	void SetRotation(double dRotation); // sjyi 2023.04.15 헤어핀 회전 파라미터 추가
	double GetRotation();				// sjyi 2023.04.15 헤어핀 회전 파라미터 추가

	// 2025.09.29 Jason 사용을 위한 함수 추가 --->
	void SetOriPen(int nID) { m_nOriPenID = nID; }
	int GetOriPen() { return m_nOriPenID; };

	void SetPens(IWS_LPEN* pPens) { m_pPens = pPens; };
	void SetPenID(int nID, BOOL bOrigin = FALSE) { m_iPenID = nID; if (bOrigin) { m_nOriPenID = nID; } };
	int GetPenID() { return m_iPenID; };
	int* GetPenIDPtr() { return &m_iPenID; };

	IWS_LPEN* GetPens() { return m_pPens; };

	// 2025.09.29 Jason 사용을 위한 함수 추가 <---

public:
	CEntity *m_entity;
	CScanner *m_pScan;
	IWS_LPEN *m_pPens;

	int m_iPenID;
	int m_iRepeat;
	int m_nZRepeat;
	BOOL m_bSimulation;
	BOOL m_bRework;
	
	double m_d1StLength;
	double m_d1StWidth;
	double m_d2NdWidth;
	double m_d2NdOver;
	double m_d2NdWidth2;	// sjyi 2022-08-29 모래시계 패턴을 위한 변수 추가

	double m_dX, m_dY, m_dZ;

	int m_nShape1st;		// sjyi 2022-09-15 1차 용접 패턴을 알기 위한 변수
	int m_nShape2nd;		// sjyi 2022-09-15 2차 용접 패턴을 알기 위한 변수

	int m_nOriPenID;
	
protected:
	void	DoHatch(int nRepeat);
	CString m_strJobTypeName;
	vector_point3dbl m_PathPwrShape;
	std::vector<double> m_PulseShape;

	// sjyi 2022-06-02 개별 리피트 횟수별 파워 관련 변수 추가 -->
	BOOL m_bUseIndividualRptPower;	// 개별 파워 사용 여부
	vector_RepeatPowerInfo m_RptPower;	// 개별 파워 파라미터
	// sjyi 2022-06-02 개별 리피트 횟수별 파워 관련 변수 추가 <--

	double m_dRotation;	// sjyi 2023.04.15 해어핀 회전 추가
};