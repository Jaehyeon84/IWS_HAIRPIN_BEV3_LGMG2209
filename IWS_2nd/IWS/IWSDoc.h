// 이 MFC 샘플 소스 코드는 MFC Microsoft Office Fluent 사용자 인터페이스("Fluent UI")를
// 사용하는 방법을 보여 주며, MFC C++ 라이브러리 소프트웨어에 포함된
// Microsoft Foundation Classes Reference 및 관련 전자 문서에 대해
// 추가적으로 제공되는 내용입니다.
// Fluent UI를 복사, 사용 또는 배포하는 데 대한 사용 약관은 별도로 제공됩니다.
// Fluent UI 라이선싱 프로그램에 대한 자세한 내용은
// https://go.microsoft.com/fwlink/?LinkId=238214.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

// IWSDoc.h: CIWSDoc 클래스의 인터페이스
//

#pragma once

#include "JobWeld.h"
#include "IWSWorkHairpin.h"
#include "LaserPen.h"
#include "ConfigIWS.h"
#include "CMelsecPLC.h"
#include "CEMiniSerial.h"
#include "EthernetPLC.h"
#include "MgrUser.h"

#define M_PI 3.14159265358979323846
#define UWM_INTERFACE_THREAD_MSG _T("Interface_Thread_Message")
#define INTERFACE_WELD_START 10
#define INTERFACE_INSPECT_START 20
#define D2R 0.0174532925199433 // Dgree to Radian

#include <vector>

class CHomeView;
class CIWSView;
class CMainFrame;

//typedef std::vector<CEntity>
class CIWSDoc : public CDocument
{
protected: // serialization에서만 만들어집니다.
	CIWSDoc() noexcept;
	DECLARE_DYNCREATE(CIWSDoc)

// 특성입니다.
private:
	BOOL m_bUseInterface;
public:
	CIWSView *m_pDrawView;
	CHomeView *m_pHomeView;
	CMainFrame *m_pMainFrm;
	//IWS_WORK m_work;
	CMelsecPLC m_plc;
	CEthernetPLC *m_plcData;
	BOOL m_bImportPLCData;
	IWS_STATUS m_status;
	
	CIWSWorkHairpin m_work;
	
	IWS_LPEN m_pens;
	CScanner *m_pScanner;

	CConfigIWS m_config;
	CMgrUser   m_User;

	CString m_strWorkPath;
	CString m_strExePath;
	CString m_strImgPath;
	CString m_strFileLaserPen;
	CString m_strConfigPath;

	CString m_strLastFilePath;
	CString m_strLastFileName;
	
	int m_iCurrentWork;
	BOOL m_bRunInterfaceThread;
	

	HANDLE m_hEvtOpenRecipe;

	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) -->
	CEMiniSerial m_CeminiSerial;	
	// sjyi 2022-06-08 시리얼 다시 살림(남경1차는 시리얼 통신) <--

	int m_nLastRcpIdx;

	//자동 좌표 보정 값 저장하는 변수
	std::vector<_IWS_LOCATION> m_Vec3DData, m_VecIWSData, m_VecFitdata;
	std::vector<_IWS_LOCATION> m_VecData;
	int m_nVecIndex, m_nVecCount;

// 작업입니다.
public:
//	void SaveFile(CString fileName, BOOL isCurrent = FALSE);
	void SaveFile(CString fileName, BOOL isCurrent = FALSE, BOOL bBackUP = TRUE);

	static UINT InterfaceThread(LPVOID lpParam);
	UINT InterfaceThreadBody();

	// 2019.09.23 mspark interface VSCAN
	//BOOL InspectStator();
	void GetRefImgSize(double *wid, double *height);
	void GetInspectResults(POINTF *pRes, int *nData = NULL);
	void ReqStitchImage(BOOL bScan, int* pWidthPxl, int* pHeightPxl,
		float* pWidthLength, float* pHeightLength);
	BOOL PeekAndPump();

	void SetWorkOffset();
	void ClearWorkOffset();
	
	CString GetExePath() { return m_strExePath; }

	void ChangeScanRange();
	POINTF GetScanCenter();
	void GetCcdResolution(float* resolX, float* resolY);	// yjko
	void GetWorkRange(float* maxScanRangeX, float* maxScanRangeY);	// yjko
	void GetCcdInfo(UINT* nWidth, UINT* nHeight, UINT* nChannel, CString* pStrSNCCD=NULL, int* pFR2Want=NULL); //yjko
	void SnapSave();

	void GetCcd2ImgInfo(UINT* nWidth, UINT* nHeight);

	void BackUpWork(CString strWorkFile);


// 재정의입니다.
private:
	void SetOffsetEntity(CJobWeld* jobWled);
	BOOL Process2D(CString strTokken, int nIdx, int nJob);
public:
	std::vector<CString> SplitString(CString strData);
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

	void ParseDxf();
	CString CreateFolder(CString strBasePath, CString strPath);
	void DrawAddEntity(HANDLE handle);
	void DrawDeleteEntity(HANDLE entity);
	void DrawSelectedEntity(HANDLE entity);
	void DrawChangedEntity(HANDLE entity);
	void DrawChangedEntityHatch(HANDLE entity);
	void DrawUpdateJob(CJob *pJob);
	void OpenLaserPen(CString strName);
	void SaveLaserPen(CString strName);
	void ImportDxf();
	void AddHatch(double dist, double angle);
	BOOL StartWork(BOOL bSimulation, double dRatioPwr = 1.0, double dRatioSpd = 1.0);

	void InitializeIWS();
	void TerminateIWS();
	BOOL OpenWork(CString strFilename);
	// TODO: Online 모드 사용 시 활성화 - hyun
	void SetUseInterface(BOOL bFlag) { m_bUseInterface = bFlag; };
	BOOL GetUseInterface() { return m_bUseInterface; };

	BOOL Update2DPosition();
	BOOL Update3DPosition();
	
	void ChangePrgNo();

	void SaveImageAndPos();
	BOOL _CreateDirectory(CString sDirPath);

	void MoveCenterPos(); // sjyi 2024.06.15 2차패턴 중심으로 이동

	// 2025.09.29 Jason 사용을 위한 함수 추가 --->
	// Get Job Vector
	IWS_VECTOR_JOB_GROUP*		GetJobs() { return &m_work.m_JobGroup; };
	IWS_LPEN *					GetPens() { return &m_pens; };
	CLaserPen					GetLaserPen(int nIndex) { return m_pens[nIndex]; };

	BOOL CheckJson(CString strPath); // 파일 확장자가 Json인지 아닌지 확인하는 함수
	// 2025.09.29 Jason 사용을 위한 함수 추가 <---

#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 구현입니다.
public:
	virtual ~CIWSDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	DECLARE_MESSAGE_MAP()

#ifdef SHARED_HANDLERS
	// 검색 처리기에 대한 검색 콘텐츠를 설정하는 도우미 함수
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg void OnFileSave();
	afx_msg void OnFileOpen();
	afx_msg void OnFileCurrToWork();
	afx_msg void OnFileHpConfig();
	afx_msg void OnFileImportDxf();
	afx_msg void OnFileShowInspect();
	afx_msg void OnMatrix();
	afx_msg void OnFileLocationInfoSave();
	
	double m_dCenOffX;
	double m_dCenOffY;
	double m_dBaseR;
	double m_dBaseH;
	double m_dHRRatio;

};
