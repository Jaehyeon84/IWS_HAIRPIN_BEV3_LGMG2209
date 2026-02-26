#pragma once
#include "Entity.h"

#define STR_JOBTYPE_WELD _T("JobWeld")
#define STR_JOBTYPE_SWDELAY _T("JobSWdelay")
#define STR_JOBTYPE_DIO	_T("JobTypeDIO") 

typedef enum _JobType {
	JOB_TYPE_WELD = 0,
	JOB_TYPE_SWDELAY,
	JOB_TYPE_DIO,
	JOB_TYPE_END
}JobType;

typedef enum _JobReturn {
	JOB_OK = 0,
	JOB_ERR,
	JOB_NG,
}JobReturn;

class CJob
{
public:
	CJob();
	virtual ~CJob();
	
	virtual BOOL Save(CFile *pf) = 0;
	virtual BOOL Load(CFile *pf) = 0;
	virtual int DoJob(CIWSDoc *pDoc, BOOL bSimulation, int nRepeat, double *pRtpProfile = NULL) = 0;
	virtual int GetJobType() = 0;
	virtual CString GetJobTypeName() = 0;
	virtual void DisplayProperties(CMFCPropertyGridCtrl *pProp) = 0;

	virtual void SetEntity(CEntity *pEnt) = 0;
	virtual CString GetJobName() = 0;
	virtual CEntity *GetEntity() = 0;
	virtual void SetDoJob(BOOL bDoJob) = 0;
	BOOL GetDoJob() { return m_bDoJob & m_bSelected; };	
	BOOL IsDoJob() { return m_bDoJob; };
	BOOL m_bSelected;
	
	int m_ptrnType;	

	/*double m_d1StPowerRamp[WELD_REPEAT_MAX];
	double m_d2ndPowerRamp[WELD_REPEAT_MAX];*/

protected:
	BOOL m_bDoJob;
	int m_iJobType;
};

