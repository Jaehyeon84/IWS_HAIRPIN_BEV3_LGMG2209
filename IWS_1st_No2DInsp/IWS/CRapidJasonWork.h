#pragma once

//RapidJson API Using 2024.04.11  ksu 헤더추가
#include "RapidJson/document.h"
#include "RapidJson/writer.h"
#include "RapidJson/stringbuffer.h"
#include "RapidJson/filereadstream.h"
#include "RapidJson/filewritestream.h"
#include "RapidJson/prettywriter.h"
#include "RapidJson/stringbuffer.h"
#include <fstream>

#include "IWSWork.h"

#include "Entity.h"
#include "EntityHatch.h"
#include "EntityRect.h"
#include "EntitySpiral.h"
#include "EntyArc.h"
#include "EntyCircle.h"
#include "EntyEllipse.h"
#include "EntyLine.h"
#include "EntyPoint.h"
#include "EntyPolyline.h"

typedef rapidjson::PrettyWriter<rapidjson::StringBuffer> RJsonWriter; // Rapid Json Writer
typedef rapidjson::Document RJsonReader; // Rapid Json Reader
typedef rapidjson::Value RJsonValue;	// Rapid Json Value

class CRapidJasonWork
{
// Member Variables ---------------------------------------------------------
protected:

public:


// Member Functions ---------------------------------------------------------
protected:
	// Get Value in Json
	int      GetJsInt(char* sec, RJsonValue& Val, int nDef);
	int      GetJsInt(int   idx, RJsonValue& Val, int nDef);
	double   GetJsDouble(char* sec, RJsonValue& Val, double dblDef);
	double   GetJsDouble(int   idx, RJsonValue& Val, double dblDef);
	CStringA GetJsString(char* sec, RJsonValue& Val, CStringA strDef);
	BOOL     GetJsBool(char* sec, RJsonValue& Val, BOOL bDef);

	// Make Json Datas
	void MakeJasonHeader(RJsonWriter& writer, CIWSDoc *pDoc);
	
	void MakeJasonHeaderHairpin(RJsonWriter& writer, CIWSDoc *pDoc);
	void MakeJasonHeaderZComp(RJsonWriter& writer, CIWSDoc *pDoc);
	void MakeJasonHeaderETC(RJsonWriter& writer, CIWSDoc *pDoc);

	void MakeJasonPens(RJsonWriter& writer, CIWSDoc *pDoc);
	void MakeJasonPen(RJsonWriter& writer, CLaserPen* pPen, int nPenNo);

	void MakeJasonWorks(RJsonWriter& writer, CIWSDoc *pDoc);
	void MakeJasonWorkHead(RJsonWriter& writer, CIWSDoc *pDoc);
	void MakeJasonJob(RJsonWriter& writer, CJobWeld* pJob);

	void MakeJasonEntity(RJsonWriter& writer, CEntity* pEntity, int nEntType);
	void MakeJasonEntityHatch(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityRect(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntitySpiral(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityArc(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityCircle(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityEllipse(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityLine(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityPoint(RJsonWriter& writer, CEntity* pEntity);
	void MakeJasonEntityPolyline(RJsonWriter& writer, CEntity* pEntity);

	BOOL OpenJsonFile(CString strFilePath, RJsonReader& reader);

	// Parse(Read) Json Values
	BOOL ParseJsonHeader(RJsonReader& reader, CIWSDoc *pDoc);

	BOOL ParseJsonHeaderHairpin(RJsonValue& reader, CIWSDoc *pDoc);
	BOOL ParseJsonHeaderZComp(RJsonValue& reader, CIWSDoc *pDoc);
	BOOL ParseJsonHeaderETC(RJsonValue& reader, CIWSDoc *pDoc);

	BOOL ParseJasonPens(RJsonReader& reader, CIWSDoc *pDoc);
	BOOL ParseJsonPen(RJsonValue& reader, CLaserPen *pPen, int nPenNo);

	BOOL ParseJasonWorks(RJsonReader& reader, CIWSDoc *pDoc);
	BOOL ParseJsonWorkHead(RJsonValue& reader, CIWSDoc *pDoc);
	BOOL ParseJsonWorkJobGroup(RJsonValue& reader, CIWSDoc *pDoc, IWS_VECTOR_JOB_GROUP* pJobGroup, int nGrpNo);
	BOOL ParseJsonWorkJobs(RJsonValue& reader, CIWSDoc *pDoc, IWS_VECTOR_JOB* pJobs, int nJobNo);

	BOOL ParseJsonEntity(RJsonValue& reader, CJobWeld* pJob, int nEntType, CIWSDoc *pDoc);
	BOOL ParseJsonEntityHatch(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityRect(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntitySpiral(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityArc(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityCircle(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityEllipse(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityLine(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityPoint(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);
	BOOL ParseJsonEntityPolyline(RJsonValue& reader, CJobWeld* pJob, CIWSDoc *pDoc);

public:
	CRapidJasonWork();
	virtual ~CRapidJasonWork();

	// Save Json Work File
	BOOL SaveWork(CFile *pf, CIWSDoc *pDoc);

	// Open Json Work File
	BOOL LoadWork(CString strFilePath, CIWSDoc *pDoc);
};

