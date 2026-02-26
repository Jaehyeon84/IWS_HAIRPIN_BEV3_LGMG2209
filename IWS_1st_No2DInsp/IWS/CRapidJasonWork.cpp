#include "stdafx.h"
#include "CRapidJasonWork.h"
#include "IWS.h"
#include "IWSDoc.h"
#include "CJobGroup.h"
#include "Job.h"
#include "JobWeld.h"

// 2025.09.29 Jason 사용을 위한 Define 추가 --->
// Jason INFOS ---------------
#define JASON_HEADER_INFOS			"[HEADER_INFO]"

#define JASON_HEADER_HAIRPIN		"[HAIRPIN_INFO]"
#define JASON_HEADER_ZCOMP			"[ZCOMPWELD_INFO]"
#define JASON_HEADER_ETC			"[ETC_INFO]"

#define JASON_PEN_INFOS				"[PEN_INFOS]"
#define JASON_PEN_INFO				"[PEN_%d_INFO]"

#define JASON_WORK_INFOS			"[WORK_INFOS]"
#define JASON_WORK_HEAD_INFO		"[WORK_HEAD_INFO]"
#define JASON_WORK_GROUP_INFO		"[GROUP_%d_INFO]"
#define JASON_WORK_JOB_INFO			"[JOB_%d_INFO]"
#define JASON_WORK_ENT_INFO			"[ENTITY_INFO]"

#define MAX_JASON_MEMBERCNT   3 // 2024.05.16 ksu 멤버카운트 예외처리,코드 추가, 현재 멤버 카운트는 2개(PEN_INFO, WORK_INFO)
// 2025.09.29 Jason 사용을 위한 Define 추가 <---

CRapidJasonWork::CRapidJasonWork()
{
}

CRapidJasonWork::~CRapidJasonWork()
{

}

int CRapidJasonWork::GetJsInt(char* sec, RJsonValue& Val, int nDef) 
{
	if (!Val.HasMember(sec))
	{
		_tracer(TRACE_NORMAL, 9998, L"%s is Not exist.", CString(sec));
		return nDef;
	}
	int resInt = Val[sec].GetInt();
	return resInt;
}

int CRapidJasonWork::GetJsInt(int idx, RJsonValue & Val, int nDef)
{
	if (!Val[idx].IsInt())
	{
		_tracer(TRACE_NORMAL, 9998, L"%d is Not exist.", idx);
		return nDef;
	}
	int resInt = Val[idx].GetInt();
	return resInt;
}

double CRapidJasonWork::GetJsDouble(char* sec, RJsonValue& Val, double dblDef) 
{
	if (!Val.HasMember(sec))
	{
		_tracer(TRACE_NORMAL, 9998, L"%s is Not exist.", CString(sec));
		return dblDef;
	}
	double resDbl = Val[sec].GetDouble();
	return resDbl;
}

double CRapidJasonWork::GetJsDouble(int idx, RJsonValue& Val, double dblDef) 
{
	if (!Val[idx].IsDouble())
	{
		_tracer(TRACE_NORMAL, 9998, L"%s is Not exist.", CString(Val.GetString()));
		return dblDef;
	}
	double resDbl = Val[idx].GetDouble();
	return resDbl;
}



CStringA CRapidJasonWork::GetJsString(char* sec, RJsonValue& Val, CStringA strDef) 
{
	if (!Val.HasMember(sec))
	{
		_tracer(TRACE_NORMAL, 9998, L"%s is Not exist.", CString(sec));
		return strDef;
	}
	CStringA resStrA = Val[sec].GetString();
	return resStrA.GetString();
}

BOOL CRapidJasonWork::GetJsBool(char* sec, RJsonValue& Val, BOOL bDef) 
{
	if (!Val.HasMember(sec))
	{
		_tracer(TRACE_NORMAL, 9998, L"%s is Not exist.", CString(sec));
		return bDef;
	}
	BOOL resBool = Val[sec].GetBool();
	return resBool;
}

void CRapidJasonWork::MakeJasonPen(RJsonWriter& writer, CLaserPen* pPen, int nPenNo)
{
	CStringA strPenKey;
	strPenKey.Format(JASON_PEN_INFO, nPenNo);

	writer.Key(strPenKey);
	writer.StartObject(); //"{" Pen 오브젝트 시작

	writer.Key("m_iPenID");        // penID (0~14) [15개까지 있으므로 0부터 14번까지 있음.]
	writer.Int(pPen->m_iPenID);

	writer.Key("m_dblPower");      // Pen Power
	writer.Double(pPen->m_dblPower);

	writer.Key("m_dblFrequency");  // Pen Frequency
	writer.Double(pPen->m_dblFrequency);

	writer.Key("m_dblPulseWidth"); // Pen Pulse Width
	writer.Double(pPen->m_dblPulseWidth);

	writer.Key("m_dblMarkSpeed");  // Mark Speed
	writer.Double(pPen->m_dblMarkSpeed);

	writer.Key("m_dblJumpSpeed");  // Jump Speed
	writer.Double(pPen->m_dblJumpSpeed);

	writer.Key("m_nMarkDelay");    // Mark Delay
	writer.Double(pPen->m_nMarkDelay);

	writer.Key("m_nJumpDelay");    // Jump Delay
	writer.Double(pPen->m_nJumpDelay);

	writer.Key("m_nPolygonDelay"); // PolyGonDelay(실제 사용은 못봤음.)
	writer.Int(pPen->m_nPolygonDelay);

	writer.Key("m_nLaserOnDelay"); // LaserOnDelay
	writer.Int(pPen->m_nLaserOnDelay);

	writer.Key("m_nLaserOffDelay");// LaserOffDelay
	writer.Int(pPen->m_nLaserOffDelay);

	writer.Key("m_bUseWobbel");    // 와블 사용 미사용
	writer.Int(pPen->m_bUseWobbel);

	//writer.Key("m_wbType");		// 와블 타입
	//writer.Int(pPen->m_wbType);	

	// Snow man, sjyi 2025.09.29, 현재는 생략, 나중에 추가 해야 함.

	writer.Key("m_dWbFreq");       // 와블 주파수(반복도)
	writer.Double(pPen->m_dWbFreq);

	writer.Key("m_dWbLongitudinal"); // 와블 높이
	writer.Double(pPen->m_dWbLongitudinal);

	writer.Key("m_dWbTransbersal");  // 와블 너비
	writer.Double(pPen->m_dWbTransbersal);

	writer.Key("m_bUsePowerShape");  // 파워 셰이프 사용 미사용
	writer.Int(pPen->m_bUsePowerShape);

	if (pPen->m_bUsePowerShape)
	{
		writer.Key("m_pwrShape");// 파워 셰입 적용된 좌표
		writer.StartArray();        // "["파워 셰입 시작

		for (auto p : pPen->m_pwrShape)
		{
			writer.StartArray();    // "[" N번째 요소 시작
			writer.Double(p.x);     // 파워 셰입 적용된 좌표
			writer.Double(p.y);     // 파워 셰입 적용된 좌표
			writer.EndArray();      // "]" N번째 요소 끝
		}
		writer.EndArray();          // "]"파워 셰입 끝
	}


	writer.Key("m_bUsePulseShape"); // 펄스 셰입 사용 미사용
	writer.Int(pPen->m_bUsePulseShape);


	if (pPen->m_bUsePulseShape)
	{
		writer.Key("m_pulseShape");      // 펄스 셰입 적용된 좌표
		writer.StartArray();        // "["펄스 셰입 시작
		for (auto p : pPen->m_pulseShape)
		{

			writer.StartArray();   // "{" N번째 요소 시작
			writer.Double(p.x);     // 파워 셰입 적용된 좌표
			writer.Double(p.y);     // 파워 셰입 적용된 좌표
			writer.EndArray();     // "}" N번째 요소 끝
		}
		writer.EndArray();          // "]"펄스 셰입 끝
	}

	writer.Key("m_iPWS");     // pulse wave table     - spi레이져, Trupulse
	writer.Int(pPen->m_iPWS);

	//writer.Key("m_iProcNum"); // Laser program number - Trumpf레이저 
	//writer.Int(pPen->m_iProcNum);

	//writer.Key("m_dEngTransferRefSpd_mm");      // 에너지 변환 기준, 스피드 mm: 길이,속도에 대한 에너지 변환 값
	//writer.Double(pPen->m_dEngTransferRefSpd_mm);

	//writer.Key("m_dEngTransferRefPow_percent"); // 에너지 변환 기준, 파워(퍼센트,비율): 파워에 대한 에너지 변환 값
	//writer.Double(pPen->m_dEngTransferRefPow_percent);

	writer.EndObject();   //"}" Pen 오브젝트 끝
}


void CRapidJasonWork::MakeJasonEntityHatch(RJsonWriter& writer, CEntity* pEntity)
{
	CEntityHatch* pEnt = (CEntityHatch*)pEntity;

	CStringA strName(pEnt->m_strName);
	writer.Key("strName");
	writer.String(strName);

	writer.Key("ptrnScale");
	writer.Double(pEnt->ptrnScale);

	writer.Key("ptrnAngle");
	writer.Double(pEnt->ptrnAngle);

	writer.Key("ptrnIdx");
	writer.Int(pEnt->ptrnIdx);

	int nPath = (int)pEnt->hatchLoops.size();
	writer.Key("nPath");
	writer.Int(nPath);

	int nLoop = 0;
	writer.Key("HatchLoopData");
	writer.StartArray();//"[" HatchLoop 배열 시작
	for (auto p : pEnt->hatchLoops)
	{
		writer.StartObject();//"{" HatchLoop object 시작
		nLoop = (int)p.size();
		writer.Key("nLoop");
		writer.Int(nLoop);

		writer.Key("p");
		writer.StartArray();//"[" p 배열 시작
		for (auto pp : p)
		{
			writer.StartArray();//"[" pp 배열 시작
			writer.Double(pp.x);
			writer.Double(pp.y);
			writer.Double(pp.z);
			writer.EndArray();//"]" pp 배열 끝
		}
		writer.EndArray();//"]" p 배열 끝
		writer.EndObject();//"}" HatchLoop object 끝
	}
	writer.EndArray();//"]" HatchLoop 배열 끝

	writer.Key("biDir");
	writer.Bool(pEnt->biDir);
}

void CRapidJasonWork::MakeJasonEntityRect(RJsonWriter& writer, CEntity* pEntity)
{
	CEntityRect* pEnt = (CEntityRect*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("center");
	writer.StartArray();
	writer.Double(pEnt->center.x);
	writer.Double(pEnt->center.y);
	writer.EndArray();

	writer.Key("width");
	writer.Double(pEnt->width);

	writer.Key("height");
	writer.Double(pEnt->height);

	writer.Key("angle");
	writer.Double(pEnt->angle);

	writer.Key("radius");
	writer.Double(pEnt->radius);

	writer.Key("iDir");
	writer.Bool(pEnt->iDir);
}

void CRapidJasonWork::MakeJasonEntitySpiral(RJsonWriter& writer, CEntity* pEntity)
{
	CEntitySpiral* pEnt = (CEntitySpiral*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("m_center");
	writer.StartArray();
	writer.Double(pEnt->m_center.x);
	writer.Double(pEnt->m_center.y);
	writer.EndArray();

	writer.Key("m_radiusX");
	writer.Double(pEnt->m_radiusX);

	writer.Key("m_radiusY");
	writer.Double(pEnt->m_radiusY);

	writer.Key("m_turn");
	writer.Double(pEnt->m_turn);

	writer.Key("m_rotAng");
	writer.Double(pEnt->m_rotAng);

	writer.Key("m_resolution");
	writer.Double(pEnt->m_resolution);

	writer.Key("m_bClose");
	writer.Bool(pEnt->m_bClose);

	writer.Key("m_dirOut2In");
	writer.Bool(pEnt->m_dirOut2In);
}

void CRapidJasonWork::MakeJasonEntityArc(RJsonWriter& writer, CEntity* pEntity)
{
	CEntyArc* pEnt = (CEntyArc*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("center");
	writer.StartArray();
	writer.Double(pEnt->center.x);
	writer.Double(pEnt->center.y);
	writer.EndArray();
	writer.Key("angle_arc");
	writer.Double(pEnt->angle_arc);

	writer.Key("angle_st");
	writer.Double(pEnt->angle_st);

	writer.Key("radius");
	writer.Double(pEnt->radius);

	writer.Key("iDir");
	writer.Int(pEnt->iDir);
}

void CRapidJasonWork::MakeJasonEntityCircle(RJsonWriter& writer, CEntity* pEntity)
{
	CEntyCircle* pEnt = (CEntyCircle*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("center");
	writer.StartArray();
	writer.Double(pEnt->center.x);
	writer.Double(pEnt->center.y);
	writer.EndArray();

	writer.Key("radius");
	writer.Double(pEnt->radius);

	writer.Key("iDir");
	writer.Int(pEnt->iDir);

	writer.Key("StartAngle");
	writer.Double(pEnt->StartAngle);

	writer.Key("overpath");
	writer.Double(pEnt->overpath);
}

void CRapidJasonWork::MakeJasonEntityEllipse(RJsonWriter& writer, CEntity* pEntity)
{
	CEntyEllipse* pEnt = (CEntyEllipse*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("center");
	writer.StartArray();
	writer.Double(pEnt->center.x);
	writer.Double(pEnt->center.y);
	writer.EndArray();

	writer.Key("Rmajor");
	writer.Double(pEnt->Rmajor);

	writer.Key("Rminor");
	writer.Double(pEnt->Rminor);

	writer.Key("RotAngle");
	writer.Double(pEnt->RotAngle);

	writer.Key("StartAngle");
	writer.Double(pEnt->StartAngle);

	writer.Key("ArcAngle");
	writer.Double(pEnt->ArcAngle);

	writer.Key("iDir");
	writer.Int(pEnt->iDir);
}

void CRapidJasonWork::MakeJasonEntityLine(RJsonWriter& writer, CEntity* pEntity)
{
	CEntyLine* pEnt = (CEntyLine*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("point0");
	writer.StartArray();
	writer.Double(pEnt->point0.x);
	writer.Double(pEnt->point0.y);
	writer.EndArray();

	writer.Key("point1");
	writer.StartArray();
	writer.Double(pEnt->point1.x);
	writer.Double(pEnt->point1.y);
	writer.EndArray();

	writer.Key("length");
	writer.Double(pEnt->length);

	writer.Key("iDir");
	writer.Bool(pEnt->iDir);
}

void CRapidJasonWork::MakeJasonEntityPoint(RJsonWriter& writer, CEntity* pEntity)
{
	CEntyPoint* pEnt = (CEntyPoint*)pEntity;

	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("point0");
	writer.StartArray();
	writer.Double(pEnt->point0.x);
	writer.Double(pEnt->point0.y);
	writer.EndArray();

	writer.Key("delay");
	writer.Int(pEnt->delay);
}

void CRapidJasonWork::MakeJasonEntityPolyline(RJsonWriter& writer, CEntity* pEntity)
{
	CEntyPolyline* pEnt = (CEntyPolyline*)pEntity;

	int nSize = (int)pEnt->m_points.size();
	CStringA strName(pEnt->m_strName);

	writer.Key("strName");
	writer.String(strName);

	writer.Key("m_iFitType");
	writer.Int(pEnt->m_iFitType);

	writer.Key("nSize");
	writer.Int(nSize);

	writer.Key("m_points");
	writer.StartArray();//"[" Points 배열 시작
	for (auto p : pEnt->m_points)
	{
		writer.StartObject();//"{" Points object 시작
		writer.Key("p.arc_ang");
		writer.Double(p.arc_ang);
		writer.Key("p.arc_r");
		writer.Double(p.arc_r);
		writer.Key("p.x");
		writer.Double(p.x);
		writer.Key("p.y");
		writer.Double(p.y);
		writer.Key("p.z");
		writer.Double(p.z);
		writer.EndObject();//"}" Points object 끝
	}
	writer.EndArray();//"]" Points 배열 끝

	writer.Key("iDir");
	writer.Bool(pEnt->iDir);
}

void CRapidJasonWork::MakeJasonEntity(RJsonWriter& writer, CEntity* pEntity, int nEntType)
{
	CStringA strKey;
	strKey.Format(JASON_WORK_ENT_INFO);
	writer.Key(strKey);
	writer.StartObject(); // "{" ENT_INFO 시작

	switch (nEntType)
	{
		case LC_ENT_HATCH:
			MakeJasonEntityHatch(writer, pEntity);
			break;
		case LC_ENT_RECT:
			MakeJasonEntityRect(writer, pEntity);
			break;
		case LC_ENT_SPIRAL:
			MakeJasonEntitySpiral(writer, pEntity);
			break;
		case LC_ENT_ARC:
			MakeJasonEntityArc(writer, pEntity);
			break;
		case LC_ENT_CIRCLE:
			MakeJasonEntityCircle(writer, pEntity);
			break;
		case LC_ENT_ELLIPSE:
			MakeJasonEntityEllipse(writer, pEntity);
			break;
		case LC_ENT_LINE:
			MakeJasonEntityLine(writer, pEntity);
			break;
		case LC_ENT_POINT:
			MakeJasonEntityPoint(writer, pEntity);
			break;
		case LC_ENT_POLYLINE:
			MakeJasonEntityPolyline(writer, pEntity);
			break;
	}

	writer.EndObject(); // "}" ENT_INFO 시작
}

void CRapidJasonWork::MakeJasonJob(RJsonWriter& writer, CJobWeld* pJob)
{
	writer.Key("m_iPenID");
	writer.Int(pJob->m_iPenID);

	writer.Key("m_nOriPenID");
	writer.Int(pJob->GetOriPen());

	writer.Key("m_iRepeat");
	writer.Int(pJob->m_iRepeat);

	writer.Key("m_bDoJob");
	writer.Int(pJob->IsDoJob());

	int nType = pJob->GetEntity()->GetEntityType();

	writer.Key("nType");
	writer.Int(nType);

	writer.Key("m_d1StLength");
	writer.Double(pJob->m_d1StLength);

	writer.Key("m_d1StWidth");
	writer.Double(pJob->m_d1StWidth);

	writer.Key("m_d2NdWidth");
	writer.Double(pJob->m_d2NdWidth);

	writer.Key("m_d2NdOver");
	writer.Double(pJob->m_d2NdOver);

	writer.Key("m_dRotation");
	writer.Double(pJob->GetRotation());

	writer.Key("m_bUseIndividualRptPower");
	writer.Bool(pJob->GetUseIndividualPower());

	writer.Key("m_RptPower");
	writer.StartArray();//"[" m_RptPower 배열 시작
	for (int i=0; i< pJob->GetIndividualPowerCnt(); i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pJob->GetIndividualPower(i));
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();//"]" m_RptPower 배열 끝

	MakeJasonEntity(writer, pJob->GetEntity(), nType);
}

void CRapidJasonWork::MakeJasonWorkHead(RJsonWriter& writer, CIWSDoc *pDoc)
{
	IWS_VECTOR_JOB_GROUP* pJobs = pDoc->GetJobs();
	int nGroup = (int)pJobs->size();
	int nJob = 0;
	if (nGroup != 0) nJob = pJobs->front()->m_jobs.size();

	// Hairpin
	writer.Key("bUseHairpin");
	writer.Bool(pDoc->m_config.m_bUseHairpin);

	// Size
	writer.Key("Group_Size");
	writer.Int(nGroup);

	// Job Cnt
	writer.Key("nJob");
	writer.Int(nJob);

	// Simulation
	writer.Key("m_bSimulation");
	writer.Bool(pDoc->m_work.m_bSimulation);

	int nGrpIndex = 0;
	CStringA strKey;
	for (auto group : *pJobs)
	{
		strKey.Format(JASON_WORK_GROUP_INFO, nGrpIndex++);
		writer.Key(strKey);
		writer.StartObject();//"{"GROUP_INFO 객체 시작

		writer.Key("m_nIndex");
		writer.Int(group->m_nIndex);

		writer.Key("m_nSlot");
		writer.Int(group->m_nSlot);

		writer.Key("m_nLayer");
		writer.Int(group->m_nLayer);

		nJob = (int)group->m_jobs.size();

		writer.Key("nJobSize");
		writer.Int(nJob);

		//writer.Key("m_iRepeat");
		//writer.Int(group->m_nWeldCnt);

		int nJobIndex = 0;
		for (auto job : group->m_jobs)
		{
			strKey.Format(JASON_WORK_JOB_INFO, nJobIndex++);
			writer.Key(strKey);
			writer.StartObject();//"{"JOB_INFO 객체 시작
			int iType = job->GetJobType();

			writer.Key("iType");//weld type
			writer.Int(iType);

			MakeJasonJob(writer, (CJobWeld*)job);
			writer.EndObject();//"}"JOB_INFO 객체 끝
		}
		writer.EndObject();//"}"GROUP_INFO 객체 끝
	}
}

void CRapidJasonWork::MakeJasonHeader(RJsonWriter& writer, CIWSDoc *pDoc)
{
	writer.Key(JASON_HEADER_INFOS);
	writer.StartObject();//"{" HEADER INFO 시작

	// IWS Version Info
	writer.Key("IWS_VERSION");
	writer.String(CStringA(IWS_VERSION));

	// Build Date Info
	writer.Key("BUILD DATE");
	writer.String(CStringA(_T(__DATE__)));

	// Workfile Version No
	writer.Key("WORKFILE_VER");
	writer.Int(WELDJOB_VERC);

	// Hairpin을 위한 Header 추가 -------------->
	// Hairpin Info
	MakeJasonHeaderHairpin(writer, pDoc);

	// ZComp Info
	MakeJasonHeaderZComp(writer, pDoc);

	// ETC Info
	MakeJasonHeaderETC(writer, pDoc);
	// Hairpin을 위한 Header 추가 <--------------

	// 아래는 Normal 버전(IWS2용)
	//// Active Head No
	//writer.Key("ACTIVE_HEAD");
	//writer.Int(pDoc->GetActiveHeadIndex());

	//// Vision Scan 
	//writer.Key("Vision Scan Start(x)");
	//writer.Double(pDoc->m_work.m_dblVSsx);

	//writer.Key("Vision Scan Start(y)");
	//writer.Double(pDoc->m_work.m_dblVSsy);

	//writer.Key("Vision Scan End(x)");
	//writer.Double(pDoc->m_work.m_dblVSex);

	//writer.Key("Vision Scan End(y)");
	//writer.Double(pDoc->m_work.m_dblVSey);

	writer.EndObject();//"}" HEADER INFO종료
}

void CRapidJasonWork::MakeJasonHeaderHairpin(RJsonWriter& writer, CIWSDoc *pDoc)
{
	// Hairpin Info
	CStringA strHairpinKey;
	strHairpinKey.Format(JASON_HEADER_HAIRPIN);

	writer.Key(strHairpinKey);
	writer.StartObject(); //"{" HAIRPIN 오브젝트 시작

	// Slot 개수
	writer.Key("nSlot");
	writer.Int(pDoc->m_work.m_hp.nSlot);

	// 화면에 표시할 Slot 개수(실제 용접할 Slot)
	writer.Key("nDrawSlot");
	writer.Int(pDoc->m_work.m_hp.nDrawSlot);

	// Layer 개수
	writer.Key("nLayer");
	writer.Int(pDoc->m_work.m_hp.nLayer);

	// 헤어핀 생성 방향(0 - CCW, 1 - CW)
	writer.Key("nGenDir");
	writer.Int(pDoc->m_work.m_hp.nGenDir);

	// Slot 시작 각도
	writer.Key("startAng");
	writer.Double(pDoc->m_work.m_hp.startAng);

	// Hairpin 1차패턴 길이
	writer.Key("HpSize");
	writer.Double(pDoc->m_work.m_hp.HpSize);

	// Hairpin 1차패턴 폭
	writer.Key("HpSizeW");
	writer.Double(pDoc->m_work.m_hp.HpSizeW);

	// Hairpin 2차패턴 폭
	writer.Key("HpSize2nd");
	writer.Double(pDoc->m_work.m_hp.HpSize2nd);

	// Layer 반지름
	writer.Key("LayerDia");
	writer.StartArray();        // "["반지름 시작
	for (int i = 0; i < MAX_HAIRPIN_LAYER; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.LayerDia[i]);     
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"반지름 끝

	// 2차패턴 over length
	writer.Key("HpOverLen");
	writer.Double(pDoc->m_work.m_hp.HpOverLen);

	// 1차패턴 Count
	writer.Key("nCntP1");
	writer.Int(pDoc->m_work.m_hp.nCntP1);

	// 1차패턴 Pen
	writer.Key("nPenP1");
	writer.Int(pDoc->m_work.m_hp.nPenP1);

	// 1차패턴 Shape
	writer.Key("nWeldShapeP1");
	writer.Int(pDoc->m_work.m_hp.nWeldShapeP1);

	// 1차패턴 Zigzag Freq
	writer.Key("nWeldZigZagFreqP1");
	writer.Int(pDoc->m_work.m_hp.nWeldZigZagFreqP1);

	// 2차패턴 Pen
	writer.Key("nPenP2");
	writer.Int(pDoc->m_work.m_hp.nPenP2);

	// 2차패턴 Count
	writer.Key("nCntP2");
	writer.Int(pDoc->m_work.m_hp.nCntP2);

	// 2차패턴 Shape
	writer.Key("nWeldShapeP2");
	writer.Int(pDoc->m_work.m_hp.nWeldShapeP2);

	// 2차패턴 Zigzag Freq
	writer.Key("nWeldZigZagFreqP2");
	writer.Int(pDoc->m_work.m_hp.nWeldZigZagFreqP2);

	// 스위치 웰딩 사용여부(Z보상에 대해서만 적용)
	writer.Key("nSwitchWelding");
	writer.Int(pDoc->m_work.m_hp.nSwitchWelding);

	// 용접 타입
	writer.Key("nWeldType");
	writer.Int(pDoc->m_work.m_hp.nWeldType);

	// 용접 세기 비율
	writer.Key("dblPowerRatio");
	writer.Double(pDoc->m_work.m_hp.dblPowerRatio);

	// 용접 속도 비율
	writer.Key("dblSpeedRatio");
	writer.Double(pDoc->m_work.m_hp.dblSpeedRatio);

	// Bottom or Top(하부, 상부)
	writer.Key("nModel");
	writer.Int(pDoc->m_work.m_hp.nModel);

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	writer.Key("dDegree12");
	writer.Double(pDoc->m_work.m_hp.dDegree12);

	writer.Key("dDegree34");
	writer.Double(pDoc->m_work.m_hp.dDegree34);

	writer.Key("dDegree56");
	writer.Double(pDoc->m_work.m_hp.dDegree56);

	writer.Key("dDegree78");
	writer.Double(pDoc->m_work.m_hp.dDegree78);

	writer.Key("dDegree90");
	writer.Double(pDoc->m_work.m_hp.dDegree90);
	// 2022-07-07 Matrix 관련 변수 추가<---

	// 헤어핀 Offset
	writer.Key("hpOffset");
	writer.StartArray();        // "["Offset 시작
	for (int i = 0; i < 512; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.hpOffset[i].ax);
		writer.Double(pDoc->m_work.m_hp.hpOffset[i].ay);
		writer.Double(pDoc->m_work.m_hp.hpOffset[i].bx);
		writer.Double(pDoc->m_work.m_hp.hpOffset[i].by);
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"Offset 끝

	// Pen Power
	writer.Key("m_dPenPower");
	writer.StartArray();        // "["Pen Power 시작
	for (int i = 0; i < 10; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.m_dPenPower[i]);
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"Pen Power 끝

	// Pen Speed
	writer.Key("m_dPenSpeed");
	writer.StartArray();        // "["Pen Speed 시작
	for (int i = 0; i < 10; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.m_dPenSpeed[i]);
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"Pen Speed 끝

	// mspark 2022.04.22
	// 반복횟수별 파워 조정 사용여부(1차 패턴)
	writer.Key("bUseRptPowerProfile_1st");
	writer.Bool(pDoc->m_work.m_hp.bUseRptPowerProfile_1st);

	// 반복횟수별 파워 조정 사용여부(2차 패턴)
	writer.Key("bUseRptPowerProfile_2nd");
	writer.Bool(pDoc->m_work.m_hp.bUseRptPowerProfile_2nd);

	// 반복횟수별 파워(1차 패턴) - 1.0이 100%임
	writer.Key("dRptPowerProfile_1st");
	writer.StartArray();        // "["반복 횟수별 파워 시작
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.dRptPowerProfile_1st[i]);
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"반복 횟수별 파워 끝

	// 반복횟수별 파워(2차 패턴) - 1.0이 100%임
	writer.Key("dRptPowerProfile_2nd");
	writer.StartArray();        // "["반복 횟수별 파워 시작
	for (int i = 0; i < MAX_NUM_REPEAT_WELD; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.dRptPowerProfile_2nd[i]);
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"반복 횟수별 파워 끝

	// Sortie 반복 활설화 Flag
	writer.Key("bEnableSortiRpt");
	writer.Bool(pDoc->m_work.m_hp.bEnableSortiRpt);

	// Sortie 반복 횟수
	writer.Key("nSortieCnt");
	writer.Int(pDoc->m_work.m_hp.nSortieCnt);

	// Sortie 방향(0 - 한 방향, 1 - 양방향) // sjyi 2024.02.13 Sortie 반복 방향 설정 추가
	writer.Key("nSortieDir");
	writer.Int(pDoc->m_work.m_hp.nSortieDir);

	// 모래시계(SandGlass) 패턴을 위한 변수(2차패턴 Width2)
	writer.Key("HpSize2nd2");
	writer.Double(pDoc->m_work.m_hp.HpSize2nd2);

	// sjyi 2023.04.14 패턴 회전각
	writer.Key("dPtnRotationAngle");
	writer.Double(pDoc->m_work.m_hp.dPtnRotationAngle);

	writer.Key("dPtnRotationAngle2");
	writer.Double(pDoc->m_work.m_hp.dPtnRotationAngle2);

	// sjyi 2023.06.15 Weld Type관련 값 추가
	writer.Key("nDivStartSlot");
	writer.Int(pDoc->m_work.m_hp.nDivStartSlot);

	writer.Key("nDivJumpOffset");
	writer.Int(pDoc->m_work.m_hp.nDivJumpOffset);

	// dhpark 2023.07.13 2D Offset 추가
	writer.Key("b3DUse");
	writer.Bool(pDoc->m_work.m_hp.b3DUse);

	// 헤어핀 2DOffset
	writer.Key("hpOffset2D");
	writer.StartArray();        // "["Offset 시작
	for (int i = 0; i < 512; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_hp.hpOffset2D[i].ax);
		writer.Double(pDoc->m_work.m_hp.hpOffset2D[i].ay);
		writer.Double(pDoc->m_work.m_hp.hpOffset2D[i].bx);
		writer.Double(pDoc->m_work.m_hp.hpOffset2D[i].by);
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"Offset 끝

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
	writer.Key("bReversal");
	writer.Bool(pDoc->m_work.m_hp.bReversal);


	// sjyi 2024.01.31 스파이럴 패턴 관련 내용 추가
	// Sprial Out to In 옵션(1st Pattern)
	writer.Key("bUseOutToInP1");
	writer.Bool(pDoc->m_work.m_hp.bUseOutToInP1);

	// Sprial Out to In 옵션(2nd Pattern)
	writer.Key("bUseOutToInP2");
	writer.Bool(pDoc->m_work.m_hp.bUseOutToInP2);

	// Sprial Closed 옵션(1st Pattern)
	writer.Key("bCloseSpiralP1");
	writer.Bool(pDoc->m_work.m_hp.bCloseSpiralP1);

	// Sprial Closed 옵션(2nd Pattern)
	writer.Key("bCloseSpiralP2");
	writer.Bool(pDoc->m_work.m_hp.bCloseSpiralP2);

	writer.EndObject();   //"}" HAIRPIN 오브젝트 끝
}

void CRapidJasonWork::MakeJasonHeaderZComp(RJsonWriter& writer, CIWSDoc *pDoc)
{
	// Hairpin Info
	CStringA strHairpinKey;
	strHairpinKey.Format(JASON_HEADER_ZCOMP);

	writer.Key(strHairpinKey);
	writer.StartObject(); //"{" ZCOMP 오브젝트 시작

	// 사용 여부
	writer.Key("bUse");
	writer.Bool(pDoc->m_work.m_zComp.bUse);

	// Threshold 값
	writer.Key("dblThreshold");
	writer.Double(pDoc->m_work.m_zComp.dblThreshold);

	// Z 보상 용접 횟수
	writer.Key("nCnt");
	writer.Int(pDoc->m_work.m_zComp.nCnt);

	// Z 보상 용접 파워값
	writer.Key("dblPower");
	writer.Double(pDoc->m_work.m_zComp.dblPower);

	// Hp Z Array
	writer.Key("zHpArr");
	writer.StartArray();        // "["zHpArr 시작
	for (int i = 0; i < 10; i++)
	{
		writer.StartArray();    // "[" N번째 요소 시작
		writer.Double(pDoc->m_work.m_zComp.zHpArr[i].dblMin); // Min
		writer.Double(pDoc->m_work.m_zComp.zHpArr[i].dblMax); // Max
		writer.Int(pDoc->m_work.m_zComp.zHpArr[i].nCnt);	// Counter
		writer.Double(pDoc->m_work.m_zComp.zHpArr[i].dblPower); // Power
		writer.EndArray();      // "]" N번째 요소 끝
	}
	writer.EndArray();          // "]"zHpArr 끝

	writer.EndObject();   //"}" ZCOMP 오브젝트 끝
}

void CRapidJasonWork::MakeJasonHeaderETC(RJsonWriter& writer, CIWSDoc *pDoc)
{
	// Hairpin Info
	CStringA strHairpinKey;
	strHairpinKey.Format(JASON_HEADER_ETC);

	writer.Key(strHairpinKey);
	writer.StartObject(); //"{" ETC 오브젝트 시작

	// 기본 Layer
	writer.Key("m_nDefLayer");
	writer.Int(pDoc->m_work.m_nDefLayer);

	// sjyi 2023.09.12 추가(2D 검사 내용 반영)
	writer.Key("m_n2DPathLength");
	writer.Int(pDoc->m_work.m_n2DPathLength);

	writer.Key("m_str2DRecipePath");
	writer.String(CStringA(pDoc->m_work.m_str2DRecipePath));

	writer.Key("m_b2DInspection");
	writer.Bool(pDoc->m_work.m_b2DInspection);

	writer.EndObject();   //"}" ETC 오브젝트 끝
}

void CRapidJasonWork::MakeJasonPens(RJsonWriter& writer, CIWSDoc *pDoc)
{
	int nPen = (int)(pDoc->GetPens()->size());

	writer.Key(JASON_PEN_INFOS);
	writer.StartObject();//"{" PEN_INFOS 시작

	writer.Key("NO_OF_PENS");//팬개수
	writer.Int(nPen);

	for (int i = 0; i < nPen; i++)
	{
		MakeJasonPen(writer, &pDoc->GetLaserPen(i), i);
	}

	writer.EndObject();//"}" PEN_INFOS 종료
}

void CRapidJasonWork::MakeJasonWorks(RJsonWriter& writer, CIWSDoc *pDoc)
{
	writer.Key(JASON_WORK_INFOS);
	writer.StartObject();//"{" JOB_INFO 시작

	// Save Work
	MakeJasonWorkHead(writer, pDoc);

	writer.EndObject();//"}" JOB_INFO 종료
}

BOOL CRapidJasonWork::SaveWork(CFile *pf, CIWSDoc *pDoc)
{
	//2024.04.12 ksu json 파일 변환-->
	rapidjson::StringBuffer s;
	RJsonWriter writer(s);

	writer.StartObject();                 //"{" Jason 시작

	// Header
	MakeJasonHeader(writer, pDoc);

	// Make Pens Jason
	MakeJasonPens(writer, pDoc);

	// Make Works Json
	MakeJasonWorks(writer, pDoc);

	writer.EndObject();                   //"}" Jason 끝

	//파일 저장
	pf->Write(s.GetString(), s.GetSize());// 데이터 쓰기

	return TRUE;
}

BOOL CRapidJasonWork::LoadWork(CString strFilePath, CIWSDoc *pDoc)
{
	// OPen Json File
	RJsonReader reader;
	if (OpenJsonFile(strFilePath, reader) == FALSE) return FALSE;

	// Parse Header
	if (ParseJsonHeader(reader, pDoc) == FALSE) return FALSE;

	// Parse Pens Info
	if (ParseJasonPens(reader, pDoc) == FALSE) return FALSE;

	// Parse Works Info
	if (ParseJasonWorks(reader, pDoc) == FALSE) return FALSE;

	return TRUE;
}

BOOL CRapidJasonWork::OpenJsonFile(CString strFilePath, RJsonReader& reader)
{
	//2024.04.30 ksu Open Work 주변 정리-->
	std::ifstream ifs(strFilePath);

	// File Open
	if (!ifs.is_open())
	{
		_tracer(TRACE_ERROR, 1000, _T("Cannot Open Work File(%s)"), strFilePath);
		return FALSE;
	}

	// 파일 내용 읽어, Jason Reder에 입력
	std::string content((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	const char* json = content.c_str();

	reader.Parse(json);

	if (reader.HasParseError())
	{
		_tracer(TRACE_ERROR, 1000, L"ParseError json File : %s", strFilePath);
		return FALSE;
	}

	//2024.05.16 ksu 멤버카운트 예외처리,코드 추가-->
	rapidjson::SizeType nMemberCnt = reader.MemberCount();
	if (nMemberCnt != MAX_JASON_MEMBERCNT)
	{
		ASSERT(reader.HasMember("IWS2Version"));
		_tracer (TRACE_ERROR, 1000, L"!!!Version Error!!!");
		return FALSE;
	}

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonHeader(RJsonReader& reader, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_HEADER_INFOS))
	{
		_tracer(TRACE_ERROR, 1, L"[HEADER_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valHeader = reader[JASON_HEADER_INFOS];

	// IWS Version Info
	CString strIWSVersion = CString(GetJsString("IWS_VERSION", valHeader, ""));

	// Build Date Info
	CString strBuildDate = CString(GetJsString("BUILD DATE", valHeader, ""));

	// WorkFile Version No
	int nVersionNo = GetJsInt("WORKFILE_VER", valHeader, 0);

	// Hairpin 을 위한 Read 추가 ------------------------------------------>
	// HP Info
	if (ParseJsonHeaderHairpin(valHeader, pDoc) == FALSE) return FALSE;

	// ZComp Weld Info
	if (ParseJsonHeaderZComp(valHeader, pDoc) == FALSE) return FALSE;

	// ETC Info
	if (ParseJsonHeaderETC(valHeader, pDoc) == FALSE) return FALSE;
	// Hairpin 을 위한 Read 추가 <------------------------------------------

	//// Active Head No
	//int nActiveHeadNo = GetJsInt("ACTIVE_HEAD", valHeader, 0);
	//pDoc->SetActiveHead(nActiveHeadNo, FALSE);

	//// Vision Scan 
	//pDoc->m_work.m_dblVSsx = GetJsDouble("Vision Scan Start(x)", valHeader, 0.0);
	//pDoc->m_work.m_dblVSsy = GetJsDouble("Vision Scan Start(y)", valHeader, 0.0);
	//pDoc->m_work.m_dblVSex = GetJsDouble("Vision Scan End(x)", valHeader, 0.0);
	//pDoc->m_work.m_dblVSey = GetJsDouble("Vision Scan End(y)", valHeader, 0.0);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonHeaderHairpin(RJsonValue& reader, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_HEADER_HAIRPIN))
	{
		_tracer(TRACE_ERROR, 1, L"[HAIRPIN_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valHeader = reader[JASON_HEADER_HAIRPIN];

	// Slot 개수
	pDoc->m_work.m_hp.nSlot = GetJsInt("nSlot", valHeader, 0);

	// 화면에 표시할 Slot 개수(실제 용접할 Slot)
	pDoc->m_work.m_hp.nDrawSlot = GetJsInt("nDrawSlot", valHeader, 0);

	// Layer 개수
	pDoc->m_work.m_hp.nLayer = GetJsInt("nLayer", valHeader, 0);

	// 헤어핀 생성 방향(0 - CCW, 1 - CW)
	pDoc->m_work.m_hp.nGenDir = GetJsInt("nGenDir", valHeader, 0);

	// Slot 시작 각도
	pDoc->m_work.m_hp.startAng = GetJsDouble("startAng", valHeader, 0);

	// Hairpin 1차패턴 길이
	pDoc->m_work.m_hp.HpSize = GetJsDouble("HpSize", valHeader, 0);

	// Hairpin 1차패턴 폭
	pDoc->m_work.m_hp.HpSizeW = GetJsDouble("HpSizeW", valHeader, 0);

	// Hairpin 2차패턴 폭
	pDoc->m_work.m_hp.HpSize2nd = GetJsDouble("HpSize2nd", valHeader, 0);

	// Layer 반지름
	RJsonValue& ValArrayDia = valHeader["LayerDia"];
	for (int i = 0; i < ValArrayDia.Size(); i++)
	{
		RJsonValue& ValData = ValArrayDia[i];
		pDoc->m_work.m_hp.LayerDia[i] = GetJsDouble(0, ValData, 0);
	}

	// 2차패턴 over length
	pDoc->m_work.m_hp.HpOverLen = GetJsDouble("HpOverLen", valHeader, 0);

	// 1차패턴 Count
	pDoc->m_work.m_hp.nCntP1 = GetJsInt("nCntP1", valHeader, 0);

	// 1차패턴 Pen
	pDoc->m_work.m_hp.nPenP1 = GetJsInt("nPenP1", valHeader, 0);

	// 1차패턴 Shape
	pDoc->m_work.m_hp.nWeldShapeP1 = GetJsInt("nWeldShapeP1", valHeader, 0);

	// 1차패턴 Zigzag Freq
	pDoc->m_work.m_hp.nWeldZigZagFreqP1 = GetJsInt("nWeldZigZagFreqP1", valHeader, 0);

	// 2차패턴 Pen
	pDoc->m_work.m_hp.nPenP2 = GetJsInt("nPenP2", valHeader, 0);

	// 2차패턴 Count
	pDoc->m_work.m_hp.nCntP2 = GetJsInt("nCntP2", valHeader, 0);

	// 2차패턴 Shape
	pDoc->m_work.m_hp.nWeldShapeP2 = GetJsInt("nWeldShapeP2", valHeader, 0);

	// 2차패턴 Zigzag Freq
	pDoc->m_work.m_hp.nWeldZigZagFreqP2 = GetJsInt("nWeldZigZagFreqP2", valHeader, 0);

	// 스위치 웰딩 사용여부(Z보상에 대해서만 적용)
	pDoc->m_work.m_hp.nSwitchWelding = GetJsInt("nSwitchWelding", valHeader, 0);

	// 용접 타입
	pDoc->m_work.m_hp.nWeldType = GetJsInt("nWeldType", valHeader, 0);

	// 용접 세기 비율
	pDoc->m_work.m_hp.dblPowerRatio = GetJsDouble("dblPowerRatio", valHeader, 1.0);

	// 용접 속도 비율
	pDoc->m_work.m_hp.dblSpeedRatio = GetJsDouble("dblSpeedRatio", valHeader, 1.0);

	// Bottom or Top(하부, 상부)
	pDoc->m_work.m_hp.nModel = GetJsInt("nModel", valHeader, 0);

	// 2022-07-07 Matrix 관련 변수 추가--->
	//Layer 별 각도 Offset
	pDoc->m_work.m_hp.dDegree12 = GetJsDouble("dDegree12", valHeader, 0.0);
	pDoc->m_work.m_hp.dDegree34 = GetJsDouble("dDegree34", valHeader, 0.0);
	pDoc->m_work.m_hp.dDegree56 = GetJsDouble("dDegree56", valHeader, 0.0);
	pDoc->m_work.m_hp.dDegree78 = GetJsDouble("dDegree78", valHeader, 0.0);
	pDoc->m_work.m_hp.dDegree90 = GetJsDouble("dDegree90", valHeader, 0.0);
	// 2022-07-07 Matrix 관련 변수 추가<---

	// 헤어핀 Offset
	RJsonValue& ValArrayOffset = valHeader["hpOffset"];
	for (int i = 0; i < ValArrayOffset.Size(); i++)
	{
		RJsonValue& ValData = ValArrayOffset[i];
		pDoc->m_work.m_hp.hpOffset[i].ax = GetJsDouble(0, ValData, 0);
		pDoc->m_work.m_hp.hpOffset[i].ay = GetJsDouble(1, ValData, 0);
		pDoc->m_work.m_hp.hpOffset[i].bx = GetJsDouble(2, ValData, 0);
		pDoc->m_work.m_hp.hpOffset[i].by = GetJsDouble(3, ValData, 0);
	}

	// Pen Power
	RJsonValue& ValArrayPenPower = valHeader["m_dPenPower"];
	for (int i = 0; i < ValArrayPenPower.Size(); i++)
	{
		RJsonValue& ValData = ValArrayPenPower[i];
		pDoc->m_work.m_hp.m_dPenPower[i] = GetJsDouble(0, ValData, 0);
	}

	// Pen Speed
	RJsonValue& ValArrayPenSpeed = valHeader["m_dPenSpeed"];
	for (int i = 0; i < ValArrayPenSpeed.Size(); i++)
	{
		RJsonValue& ValData = ValArrayPenSpeed[i];
		pDoc->m_work.m_hp.m_dPenSpeed[i] = GetJsDouble(0, ValData, 0);
	}

	// mspark 2022.04.22
	// 반복횟수별 파워 조정 사용여부(1차 패턴)
	pDoc->m_work.m_hp.bUseRptPowerProfile_1st = GetJsBool("bUseRptPowerProfile_1st", valHeader, FALSE);

	// 반복횟수별 파워 조정 사용여부(2차 패턴)
	pDoc->m_work.m_hp.bUseRptPowerProfile_2nd = GetJsBool("bUseRptPowerProfile_2nd", valHeader, FALSE);

	// 반복횟수별 파워(1차 패턴) - 1.0이 100%임
	RJsonValue& ValArrayPowerProfile1st = valHeader["dRptPowerProfile_1st"];
	for (int i = 0; i < ValArrayPowerProfile1st.Size(); i++)
	{
		RJsonValue& ValData = ValArrayPowerProfile1st[i];
		pDoc->m_work.m_hp.dRptPowerProfile_1st[i] = GetJsDouble(0, ValData, 0);
	}

	// 반복횟수별 파워(2차 패턴) - 1.0이 100%임
	RJsonValue& ValArrayPowerProfile2nd = valHeader["dRptPowerProfile_2nd"];
	for (int i = 0; i < ValArrayPowerProfile2nd.Size(); i++)
	{
		RJsonValue& ValData = ValArrayPowerProfile2nd[i];
		pDoc->m_work.m_hp.dRptPowerProfile_2nd[i] = GetJsDouble(0, ValData, 0);
	}

	// Sortie 반복 활설화 Flag
	pDoc->m_work.m_hp.bEnableSortiRpt = GetJsBool("bEnableSortiRpt", valHeader, FALSE);

	// Sortie 반복 횟수
	pDoc->m_work.m_hp.nSortieCnt = GetJsInt("nSortieCnt", valHeader, 0);

	// Sortie 방향(0 - 한 방향, 1 - 양방향) // sjyi 2024.02.13 Sortie 반복 방향 설정 추가
	pDoc->m_work.m_hp.nSortieDir = GetJsInt("nSortieDir", valHeader, 0);

	// 모래시계(SandGlass) 패턴을 위한 변수(2차패턴 Width2)
	pDoc->m_work.m_hp.HpSize2nd2 = GetJsDouble("HpSize2nd2", valHeader, 0.0);

	// sjyi 2023.04.14 패턴 회전각
	pDoc->m_work.m_hp.dPtnRotationAngle = GetJsDouble("dPtnRotationAngle", valHeader, 0.0);
	pDoc->m_work.m_hp.dPtnRotationAngle2 = GetJsDouble("dPtnRotationAngle2", valHeader, 0.0);

	// sjyi 2023.06.15 Weld Type관련 값 추가
	pDoc->m_work.m_hp.nDivStartSlot = GetJsInt("nDivStartSlot", valHeader, 0);
	pDoc->m_work.m_hp.nDivJumpOffset = GetJsInt("nDivJumpOffset", valHeader, 0);

	// dhpark 2023.07.13 2D Offset 추가
	pDoc->m_work.m_hp.b3DUse = GetJsBool("b3DUse", valHeader, FALSE);

	// 헤어핀 2DOffset
	RJsonValue& ValArrayOffset2D = valHeader["hpOffset2D"];
	for (int i = 0; i < ValArrayOffset2D.Size(); i++)
	{
		RJsonValue& ValData = ValArrayOffset2D[i];
		pDoc->m_work.m_hp.hpOffset2D[i].ax = GetJsDouble(0, ValData, 0);
		pDoc->m_work.m_hp.hpOffset2D[i].ay = GetJsDouble(1, ValData, 0);
		pDoc->m_work.m_hp.hpOffset2D[i].bx = GetJsDouble(2, ValData, 0);
		pDoc->m_work.m_hp.hpOffset2D[i].by = GetJsDouble(3, ValData, 0);
	}

	// sjyi 2024.02.28 헤어핀 외경(짝수열) 패턴 반전기능 사용여부 추가
	pDoc->m_work.m_hp.bReversal = GetJsBool("bReversal", valHeader, FALSE);


	// sjyi 2024.01.31 스파이럴 패턴 관련 내용 추가
	// Sprial Out to In 옵션(1st Pattern)
	pDoc->m_work.m_hp.bUseOutToInP1 = GetJsBool("bUseOutToInP1", valHeader, FALSE);

	// Sprial Out to In 옵션(2nd Pattern)
	pDoc->m_work.m_hp.bUseOutToInP2 = GetJsBool("bUseOutToInP2", valHeader, FALSE);

	// Sprial Closed 옵션(1st Pattern)
	pDoc->m_work.m_hp.bCloseSpiralP1 = GetJsBool("bCloseSpiralP1", valHeader, FALSE);

	// Sprial Closed 옵션(2nd Pattern)
	pDoc->m_work.m_hp.bCloseSpiralP2 = GetJsBool("bCloseSpiralP2", valHeader, FALSE);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonHeaderZComp(RJsonValue& reader, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_HEADER_ZCOMP))
	{
		_tracer(TRACE_ERROR, 1, L"[ZCOMPWELD_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valHeader = reader[JASON_HEADER_ZCOMP];

	// 사용 여부
	pDoc->m_work.m_zComp.bUse = GetJsBool("bUse", valHeader, FALSE);

	// Threshold 값
	pDoc->m_work.m_zComp.dblThreshold = GetJsDouble("dblThreshold", valHeader, 0.0);

	// Z 보상 용접 횟수
	pDoc->m_work.m_zComp.nCnt = GetJsInt("nCnt", valHeader, 0);

	// Z 보상 용접 파워값
	pDoc->m_work.m_zComp.dblPower = GetJsDouble("dblPower", valHeader, 0.0);

	// Hp Z Array
	RJsonValue& ValArrayHPZ = valHeader["zHpArr"];
	for (int i = 0; i < ValArrayHPZ.Size(); i++)
	{
		RJsonValue& ValData = ValArrayHPZ[i];
		pDoc->m_work.m_zComp.zHpArr[i].dblMin = GetJsDouble(0, ValData, 0);	// Min
		pDoc->m_work.m_zComp.zHpArr[i].dblMax = GetJsDouble(1, ValData, 0);	// Max
		pDoc->m_work.m_zComp.zHpArr[i].nCnt   = GetJsInt(2, ValData, 0);	// Counter
		pDoc->m_work.m_zComp.zHpArr[i].dblPower = GetJsDouble(3, ValData, 0);	// Power
	}

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonHeaderETC(RJsonValue& reader, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_HEADER_ETC))
	{
		_tracer(TRACE_ERROR, 1, L"[ETC_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valHeader = reader[JASON_HEADER_ETC];

	// 기본 Layer
	pDoc->m_work.m_nDefLayer = GetJsInt("m_nDefLayer", valHeader, 0);

	// sjyi 2023.09.12 추가(2D 검사 내용 반영)
	pDoc->m_work.m_n2DPathLength = GetJsInt("m_n2DPathLength", valHeader, 0);
	pDoc->m_work.m_str2DRecipePath = CString(GetJsString("m_str2DRecipePath", valHeader, ""));
	pDoc->m_work.m_b2DInspection = GetJsBool("m_b2DInspection", valHeader, FALSE);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJasonPens(RJsonReader& reader, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_PEN_INFOS))
	{
		_tracer(TRACE_ERROR, 1, L"[PEN_INFOS] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valPenData = reader[JASON_PEN_INFOS];

	int nPen = GetJsInt("NO_OF_PENS", valPenData, -65565);
	
	pDoc->GetPens()->clear();

	for (int i = 0; i < nPen; i++)
	{
		CLaserPen pen;

		if (ParseJsonPen(valPenData, &pen, i) == FALSE) return FALSE;

		pDoc->GetPens()->push_back(pen);
	}

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonPen(RJsonValue& reader, CLaserPen *pPen, int nPenNo)
{
	CStringA strKey;
	strKey.Format(JASON_PEN_INFO, nPenNo);

	if (!reader.HasMember(strKey))
	{
		_tracer(TRACE_ERROR, 1, L"[PEN_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valPenData = reader[strKey.GetBuffer()];

	pPen->m_iPenID			= GetJsInt("m_iPenID", valPenData, 0);
	pPen->m_dblPower		= GetJsDouble("m_dblPower", valPenData, 0.0);
	pPen->m_dblFrequency	= GetJsDouble("m_dblFrequency", valPenData, 0.0);
	pPen->m_dblPulseWidth	= GetJsDouble("m_dblPulseWidth", valPenData, 0.0);
	pPen->m_dblMarkSpeed	= GetJsDouble("m_dblMarkSpeed", valPenData, 0.0);
	pPen->m_dblJumpSpeed	= GetJsDouble("m_dblJumpSpeed", valPenData, 0.0);
	pPen->m_nMarkDelay		= GetJsDouble("m_nMarkDelay", valPenData, 0.0);
	pPen->m_nJumpDelay		= GetJsDouble("m_nJumpDelay", valPenData, 0.0);
	pPen->m_nPolygonDelay	= GetJsInt("m_nPolygonDelay", valPenData, 0);
	pPen->m_nLaserOnDelay	= GetJsInt("m_nLaserOnDelay", valPenData, 0);
	pPen->m_nLaserOffDelay	= GetJsInt("m_nLaserOffDelay", valPenData, 0);
	pPen->m_bUseWobbel		= GetJsInt("m_bUseWobbel", valPenData, 0);
//	pPen->m_wbType			= (CLaserPen::WobbleMode)GetJsInt("m_wbType", valPenData, 0);
	pPen->m_dWbFreq			= GetJsDouble("m_dWbFreq", valPenData, 0.0);
	pPen->m_dWbLongitudinal = GetJsDouble("m_dWbLongitudinal", valPenData, 0.0);
	pPen->m_dWbTransbersal	= GetJsDouble("m_dWbTransbersal", valPenData, 0.0);
	pPen->m_bUsePowerShape	= GetJsInt("m_bUsePowerShape", valPenData, 0);

	if (pPen->m_bUsePowerShape)
	{
		if (!valPenData.HasMember("m_pwrShape"))
		{
			_tracer(TRACE_ERROR, 1, L"m_pwrShape 항목을 지원하지 않는 파일입니다.");
			return FALSE;
		}
		else
		{
			RJsonValue& ValPwrShape = valPenData["m_pwrShape"];

			for (int i = 0; i < ValPwrShape.Size(); i++)
			{
				POINTDOUBLE p;
				RJsonValue& ValPwrShapeidx = ValPwrShape[i];
				p.x = GetJsDouble(0, ValPwrShapeidx, 0);
				p.y = GetJsDouble(1, ValPwrShapeidx, 0);
				pPen->m_pwrShape.push_back(p);
			}
		}

	}

	pPen->m_bUsePulseShape = GetJsInt("m_bUsePulseShape", valPenData, 0);
	if (pPen->m_bUsePulseShape)
	{
		if (!valPenData.HasMember("m_pulseShape"))
		{
			_tracer(TRACE_ERROR, 1, L"m_pulseShape 항목을 지원하지 않는 파일입니다.");
			return FALSE;
		}
		else
		{
			RJsonValue& ValPulseShape = valPenData["m_pulseShape"];
			for (int i = 0; i < ValPulseShape.Size(); i++)
			{
				POINTDOUBLE p;
				RJsonValue& ValPulseShapeidx = ValPulseShape[i];

				p.x = GetJsDouble(0, ValPulseShapeidx, 0.0);
				p.y = GetJsDouble(1, ValPulseShapeidx, 0.0);

				pPen->m_pulseShape.push_back(p);
			}
		}

	}

	pPen->m_iPWS = GetJsInt("m_iPWS", valPenData, 0);
	//pPen->m_iProcNum = GetJsInt("m_iProcNum", valPenData, 0);
	//pPen->m_dEngTransferRefSpd_mm = GetJsDouble("m_dEngTransferRefSpd_mm", valPenData, 0.0);
	//pPen->m_dEngTransferRefPow_percent = GetJsDouble("m_dEngTransferRefPow_percent", valPenData, 0.0);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJasonWorks(RJsonReader& reader, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_WORK_INFOS))
	{
		_tracer(TRACE_ERROR, 1, L"[WORK_INFOS] 데이터가 없습니다.");
		return FALSE;
	}

	RJsonValue& valWorkData = reader[JASON_WORK_INFOS];

	return ParseJsonWorkHead(valWorkData, pDoc);
}

BOOL CRapidJasonWork::ParseJsonWorkHead(RJsonValue& reader, CIWSDoc *pDoc)
{

	// Hairpin
	BOOL bUseHairpin = GetJsBool("bUseHairpin", reader, FALSE);
	
	if (bUseHairpin == FALSE && pDoc->m_config.m_bUseHairpin == TRUE)
	{
		AfxMessageBox(L"The saved file cannot be loaded because it was saved in Normal Mode.");
		return FALSE;
	}
	else if (bUseHairpin == TRUE && pDoc->m_config.m_bUseHairpin == FALSE)
	{
		AfxMessageBox(_T("The saved file cannot be loaded because it was saved in Hairpin Mode."));
		return FALSE;
	}

	// Group Size
	int nGrpSize = GetJsInt("Group_Size", reader, 0);

	// Simulation
	pDoc->m_work.m_bSimulation = GetJsBool("m_bSimulation", reader, FALSE);

	IWS_VECTOR_JOB_GROUP* pJobGroup = pDoc->GetJobs();
	pJobGroup->clear();
	for (int i = 0; i < nGrpSize; i++)
	{
		if(ParseJsonWorkJobGroup(reader, pDoc, pJobGroup, i) == FALSE) return FALSE;
	}

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonWorkJobGroup(RJsonValue& reader, CIWSDoc *pDoc, IWS_VECTOR_JOB_GROUP* pJobGroup, int nGrpNo)
{
	CStringA strKey;
	strKey.Format(JASON_WORK_GROUP_INFO, nGrpNo);

	if (!reader.HasMember(strKey))
	{
		_tracer(TRACE_ERROR, 1, L"[GROUP_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valData = reader[strKey.GetBuffer()];
	
	CJobGroup* group = new CJobGroup();
	group->m_nIndex = GetJsInt("m_nIndex", valData, 0);
	group->m_nSlot = GetJsInt("m_nSlot", valData, 0);
	group->m_nLayer = GetJsInt("m_nLayer", valData, 0);

	//group->m_nWeldCnt = GetJsInt("m_iRepeat", valData, 0);

	int nJob = GetJsInt("nJobSize", valData, 0);

	IWS_VECTOR_JOB jobs;
	for (int i = 0; i < nJob; i++)
	{
		if( ParseJsonWorkJobs(valData, pDoc, &jobs, i) == FALSE) return FALSE;
		group->AddJob(jobs[i]);
	}

//	if (jobs.size() == 1)
//	{
//		group->AddJob(jobs[0]);
//	}
//	else
//	{
//		group->AddJobs(&jobs, pDoc->m_work.GetTreeListCtrl());
//	}

	pJobGroup->push_back(group);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonWorkJobs(RJsonValue& reader, CIWSDoc *pDoc, IWS_VECTOR_JOB* pJobs, int nJobNo)
{
	CStringA strKey;
	strKey.Format(JASON_WORK_JOB_INFO, nJobNo);

	if (!reader.HasMember(strKey))
	{
		_tracer(TRACE_ERROR, 1, L"[JOB_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valData = reader[strKey.GetBuffer()];

	int iType = GetJsInt("iType", valData, 0);

	HANDLE hDrw, hWnd;
	//hWnd = pDoc->GetCadWndHanle(nHeadNo);
	//hDrw = pDoc->GetCadDrwHanle(nHeadNo);
	//hWnd = g_hLcWnd;
	//hDrw = g_hLcDrw;

	CJobWeld *pJob = new CJobWeld();

	//pJob->SetDrawWobbel(FALSE);
	pJob->SetPens(pDoc->GetPens());

	int nPenID = GetJsInt("m_iPenID", valData, 0);
	int nOriPenID = GetJsInt("m_nOriPenID", valData, 0);
	pJob->SetPenID(nPenID);
	pJob->SetOriPen(nOriPenID);

	pJob->m_iRepeat = GetJsInt("m_iRepeat", valData, 0);
	int nDoJob = GetJsInt("m_bDoJob", valData, 0);
	pJob->SetDoJob(nDoJob);

	int nEntType = GetJsInt("nType", valData, 0);

	pJob->m_d1StLength = GetJsDouble("m_d1StLength", valData, 0.0);
	pJob->m_d1StWidth = GetJsDouble("m_d1StWidth", valData, 0.0);
	pJob->m_d2NdWidth = GetJsDouble("m_d2NdWidth", valData, 0.0);
	pJob->m_d2NdOver = GetJsDouble("m_d2NdOver", valData, 0.0);

	pJob->SetRotation(GetJsDouble("m_dRotation", valData, 0.0));
	pJob->SetUseIndividualPower(GetJsBool("m_bUseIndividualRptPower", valData, FALSE));

	RJsonValue& ValArrayRptPower = valData["m_RptPower"];
	pJob->ClearIndividualPower();
	for (int i = 0; i < ValArrayRptPower.Size(); i++)
	{
		RJsonValue& ValData = ValArrayRptPower[i];
		pJob->AddIndividualPower(GetJsDouble(0, ValData, 0));
	}

	if (iType == JOB_TYPE_WELD)
	{
		pJob->m_pScan = pDoc->m_pScanner;
		pJob->m_pPens = &(pDoc->m_pens);
	}
	
	if(ParseJsonEntity(valData, pJob, nEntType, pDoc) == FALSE) return FALSE;

	pJobs->push_back(pJob);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntity(RJsonValue& reader, CJobWeld* pJob, int nEntType, CIWSDoc *pDoc)
{
	if (!reader.HasMember(JASON_WORK_ENT_INFO))
	{
		_tracer(TRACE_ERROR, 1, L"[ENTITY_INFO] 항목을 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valData = reader[JASON_WORK_ENT_INFO];

	BOOL bRtn = FALSE;

	switch (nEntType)
	{
	case LC_ENT_HATCH:
		bRtn = ParseJsonEntityHatch(valData, pJob, pDoc);
		break;
	case LC_ENT_RECT:
		bRtn = ParseJsonEntityRect(valData, pJob, pDoc);
		break;
	case LC_ENT_SPIRAL:
		bRtn = ParseJsonEntitySpiral(valData, pJob, pDoc);
		break;
	case LC_ENT_ARC:
		bRtn = ParseJsonEntityArc(valData, pJob, pDoc);
		break;
	case LC_ENT_CIRCLE:
		bRtn = ParseJsonEntityCircle(valData, pJob, pDoc);
		break;
	case LC_ENT_ELLIPSE:
		bRtn = ParseJsonEntityEllipse(valData, pJob, pDoc);
		break;
	case LC_ENT_LINE:
		bRtn = ParseJsonEntityLine(valData, pJob, pDoc);
		break;
	case LC_ENT_POINT:
		bRtn = ParseJsonEntityPoint(valData, pJob, pDoc);
		break;
	case LC_ENT_POLYLINE:
		bRtn = ParseJsonEntityPolyline(valData, pJob, pDoc);
		break;
	}

	return bRtn;
}

BOOL CRapidJasonWork::ParseJsonEntityHatch(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntityHatch* pEnt = new CEntityHatch();
	
	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
//	pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, "-1");
	pEnt->ptrnScale = GetJsDouble("ptrnScale", reader, -1);
	pEnt->ptrnAngle = GetJsDouble("ptrnAngle", reader, -1);
	pEnt->ptrnIdx = GetJsInt("ptrnIdx", reader, -1);

	int nPath = 0;
	//nPath     = doc["nPath"    ].GetInt ();
	nPath = GetJsInt("nPath", reader, -1);
	if (!reader.HasMember("HatchLoopData"))
	{
		_tracer(TRACE_ERROR, 1, L"HatchLoopData 를 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valHatchLoop = reader["HatchLoopData"];
	
	int nLoop;
	pEnt->hatchLoops.clear();
	for (int i = 0; i < nPath; i++)
	{
		nLoop = GetJsInt("nLoop", valHatchLoop[i], -1);
		vector_point3dbl loop;

		for (int j = 0; j < nLoop; j++)
		{
			Point3Dbl p;

			RJsonValue& valnLoop = valHatchLoop[i]["p"][j];

			p.x = GetJsDouble(0, valnLoop, -1.7E+308);
			p.y = GetJsDouble(1, valnLoop, -1.7E+308);
			p.z = GetJsDouble(2, valnLoop, -1.7E+308);

			loop.push_back(p);
		}
		pEnt->hatchLoops.push_back(loop);
	}

	pEnt->biDir = GetJsBool("biDir", reader, FALSE); 

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityRect(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntityRect* pEnt = new CEntityRect();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
	//pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	if (!reader.HasMember("center"))
	{
		_tracer(TRACE_ERROR, 1, L"center 를 지원하지 않는 파일입니다.");

		return FALSE;
	}

	RJsonValue& valcenter = reader["center"];
	pEnt->center.x = GetJsDouble(0, valcenter, -1.7E+308);
	pEnt->center.y = GetJsDouble(1, valcenter, -1.7E+308);
	pEnt->width = GetJsDouble("width", reader, -1.7E+308);
	pEnt->height = GetJsDouble("height", reader, -1.7E+308);
	pEnt->angle = GetJsDouble("angle", reader, -1.7E+308);

	pEnt->iDir = GetJsBool("iDir", reader, FALSE);
	pEnt->radius = GetJsDouble("radius", reader, -1.7E+308);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntitySpiral(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntitySpiral* pEnt = new CEntitySpiral();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
//	pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");

	if (!reader.HasMember("m_center"))
	{
		_tracer(TRACE_ERROR, 1, L"m_center 를 지원하지 않는 파일입니다.");
		return FALSE;
	}

	RJsonValue& valcenter = reader["m_center"];

	pEnt->m_center.x = GetJsDouble(0, valcenter, -1.7E+308);
	pEnt->m_center.y = GetJsDouble(1, valcenter, -1.7E+308);

	pEnt->m_radiusX = GetJsDouble("m_radiusX", reader, -1.7E+308);
	pEnt->m_radiusY = GetJsDouble("m_radiusY", reader, -1.7E+308);
	pEnt->m_turn = GetJsDouble("m_turn", reader, -1.7E+308);
	pEnt->m_rotAng = GetJsDouble("m_rotAng", reader, -1.7E+308);
	pEnt->m_resolution = GetJsDouble("m_resolution", reader, -1.7E+308);
	pEnt->m_bClose = GetJsBool("m_bClose", reader, FALSE);

	pEnt->m_dirOut2In = GetJsBool("m_dirOut2In", reader, FALSE);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);

	// 설정값 Entity에 적용
	pEnt->UpdateEntity(NULL, COleVariant::COleVariant(0.0));
	pEnt->SetDrawEntity(hEnt);

	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityArc(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntyArc* pEnt = new CEntyArc();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
	//pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	
	if (!reader.HasMember("center"))
	{
		_tracer(TRACE_ERROR, 1, L"center 를 지원하지 않는 파일입니다.");
		return FALSE;
	}
	
	rapidjson::Value& valcenter = reader["center"];
	pEnt->center.x = GetJsDouble(0, valcenter, -1.7E+308);
	pEnt->center.y = GetJsDouble(1, valcenter, -1.7E+308);
	pEnt->angle_arc = GetJsDouble("angle_arc", reader, -1.7E+308);
	pEnt->angle_st = GetJsDouble("angle_st", reader, -1.7E+308);
	pEnt->radius = GetJsDouble("radius", reader, -1.7E+308);
	
	pEnt->iDir = GetJsInt("iDir", reader, FALSE);


	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityCircle(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntyCircle* pEnt = new CEntyCircle();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
//	pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	if (!reader.HasMember("center"))
	{
		_tracer(TRACE_ERROR, 1, L"center 를 지원하지 않는 파일입니다.");
		return FALSE;
	}
	
	RJsonValue& valcenter = reader["center"];
	
	pEnt->center.x = GetJsDouble(0, valcenter, -1.7E+308);
	pEnt->center.y = GetJsDouble(1, valcenter, -1.7E+308);
	pEnt->radius = GetJsDouble("radius", reader, -1.7E+308);
	pEnt->StartAngle = GetJsDouble("StartAngle", reader, -1.7E+308);
	pEnt->iDir = GetJsInt("iDir", reader, -65565);
	pEnt->overpath = GetJsDouble("overpath", reader, -1.7E+308);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityEllipse(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntyEllipse* pEnt = new CEntyEllipse();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
	//pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	if (!reader.HasMember("center"))
	{
		_tracer(TRACE_ERROR, 1, L"center 를 지원하지 않는 파일입니다.");
		return FALSE;
	}
	
	RJsonValue& valcenter = reader["center"];
	
	pEnt->center.x = GetJsDouble(0, valcenter, -1.7E+308);
	pEnt->center.y = GetJsDouble(1, valcenter, -1.7E+308);
	pEnt->Rmajor = GetJsDouble("Rmajor", reader, -1.7E+308);
	pEnt->Rminor = GetJsDouble("Rminor", reader, -1.7E+308);
	pEnt->RotAngle = GetJsDouble("RotAngle", reader, -1.7E+308);
	pEnt->StartAngle = GetJsDouble("StartAngle", reader, -1.7E+308);
	pEnt->ArcAngle = GetJsDouble("ArcAngle", reader, -1.7E+308);
	pEnt->iDir = GetJsInt("iDir", reader, -65565);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityLine(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntyLine* pEnt = new CEntyLine();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
	//pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	if (!reader.HasMember("point0"))
	{
		_tracer(TRACE_ERROR, 1, L"point0 를 지원하지 않는 파일입니다.");
		return FALSE;
	}

	if (!reader.HasMember("point1"))
	{
		_tracer(TRACE_ERROR, 1, L"point1 를 지원하지 않는 파일입니다.");
		return FALSE;
	}

	rapidjson::Value& valPoint0 = reader["point0"];
	rapidjson::Value& valPoint1 = reader["point1"];

	pEnt->point0.x = GetJsDouble(0, valPoint0, -1.7E+308);
	pEnt->point0.y = GetJsDouble(1, valPoint0, -1.7E+308);
	pEnt->point1.x = GetJsDouble(0, valPoint1, -1.7E+308);
	pEnt->point1.y = GetJsDouble(1, valPoint1, -1.7E+308);
	pEnt->length = GetJsDouble("length", reader, -1.7E+308);
	pEnt->iDir = GetJsBool("iDir", reader, FALSE);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityPoint(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntyPoint* pEnt = new CEntyPoint();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
	//pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	if (!reader.HasMember("point0"))
	{
		_tracer(TRACE_ERROR, 1, L"point0 를 지원하지 않는 파일입니다.");
		return FALSE;
	}
	
	rapidjson::Value& valPoint = reader["point0"];
	
	pEnt->point0.x = GetJsDouble(0, valPoint, -1.7E+308);
	pEnt->point0.y = GetJsDouble(1, valPoint, -1.7E+308);
	pEnt->delay = GetJsInt("delay", reader, -65565);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}

BOOL CRapidJasonWork::ParseJsonEntityPolyline(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc)
{
	CEntyPolyline* pEnt = new CEntyPolyline();

	// Set Pen ID & DrawWobbel
	int *pPenID = pJob->GetPenIDPtr();
	pEnt->SetPen(pPenID);
	//pEnt->SetDrawWobbel(pDoc->m_config.m_bDrawWobbel);

	pEnt->m_strName = GetJsString("strName", reader, " ");
	pEnt->m_iFitType = GetJsInt("m_iFitType", reader, -65565);
	
	
	int nSize = 0;
	nSize = GetJsInt("nSize", reader, -65565);

	if (!reader.HasMember("m_points"))
	{
		_tracer(TRACE_ERROR, 1, L"m_points 를 지원하지 않는 파일입니다.");
		return FALSE;
	}

	rapidjson::Value& valPoint = reader["m_points"];

	for (int i = 0; i < nSize; i++)
	{
		PointPoly p;
		p.arc_ang = GetJsDouble("p.arc_ang", valPoint[i], -1.7E+308);
		p.arc_r = GetJsDouble("p.arc_r", valPoint[i], -1.7E+308);
		p.x = GetJsDouble("p.x", valPoint[i], -1.7E+308);
		p.y = GetJsDouble("p.y", valPoint[i], -1.7E+308);
		p.z = GetJsDouble("p.z", valPoint[i], -1.7E+308);

		pEnt->m_points.push_back(p);
	}

	pEnt->iDir = GetJsBool("iDir", reader, FALSE);

	// Set Add to Cad View
	lcEventsEnable(FALSE);
	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	HANDLE hEnt = pEnt->AddCurrEntityToBlock(hBlock);
	pEnt->SetDrawEntity(hEnt);
	lcEventsEnable(TRUE);

	// Set Entity Pointer to Job
	pJob->SetEntity(pEnt);

	return TRUE;
}