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
}


CConfigIWS::~CConfigIWS()
{
}

void CConfigIWS::ReadConfig()
{
	CIni ini;
	ini.SetPathName(m_strConfigPath);
	m_dWPCalibStep = ini.GetDouble(STR_SECTION_SYSTEM, L"WeldPtCalibStep", 45.0f);
	m_nCard = ini.GetInt(STR_SECTION_SYSTEM, STR_KEY_NCARD, 1);
	m_nLens = ini.GetInt(STR_SECTION_SYSTEM, L"NumOfLens", 1);
	m_iCurrLens = ini.GetInt(STR_SECTION_SYSTEM, L"CurrentLens", 0);
	m_nLightPort = ini.GetInt(STR_SECTION_SYSTEM, L"LightPort", 1);
	m_nLightPower = ini.GetInt(STR_SECTION_SYSTEM, L"LightPower", 255);
	m_bUseLight = ini.GetInt(STR_SECTION_SYSTEM, L"UseLight", 1);
	m_strPassword = ini.GetString(L"IPE", L"EDW", L"");
	if (m_strPassword != "")
	{
		char in[255];
		char out[255];

		USES_CONVERSION;
		memcpy(in, W2A(m_strPassword), 255);
		Decrypt(in, out);

		m_strPassword = A2W(out);
	}

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

		corrParam.dblCCDPointOffsetX = ini.GetDouble(section, STR_KEY_CORR_CCD_OFFSETX, 0.0);
		corrParam.dblCCDPointOffsetY = ini.GetDouble(section, STR_KEY_CORR_CCD_OFFSETY, 0.0);


		m_arCorrParam.Add(corrParam);
	}
	/* YJKO: add ccd ips */
	m_strSNCCD[CCD_MAIN] = ini.GetString(STR_SECTION_VISION, L"CCD_SN", L"00000000");
	m_strSNCCD[CCD_2ND] = ini.GetString(STR_SECTION_VISION, L"CCD2_SN", L"00000000");
	m_nFR2Want = ini.GetInt(STR_SECTION_VISION, L"CCD2_FR", 60);

	/* */
	m_nCcdHeight = ini.GetInt(STR_SECTION_VISION, L"CCD_Height", 1024);
	m_nCcdWidth = ini.GetInt(STR_SECTION_VISION, L"CCD_Width", 1280);

	m_nCcd2ImgHeight = ini.GetInt(STR_SECTION_VISION, L"CCD2_ImgHeight", 1024);
	m_nCcd2ImgWidth = ini.GetInt(STR_SECTION_VISION, L"CCD2_ImgWidth", 1280);

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
	m_bUseHairpin = ini.GetBool(STR_SECTION_SYSTEM, L"UseHairpin", FALSE);
	m_bUse2DProc = ini.GetBool(STR_SECTION_SYSTEM, L"Use2DProc", FALSE);
	m_bUseStartRoom = ini.GetBool(STR_SECTION_SYSTEM, L"UseStartRoom", FALSE);
	m_bUseLegend = ini.GetBool(STR_SECTION_SYSTEM, L"UseLegend", FALSE);

	m_dHpLegendStartAng = ini.GetDouble(STR_SECTION_SYSTEM, L"HpLegendStartAng", 180.0);
	m_dHpLegendDir = ini.GetDouble(STR_SECTION_SYSTEM, L"HpLegendDir", -1);
	m_dHpLegendDia = ini.GetDouble(STR_SECTION_SYSTEM, L"dHpLegendDia", 120.0f);

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
	for (int i = 0; i < MATRIX_DIVISION; i++) {
		sMsg.Format(L"M%d", i + 1);
		m_MatrixA[i].a11 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a11", 1.0f);
		m_MatrixA[i].a12 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a12", 0.0f);
		m_MatrixA[i].a13 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a13", 0.0f);
		m_MatrixA[i].a21 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a21", 0.0f);
		m_MatrixA[i].a22 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a22", 1.0f);
		m_MatrixA[i].a23 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a23", 0.0f);
	}

	// Jig B Matrix
	sMsg = L"";
	for (int i = 0; i < MATRIX_DIVISION; i++) {
		sMsg.Format(L"M%d", i + 1);
		m_MatrixB[i].a11 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a11", 1.0f);
		m_MatrixB[i].a12 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a12", 0.0f);
		m_MatrixB[i].a13 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a13", 0.0f);
		m_MatrixB[i].a21 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a21", 0.0f);
		m_MatrixB[i].a22 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a22", 1.0f);
		m_MatrixB[i].a23 = ini.GetDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a23", 0.0f);
	}
	// 2022-07-08 Matrix 결과 JigA, JigB 구분을 위해 수정 <----
	
	//Pattern Limit
	//m_d1StLength = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"1StLength", 0.7f);
	m_d1StWidth = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"1StWidth", 0.1f);
	m_n1StRepeat = ini.GetInt(STR_SECTION_PTRNLIMIT, L"1StRepeat", 1);	
	//m_d2ndLength = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"2ndLength", 0.5f);
	m_d2ndWidth = ini.GetDouble(STR_SECTION_PTRNLIMIT, L"2ndWidth", 0.4f);
	m_n2ndRepeat = ini.GetInt(STR_SECTION_PTRNLIMIT, L"2ndRepeat", 1);
	m_dLimitEnergy = ini.GetInt(STR_SECTION_PTRNLIMIT, L"EnergyLimit", 10000);
	m_nLaserMax = ini.GetInt(STR_SECTION_PTRNLIMIT, L"LaserMaxPower", 5000);

	// sjyi 2024.04.06 추가(CCD Live 에 패턴 2개 보여주기 여부 - 2차 용접기용)
	m_bUseCCD2ndPtnDisplay =  ini.GetBool(STR_SECTION_VISION, L"Use 2nd Ptn Display", FALSE);
	// sjyi 2024.04.06 추가(패턴 2개 보여줄 때, Offset 사용 여부)
	m_bUseCCDPtnOffset = ini.GetBool(STR_SECTION_VISION, L"Use CCD Offset", FALSE);

	m_bSaveImage = ini.GetBool(STR_SECTION_SYSTEM, STR_KEY_SYSTEM_SAVE_IMAGE, FALSE);
	m_nSaveDelay = ini.GetInt(STR_SECTION_SYSTEM, STR_KEY_SYSTEM_SVAE_DELAY, 500);

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
	ini.WriteInt(STR_SECTION_SYSTEM, L"UseCircleFit", m_bUseCircleFit);
	ini.WriteInt(STR_SECTION_SYSTEM, L"UseLight", m_bUseLight);
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

		ini.WriteDouble(section, STR_KEY_CORR_CCD_OFFSETX, p->dblCCDPointOffsetX);
		ini.WriteDouble(section, STR_KEY_CORR_CCD_OFFSETY, p->dblCCDPointOffsetY);
	}

	/* YJKO: add ccd SN param & # */
	ini.WriteString(STR_SECTION_VISION, L"CCD_SN", m_strSNCCD[CCD_MAIN]);
	ini.WriteString(STR_SECTION_VISION, L"CCD2_SN", m_strSNCCD[CCD_2ND]);
	ini.WriteInt(STR_SECTION_VISION, L"CCD2_FR", m_nFR2Want);

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
	for (int i = 0; i < MATRIX_DIVISION; i++) {
		sMsg.Format(L"M%d", i + 1);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a11", m_MatrixA[i].a11);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a12", m_MatrixA[i].a12);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a13", m_MatrixA[i].a13);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a21", m_MatrixA[i].a21);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a22", m_MatrixA[i].a22);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGA, sMsg + L"_a23", m_MatrixA[i].a23);
	}

	// Jig B Matrix
	sMsg = L"";
	for (int i = 0; i < MATRIX_DIVISION; i++) {
		sMsg.Format(L"M%d", i + 1);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a11", m_MatrixB[i].a11);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a12", m_MatrixB[i].a12);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a13", m_MatrixB[i].a13);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a21", m_MatrixB[i].a21);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a22", m_MatrixB[i].a22);
		ini.WriteDouble(STR_SECTION_CALIBMETRIX_JIGB, sMsg + L"_a23", m_MatrixB[i].a23);
	}
	// sjyi 2022-07-08 JigA, JigB 구분하기 위해 수정 <----

	//Pattern Limit
	//ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"1StLength", m_d1StLength);
	ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"1StWidth", m_d1StWidth);
	ini.WriteInt(STR_SECTION_PTRNLIMIT, L"1StRepeat", m_n1StRepeat);

	//ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"2ndLength", m_d2ndLength);
	ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"2ndWidth", m_d2ndWidth);
	ini.WriteInt(STR_SECTION_PTRNLIMIT, L"2ndRepeat", m_n2ndRepeat);
	ini.WriteDouble(STR_SECTION_PTRNLIMIT, L"EnergyLimit", m_dLimitEnergy);	
	ini.WriteInt(STR_SECTION_PTRNLIMIT, L"LaserMaxPower", m_nLaserMax);

	// sjyi 2024.04.06 추가(CCD Live 에 패턴 2개 보여주기 여부 - 2차 용접기용)
	ini.WriteBool(STR_SECTION_VISION, L"Use 2nd Ptn Display", m_bUseCCD2ndPtnDisplay);
	// sjyi 2024.04.06 추가(패턴 2개 보여줄 때, Offset 사용 여부)
	ini.WriteBool(STR_SECTION_VISION, L"Use CCD Offset", m_bUseCCDPtnOffset);

	ini.WriteBool(STR_SECTION_SYSTEM, STR_KEY_SYSTEM_SAVE_IMAGE, m_bSaveImage);
	ini.WriteInt(STR_SECTION_SYSTEM, STR_KEY_SYSTEM_SVAE_DELAY, m_nSaveDelay);
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
	if (CopyFile(m_strConfigPath, strBackUPPath, FALSE))
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



