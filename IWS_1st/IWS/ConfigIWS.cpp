#include "stdafx.h"
#include "ConfigIWS.h"

#include <WinCrypt.h>

#define AES_KEY_LENGTH     32
#define SOURCE_MULTIPLE    16
#define BLOCK_SIZE         16

CConfigIWS::CConfigIWS()
{
	//	m_bWeldAllAtOnce = FALSE;
	m_bUseInspect = FALSE;
	m_bUseCircleFit = FALSE;
	m_nLens = 1;

	m_dHpLegendStartAng = 180;
	m_dHpLegendDir = -1; // CW : 1, CCW : -1
	m_dHpLegendDia = 120.0f; //#define HP_TXT_DIA		120.0f
	//m_nInOutAirSleep = 1000;

	m_nLaserMax = 5000;

	// 분할 용접(레이어별)
	m_bDivisionWelding = 0;

	m_bMatrixByModel = 0;

	m_bTimeSync = FALSE;

	m_bUseHeightComp = FALSE;
	m_dHCompCenterX = 0.0;
	m_dHCompCenterY = 0.0;
	m_dHCompWD = 530.0;		// 420Lens

	//m_bUseHCompNGCheck = FALSE;
	//m_dHCompNGDistance = 10.0;

	//// 2023.04.26 sjyi Light Mode 추가
	//m_nLightMode = LIGHT_MODE_DIO;

	// 2023.06.12 sjyi 부분스캔 적용
	m_bUsePartialScan = FALSE;

	// 2023.12.13 sjyi 항상 스캔하는 옵션 JigA, JigB 구분
//	m_bUseAlwaysScan = FALSE;
	m_bUseAlwaysScanJigA = FALSE;
	m_bUseAlwaysScanJigB = FALSE;
//	m_bUseAlwaysScanRework = FALSE; // sjyi 2024.05.28 재용접 스캔여부 추가


	m_nScanSleepTime = 30;

	// 2023.07.12 dhpark 2D
	m_b3DUse = FALSE;

	// 2023.10.10 dhpark 1:1 Matching
	m_bUseOneByOnMatrix = FALSE;
	for (int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
	{
		m_dOnebyOneMatchingX_JigA[i] = 1.f;
		m_dOnebyOneMatchingY_JigA[i] = 1.f;
		m_dOnebyOneMatchingX_JigB[i] = 1.f;
		m_dOnebyOneMatchingY_JigB[i] = 1.f;
	}

	m_bUseNGCheckJigA = FALSE;
	m_dNGDistanceJigA = 10.0;


//	m_pMatrixA = NULL;
//	m_pMatrixB = NULL;
}


CConfigIWS::~CConfigIWS()
{
//	delete[] m_pMatrixA;
//	delete[] m_pMatrixB;
}

void CConfigIWS::ReadConfig()
{
	CIni ini;
	ini.SetPathName(m_strConfigPath);
	m_dWPCalibStep = ini.GetDouble(STR_SECTION_SYSTEM, L"WeldPtCalibStep", 45.0f);
	m_nCard = ini.GetInt(STR_SECTION_SYSTEM, STR_KEY_NCARD, 1);
	m_nLens = ini.GetInt(STR_SECTION_SYSTEM, L"NumOfLens", 1);
	m_iCurrLens = ini.GetInt(STR_SECTION_SYSTEM, L"CurrentLens", 0);
	m_bUseHairpin = ini.GetBool(STR_SECTION_SYSTEM, L"UseHairpin", TRUE);
	m_nLightPort = ini.GetInt(STR_SECTION_SYSTEM, L"LightPort", 1);
	m_nLightPower = ini.GetInt(STR_SECTION_SYSTEM, L"LightPower", 255);
	m_bUseLight = ini.GetInt(STR_SECTION_SYSTEM, L"UseLight", 1);
	m_strPassword = ini.GetString(L"IPE", L"EDW", L"");
	// 2024.02.15 jh.kim 주석
	//if (m_strPassword != "")
	//{
	//	char in[255];
	//	char out[255];
	//	USES_CONVERSION;
	//	memcpy(in, W2A(m_strPassword), 255);
	//	Decrypt(in, out);
	//	m_strPassword = A2W(out);
	//}

	//m_nMatrixDiv = ini.GetInt(STR_SECTION_SYSTEM, _T("MatrixDivision"), MATRIX_DIVISION);
	m_nMatrixDiv = ini.GetInt(STR_SECTION_SYSTEM, _T("MatrixDivision"), 4);

	//if (m_pMatrixA) delete[] m_pMatrixA;
	//if (m_pMatrixB) delete[] m_pMatrixB;
	//m_pMatrixA = new CALI_MATRIX[m_nMatrixDiv];
	//m_pMatrixB = new CALI_MATRIX[m_nMatrixDiv];

	// RCP Size
	for (int i = 0; i < 6; i++)
	{
		CString strTmp;

		strTmp.Format(_T("RcpSacle_%d"), i);
		m_dRcpScale[i] = ini.GetDouble(L"SIZE", strTmp, 1.0);

		strTmp.Format(_T("RcpAngle_%d"), i);
		m_dRcpAngle[i] = ini.GetDouble(L"SIZE", strTmp, 0.0);

		strTmp.Format(_T("RcpOffsetX_%d"), i);
		m_dRcpOffsetX[i] = ini.GetDouble(L"SIZE", strTmp, 0.0);

		strTmp.Format(_T("RcpOffsetY_%d"), i);
		m_dRcpOffsetY[i] = ini.GetDouble(L"SIZE", strTmp, 0.0);
	}

	// 2023.07.12 2D Setting add
	// RCP Size
	m_b3DUse = ini.GetBool(L"SIZE", L"3D_USE", FALSE);
	for (int i = 0; i < 6; i++)
	{
		CString strTmp;

		strTmp.Format(_T("2D_RcpSacle_%d"), i);
		m_d2DRcpScale[i] = ini.GetDouble(L"SIZE", strTmp, 1.0);

		strTmp.Format(_T("2D_RcpAngle_%d"), i);
		m_d2DRcpAngle[i] = ini.GetDouble(L"SIZE", strTmp, 0.0);

		strTmp.Format(_T("2D_RcpOffsetX_%d"), i);
		m_d2DRcpOffsetX[i] = ini.GetDouble(L"SIZE", strTmp, 0.0);

		strTmp.Format(_T("2D_RcpOffsetY_%d"), i);
		m_d2DRcpOffsetY[i] = ini.GetDouble(L"SIZE", strTmp, 0.0);
	}

	//for (int i = 0; i < 10; i++)
	//{
	//	CString strTmp;
	//	strTmp.Format(_T("Pen%d"), i);
	//	m_dPenPower[i] = ini.GetDouble(L"PENPW", strTmp, 0.0);

	//	strTmp.Format(_T("PenSpeed%d"), i);
	//	m_dPenSpeed[i] = ini.GetDouble(L"PENPW", strTmp, 0.0);
	//}

	//21.05.27 Pen 별 Weld, Jump Speed Raange Limit
	for (int i = 0; i < MAX_PENS; i++)
	{
		CString strTmp;
		strTmp.Format(_T("Pen%d_JumpSpeed_Min"), i);
		m_dJumpSpdmin[i] = ini.GetDouble(L"PEN_SPEED_RANGE", strTmp, 0.0);

		strTmp.Format(_T("Pen%d_JumpSpeed_Max"), i);
		m_dJumpSpdmax[i] = ini.GetDouble(L"PEN_SPEED_RANGE", strTmp, 0.0);

		strTmp.Format(_T("Pen%d_WeldSpeed_Min"), i);
		m_dWeldSpdmin[i] = ini.GetDouble(L"PEN_WELDSPEED_RANGE", strTmp, 0.0);

		strTmp.Format(_T("Pen%d_WeldSpeed_Max"), i);
		m_dWeldSpdmax[i] = ini.GetDouble(L"PEN_WELDSPEED_RANGE", strTmp, 0.0);
	}

	m_arCorrParam.RemoveAll();
	for (int i = 0; i < m_nLens; i++) {
		CorrParam corrParam;
		CString section;
		section.Format(_T("%s%d"), STR_SECTION_CORRPARAM, i);
		int n;
		n = ini.GetString(section, L"Name", corrParam.szName, MAX_PATH, _T(""));
		corrParam.szName[n] = '\0';
		n = ini.GetString(section, L"CCD_Cal", corrParam.szCcdFile, MAX_PATH, _T(""));
		corrParam.szCcdFile[n] = '\0';
		n = ini.GetString(section, STR_KEY_CORR_FILE, corrParam.szCorFile, MAX_PATH, _T(""));
		corrParam.szCorFile[n] = '\0';
		corrParam.dblScaleFactor = ini.GetDouble(section, STR_KEY_CORR_SCALEFACTOR, 100);
		corrParam.dblKX = ini.GetDouble(section, STR_KEY_CORR_KX, 1.0);
		corrParam.dblKY = ini.GetDouble(section, STR_KEY_CORR_KY, 1.0);
		corrParam.dblOffsetX = ini.GetDouble(section, STR_KEY_CORR_OFFSETX, 0.0);
		corrParam.dblOffsetY = ini.GetDouble(section, STR_KEY_CORR_OFFSETY, 0.0);
		corrParam.dblTheta = ini.GetDouble(section, STR_KEY_CORR_THETA, 0.0);
		corrParam.dblWorkX = ini.GetDouble(section, L"WorkX", 100.0);
		corrParam.dblWorkY = ini.GetDouble(section, L"WorkY", 100.0);
		corrParam.dblImgCenX = ini.GetDouble(section, L"ImgCenX", 0.0);
		corrParam.dblImgCenY = ini.GetDouble(section, L"ImgCenY", 0.0);
		corrParam.dblImgScaleX = ini.GetDouble(section, L"ImgScaleX", 1.0);
		corrParam.dblImgScaleY = ini.GetDouble(section, L"ImgScaleY", 1.0);
		corrParam.dblLensPowerX = ini.GetDouble(section, L"LensPowerX", 1.0);
		corrParam.dblLensPowerY = ini.GetDouble(section, L"LensPowerY", 1.0);
		corrParam.dblVSsx = ini.GetDouble(section, L"VScan_sx", -10);
		corrParam.dblVSsy = ini.GetDouble(section, L"VScan_sy", 10);
		corrParam.dblVSex = ini.GetDouble(section, L"VScan_ex", 10);
		corrParam.dblVSey = ini.GetDouble(section, L"VScan_ey", -10);

		//DHChoi 20200624
		corrParam.bUseCalib = ini.GetBool(section, L"VScan_UseCalib", FALSE);
		corrParam.iCalibX = ini.GetInt(section, L"VScan_CalibX", 0);
		corrParam.iCalibY = ini.GetInt(section, L"VScan_CalibY", 0);
		corrParam.iVSOverlap = ini.GetInt(section, L"VScan_Overlap", 0);
		n = ini.GetString(section, L"VScan_CalibData", corrParam.szCalibDir, MAX_PATH, _T(""));
		corrParam.szCalibDir[n] = '\0';


		m_arCorrParam.Add(corrParam);
	}
	m_nCcdHeight = ini.GetInt(STR_SECTION_VISION, L"CCD_Height", 1024);
	m_nCcdWidth = ini.GetInt(STR_SECTION_VISION, L"CCD_Width", 1280);
	m_fCcdResol_X = (float)ini.GetDouble(STR_SECTION_VISION, L"CCD_ResoluX", 5.3);
	m_fCcdResol_Y = (float)ini.GetDouble(STR_SECTION_VISION, L"CCD_ResoluY", 5.3);
	m_nWorkList = ini.GetInt(STR_SECTION_WORKLIST, L"nWorkList", 0);
	for (int i = 0; i < m_nWorkList; i++)
	{
		CString strKey;
		strKey.Format(_T("%s%d"), L"Work", i);
		CString strVal;
		strVal = ini.GetString(STR_SECTION_WORKLIST, strKey);
		m_arWorkList.Add(strVal);
	}
	/*m_hp.nSlot = ini.GetInt(STR_SECTION_HAIRPIN, L"nSlot", 48);
	m_hp.nLayer = ini.GetInt(STR_SECTION_HAIRPIN, L"nLayer", 4);
	m_hp.startAng = ini.GetDouble(STR_SECTION_HAIRPIN, L"startAng", 0);
	m_hp.HpSize = ini.GetDouble(STR_SECTION_HAIRPIN, L"PinSize", 0.7);
	m_hp.HpSizeW = ini.GetDouble(STR_SECTION_HAIRPIN, L"HpSizeW", 0.1);
	m_hp.HpSize2nd = ini.GetDouble(STR_SECTION_HAIRPIN, L"HpSize2nd", 0.4);
	m_hp.HpOverLen = ini.GetDouble(STR_SECTION_HAIRPIN, L"HpOverlength", 0.5);*/
	m_dDetectRange = ini.GetDouble(STR_SECTION_SYSTEM, L"DetectRange", 2.0);
	//	m_bWeldAllAtOnce = ini.GetBool(STR_SECTION_SYSTEM, L"WeldAllAtOnce", FALSE);
	m_bUse2DProc = ini.GetBool(STR_SECTION_SYSTEM, L"Use2DProc", FALSE);
	m_bUseStartRoom = ini.GetBool(STR_SECTION_SYSTEM, L"UseStartRoom", FALSE);
	m_bUseLegend = ini.GetBool(STR_SECTION_SYSTEM, L"UseLegend", FALSE);

	m_dHpLegendStartAng = ini.GetDouble(STR_SECTION_SYSTEM, L"HpLegendStartAng", 180.0);
	m_dHpLegendDir = ini.GetDouble(STR_SECTION_SYSTEM, L"HpLegendDir", -1);
	m_dHpLegendDia = ini.GetDouble(STR_SECTION_SYSTEM, L"dHpLegendDia", 120.0f);

	// 2022.08.31 jh.kim 추가 - 분할 용접 사용 유무
	m_bDivisionWelding = ini.GetBool(STR_SECTION_SYSTEM, L"UseDivisionWelding", FALSE);

	m_bMatrixByModel = ini.GetBool(STR_SECTION_SYSTEM, L"UseMatrixByModel", FALSE);

	m_bTimeSync = ini.GetBool(STR_SECTION_SYSTEM, L"UseTimeSync", FALSE);

	m_bReconnectPLC = ini.GetBool(STR_SECTION_SYSTEM, L"UseReconnectPLC", TRUE);

	// 2023.10.09 dhpark - 1 : 1 Matrix 사용 유무
	m_bUseOneByOnMatrix = ini.GetBool(STR_SECTION_SYSTEM, L"UseOneByOneMatrix", FALSE);

	// 2024.02.05 jh.kim 추가 - 용접 후 좌표 저장 유무
	m_bUseWeldPosSave = ini.GetBool(STR_SECTION_SYSTEM, L"UseWeldPosSave", FALSE);

	m_nJumpDelay = ini.GetInt(STR_SECTION_SYSTEM, L"JumpDelay", 1);		//20251202 

	/*double dia;
	for (int i = 0; i < MAX_HAIRPIN_LAYER; i++) {
		CString str;
		str.Format(L"LayerDia%d", i);
		if (i % 2)  dia = m_hp.HpSize * 6;
		else
			dia = m_hp.HpSize * 2;
		m_hp.LayerDia[i] =ini.GetDouble(STR_SECTION_HAIRPIN, str, 40 + i*dia);
	}
	m_hp.nCntP1 = ini.GetInt(STR_SECTION_HAIRPIN, L"nCntP1", 1);
	m_hp.nPenP1 = ini.GetInt(STR_SECTION_HAIRPIN, L"nPenP1", 1);
	m_hp.nWeldShapeP1 = ini.GetInt(STR_SECTION_HAIRPIN, L"nWeldShapeP1", 1);
	m_hp.nWeldZigZagFreqP1 = ini.GetInt(STR_SECTION_HAIRPIN, L"nWeldZigZagFreqP1", 3);

	m_hp.nCntP2 = ini.GetInt(STR_SECTION_HAIRPIN, L"nCntP2", 1);
	m_hp.nPenP2 = ini.GetInt(STR_SECTION_HAIRPIN, L"nPenP2", 1);
	m_hp.nWeldShapeP2 = ini.GetInt(STR_SECTION_HAIRPIN, L"nWeldShapeP2", 1);
	m_hp.nWeldZigZagFreqP2 = ini.GetInt(STR_SECTION_HAIRPIN, L"nWeldZigZagFreqP2", 3);

	m_hp.nSwitchWelding = ini.GetInt(STR_SECTION_HAIRPIN, L"SwitchWelding", 0);
	m_hp.nWeldType = ini.GetInt(STR_SECTION_HAIRPIN, L"WeldType", 0);
	m_hp.nStartRoom = ini.GetInt(STR_SECTION_HAIRPIN, L"StartRoom", 0);*/

	m_cp.dModel1_X = ini.GetDouble(STR_SECTION_CENTER, L"Model1_X", 0);
	m_cp.dModel1_Y = ini.GetDouble(STR_SECTION_CENTER, L"Model1_Y", 0);
	m_cp.dModel2_X = ini.GetDouble(STR_SECTION_CENTER, L"Model2_X", 0);
	m_cp.dModel2_Y = ini.GetDouble(STR_SECTION_CENTER, L"Model2_Y", 0);
	m_cp.dModel3_X = ini.GetDouble(STR_SECTION_CENTER, L"Model3_X", 0);
	m_cp.dModel3_Y = ini.GetDouble(STR_SECTION_CENTER, L"Model3_Y", 0);
	m_cp.dModel4_X = ini.GetDouble(STR_SECTION_CENTER, L"Model4_X", 0);
	m_cp.dModel4_Y = ini.GetDouble(STR_SECTION_CENTER, L"Model4_Y", 0);

	m_bUsePLC = ini.GetBool(STR_SECTION_SYSTEM, L"UsePLC", FALSE);
	m_strPLC_IP = ini.GetString(STR_SECTION_SYSTEM, L"PLC_IP", L"192.168.3.1");
	m_iPLC_Port = ini.GetInt(STR_SECTION_SYSTEM, L"PLC_PORT", 8100);
	m_nPLC_Read  = ini.GetInt(STR_SECTION_SYSTEM, L"PLC_READ", 30100);
	m_nPLC_Write = ini.GetInt(STR_SECTION_SYSTEM, L"PLC_WRITE", 30000);

	// 2022.09.26 jh.kim 추가 - Ford3차 : MES 전송 데이터
	m_bUseWritePattern = ini.GetBool(STR_SECTION_SYSTEM, L"UseWritePattern", FALSE);


	m_bUseInspect = ini.GetBool(STR_SECTION_SYSTEM, L"INSPECT", FALSE);
	m_iLaser = ini.GetInt(STR_SECTION_SYSTEM, L"LaserModel", IPG_CW_YLR6000QCW);
	m_bUseCircleFit = ini.GetBool(STR_SECTION_SYSTEM, L"UseCircleFit", FALSE);
	//m_nInOutAirSleep = ini.GetInt(STR_SECTION_SYSTEM, L"CringSleep", 1000);

	m_nJigNo = ini.GetInt(STR_SECTION_SYSTEM, L"JIG_NO", 1);

	// 2022-07-08 Matrix 결과 JigA, JigB 구분을 위해 수정 ---->
	//// 2022-07-07 Matrix 관련 코드 다시 활성화 --->
	////Calibration Matrix
	//m_dCalibMtx_a11 = ini.GetDouble(STR_SECTION_CALIBMETRIX, L"a11", 1.0f);
	//m_dCalibMtx_a12 = ini.GetDouble(STR_SECTION_CALIBMETRIX, L"a12", 0.0f);
	//m_dCalibMtx_a13 = ini.GetDouble(STR_SECTION_CALIBMETRIX, L"a13", 0.0f);
	//m_dCalibMtx_a21 = ini.GetDouble(STR_SECTION_CALIBMETRIX, L"a21", 0.0f);
	//m_dCalibMtx_a22 = ini.GetDouble(STR_SECTION_CALIBMETRIX, L"a22", 1.0f);
	//m_dCalibMtx_a23 = ini.GetDouble(STR_SECTION_CALIBMETRIX, L"a23", 0.0f);
	//// 2022-07-07 Matrix 관련 코드 다시 활성화 <---


	// Jig A Matrix
	CString sMsg = L"";
	//for (int i = 0; i < MATRIX_DIVISION; i++) {
	for (int i = 0; i < m_nMatrixDiv; i++) {
		sMsg.Format(L"M%d", i + 1);
		m_pMatrixA[i].a11 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a11", 1.0f);
		m_pMatrixA[i].a12 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a12", 0.0f);
		m_pMatrixA[i].a13 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a13", 0.0f);
		m_pMatrixA[i].a21 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a21", 0.0f);
		m_pMatrixA[i].a22 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a22", 1.0f);
		m_pMatrixA[i].a23 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a23", 0.0f);
	}

	// Jig B Matrix
	sMsg = L"";
	//for (int i = 0; i < MATRIX_DIVISION; i++) {
	for (int i = 0; i < m_nMatrixDiv; i++) {
		sMsg.Format(L"M%d", i + 1);
		m_pMatrixB[i].a11 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a11", 1.0f);
		m_pMatrixB[i].a12 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a12", 0.0f);
		m_pMatrixB[i].a13 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a13", 0.0f);
		m_pMatrixB[i].a21 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a21", 0.0f);
		m_pMatrixB[i].a22 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a22", 1.0f);
		m_pMatrixB[i].a23 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a23", 0.0f);
	}
	// 2022-07-08 Matrix 결과 JigA, JigB 구분을 위해 수정 <----


	// 1:1 Matching
	for(int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++) 
	{
		sMsg.Format(L"X %d", i + 1);
		m_dOnebyOneMatchingX_JigA[i] = ini.GetDouble(STR_SECTION_CALIB_ONEBYONE_JIGA, sMsg, 1.0f);
		sMsg.Format(L"Y %d", i + 1);
		m_dOnebyOneMatchingY_JigA[i] = ini.GetDouble(STR_SECTION_CALIB_ONEBYONE_JIGA, sMsg, 1.0f);
	}

	for(int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
	{
		sMsg.Format(L"X %d", i + 1);
		m_dOnebyOneMatchingX_JigB[i] = ini.GetDouble(STR_SECTION_CALIB_ONEBYONE_JIGB, sMsg, 1.0f);
		sMsg.Format(L"Y %d", i + 1);
		m_dOnebyOneMatchingY_JigB[i] = ini.GetDouble(STR_SECTION_CALIB_ONEBYONE_JIGB, sMsg, 1.0f);
	}

	
	//Pattern Limit
	//m_d1StLength = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"1StLength", 0.7f);
	m_d1StWidth = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"1StWidth", 0.1f);
	m_n1StRepeat = ini.GetInt(STR_SECTION_PTRNLIMIT, L"1StRepeat", 1);	
	//m_d2ndLength = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"2ndLength", 0.5f);
	m_d2ndWidth = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"2ndWidth", 0.4f);
	m_n2ndRepeat = ini.GetInt(STR_SECTION_PTRNLIMIT, L"2ndRepeat", 1);
	m_dLimitEnergy = ini.GetInt(STR_SECTION_PTRNLIMIT, L"EnergyLimit", 10000);
	m_nLaserMax = ini.GetInt(STR_SECTION_PTRNLIMIT, L"LaserMaxPower", 5000);


	// 2023.04.07 sjyi 높이 변화에 따른 좌표 보상 사용여부
	m_bUseHeightComp = ini.GetBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_USE, FALSE);
	//m_dHCompRefHeight = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_REF_H, 0.0);
	CString strKey;
	// 77 Model
	strKey.Format(_T("%s_77Model_JigA"), STR_KEY_HCOMP_REF_H);
	m_dHCompRefHeight[0][0] = ini.GetDouble(STR_SECTION_HEIGTCOMP, strKey, 0.0);
	strKey.Format(_T("%s_77Model_JigB"), STR_KEY_HCOMP_REF_H);
	m_dHCompRefHeight[0][1] = ini.GetDouble(STR_SECTION_HEIGTCOMP, strKey, 0.0);

	// 76 Model
	strKey.Format(_T("%s_76Model_JigA"), STR_KEY_HCOMP_REF_H);
	m_dHCompRefHeight[1][0] = ini.GetDouble(STR_SECTION_HEIGTCOMP, strKey, 0.0);
	strKey.Format(_T("%s_76Model_JigB"), STR_KEY_HCOMP_REF_H);
	m_dHCompRefHeight[1][1] = ini.GetDouble(STR_SECTION_HEIGTCOMP, strKey, 0.0);

	//m_bUseHCompNGCheck = ini.GetBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_USE_NGCHK, FALSE);
	//m_dHCompNGDistance = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_NG_DISTANCE, 10.0);
	m_bSaveHeightComp = ini.GetBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_SAVE_RESULT, FALSE);

	m_dHCompCenterX = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_CENTER_X, 0.0);
	m_dHCompCenterX = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_CENTER_Y, 0.0);
	m_dHCompWD = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_WD, 530.0);
	m_dHCompTCMaxAngle = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_TC_ANG_MAX, 10.0);
	m_dHCompWorkRadius = ini.GetDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_WORKING_RADIUS, 100.0);	
	m_bViewHCompManual = ini.GetBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_VIEW_MANUAL, FALSE);

	// 2023.06.12 sjyi 부분 스캔 기능 추가
	m_bUsePartialScan = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_PARTIAL, FALSE);
	m_strPartialScanMap = ini.GetString(STR_SECTION_VISIONSCAN, STR_KEY_VS_PARTIAL_MAP_PATH);

	// 2023.12.13 sjyi 항상 스캔하는 옵션, JigA, JigB 구분
	//m_bUseAlwaysScan = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_ALWAYS_SCAN, FALSE);
	m_bUseAlwaysScanJigA = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_ALWAYS_SCAN_JIGA, FALSE);
	m_bUseAlwaysScanJigB = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_ALWAYS_SCAN_JIGB, FALSE);
//	m_bUseAlwaysScanRework = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_REWORK, FALSE);
	m_bUseAlwaysScanReworkJigA = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_REWORK_A, FALSE);
	m_bUseAlwaysScanReworkJigB = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_REWORK_B, FALSE);
	m_bUseAlwaysScanWeldJigA = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_WELD_A, FALSE);
	m_bUseAlwaysScanWeldJigB = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_WELD_B, FALSE);

	m_nScanSleepTime = ini.GetInt(STR_SECTION_VISIONSCAN, STR_KEY_VS_SCAN_SLEEP, 30);
	m_bSaveImage = ini.GetBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_SAVE_IMAGE, FALSE);


	// 2024.02.12 jh.kim 추가 - Image position scale, offset
	m_dPosScaleX  = ini.GetDouble(STR_SECTION_VISIONSCAN, L"PosScaleX", 1.0);
	m_dPosScaleY  = ini.GetDouble(STR_SECTION_VISIONSCAN, L"PosScaleY", 1.0);
	m_dPosOffsetX = ini.GetDouble(STR_SECTION_VISIONSCAN, L"PosOffsetX", 0);
	m_dPosOffsetY = ini.GetDouble(STR_SECTION_VISIONSCAN, L"PosOffsetY", 0);

	ReadVScanMap();

	// sjyi 2024.05.28 추가 모래시계, 스파이럴 패턴 사용 여부
	m_bUseSpiralPtn = ini.GetBool(STR_SECTION_SYSTEM, L"UseSpiralPtn", FALSE);
	m_bUseSandGlassPtn = ini.GetBool(STR_SECTION_SYSTEM, L"UseSandGlassPtn", FALSE);
	m_bUseSortieRepeat = ini.GetBool(STR_SECTION_SYSTEM, L"UseSortirRepeat", FALSE);

	m_bSavePos = ini.GetBool(STR_SECTION_VISIONSCAN, L"SavePos", FALSE);

	// sjyi 2024.06.19 추가, 거리 체크 기능
	m_bUseNGCheckJigA = ini.GetBool(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_USE_CHECK_JIGA, FALSE);
	m_bUseNGCheckJigB = ini.GetBool(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_USE_CHECK_JIGB, FALSE);
	m_dNGDistanceJigA = ini.GetDouble(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_NG_RANGE_JIGA, 10.0);
	m_dNGDistanceJigB = ini.GetDouble(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_NG_RANGE_JIGB, 10.0);
	m_strPosJigA = ini.GetString(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_POS_PATH_JIGA, _T(""));
	m_strPosJigB = ini.GetString(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_POS_PATH_JIGB, _T(""));

	CFileFind finder;
	BOOL bFlag = finder.FindFile(m_strConfigPath);
	if (!bFlag)
		WriteConfig();
}

void CConfigIWS::WriteConfig()
{
	BackUpConfig(); // sjyi 2024.06.14 추가, 안전 및 수정사항 확인을 위해 추가

	CIni ini;
	ini.SetPathName(m_strConfigPath);
	ini.WriteDouble(STR_SECTION_SYSTEM, L"WeldPtCalibStep", m_dWPCalibStep);
	ini.WriteInt(STR_SECTION_SYSTEM, STR_KEY_NCARD, m_nCard);
	ini.WriteInt(STR_SECTION_SYSTEM, L"NumOfLens", m_nLens);
	ini.WriteInt(STR_SECTION_SYSTEM, L"CurrentLens", m_iCurrLens);
	ini.WriteInt(STR_SECTION_SYSTEM, L"LightPort", m_nLightPort);
	ini.WriteInt(STR_SECTION_SYSTEM, L"LightPower", m_nLightPower);
	ini.WriteInt(STR_SECTION_SYSTEM, L"UseHairpin", m_bUseHairpin);
	ini.WriteInt(STR_SECTION_SYSTEM, L"UseCircleFit", m_bUseCircleFit);
	ini.WriteInt(STR_SECTION_SYSTEM, L"UseLight", m_bUseLight);

	ini.WriteInt(STR_SECTION_SYSTEM, _T("MatrixDivision"), m_nMatrixDiv);

	ini.WriteInt(STR_SECTION_SYSTEM, L"JumpDelay", m_nJumpDelay);			//20251202



	//ini.WriteInt(STR_SECTION_SYSTEM, L"CringSleep", m_nInOutAirSleep);	
	/*if (m_strPassword != "")
	{
		char in[255];
		char out[255];

		USES_CONVERSION;
		memcpy(in, W2A(m_strPassword), 255);

		Encrypt(in, out, 255);
		m_strPassword = A2W(out);
	}
	ini.WriteString(L"IPE", L"EDW", m_strPassword);*/

	for (int i = 0; i < 6; i++)
	{
		CString strTmp;

		strTmp.Format(_T("RcpSacle_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_dRcpScale[i]);

		strTmp.Format(_T("RcpAngle_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_dRcpAngle[i]);

		strTmp.Format(_T("RcpOffsetX_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_dRcpOffsetX[i]);

		strTmp.Format(_T("RcpOffsetY_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_dRcpOffsetY[i]);
	}

	// 2023.07.12 2D Setting add
	ini.WriteBool(L"SIZE", L"3D_USE", m_b3DUse);
	for (int i = 0; i < 6; i++)
	{
		CString strTmp;

		strTmp.Format(_T("2D_RcpSacle_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_d2DRcpScale[i]);

		strTmp.Format(_T("2D_RcpAngle_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_d2DRcpAngle[i]);

		strTmp.Format(_T("2D_RcpOffsetX_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_d2DRcpOffsetX[i]);

		strTmp.Format(_T("2D_RcpOffsetY_%d"), i);
		ini.WriteDouble(L"SIZE", strTmp, m_d2DRcpOffsetY[i]);
	}

	//for (int i = 0; i < 10; i++)
	//{
	//	CString strTmp;
	//	strTmp.Format(_T("Pen%d"), i);
	//	ini.WriteDouble(L"PENPW", strTmp, m_dPenPower[i]);

	//	strTmp.Format(_T("PenSpeed%d"), i);
	//	ini.WriteDouble(L"PENPW", strTmp, m_dPenSpeed[i]);
	//}
	//Pen 별 Weld, Jump Speed Raange Limit
	for (int i = 0; i < MAX_PENS; i++)
	{
		CString strTmp;
		strTmp.Format(_T("Pen%d_JumpSpeed_Min"), i);
		ini.WriteDouble(L"PEN_SPEED_RANGE", strTmp, m_dJumpSpdmin[i]);

		strTmp.Format(_T("Pen%d_JumpSpeed_Max"), i);
		ini.WriteDouble(L"PEN_SPEED_RANGE", strTmp, m_dJumpSpdmax[i]);

		strTmp.Format(_T("Pen%d_WeldSpeed_Min"), i);
		ini.WriteDouble(L"PEN_WELDSPEED_RANGE", strTmp, m_dWeldSpdmin[i]);

		strTmp.Format(_T("Pen%d_WeldSpeed_Max"), i);
		ini.WriteDouble(L"PEN_WELDSPEED_RANGE", strTmp, m_dWeldSpdmax[i]);
	}

	for (int i = 0; i < m_arCorrParam.GetSize(); i++) {
		CString section;
		CorrParam *p = &(m_arCorrParam[i]);
		section.Format(_T("CorrParam%d"), i);
		ini.WriteString(section, L"Name", p->szName);
		ini.WriteString(section, L"CCD_Cal", p->szCcdFile);
		ini.WriteString(section, STR_KEY_CORR_FILE, p->szCorFile);
		ini.WriteDouble(section, STR_KEY_CORR_SCALEFACTOR, p->dblScaleFactor);
		ini.WriteDouble(section, STR_KEY_CORR_KX, p->dblKX);
		ini.WriteDouble(section, STR_KEY_CORR_KY, p->dblKY);
		ini.WriteDouble(section, STR_KEY_CORR_OFFSETX, p->dblOffsetX);
		ini.WriteDouble(section, STR_KEY_CORR_OFFSETY, p->dblOffsetY);
		ini.WriteDouble(section, STR_KEY_CORR_THETA, p->dblTheta);
		ini.WriteDouble(section, L"WorkX", p->dblWorkX);
		ini.WriteDouble(section, L"WorkY", p->dblWorkY);
		ini.WriteDouble(section, L"ImgCenX", p->dblImgCenX);
		ini.WriteDouble(section, L"ImgCenY", p->dblImgCenY);
		ini.WriteDouble(section, L"ImgScaleX", p->dblImgScaleX);
		ini.WriteDouble(section, L"ImgScaleY", p->dblImgScaleY);
		ini.WriteDouble(section, L"LensPowerX", p->dblLensPowerX);
		ini.WriteDouble(section, L"LensPowerY", p->dblLensPowerY);
		ini.WriteDouble(section, L"VScan_sx", p->dblVSsx);
		ini.WriteDouble(section, L"VScan_sy", p->dblVSsy);
		ini.WriteDouble(section, L"VScan_ex", p->dblVSex);
		ini.WriteDouble(section, L"VScan_ey", p->dblVSey);

		//DHChoi 20200624
		ini.WriteBool(section, L"VScan_UseCalib", p->bUseCalib);
		ini.WriteInt(section, L"VScan_CalibX", p->iCalibX);
		ini.WriteInt(section, L"VScan_CalibY", p->iCalibY);
		ini.WriteInt(section, L"VScan_Overlap", p->iVSOverlap);
		ini.WriteString(section, L"VScan_CalibData", p->szCalibDir);
	}

	ini.WriteInt(STR_SECTION_VISION, L"CCD_Height", m_nCcdHeight);
	ini.WriteInt(STR_SECTION_VISION, L"CCD_Width", m_nCcdWidth);
	ini.WriteDouble(STR_SECTION_VISION, L"CCD_ResoluX", (float)m_fCcdResol_X);
	ini.WriteDouble(STR_SECTION_VISION, L"CCD_ResoluY", (float)m_fCcdResol_Y);
	m_nWorkList = (int)m_arWorkList.GetSize();
	ini.WriteInt(STR_SECTION_WORKLIST, L"nWorkList", m_nWorkList);
	for (int i = 0; i < m_nWorkList; i++) {
		CString strKey;
		strKey.Format(_T("%s%d"), L"Work", i);
		CString strVal = m_arWorkList[i];
		ini.WriteString(STR_SECTION_WORKLIST, strKey, strVal);
	}
	/*ini.WriteInt(STR_SECTION_HAIRPIN, L"nSlot", m_hp.nSlot);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nLayer", m_hp.nLayer);
	ini.WriteDouble(STR_SECTION_HAIRPIN, L"startAng", m_hp.startAng);
	ini.WriteDouble(STR_SECTION_HAIRPIN, L"PinSize", m_hp.HpSize);
	ini.WriteDouble(STR_SECTION_HAIRPIN, L"HpSizeW", m_hp.HpSizeW);
	ini.WriteDouble(STR_SECTION_HAIRPIN, L"HpSize2nd", m_hp.HpSize2nd);
	ini.WriteDouble(STR_SECTION_HAIRPIN, L"HpOverlength", m_hp.HpOverLen);*/
	ini.WriteDouble(STR_SECTION_SYSTEM, L"DetectRange", m_dDetectRange);
	//	ini.WriteBool(STR_SECTION_SYSTEM, L"WeldAllAtOnce", m_bWeldAllAtOnce);

	ini.WriteDouble(STR_SECTION_SYSTEM, L"HpLegendStartAng", m_dHpLegendStartAng);
	ini.WriteDouble(STR_SECTION_SYSTEM, L"HpLegendDir", m_dHpLegendDir);
	ini.WriteDouble(STR_SECTION_SYSTEM, L"dHpLegendDia", m_dHpLegendDia);

	/*for (int i = 0; i < MAX_HAIRPIN_LAYER; i++) {
		CString str;
		str.Format(L"LayerDia%d", i);
		ini.WriteDouble(STR_SECTION_HAIRPIN, str, m_hp.LayerDia[i]);
	}
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nCntP1", m_hp.nCntP1);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nPenP1", m_hp.nPenP1);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nWeldShapeP1", m_hp.nWeldShapeP1);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nWeldZigZagFreqP1", m_hp.nWeldZigZagFreqP1);

	ini.WriteInt(STR_SECTION_HAIRPIN, L"nCntP2", m_hp.nCntP2);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nPenP2", m_hp.nPenP2);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nWeldShapeP2", m_hp.nWeldShapeP2);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"nWeldZigZagFreqP2", m_hp.nWeldZigZagFreqP2);*/

	ini.WriteDouble(STR_SECTION_CENTER, L"Model1_X", m_cp.dModel1_X);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model1_Y", m_cp.dModel1_Y);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model2_X", m_cp.dModel2_X);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model2_Y", m_cp.dModel2_Y);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model3_X", m_cp.dModel3_X);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model3_Y", m_cp.dModel3_Y);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model4_X", m_cp.dModel4_X);
	ini.WriteDouble(STR_SECTION_CENTER, L"Model4_Y", m_cp.dModel4_Y);

	/*ini.WriteInt(STR_SECTION_HAIRPIN, L"SwitchWelding", m_hp.nSwitchWelding);
	ini.WriteInt(STR_SECTION_HAIRPIN, L"WeldType", m_hp.nWeldType);*/

	ini.WriteString(STR_SECTION_SYSTEM, L"PLC_IP", m_strPLC_IP);
	ini.WriteInt(STR_SECTION_SYSTEM, L"PLC_PORT", m_iPLC_Port);
	ini.WriteBool(STR_SECTION_SYSTEM, L"INSPECT", m_bUseInspect);
	ini.WriteInt(STR_SECTION_SYSTEM, L"LaserModel", m_iLaser);
	
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 ---->
	//// 2022-07-07 Matrix 관련 코드 다시 활성화 --->
	//ini.WriteDouble(STR_SECTION_CALIBMETRIX, L"a11", m_dCalibMtx_a11);
	//ini.WriteDouble(STR_SECTION_CALIBMETRIX, L"a12", m_dCalibMtx_a12);
	//ini.WriteDouble(STR_SECTION_CALIBMETRIX, L"a13", m_dCalibMtx_a13);
	//ini.WriteDouble(STR_SECTION_CALIBMETRIX, L"a21", m_dCalibMtx_a21);
	//ini.WriteDouble(STR_SECTION_CALIBMETRIX, L"a22", m_dCalibMtx_a22);
	//ini.WriteDouble(STR_SECTION_CALIBMETRIX, L"a23", m_dCalibMtx_a23);
	//// 2022-07-07 Matrix 관련 코드 다시 활성화 <---

	// Jig A Matrix
	CString sMsg = L"";
	//for (int i = 0; i < MATRIX_DIVISION; i++) {
	for (int i = 0; i < m_nMatrixDiv; i++) {
		sMsg.Format(L"M%d", i + 1);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a11", m_pMatrixA[i].a11);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a12", m_pMatrixA[i].a12);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a13", m_pMatrixA[i].a13);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a21", m_pMatrixA[i].a21);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a22", m_pMatrixA[i].a22);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a23", m_pMatrixA[i].a23);
	}

	// Jig B Matrix
	sMsg = L"";
	//for (int i = 0; i < MATRIX_DIVISION; i++) {
	for (int i = 0; i < m_nMatrixDiv; i++) {
		sMsg.Format(L"M%d", i + 1);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a11", m_pMatrixB[i].a11);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a12", m_pMatrixB[i].a12);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a13", m_pMatrixB[i].a13);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a21", m_pMatrixB[i].a21);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a22", m_pMatrixB[i].a22);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a23", m_pMatrixB[i].a23);
	}
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 <----

	// sjyi 2024.02.28 1:1 Matching 삭제, Write 행위 자체가 너무 오래 걸림
	// 1:1 Matching
	//for(int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
	//{
	//	sMsg.Format(L"X %d", i + 1);
	//	ini.WriteDouble(STR_SECTION_CALIB_ONEBYONE_JIGA, sMsg, m_dOnebyOneMatchingX_JigA[i]);
	//	sMsg.Format(L"Y %d", i + 1);
	//	ini.WriteDouble(STR_SECTION_CALIB_ONEBYONE_JIGA, sMsg, m_dOnebyOneMatchingY_JigA[i]);
	//}

	//for(int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
	//{
	//	sMsg.Format(L"X %d", i + 1);
	//	ini.WriteDouble(STR_SECTION_CALIB_ONEBYONE_JIGB, sMsg, m_dOnebyOneMatchingX_JigB[i]);
	//	sMsg.Format(L"Y %d", i + 1);
	//	ini.WriteDouble(STR_SECTION_CALIB_ONEBYONE_JIGB, sMsg, m_dOnebyOneMatchingY_JigB[i]);
	//}

	//Pattern Limit
	//ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"1StLength", m_d1StLength);
	ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"1StWidth", m_d1StWidth);
	ini.WriteInt(STR_SECTION_PTRNLIMIT, L"1StRepeat", m_n1StRepeat);

	//ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"2ndLength", m_d2ndLength);
	ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"2ndWidth", m_d2ndWidth);
	ini.WriteInt(STR_SECTION_PTRNLIMIT, L"2ndRepeat", m_n2ndRepeat);
	ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"EnergyLimit", m_dLimitEnergy);	
	ini.WriteInt(STR_SECTION_PTRNLIMIT, L"LaserMaxPower", m_nLaserMax);


	ini.WriteBool(STR_SECTION_SYSTEM, L"UseMatrixByModel", m_bMatrixByModel);

	ini.WriteBool(STR_SECTION_SYSTEM, L"UseTimeSync", m_bTimeSync);

	ini.WriteBool(STR_SECTION_SYSTEM, L"UseReconnectPLC", m_bReconnectPLC);
	
	// 2023.10.09
	ini.WriteBool(STR_SECTION_SYSTEM, L"UseOneByOneMatrix", m_bUseOneByOnMatrix);

	//ini.WriteBool(STR_SECTION_SYSTEM, L"UseWeldPosSave", m_bUseWeldPosSave);

	// 2023.04.07 sjyi 높이 변화에 따른 좌표 보상 사용여부
	ini.WriteBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_USE, m_bUseHeightComp);
	// ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_REF_H, m_dHCompRefHeight);
	CString strKey;
	// 77 Model
	strKey.Format(_T("%s_77Model_JigA"), STR_KEY_HCOMP_REF_H);
	ini.WriteDouble(STR_SECTION_HEIGTCOMP, strKey, m_dHCompRefHeight[0][0]);
	strKey.Format(_T("%s_77Model_JigB"), STR_KEY_HCOMP_REF_H);
	ini.WriteDouble(STR_SECTION_HEIGTCOMP, strKey, m_dHCompRefHeight[0][1]);

	// 77 Model
	strKey.Format(_T("%s_76Model_JigA"), STR_KEY_HCOMP_REF_H);
	ini.WriteDouble(STR_SECTION_HEIGTCOMP, strKey, m_dHCompRefHeight[1][0]);
	strKey.Format(_T("%s_76Model_JigB"), STR_KEY_HCOMP_REF_H);
	ini.WriteDouble(STR_SECTION_HEIGTCOMP, strKey, m_dHCompRefHeight[1][1]);

	ini.WriteBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_SAVE_RESULT, m_bSaveHeightComp);

	//ini.WriteBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_USE_NGCHK, m_bUseHCompNGCheck);
	//ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_NG_DISTANCE, m_dHCompNGDistance);


	// 2023.04.12 sjyi 아래 내용은 개발자만 확인 가능하도록 주석 처리
	//ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_CENTER_X, m_dHCompCenterX);
	//ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_CENTER_Y, m_dHCompCenterX);
	//ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_WD, m_dHCompWD);
	//ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_TC_ANG_MAX, m_dHCompTCMaxAngle);
	//ini.WriteDouble(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_WORKING_RADIUS, m_dHCompWorkRadius);
	//ini.WriteBool(STR_SECTION_HEIGTCOMP, STR_KEY_HCOMP_VIEW_MANUAL, m_bViewHCompManual);

	// 2023.06.12 sjyi 부분 스캔 기능 추가
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_PARTIAL, m_bUsePartialScan);
	ini.WriteString(STR_SECTION_VISIONSCAN, STR_KEY_VS_PARTIAL_MAP_PATH, m_strPartialScanMap);

	// 2023.12.13 sjyi 항상 스캔하는 옵션 JigA, JigB 구분
//	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_ALWAYS_SCAN, m_bUseAlwaysScan);
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_ALWAYS_SCAN_JIGA, m_bUseAlwaysScanJigA);
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_USE_ALWAYS_SCAN_JIGB, m_bUseAlwaysScanJigB);
//	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_REWORK, m_bUseAlwaysScanRework);
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_REWORK_A, m_bUseAlwaysScanReworkJigA);
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_REWORK_B, m_bUseAlwaysScanReworkJigB);

	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_WELD_A, m_bUseAlwaysScanWeldJigA);
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_SV_USE_ALWAYS_SCAN_WELD_B, m_bUseAlwaysScanWeldJigB);

	ini.WriteInt(STR_SECTION_VISIONSCAN, STR_KEY_VS_SCAN_SLEEP, m_nScanSleepTime);
	ini.WriteBool(STR_SECTION_VISIONSCAN, STR_KEY_VS_SAVE_IMAGE, m_bSaveImage);

	// 2024.02.12 jh.kim 추가 -  - Image position scale, offset
	//ini.WriteDouble(STR_SECTION_VISIONSCAN, L"PosScaleX", m_dPosScaleX	);
	//ini.WriteDouble(STR_SECTION_VISIONSCAN, L"PosScaleY", m_dPosScaleY	);
	//ini.WriteInt	(STR_SECTION_VISIONSCAN, L"PosOffsetX", m_nPosOffsetX	);
	//ini.WriteInt	(STR_SECTION_VISIONSCAN, L"PosOffsetY", m_nPosOffsetY	);

	// sjyi 2024.05.28 추가 모래시계, 스파이럴 패턴 사용 여부
	ini.WriteBool(STR_SECTION_SYSTEM, L"UseSpiralPtn", m_bUseSpiralPtn);
	ini.WriteBool(STR_SECTION_SYSTEM, L"UseSandGlassPtn", m_bUseSandGlassPtn);
	ini.WriteBool(STR_SECTION_SYSTEM, L"UseSortirRepeat", m_bUseSortieRepeat);

	ini.WriteBool(STR_SECTION_VISIONSCAN, L"SavePos", m_bSavePos);

	// sjyi 2024.06.19 추가, 거리 체크 기능
	ini.WriteBool(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_USE_CHECK_JIGA, m_bUseNGCheckJigA);
	ini.WriteBool(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_USE_CHECK_JIGB, m_bUseNGCheckJigB);
	ini.WriteDouble(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_NG_RANGE_JIGA, m_dNGDistanceJigA);
	ini.WriteDouble(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_NG_RANGE_JIGB, m_dNGDistanceJigB);
	ini.WriteString(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_POS_PATH_JIGA, m_strPosJigA);
	ini.WriteString(STR_SECTION_DISTANCE_CHECK, STR_KEY_DC_POS_PATH_JIGB, m_strPosJigB);
}

CorrParam * CConfigIWS::pGetCorrParam(int iLens)
{
	if (!m_arCorrParam.GetSize()) return NULL;
	if (iLens >= m_nLens) return NULL;
	return &m_arCorrParam[iLens];
}

CorrParam CConfigIWS::GetCorrParam(int iLens)
{
	//if (!m_arCorrParam.GetSize()) return ;
	if (iLens >= m_nLens) iLens = 0;
	return m_arCorrParam[iLens];
}

// 2024.02.11 jh.kim
void CConfigIWS::GetScannerRotation(double *theta)
{
	*theta = m_arCorrParam[m_iCurrLens].dblTheta;

}

void CConfigIWS::GetWorkRng(double *x, double *y)
{
	*x = m_arCorrParam[m_iCurrLens].dblWorkX;
	*y = m_arCorrParam[m_iCurrLens].dblWorkY;
}

void CConfigIWS::GetWorkCnt(double * cx, double * cy)
{
	*cx = m_arCorrParam[m_iCurrLens].dblOffsetX;
	*cy = m_arCorrParam[m_iCurrLens].dblOffsetY;
}

void CConfigIWS::GetRefImgScale(double * x, double * y)
{
	*x = m_arCorrParam[m_iCurrLens].dblImgScaleX;
	*y = m_arCorrParam[m_iCurrLens].dblImgScaleY;
}

void CConfigIWS::GetRefImgCnt(double * cx, double * cy)
{
	*cx = m_arCorrParam[m_iCurrLens].dblImgCenX;
	*cy = m_arCorrParam[m_iCurrLens].dblImgCenY;
}

void CConfigIWS::GetScanRng(double * sx, double * sy, double * ex, double * ey)
{
	*sx = m_arCorrParam[m_iCurrLens].dblVSsx;
	*sy = m_arCorrParam[m_iCurrLens].dblVSsy;
	*ex = m_arCorrParam[m_iCurrLens].dblVSex;
	*ey = m_arCorrParam[m_iCurrLens].dblVSey;
}

void CConfigIWS::SetScanRng(double sx, double sy, double ex, double ey)
{
	m_arCorrParam[m_iCurrLens].dblVSsx = sx;
	m_arCorrParam[m_iCurrLens].dblVSsy = sy;
	m_arCorrParam[m_iCurrLens].dblVSex = ex;
	m_arCorrParam[m_iCurrLens].dblVSey = ey;
}

void CConfigIWS::SetCenterPos(double m1x, double m1y, double m2x, double m2y, double m3x, double m3y, double m4x, double m4y)
{
	m_cp.dModel1_X = m1x;
	m_cp.dModel1_Y = m1y;
	m_cp.dModel2_X = m2x;
	m_cp.dModel2_Y = m2y;
	m_cp.dModel3_X = m3x;
	m_cp.dModel3_Y = m3y;
	m_cp.dModel4_X = m4x;
	m_cp.dModel4_Y = m4y;
}

//DHChoi 20200624
void CConfigIWS::SetRefImgCnt(double cx, double cy)
{
	m_arCorrParam[m_iCurrLens].dblImgCenX = cx;
	m_arCorrParam[m_iCurrLens].dblImgCenY = cy;
}

CConfigIWS& CConfigIWS::operator=(const CConfigIWS &rhs)
{
	this->m_strConfigPath = rhs.m_strConfigPath;

	return *this;
}

int CConfigIWS::Encrypt(LPCSTR lpszIn, LPSTR lpszOut, int nBufLen)
{
	int result = 0;

	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	HCRYPTKEY hKey;
	CStringA  csPass = "PASSWORD";
	DWORD  dwLen = strlen(lpszIn);

	if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
	{
		if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
		{
			return -1;
		}
	}
	lstrcpyA(lpszOut, lpszIn);

	CryptCreateHash(hProv, CALG_SHA, 0, 0, &hHash);
	CryptHashData(hHash, (BYTE*)(LPCSTR)csPass, csPass.GetLength(), 0);
	CryptDeriveKey(hProv, CALG_RC4, hHash, 0x0080 * 0x10000, &hKey);
	CryptEncrypt(hKey, 0, TRUE, 0, (BYTE*)lpszOut, &dwLen, nBufLen);
	//CryptDecrypt(hKey, 0, TRUE, 0, (BYTE*)lpszOut, &dwLen);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);
	return result;

}

int CConfigIWS::Decrypt(LPCSTR lpszIn, LPSTR lpszOut)
{
	int result = 0;

	HCRYPTPROV hProv;
	HCRYPTHASH hHash;
	HCRYPTKEY hKey;
	CStringA  csPass = "PASSWORD";
	DWORD  dwLen = strlen(lpszIn);

	if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, 0))
	{
		if (!CryptAcquireContext(&hProv, NULL, MS_ENHANCED_PROV, PROV_RSA_FULL, CRYPT_NEWKEYSET))
		{
			return -1;
		}
	}
	lstrcpyA(lpszOut, lpszIn);

	CryptCreateHash(hProv, CALG_SHA, 0, 0, &hHash);
	CryptHashData(hHash, (BYTE*)(LPCSTR)csPass, csPass.GetLength(), 0);
	CryptDeriveKey(hProv, CALG_RC4, hHash, 0x0080 * 0x10000, &hKey);
	CryptDecrypt(hKey, 0, TRUE, 0, (BYTE*)lpszOut, &dwLen);
	CryptDestroyHash(hHash);
	CryptReleaseContext(hProv, 0);

	return result;

}

void CConfigIWS::ReadMatrix(int nModelNo)
{
	_tracer(TRACE_NORMAL, 1, _T("Read VSCAN Matrix - Model No : %d"), nModelNo); // sjyi 2023.10.13 Add

	CIni ini;
	ini.SetPathName(m_strConfigPath);

	CString sMsg = L"";
	CString strSection;

	if (m_bUseOneByOnMatrix == FALSE)
	{
		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIBMETRIX_JIGA, nModelNo);
		//for (int i = 0; i < MATRIX_DIVISION; i++) {
		for (int i = 0; i < m_nMatrixDiv; i++) {
			sMsg.Format(L"M%d", i + 1);
			m_pMatrixA[i].a11 = ini.GetDouble(strSection, sMsg + L"_a11", 1.0f);
			m_pMatrixA[i].a12 = ini.GetDouble(strSection, sMsg + L"_a12", 0.0f);
			m_pMatrixA[i].a13 = ini.GetDouble(strSection, sMsg + L"_a13", 0.0f);
			m_pMatrixA[i].a21 = ini.GetDouble(strSection, sMsg + L"_a21", 0.0f);
			m_pMatrixA[i].a22 = ini.GetDouble(strSection, sMsg + L"_a22", 1.0f);
			m_pMatrixA[i].a23 = ini.GetDouble(strSection, sMsg + L"_a23", 0.0f);
		}

		// Jig B Matrix
		sMsg = L"";
		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIBMETRIX_JIGB, nModelNo);
		//for (int i = 0; i < MATRIX_DIVISION; i++) {
		for (int i = 0; i < m_nMatrixDiv; i++) {
			sMsg.Format(L"M%d", i + 1);
			m_pMatrixB[i].a11 = ini.GetDouble(strSection, sMsg + L"_a11", 1.0f);
			m_pMatrixB[i].a12 = ini.GetDouble(strSection, sMsg + L"_a12", 0.0f);
			m_pMatrixB[i].a13 = ini.GetDouble(strSection, sMsg + L"_a13", 0.0f);
			m_pMatrixB[i].a21 = ini.GetDouble(strSection, sMsg + L"_a21", 0.0f);
			m_pMatrixB[i].a22 = ini.GetDouble(strSection, sMsg + L"_a22", 1.0f);
			m_pMatrixB[i].a23 = ini.GetDouble(strSection, sMsg + L"_a23", 0.0f);
		}
	}
	else
	{
		// 1:1 Matching
		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIB_ONEBYONE_JIGA, nModelNo);
		for (int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
		{
			sMsg.Format(L"X %d", i + 1);
			m_dOnebyOneMatchingX_JigA[i] = ini.GetDouble(strSection, sMsg, 1.0f);
			sMsg.Format(L"Y %d", i + 1);
			m_dOnebyOneMatchingY_JigA[i] = ini.GetDouble(strSection, sMsg, 1.0f);
		}

		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIB_ONEBYONE_JIGB, nModelNo);
		for (int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
		{
			sMsg.Format(L"X %d", i + 1);
			m_dOnebyOneMatchingX_JigB[i] = ini.GetDouble(strSection, sMsg, 1.0f);
			sMsg.Format(L"Y %d", i + 1);
			m_dOnebyOneMatchingY_JigB[i] = ini.GetDouble(strSection, sMsg, 1.0f);
		}
	}
}

void CConfigIWS::WriteMatrix(int nModelNo)
{
	CIni ini;
	ini.SetPathName(m_strConfigPath);

	CString sMsg = L"";
	CString strSection;

	if (m_bUseOneByOnMatrix == FALSE)
	{
		// Jig A Matrix
		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIBMETRIX_JIGA, nModelNo);
		//for (int i = 0; i < MATRIX_DIVISION; i++) {
		for (int i = 0; i < m_nMatrixDiv; i++) {
			sMsg.Format(L"M%d", i + 1);
			ini.WriteDouble(strSection, sMsg + L"_a11", m_pMatrixA[i].a11);
			ini.WriteDouble(strSection, sMsg + L"_a12", m_pMatrixA[i].a12);
			ini.WriteDouble(strSection, sMsg + L"_a13", m_pMatrixA[i].a13);
			ini.WriteDouble(strSection, sMsg + L"_a21", m_pMatrixA[i].a21);
			ini.WriteDouble(strSection, sMsg + L"_a22", m_pMatrixA[i].a22);
			ini.WriteDouble(strSection, sMsg + L"_a23", m_pMatrixA[i].a23);
		}

		// Jig B Matrix
		sMsg = L"";
		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIBMETRIX_JIGB, nModelNo);
		//for (int i = 0; i < MATRIX_DIVISION; i++) {
		for (int i = 0; i < m_nMatrixDiv; i++) {
			sMsg.Format(L"M%d", i + 1);
			ini.WriteDouble(strSection, sMsg + L"_a11", m_pMatrixB[i].a11);
			ini.WriteDouble(strSection, sMsg + L"_a12", m_pMatrixB[i].a12);
			ini.WriteDouble(strSection, sMsg + L"_a13", m_pMatrixB[i].a13);
			ini.WriteDouble(strSection, sMsg + L"_a21", m_pMatrixB[i].a21);
			ini.WriteDouble(strSection, sMsg + L"_a22", m_pMatrixB[i].a22);
			ini.WriteDouble(strSection, sMsg + L"_a23", m_pMatrixB[i].a23);
		}
	}
	else
	{
		// 1:1 Matching
		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIB_ONEBYONE_JIGA, nModelNo);
		for (int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
		{
			sMsg.Format(L"X %d", i + 1);
			ini.WriteDouble(strSection, sMsg, m_dOnebyOneMatchingX_JigA[i]);
			sMsg.Format(L"Y %d", i + 1);
			ini.WriteDouble(strSection, sMsg, m_dOnebyOneMatchingY_JigA[i]);
		}

		strSection.Format(_T("%s_%02d"), STR_SECTION_CALIB_ONEBYONE_JIGB, nModelNo);
		for (int i = 0; i < MAX_HAIRPIN_SLOT * MAX_HAIRPIN_LAYER; i++)
		{
			sMsg.Format(L"X %d", i + 1);
			ini.WriteDouble(strSection, sMsg, m_dOnebyOneMatchingX_JigB[i]);
			sMsg.Format(L"Y %d", i + 1);
			ini.WriteDouble(strSection, sMsg, m_dOnebyOneMatchingY_JigB[i]);
		}
	}
}


BOOL CConfigIWS::ReadVScanMap()
{
	// Clear vmap
	m_vmapUseScan.clear();

	FILE *pFile;
	_tfopen_s(&pFile, m_strPartialScanMap, _T("r+, ccs=UNICODE"));

	if (pFile == NULL)
	{
		_tracer(TRACE_ERROR, 1, _T("Fail Scan Map"));
		return FALSE;
	}

	TCHAR szBufferData[200] = { NULL };

	// Get Data
	int nRow = 0;
	while (_fgetts(szBufferData, 200, pFile))
	{
		ParseVScanMap(szBufferData, nRow++);
	}

	fclose(pFile);

	return TRUE;
}

void CConfigIWS::ParseVScanMap(TCHAR* szBufferData, int nRow)
{
	std::vector<CString> strData;
	CString strSrc;

	// Remove \r, \n
	strSrc.Format(_T("%s"), szBufferData);
	strSrc.Remove(_T('\r'));
	strSrc.Remove(_T('\n'));

	// Devide by comma
	Tokenize(strData, strSrc, _T(','));

	// Update VScan Map
	VEC_BOOL vecBoolMap;
	BOOL bUseScan = FALSE;
	for (int i = 0; i < strData.size(); i++)
	{
		bUseScan = _tstoi(strData[i]);
		vecBoolMap.push_back(bUseScan);
	}

	// Clear CString vector
	strData.clear();

	// Add to Map Vector
	m_vmapUseScan.push_back(vecBoolMap);
}

void CConfigIWS::Tokenize(std::vector<CString> &VecStr, CString strSource, TCHAR tToken)
{
	for (int start = 0, len = strSource.GetLength(); start < len;)
	{
		int end = strSource.Find(tToken, start);
		if (end < 0) end = len;
		VecStr.push_back(strSource.Mid(start, end - start));
		start = end + 1;
	}
}


void CConfigIWS::BackUpConfig()
{
	CString strBackUPPath;
	strBackUPPath = m_strConfigPath.Left(m_strConfigPath.ReverseFind('\\'));

	// Make Directory
	strBackUPPath = strBackUPPath + _T("\\BackUp\\");
	_CreateDirectory(strBackUPPath);

	// Make Full Path
	SYSTEMTIME stTime;
	GetLocalTime(&stTime);
	strBackUPPath.Format(_T("%sconfigIWS_%04d%02d%02d_%02d%02d%02d.ini"), 
		strBackUPPath,
		stTime.wYear, stTime.wMonth, stTime.wDay,
		stTime.wHour, stTime.wMinute, stTime.wSecond);

	// Copy File
	if(CopyFile(m_strConfigPath, strBackUPPath, FALSE))
	{
		_tracer(TRACE_NORMAL, 1, _T("Back Up Config File : %s"), strBackUPPath);
	}
}


// 2024.02.11 jh.kim 추가
BOOL CConfigIWS::_CreateDirectory(CString sDirPath)
{
	int iLng = sDirPath.GetLength();
	int iPos1 = sDirPath.ReverseFind(_T('\\')) + 1;
	int iPos2 = sDirPath.ReverseFind(_T('/'));
	if (iLng != iPos1 && iLng != iPos2)
	{
		sDirPath += _T("\\");
	}

	LPCTSTR lpszPath = (LPCTSTR)sDirPath;

	TCHAR szPathBuffer[MAX_PATH];

	size_t len = _tcslen(lpszPath);

	for (size_t i = 0; i < len; i++)
	{
		szPathBuffer[i] = *(lpszPath + i);
		if (szPathBuffer[i] == _T('\\') || szPathBuffer[i] == _T('/'))
		{
			szPathBuffer[i + 1] = NULL;
			if (!PathFileExists(szPathBuffer))
			{
				if (!::CreateDirectory(szPathBuffer, NULL))
				{
					if (GetLastError() != ERROR_ALREADY_EXISTS)
						return FALSE;
				}
			}
		}
	}
	return TRUE;
}
