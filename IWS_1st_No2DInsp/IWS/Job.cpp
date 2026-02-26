#include "stdafx.h"
#include "Job.h"


CJob::CJob()
{
	m_bDoJob = TRUE;
	m_bSelected = TRUE;
	
	m_ptrnType = PATTERN_TYPE_NONE;	
}

CJob::~CJob()
{
}
