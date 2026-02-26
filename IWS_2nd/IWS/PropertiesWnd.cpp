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

#include "stdafx.h"

#include "PropertiesWnd.h"
#include "Resource.h"
#include "MainFrm.h"
#include "IWS.h"
#include "EntyCircle.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar

CPropertiesWnd::CPropertiesWnd() noexcept
{
	m_nComboHeight = 0;
	m_pJob = NULL;
	m_pDoc = NULL;
	m_nRptCntTemp = 0;
}

CPropertiesWnd::~CPropertiesWnd()
{
}

BEGIN_MESSAGE_MAP(CPropertiesWnd, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_SETFOCUS()
	ON_WM_SETTINGCHANGE()
	ON_REGISTERED_MESSAGE(AFX_WM_PROPERTY_CHANGED, OnPropertyChanged)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CResourceViewBar 메시지 처리기

void CPropertiesWnd::AdjustLayout()
{
	if (GetSafeHwnd() == nullptr || (AfxGetMainWnd() != nullptr && AfxGetMainWnd()->IsIconic()))
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndObjectCombo.SetWindowPos(nullptr, rectClient.left, rectClient.top, rectClient.Width(), m_nComboHeight, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndToolBar.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndPropList.SetWindowPos(nullptr, rectClient.left, rectClient.top + m_nComboHeight + cyTlb, rectClient.Width(), rectClient.Height() - (m_nComboHeight + cyTlb), SWP_NOACTIVATE | SWP_NOZORDER);
}

void CPropertiesWnd::ShowJobProperty(CJob *pJob)
{

	InitPropList();
	if (!pJob) return;
	m_pJob = pJob;
	m_pJobs.clear();
	DWORD	dwStyle = GetControlBarStyle();

	// sjyi 2022-06-07 리피트 회수 저장을 위해 함수 추가 -->
	if (pJob != NULL)
	{
		if (m_pDoc->m_config.m_bUseHairpin)
		{
			CJobWeld* pWeld = (CJobWeld*)pJob;
			SetRptCntTemp(pWeld->m_iRepeat);
		}
	}
	// sjyi 2022-06-07 리피트 회수 저장을 위해 함수 추가 <--
	

	pJob->DisplayProperties(&m_wndPropList);
	
	//CMFCPropertyGridProperty* PowerRmp = new CMFCPropertyGridProperty(_T("Power Ramping"));

	//for (int i = 0; i < ((CJobWeld*)m_pJobs[0])->m_iRepeat; i++)
	//m_pJob
	/*{
		CString strTmp;
		
		if(((CJobWeld*)m_pJobs[0])->m_ptrnType == PATTERN_TYPE_1ST)
		{
			strTmp.Format(_T("%.2f"), ((CJobWeld*)m_pJobs[0])->m_d1StPowerRamp[i]);
			m_pItems[PROP_WORK_POWER_RAMP] = new CMFCPropertyGridProperty(_T("Power"), strTmp, L"Power", (DWORD_PTR)PROP_WORK_POWER_RAMP);
		}
		else if (((CJobWeld*)m_pJobs[0])->m_ptrnType == PATTERN_TYPE_2ND)
		{
			strTmp.Format(_T("%.2f"), ((CJobWeld*)m_pJobs[0])->m_d2ndPowerRamp[i]);
			m_pItems[PROP_WORK_POWER_RAMP] = new CMFCPropertyGridProperty(_T("Power"), strTmp, L"Power", (DWORD_PTR)PROP_WORK_POWER_RAMP);
		}
	}*/
}

void CPropertiesWnd::ShowJobProperty(IWS_VECTOR_JOB *pJobs)
{
	InitPropList();

	if (!pJobs->size()) return;
	m_pJobs.clear();
	m_pJob = NULL;
	BOOL bCircle = TRUE;

	for (auto p : (*pJobs)) {
		m_pJobs.push_back(p);
		bCircle &= (p->GetEntity()->GetEntityType() == LC_ENT_CIRCLE);
	}

	CString strName = m_pJobs[0]->GetJobName();
	CString strPenID = CString((_variant_t)((CJobWeld*)m_pJobs[0])->m_iPenID);
	CString strWeld = CString((_variant_t)((CJobWeld*)m_pJobs[0])->GetDoJob());
	CString strRepeat = CString((_variant_t)((CJobWeld*)m_pJobs[0])->m_iRepeat);
	
	/*CStringArray arr1StPower;
	CStringArray arr2ndPower;*/

	//1차 or 2차 패턴 타입 반복횟수 & 설정한 PowerRamping Power 얻기 위한 작업
	//int n1StRp, n2ndRp;
	//double d1StPower, d2ndPower;
	//CString strTmp;
	//int n1StIndex = 0;
	//int n2ndIndex = 0;
	//for (int i = 0; i < m_pJobs.size(); i++)
	//{
	//	if (m_pJobs[i]->m_ptrnType == PATTERN_TYPE_1ST)
	//	{
	//		//n1StRp = ((CJobWeld*)m_pJobs[i])->m_iRepeat;
	//		d1StPower = ((CJobWeld*)m_pJobs[i])->m_d1StPowerRamp[n1StIndex];
	//		
	//		strTmp.Format(_T("%.2f"), d1StPower);
	//		arr1StPower.Add(strTmp);

	//		n1StIndex++;
	//	}
	//	else if (m_pJobs[i]->m_ptrnType == PATTERN_TYPE_2ND)
	//	{
	//		//n2ndRp = ((CJobWeld*)m_pJobs[i])->m_iRepeat;
	//		d2ndPower = ((CJobWeld*)m_pJobs[i])->m_d2ndPowerRamp[n2ndIndex];

	//		strTmp.Format(_T("%.2f"), d2ndPower);
	//		arr2ndPower.Add(strTmp);

	//		n2ndIndex++;
	//	}
	//}
	//
	//n1StIndex = 0;
	//n2ndIndex = 0;

	for (auto p : m_pJobs) {
		if (!p) continue;
		if (strName.GetLength() && strName != p->GetJobName())		strName = L"";
		if (strPenID.GetLength() && strPenID != CString((_variant_t)((CJobWeld*)p)->m_iPenID))  strPenID = L"";
		if (strWeld.GetLength() && strWeld != CString((_variant_t)((CJobWeld*)p)->GetDoJob()))  strWeld = L"";
		if (strRepeat.GetLength() && strRepeat != CString((_variant_t)((CJobWeld*)p)->m_iRepeat))  strRepeat = L"";

		//Power Ramping
		/*if (p->m_ptrnType == PATTERN_TYPE_1ST)
		{
			strTmp = arr1StPower.GetAt(n1StIndex);
			if (strTmp.GetLength() && strTmp != CString((_variant_t)((CJobWeld*)p)->m_d1StPowerRamp[n1StIndex]))
			{
				((CJobWeld*)p)->m_d1StPowerRamp[n1StIndex] = 0.0f;
			}
			n1StIndex++;
		}
		else if (p->m_ptrnType == PATTERN_TYPE_2ND)
		{
			strTmp = arr2ndPower.GetAt(n2ndIndex);
			if (strTmp.GetLength() && strTmp != CString((_variant_t)((CJobWeld*)p)->m_d2ndPowerRamp[n2ndIndex]))
			{
				((CJobWeld*)p)->m_d2ndPowerRamp[n2ndIndex] = 0.0f;
			}
			n2ndIndex++;
		}*/	
	}				

	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Job Info"));
	m_pItems[PROP_WORK_NAME] = new CMFCPropertyGridProperty(_T("Name"), strName, L"Job name", (DWORD_PTR)PROP_WORK_NAME);
	m_pItems[PROP_WORK_PENID] = new CMFCPropertyGridProperty(_T("Pen ID"), strPenID, L"Pen ID", (DWORD_PTR)PROP_WORK_PENID);
	m_pItems[PROP_WORK_WELDING] =	 new CMFCPropertyGridProperty(_T("Welding"), strWeld, L"Welding", (DWORD_PTR)PROP_WORK_WELDING);
	m_pItems[PROP_WORK_REPEAT] = new CMFCPropertyGridProperty(_T("Repeat"), strRepeat, L"Repeat", (DWORD_PTR)PROP_WORK_REPEAT);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_NAME]);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_PENID]);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_WELDING]);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_REPEAT]);

	m_wndPropList.AddProperty(jobInfo);

	//22.05.03 Power Ramping
	/*CMFCPropertyGridProperty* PowerRmp = new CMFCPropertyGridProperty(_T("Power Ramping"));
	for (int i = 0; i < arr1StPower.GetCount(); i++)
	{
		strTmp = arr1StPower.GetAt(i);
		m_pItems[PROP_WORK_POWER_RAMP] = new CMFCPropertyGridProperty(_T("Power"), strTmp, L"Power", (DWORD_PTR)PROP_WORK_POWER_RAMP);
	}

	for (int i = 0; i < arr2ndPower.GetCount(); i++)
	{
		strTmp = arr2ndPower.GetAt(i);
		m_pItems[PROP_WORK_POWER_RAMP] = new CMFCPropertyGridProperty(_T("Power"), strTmp, L"Power", (DWORD_PTR)PROP_WORK_POWER_RAMP);
	}

	m_wndPropList.AddProperty(PowerRmp);*/
	
	if (bCircle) {
		CString strStartAng = CString((_variant_t)((CEntyCircle*)(m_pJobs[0])->GetEntity())->StartAngle);
		CString strOverlap = CString((_variant_t)((CEntyCircle*)(m_pJobs[0])->GetEntity())->overpath);
		CString strDiameter = CString((_variant_t)((CEntyCircle*)(m_pJobs[0])->GetEntity())->diameter);
		CString strDir = CString((_variant_t)((CEntyCircle*)(m_pJobs[0])->GetEntity())->iDir);
		for (auto p : m_pJobs) {
			if (!p) continue;
			if (strStartAng.GetLength() && strStartAng != CString((_variant_t)(((CEntyCircle*)(p->GetEntity()))->StartAngle)))  strStartAng = L"";
			if (strOverlap.GetLength() && strOverlap != CString((_variant_t)(((CEntyCircle*)(p->GetEntity()))->overpath)))  strOverlap = L"";
			if (strDiameter.GetLength() && strDiameter != CString((_variant_t)(((CEntyCircle*)(p->GetEntity()))->diameter)))  strDiameter = L"";
			if (strDir.GetLength() && strDir != CString((_variant_t)(((CEntyCircle*)(p->GetEntity()))->iDir)))  strDir = L"";
		}

		CMFCPropertyGridProperty* CircleInfo = new CMFCPropertyGridProperty(_T("Circle Info"));
		m_pItems[PROP_WORK_CIRCLE_STARTANG] = new CMFCPropertyGridProperty(_T("StartAng"), strStartAng, L"StartAng", (DWORD_PTR)PROP_WORK_CIRCLE_STARTANG);
		m_pItems[PROP_WORK_CIRCLE_OVERLAP] = new CMFCPropertyGridProperty(_T("Path Overlap"), strOverlap, L"Path Overlap", (DWORD_PTR)PROP_WORK_CIRCLE_OVERLAP);
		m_pItems[PROP_WORK_CIRCLE_DIA] = new CMFCPropertyGridProperty(_T("Diameter"), strDiameter, L"Diameter", (DWORD_PTR)PROP_WORK_CIRCLE_DIA);
		m_pItems[PROB_WORK_CIRCLE_DIR] = new CMFCPropertyGridProperty(_T("CW/CCW"), strDir, L"CW/CCW", (DWORD_PTR)PROB_WORK_CIRCLE_DIR);

		CircleInfo->AddSubItem(m_pItems[PROP_WORK_CIRCLE_STARTANG]);
		CircleInfo->AddSubItem(m_pItems[PROP_WORK_CIRCLE_OVERLAP]);
		CircleInfo->AddSubItem(m_pItems[PROP_WORK_CIRCLE_DIA]);
		CircleInfo->AddSubItem(m_pItems[PROB_WORK_CIRCLE_DIR]);
		m_wndPropList.AddProperty(CircleInfo);
	}
	else {
		m_pItems[PROP_WORK_CIRCLE_STARTANG] = NULL;
		m_pItems[PROP_WORK_CIRCLE_OVERLAP] = NULL;
		m_pItems[PROP_WORK_CIRCLE_DIA] = NULL;
		m_pItems[PROB_WORK_CIRCLE_DIR] = NULL;

	}
}

int CPropertiesWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;
	DWORD dwStyle = GetControlBarStyle();
	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE);
	SetControlBarStyle(dwStyle);

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// 콤보 상자를 만듭니다.
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST | WS_BORDER | CBS_SORT | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	if (!m_wndObjectCombo.Create(dwViewStyle, rectDummy, this, 1))
	{
		TRACE0("속성 콤보 상자를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	m_wndObjectCombo.AddString(_T("응용 프로그램"));
	m_wndObjectCombo.AddString(_T("속성 창"));
	m_wndObjectCombo.SetCurSel(0);

	CRect rectCombo;
	m_wndObjectCombo.GetClientRect (&rectCombo);

	m_nComboHeight = rectCombo.Height();

	if (!m_wndPropList.Create(WS_VISIBLE | WS_CHILD, rectDummy, this, 2))
	{
		TRACE0("속성 표를 만들지 못했습니다. \n");
		return -1;      // 만들지 못했습니다.
	}

	InitPropList();

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_PROPERTIES);
	m_wndToolBar.LoadToolBar(IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_PROPERTIES_HC : IDR_PROPERTIES, 0, 0, TRUE /* 잠금 */);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndToolBar.SetOwner(this);

	// 모든 명령은 부모 프레임이 아닌 이 컨트롤을 통해 라우팅됩니다.
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	
// 	AdjustLayout();
// 	DWORD dwStyle = GetControlBarStyle();
// 	dwStyle &= ~(AFX_CBRS_CLOSE | AFX_CBRS_FLOAT | AFX_CBRS_AUTOHIDE);
// 	SetControlBarStyle(dwStyle);
	return 0;
}

void CPropertiesWnd::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}


void CPropertiesWnd::InitPropList()
{
	m_wndPropList.RemoveAll();
	SetPropListFont();

	m_wndPropList.EnableHeaderCtrl(FALSE);
	m_wndPropList.EnableDescriptionArea();
	m_wndPropList.SetVSDotNetLook();
	m_wndPropList.MarkModifiedProperties();

}

void CPropertiesWnd::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);
	m_wndPropList.SetFocus();
}

void CPropertiesWnd::OnSettingChange(UINT uFlags, LPCTSTR lpszSection)
{
	CDockablePane::OnSettingChange(uFlags, lpszSection);
	SetPropListFont();
}

LRESULT CPropertiesWnd::OnPropertyChanged(WPARAM wparam, LPARAM lparam)
{
#pragma warning(disable:4800)
	CMFCPropertyGridProperty * pProp = (CMFCPropertyGridProperty *)lparam;
	if (!pProp) return 0;
	DWORD_PTR dwData = pProp->GetData();
	COleVariant value = pProp->GetValue();
	VARTYPE vType = pProp->GetValue().vt;

	int nValue = 0;
	switch (vType)
	{
	case VT_I2: if (dwData != 0)  *((short*)dwData) = (short)pProp->GetValue().iVal; break;
	case VT_I4: if (dwData != 0)  *((int*)dwData) = (int)pProp->GetValue().intVal;
	{
		//22.05.03 Power Ramping
		if ((CString)pProp->GetName() == L"Repeat")
		{			
			//CString arrPower[WELD_REPEAT_MAX];
			//int nPowerCnt = (int)pProp->GetValue().intVal;						
			//if (nPowerCnt > WELD_REPEAT_MAX)
			//{
			//	m_PowerRaping = NULL;

			//	CString strMsg;
			//	strMsg.Format(_T("The Repeat Count can not exceed = [%d]"), WELD_REPEAT_MAX);
			//	AfxMessageBox(strMsg, MB_ICONERROR);
			//}
			//else
			//{
			//	//1차 패턴, 2차 패턴 구분..
			//	//m_pDoc->m_work.m_hp.dRptPowerProfile_1st
			//	
			//	int nTmp = m_wndPropList.GetPropertyCount();
			//	m_wndPropList.DeleteProperty(m_PowerRaping);

			//	m_PowerRaping = new CMFCPropertyGridProperty(_T("PowerRamping"));
			//	for (int i = 0; i < nPowerCnt; i++)
			//	{			

			//		m_pItems[PROB_WORK_POWER_RAMP] = new CMFCPropertyGridProperty(_T("Power"), _T(""), L"Power", (DWORD_PTR)PROB_WORK_POWER_RAMP);
			//		m_PowerRaping->AddSubItem(m_pItems[PROB_WORK_POWER_RAMP]);					
			//	}
			//	m_wndPropList.AddProperty(m_PowerRaping);
			//}
		}
	}
	break;
		case VT_R4: if (dwData != 0)  *((float*)dwData) = (float)pProp->GetValue().fltVal; break;
		case VT_R8: if (dwData != 0)  *((double*)dwData) = (double)value.dblVal; break;
		case VT_INT: if (dwData != 0)  *((int*)dwData) = (int)pProp->GetValue().intVal; break;
		case VT_BOOL: if (dwData != 0)  *((bool*)dwData) = (bool)pProp->GetValue().bVal; break;
		case VT_BSTR:
		{
			if (m_pJobs.size()) {
				CJobWeld *pJob;
				for (auto p : m_pJobs) {
					pJob = (CJobWeld*)p;
					switch (dwData)
					{
					case PROP_WORK_PENID:
						nValue = _tcstol(CString(value), NULL, 10);
						if (nValue != 15)
						{
							//pJob->m_iPenID = _tcstol(CString(value), NULL, 10);
							pJob->m_iPenID = nValue;
							pJob->m_nOriPenID = nValue;
						}
						
						break;
					case PROP_WORK_WELDING:
						pJob->SetDoJob(_tcstol(CString(value), NULL, 10));
						break;
					case PROP_WORK_REPEAT:
						pJob->m_iRepeat = _tcstol(CString(value), NULL, 10);
						break;
					case PROP_WORK_CIRCLE_STARTANG:
						((CEntyCircle*)pJob->GetEntity())->StartAngle = _tcstof(CString(value), NULL);
						pJob->GetEntity()->UpdateEntity(dwData, value);
						break;
					case PROP_WORK_CIRCLE_OVERLAP:
						((CEntyCircle*)pJob->GetEntity())->overpath = _tcstof(CString(value), NULL);
						pJob->GetEntity()->UpdateEntity(dwData, value);
						break;
					case PROP_WORK_CIRCLE_DIA:
						((CEntyCircle*)pJob->GetEntity())->diameter = _tcstof(CString(value), NULL);
						pJob->GetEntity()->UpdateEntity(dwData, value);
						break;
					case PROB_WORK_CIRCLE_DIR:
						((CEntyCircle*)pJob->GetEntity())->iDir = _tcstol(CString(value), NULL, 10);
						pJob->GetEntity()->UpdateEntity(dwData, value);
						break;
					}
				}

			}
			else if (dwData == (DWORD_PTR)&m_pJob->GetEntity()->m_strName) {
				CString str = pProp->GetValue();
				*(CString*)dwData = str;
				//TRACE("이름이 변경됨\n");
			}
		}
	}

	// hslee
	if (m_pDoc->m_config.m_bUseHairpin)
	{
		if (m_pJob)
		{
			CJobWeld *pJob = (CJobWeld*)m_pJob;
			CString strName = (CString)pProp->GetName();

			if ((CString)pProp->GetName() == L"Welding")
			{
				pJob->SetDoJob(pJob->GetDoJob());
				pJob->GetEntity()->UpdateEntity(dwData, value);
			}

			BOOL bUpdate = FALSE;
			if ((CString)pProp->GetName() == L"Pen ID")
			{
				nValue = (int)value.intVal;
				if (nValue == 15)
				{
					pJob->m_iPenID = pJob->m_nOriPenID;
				}
				else
				{
					pJob->m_iPenID = nValue;
					pJob->m_nOriPenID = nValue;
				}

				//pJob->m_iPenID = (int)value.intVal;
				bUpdate = TRUE;
			}

			if ((CString)pProp->GetName() == L"1st Length")
			{
				pJob->m_d1StLength = (double)value.dblVal;
				bUpdate = TRUE;
			}

			if ((CString)pProp->GetName() == L"1st Width")
			{
				pJob->m_d1StWidth = (double)value.dblVal;
				bUpdate = TRUE;
			}

			if ((CString)pProp->GetName() == L"2nd Length")
			{
				pJob->m_d2NdWidth = (double)value.dblVal;
				bUpdate = TRUE;
			}

			if ((CString)pProp->GetName() == L"2nd OverLen")
			{
				pJob->m_d2NdOver = (double)value.dblVal;
			}

			// sjyi 2022-06-07 Repeat 회수 수정 관련 수정 -->
			if (strName == L"Repeat")
			{
				int nDiff = pJob->m_iRepeat - m_nRptCntTemp;
				if (nDiff > 0)
				{
					for (int i = 0; i < nDiff; i++)
					{
						pJob->AddIndividualPowerAsLast();
					}
				}
				else if (nDiff < 0)
				{
					nDiff = nDiff * -1;
					for (int i = 0; i < nDiff; i++)
					{
						pJob->RevomeIndividualPowerLast();
					}
				}
				m_nRptCntTemp = pJob->m_iRepeat;

				ShowJobProperty(m_pJob);
			}
			// sjyi 2022-06-07 Repeat 회수 수정 관련 수정 -->

			// sjyi 2022-06-03 개별파워 수정여부 -->
			if (strName == L"Use Individual Power")
			{
				pJob->SetUseIndividualPower((BOOL)value.intVal);
			}
			// sjyi 2022-06-03 개별파워 수정여부 <--

			// sjyi 2022-06-03 개별 파워 값 -->
			CString strRptIndex;	// Repeat Index 값을 가져오기 위한 문자열
			int nRptIndexEndCnt;	// Repeat Index 값을 가져오기 위한 문자열 개수용
			if (strName.Left(9) == _T("Rpt Power"))
			{
				nRptIndexEndCnt = strName.GetLength() - 11;		// 숫자를 뺀 문자열의 개수가 11개
				strRptIndex = strName.Mid(10, nRptIndexEndCnt); // 10번째 부터 숫자, 숫자가 2자리 이상일 수 있으므로, nRptIndexEncCnt 변수 사용.
				int nRptIndex = _tstoi(strRptIndex) - 1;		// Index는 Zero Based Index, 화면 표시는 1 Based Index
				pJob->SetIndividualPower(nRptIndex, (double)value.dblVal); // 개별 파워 값 지정.
			}
			// sjyi 2022-06-03 개별 파워 값 <--

			if (bUpdate)
			{
				// sjyi 2023.04.14 헤어핀 패턴 회전 내용 추가, 속성을 바꿔도 회전각도 유지
//				m_pDoc->m_work.ModifyHairpin();
				m_pDoc->m_work.ModifyHairpin(0, 0, FALSE, 0, 0, TRUE);
				m_pDoc->m_pMainFrm->m_wndWorkView.UpdateWork();

				//m_pDoc->m_work.AlignHairpin();
				//lcWndRedraw(g_hLcWnd);
			}
		}
		else if (m_pJobs.size())
		{
			CJobWeld *pJob;
			for (auto p : m_pJobs) {
				pJob = (CJobWeld*)p;
				//pJob->SetDoJob(pJob->GetDoJob());
				pJob->GetEntity()->UpdateEntity(dwData, value);
			}

			// sjyi 2023.04.14 헤어핀 패턴 회전 내용 추가, 속성을 바꿔도 헤어핀 각도 유지
			//		m_pDoc->m_work.ModifyHairpin();
			m_pDoc->m_work.ModifyHairpin(0, 0, FALSE, 0, 0, TRUE);
			m_pDoc->m_pMainFrm->m_wndWorkView.UpdateWork();
		}
	}
	else
	{
		if (m_pJob)
		{
			CJobWeld *pJob = (CJobWeld*)m_pJob;
			pJob->GetEntity()->UpdateEntity(dwData, value);

		}
		else if (m_pJobs.size())
		{
			CJobWeld *pJob;
			for (auto p : m_pJobs) {
				pJob = (CJobWeld*)p;
				pJob->GetEntity()->UpdateEntity(dwData, value);
			}
		}
	}
	

	HANDLE hBlock = lcDrwGetFirstObject(g_hLcDrw, LC_OBJ_BLOCK);
	//lcBlockUnselect(hBlock);
	m_pDoc->DrawUpdateJob(m_pJob);


#pragma warning(default:4800)

	return TRUE;
}

void CPropertiesWnd::SetPropListFont()
{
	::DeleteObject(m_fntPropList.Detach());

	LOGFONT lf;
	afxGlobalData.fontRegular.GetLogFont(&lf);

	NONCLIENTMETRICS info;
	info.cbSize = sizeof(info);

	afxGlobalData.GetNonClientMetrics(info);

	lf.lfHeight = info.lfMenuFont.lfHeight;
	lf.lfWeight = info.lfMenuFont.lfWeight;
	lf.lfItalic = info.lfMenuFont.lfItalic;

	m_fntPropList.CreateFontIndirect(&lf);

	m_wndPropList.SetFont(&m_fntPropList);
	m_wndObjectCombo.SetFont(&m_fntPropList);
}

void CPropertiesWnd::SetRptCntTemp(int nRptCnt)
{
	m_nRptCntTemp = nRptCnt;
}