// DlgWorkViewer.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "DlgWorkViewer.h"
#include "afxdialogex.h"
#include "IWSDoc.h"
#include "MainFrm.h"

// CDlgWorkViewer 대화 상자

DECLARE_USER_MESSAGE(UWM_GRIDJOBLIST)
DECLARE_USER_MESSAGE(UWM_STATICCAD)

IMPLEMENT_DYNAMIC(CDlgWorkViewer, CDialogEx)

void CDlgWorkViewer::GotoScaner(double x, double y)
{
	CScanner *pScanner = m_pDoc->m_pScanner;
	CLaserPen *pOldPen = pScanner->GetLaserPen();
	CLaserPen *pPenMove = &(m_pDoc->m_pens[14]);
	pScanner->SetLaserPen(pPenMove);
	pScanner->SetLaserPenParam();
	pScanner->WaitMoving(TRUE);

	pScanner->Move(x, y);
	pScanner->WaitMoving(TRUE);

	pScanner->SetLaserPen(pOldPen);
	pScanner->SetLaserPenParam();
}

CDlgWorkViewer::CDlgWorkViewer(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_WORK_STAGE, pParent)
{
	for (int i = 0; i < PROP_WORK_END; i++) {
		m_pItems[i] = NULL;
	}
	m_bEnableEvent = FALSE;
	m_nLastImgHeight = -1;
	m_nLastImgWidth = -1;
}

CDlgWorkViewer::~CDlgWorkViewer()
{
}

//void CDlgWorkViewer::SetWorkSpace(double sx, double sy, double cx, double cy)
//{
//	m_cadDisplay.SetWorkSpace(sx, sy, cx, cy);
//}

void CDlgWorkViewer::UpdateWorkImage(CString strImagefile, double dWidth, double dHeight)
{
	POINTF sc = m_pDoc->GetScanCenter();
	m_cadDisplay.AddImage(strImagefile, dWidth, dHeight, double(sc.x), double(sc.y));
	m_strLastRefImg = strImagefile;
	m_nLastImgWidth = dWidth;
	m_nLastImgHeight = dHeight;
}

void CDlgWorkViewer::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_STATIC_CAD_VIEW, m_cadDisplay);
}


BEGIN_MESSAGE_MAP(CDlgWorkViewer, CDialogEx)
	ON_WM_CLOSE()
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_REGISTERED_MESSAGE(UWM_STATICCAD, &CDlgWorkViewer::OnUwmStaticcad)
	ON_REGISTERED_MESSAGE(UWM_GRIDJOBLIST, &CDlgWorkViewer::OnUwmGridjoblist)
END_MESSAGE_MAP()


// CDlgWorkViewer 메시지 처리기


void CDlgWorkViewer::PostNcDestroy()
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	delete  this;
	CDialogEx::PostNcDestroy();
}


void CDlgWorkViewer::OnClose()
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	ShowWindow(SW_HIDE);
	//CDialogEx::OnClose();
}


BOOL CDlgWorkViewer::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	//m_grid.AttachGrid(this, IDC_STATIC_GRID_WORKVIEW);
	//m_grid.SetReciveHandle(this->GetSafeHwnd());
	m_cadDisplay.InitStaticCad();
	m_cadDisplay.SetEventRcvHandle(this->GetSafeHwnd());
	double x, y;
	m_pDoc->m_config.GetWorkRng(&x, &y);
	m_cadDisplay.SetWorkSpace(x,y, 0, 0);
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDlgWorkViewer::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);
	int nWid = int((double)cx * 0.2+0.5);
	int ix, iy;
	if (m_cadDisplay.GetSafeHwnd()) {
		ix = 0;
		iy = 0;

		/*m_cadDisplay.MoveWindow(0, 0, cx-nWid, cy);*/
		m_cadDisplay.MoveWindow(0, 0, cx, cy);
	}
	/*if (m_grid.GetSafeHwnd()) {
		ix = cx - nWid + 1;
		iy = 0;
		m_grid.MoveWindow(ix, iy, nWid, cy);
		int iw = int(double(nWid)*(1 - 0.1) / 1);
		int in = nWid - iw * 1;
		m_grid.SetColWidth(-1, in);
		m_grid.SetColWidth(0, int(double(iw)*0.9));
	}*/
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}


void CDlgWorkViewer::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CDialogEx::OnShowWindow(bShow, nStatus);
	if (bShow) {
		if (m_cadDisplay.GetSafeHwnd()) {
			m_cadDisplay.RedrawCad();
		}
	}
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}



void CDlgWorkViewer::UpdateWork()
{	
	CIWSWork *pWork = &(m_pDoc->m_work);
	int nSlot, nLayer;
	nSlot = m_pDoc->m_work.m_hp.nSlot;
	nLayer = m_pDoc->m_work.m_hp.nLayer;
	int nJob = pWork->GetNumberOfJobs();
	CJob *pJob;
	CEntity *pEntity;

	if (m_pDoc->m_config.m_bUseHairpin) 
	{
		if (m_pDoc->m_config.m_bUseLegend)
		{
			m_cadDisplay.SetHairpinTxt(nSlot, m_pDoc->m_config.m_dHpLegendDir,
				m_pDoc->m_config.m_dHpLegendDia, m_pDoc->m_config.m_dHpLegendStartAng);
		}
	}

	m_cadDisplay.Clear();
	HANDLE hBlock = m_cadDisplay.GetBlockHandle();
	HANDLE	handle;

	for (int i = 0; i < nJob; i++) {
		pJob = pWork->GetJob(i);
		
		if(pJob->GetJobType() !=JobType::JOB_TYPE_WELD) continue;

		pEntity = pJob->GetEntity();
		lcEventsEnable(FALSE);
		handle = pEntity->AddCurrEntityToBlock(hBlock);
		//lcEntMove(handle, pEntity->m_offset.x, pEntity->m_offset.y);
		//lcEntUpdate(handle);
		//lcPropPutInt(handle, LC_PROP_DRW_COLOR, RGB(102, 204, 0));

		lcEventsEnable(TRUE);
		m_cadDisplay.m_hEntity.push_back(handle);
	}
	lcEventsEnable(FALSE);
	//lcBlockSelectEnt(hBlock, 0, true);
	//lcBlockSelMove(hBlock, 1, 1, FALSE, TRUE);
	lcBlockSelMove(hBlock, 0, 0, FALSE, FALSE);
	lcBlockUpdate(hBlock, TRUE, NULL);
	lcEventsEnable(TRUE);

	if (m_strLastRefImg.GetLength()) {
		POINTF sc = m_pDoc->GetScanCenter();
		m_cadDisplay.AddImage(m_strLastRefImg, m_nLastImgWidth, m_nLastImgHeight, double(sc.x), double(sc.y));
	}
	m_cadDisplay.Update();
	
	//int nsize = (int)pWork->GetJobs()->size();
	int nSize = 0;
	for (auto group : (*pWork->GetJobs()))
		nSize += (int)group->m_jobs.size();

	/*m_grid.SetLayer(nLayer);
	m_grid.ClearJoblist();
	for (int i = 0; i < nSize; i++) {
		m_grid.AddJoblist(pWork->GetJob(i)->GetJobName(), pWork->GetJob(i)->GetDoJob());
	}*/
}


void CDlgWorkViewer::UpdateCurrentSelect(CMFCPropertyGridProperty * pProp)
{
	DWORD_PTR dwData = pProp->GetData();
 	COleVariant value = pProp->GetValue();

	switch (dwData)
	{
	case PROP_WORK_NAME:
		pProp->SetValue(pProp->GetOriginalValue());
		return;
	}
	if (m_cadDisplay.m_idxSelect.empty()) return;
	CJobWeld *pJob;
	HANDLE hBlock = m_cadDisplay.GetBlockHandle();
	HANDLE	handle;
	for (auto idx : m_cadDisplay.m_idxSelect) {
		pJob =(CJobWeld*)GetJob(idx);
		switch (dwData)
		{
		case PROP_WORK_PENID:
			pJob->m_iPenID = _tcstol(CString(value), NULL,10);
			break;
		case PROP_WORK_WELDING:
			pJob->SetDoJob(_tcstol(CString(value), NULL,10));
			break;
		case PROP_WORK_REPEAT:
			pJob->m_iRepeat = _tcstol(CString(value), NULL,10);
			break;
		case PROP_WORK_OFFSETX:
			pJob->GetEntity()->m_offset.x = _tcstod(CString(value), NULL);
			lcEventsEnable(FALSE);
			pJob->GetEntity()->UpdateEntity(dwData, value);
			lcEventsEnable(FALSE);
			handle = pJob->GetEntity()->AddCurrEntityToBlock(hBlock);
			lcEventsEnable(FALSE);
			m_cadDisplay.ChangeEntity(idx, handle);
			lcEventsEnable(TRUE);
			m_cadDisplay.RedrawCad();
			break;
		case PROP_WORK_OFFSETY:
			pJob->GetEntity()->m_offset.y = _tcstod(CString(value), NULL);
			lcEventsEnable(FALSE);
			pJob->GetEntity()->UpdateEntity(dwData, value);
			lcEventsEnable(FALSE);
			handle = pJob->GetEntity()->AddCurrEntityToBlock(hBlock);
			lcEventsEnable(FALSE);
			m_cadDisplay.ChangeEntity(idx, handle);
			lcEventsEnable(TRUE);
			m_cadDisplay.RedrawCad();
			break;
		}
	}
// 	
// 	
// 	
// 	
// 	for (auto idx : m_cadDisplay.m_idxSelect) {
// 		pJob = GetJob(idx);
// 		if(!pJob) continue;
// 		lcEventsEnable(FALSE);
// 		pJob->GetEntity()->UpdateEntity(dwData, value);
// 		lcEventsEnable(FALSE);
// 		handle = pJob->GetEntity()->AddCurrEntityToBlock(hBlock);
// 		lcEventsEnable(FALSE);
// 		m_cadDisplay.ChangeEntity(idx, handle);
// 		lcEventsEnable(TRUE);
// 	}
// 	m_cadDisplay.RedrawCad();
}

vector_int* CDlgWorkViewer::pGetSelectionList()
{
	return &(m_cadDisplay.m_idxSelect);
}

void CDlgWorkViewer::DisplayProperties()
{
	m_pGridEntityParam->RemoveAll();
	m_pGridEntityParam->SetVSDotNetLook();
	vector_int *plist = pGetSelectionList();

	if (!plist->size()) return;
	int idx = (*plist)[0];
	CString strName = GetJob(idx)->GetJobName();
	CString strPenID = CString((_variant_t)((CJobWeld*)GetJob(idx))->m_iPenID);
	CString strWeld = CString((_variant_t)((CJobWeld*)GetJob(idx))->GetDoJob());
	CString strRepeat = CString((_variant_t)((CJobWeld*)GetJob(idx))->m_iRepeat);
	CString strX = CString((_variant_t)(GetJob(idx))->GetEntity()->m_cen.x);
	CString strY = CString((_variant_t)(GetJob(idx))->GetEntity()->m_cen.y);
	CString strOffsetX = CString((_variant_t)(GetJob(idx))->GetEntity()->m_offset.x);
	CString strOffsetY = CString((_variant_t)(GetJob(idx))->GetEntity()->m_offset.y);	

	/*CStringArray arrPowerRamp;
	for (int i = 0; i < WELD_REPEAT_MAX; i++)
	{
		arrPowerRamp.Add(CString((_variant_t)((CJobWeld*)GetJob(idx))->m_dPowerRamp[i]));
	}*/
	
	USES_CONVERSION;
	int nPenID = atoi(W2A(strPenID));

	CString strPower = CString((_variant_t)(m_pDoc->m_pens[nPenID].m_dblPower));
	CString strJumpSpeed = CString((_variant_t)(m_pDoc->m_pens[nPenID].m_dblJumpSpeed));
	CString strWeldSpeed = CString((_variant_t)(m_pDoc->m_pens[nPenID].m_dblMarkSpeed));

	for (auto p : (*plist)) {

		if (strName.GetLength() && strName != GetJob(p)->GetJobName())		strName = L"";
		if (strPenID.GetLength() && strPenID != CString((_variant_t)((CJobWeld*)GetJob(p))->m_iPenID))  strPenID = L"";
		if (strWeld.GetLength() && strWeld != CString((_variant_t)((CJobWeld*)GetJob(p))->GetDoJob()))  strWeld = L"";
		if (strRepeat.GetLength() && strRepeat != CString((_variant_t)((CJobWeld*)GetJob(p))->m_iRepeat))  strRepeat = L"";
		if (strX.GetLength() && strX != CString((_variant_t)(GetJob(p))->GetEntity()->m_cen.x))  strX = L"";
		if (strY.GetLength() && strY != CString((_variant_t)(GetJob(p))->GetEntity()->m_cen.y))  strY = L"";
		if (strOffsetX.GetLength() && strOffsetX != CString((_variant_t)(GetJob(p))->GetEntity()->m_offset.x))  strOffsetX = L"";
		if (strOffsetY.GetLength() && strOffsetY != CString((_variant_t)(GetJob(p))->GetEntity()->m_offset.y))  strOffsetY = L"";
		if (strPower.GetLength() && strPower != CString((_variant_t)(m_pDoc->m_pens[nPenID].m_dblPower)))  strPower = L"";
		if (strJumpSpeed.GetLength() && strJumpSpeed != CString((_variant_t)(m_pDoc->m_pens[nPenID].m_dblJumpSpeed)))  strJumpSpeed = L"";
		if (strWeldSpeed.GetLength() && strWeldSpeed != CString((_variant_t)(m_pDoc->m_pens[nPenID].m_dblMarkSpeed)))  strWeldSpeed = L"";		
	}

	CMFCPropertyGridProperty* jobInfo = new CMFCPropertyGridProperty(_T("Job Info"));
	m_pItems[PROP_WORK_NAME] = new CMFCPropertyGridProperty(_T("Name"), strName, L"Job name", (DWORD_PTR)PROP_WORK_NAME);
	m_pItems[PROP_WORK_PENID] = new CMFCPropertyGridProperty(_T("Pen ID"), strPenID, L"Pen ID", (DWORD_PTR)PROP_WORK_PENID);
	m_pItems[PROP_WORK_WELDING] = new CMFCPropertyGridProperty(_T("Welding"), strWeld, L"Welding", (DWORD_PTR)PROP_WORK_WELDING);
	m_pItems[PROP_WORK_REPEAT] = new CMFCPropertyGridProperty(_T("Repeat"), strRepeat, L"Repeat", (DWORD_PTR)PROP_WORK_REPEAT);
	m_pItems[PROP_WORK_X] = new CMFCPropertyGridProperty(_T("X"), strX, L"X", (DWORD_PTR)PROP_WORK_X);
	m_pItems[PROP_WORK_Y] = new CMFCPropertyGridProperty(_T("Y"), strY, L"Y", (DWORD_PTR)PROP_WORK_Y);
	m_pItems[PROP_WORK_OFFSETX] = new CMFCPropertyGridProperty(_T("Offset x"), strOffsetX, L"Offset x", (DWORD_PTR)PROP_WORK_OFFSETX);
	m_pItems[PROP_WORK_OFFSETY] = new CMFCPropertyGridProperty(_T("Offset y"), strOffsetY, L"Offset y", (DWORD_PTR)PROP_WORK_OFFSETY);	

	m_pItems[PROP_WORK_POWER] = new CMFCPropertyGridProperty(_T("Power"), strPower, L"Power", (DWORD_PTR)PROP_WORK_POWER);
	m_pItems[PROP_WROK_JUMP_SPEED] = new CMFCPropertyGridProperty(_T("Jump Speed"), strJumpSpeed, L"Jump Speed", (DWORD_PTR)PROP_WROK_JUMP_SPEED);
	m_pItems[PROP_WROK_MARK_SPEED] = new CMFCPropertyGridProperty(_T("Weld Speed"), strWeldSpeed, L"Weld Speed", (DWORD_PTR)PROP_WROK_MARK_SPEED);

	jobInfo->AddSubItem(m_pItems[PROP_WORK_NAME]);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_PENID]);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_WELDING]);
	jobInfo->AddSubItem(m_pItems[PROP_WORK_REPEAT]);
	m_pGridEntityParam->AddProperty(jobInfo);
	CMFCPropertyGridProperty* entity = new CMFCPropertyGridProperty(_T("Offset"));
	entity->AddSubItem(m_pItems[PROP_WORK_X]);
	entity->AddSubItem(m_pItems[PROP_WORK_Y]);
	entity->AddSubItem(m_pItems[PROP_WORK_OFFSETX]);
	entity->AddSubItem(m_pItems[PROP_WORK_OFFSETY]);
	m_pGridEntityParam->AddProperty(entity);
	CMFCPropertyGridProperty* WeldParam = new CMFCPropertyGridProperty(_T("WeldParam"));
	WeldParam->AddSubItem(m_pItems[PROP_WORK_POWER]);
	WeldParam->AddSubItem(m_pItems[PROP_WROK_JUMP_SPEED]);
	WeldParam->AddSubItem(m_pItems[PROP_WROK_MARK_SPEED]);

	//====================================================22.05.04 PowerRamping
	/*if (m_pDoc->m_work.m_hp.bUseRptPowerProfile_1st)
	{	
	
		int nRpCnt = atoi(W2A(strRepeat));

		m_PowerRaping = new CMFCPropertyGridProperty(_T("PowerRamping"));
		for (int i = 0; i < nRpCnt; i++)
		{
			CString strTmp;
			strTmp = arrPowerRamp.GetAt(i);

			m_pItems[PROP_WORK_POWER_RAMPING] = new CMFCPropertyGridProperty(_T("Power"), strTmp, L"Power", (DWORD_PTR)PROP_WORK_POWER_RAMPING);
			m_PowerRaping->AddSubItem(m_pItems[PROP_WORK_POWER_RAMPING]);
			m_pGridEntityParam->AddProperty(m_PowerRaping);
		}
	}*/
	/*if (m_pDoc->m_work.m_hp.bUseRptPowerProfile_2nd)
	{

	}*/
	//====================================================

	m_pGridEntityParam->AddProperty(WeldParam);

	// hslee
	m_pItems[PROP_WORK_NAME]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_PENID]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_WELDING]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_REPEAT]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_X]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_Y]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_OFFSETX]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_OFFSETY]->AllowEdit(FALSE);
	m_pItems[PROP_WORK_POWER]->AllowEdit(FALSE);
	m_pItems[PROP_WROK_JUMP_SPEED]->AllowEdit(FALSE);
	m_pItems[PROP_WROK_MARK_SPEED]->AllowEdit(FALSE);
	//m_pItems[PROP_WORK_POWER_RAMPING]->AllowEdit(FALSE);
}

void CDlgWorkViewer::ClearImage()
{
	m_cadDisplay.ClearImage();
	m_strLastRefImg=L"";	
}

CJob* CDlgWorkViewer::GetJob(int idx)
{
	CIWSWork *pWork = &(m_pDoc->m_work);
	CJob *pJob = pWork->GetJob(idx);
	if (pJob->GetJobType() != JobType::JOB_TYPE_WELD) 
		return NULL;
	return pJob;
}


BOOL CDlgWorkViewer::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	if (pMsg->message == WM_KEYDOWN) {

		if (pMsg->wParam == VK_ESCAPE) {
			if (m_cadDisplay.GetSafeHwnd())
				m_cadDisplay.ClearSelect();
		}
		else if (pMsg->wParam == VK_F2) {
			m_pDoc->m_pMainFrm->SwitchView(DRAW_VIEW);
		}
		else if (pMsg->wParam == VK_F4) {
			m_pDoc->m_work.DoVScan();
		}
		else if ((pMsg->wParam == 'p' || pMsg->wParam == 'P') && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
			m_pDoc->m_pMainFrm->OnAccLaserpen();
		}
		return TRUE;
	}
	return CDialogEx::PreTranslateMessage(pMsg);
}


afx_msg LRESULT CDlgWorkViewer::OnUwmStaticcad(WPARAM wParam, LPARAM lParam)
{
	if (!m_bEnableEvent) return 0;
	int idx = LOWORD(lParam);
	BOOL bSel = HIWORD(lParam);
	switch (wParam)
	{
	case CAD_EVENT_MOUSEMOVE:	
		break;
	case CAD_EVENT_ENTSENECT:
	{
		//m_grid.EnableEvent(FALSE);
		int nsize = (int)m_cadDisplay.m_idxSelect.size();
		/*m_grid.ClearSelections();
		m_grid.GotoRow(idx);*/
		Point3Dbl cen = GetJob(idx)->GetEntity()->m_cen;
		Point3Dbl offset = GetJob(idx)->GetEntity()->m_offset;
		GotoScaner(cen.x+offset.x, cen.y+offset.y);
		m_pGridEntityParam->MarkModifiedProperties();
		/*for (auto p : m_cadDisplay.m_idxSelect) {
			m_grid.Select(0, p);
		}*/
		
		//m_grid.EnableEvent(TRUE);
		DisplayProperties();
		//GetJob(idx)->DisplayProperties(m_pGridEntityParam);
	
		
		
	}
		
		break;
	case CAD_EVENT_ENTCHANGE:

		break;
	case CAD_EVENT_LBDOWN:
		break;
	case CAD_EVENT_SELECTCLEAR:
		/*m_grid.EnableEvent(FALSE);
		m_grid.ClearSelections();
		m_grid.HideCurrentCell();
		m_grid.RedrawAll();

		m_grid.EnableEvent(TRUE);*/
		break;
	}
	return 0;
}


afx_msg LRESULT CDlgWorkViewer::OnUwmGridjoblist(WPARAM wParam, LPARAM lParam)
{
	if (!m_bEnableEvent) return 0;
	int irow = (int)lParam;
	switch (wParam)
	{
	case MSG_GRID_CHANGEROW:
	{
		//int nCol, nRet;
		//long nRow;
		m_cadDisplay.m_idxSelect.clear();

		//nRet = m_grid.EnumFirstSelected(&nCol, &nRow);

		//// keep looping until we've run out of selected cells
		//while (nRet == UG_SUCCESS) {
		//	m_cadDisplay.m_idxSelect.push_back((int)nRow);
		//	nRet = m_grid.EnumNextSelected(&nCol, &nRow);
		//}
	}
		m_cadDisplay.SetSelectEntity(irow);
		DisplayProperties();
		break;
	break;
	}
	return 0;
}
