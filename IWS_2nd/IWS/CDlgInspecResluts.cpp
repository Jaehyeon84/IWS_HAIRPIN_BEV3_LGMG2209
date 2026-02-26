// CDlgInspecResluts.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "IWSDoc.h"
#include "CDlgInspecResluts.h"
#include "afxdialogex.h"
#include "EthernetPLC.h"
#include "VRSaux/NIVision/Include/nivision.h"
#include "CMelsecPLC.h"
// CDlgInspecResluts 대화 상자

IMPLEMENT_DYNAMIC(CDlgInspecResluts, CDialogEx)

CDlgInspecResluts::CDlgInspecResluts(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_INSPECt_RESULTS, pParent)
{
	m_pDoc = NULL;
}

CDlgInspecResluts::~CDlgInspecResluts()
{
}

void CDlgInspecResluts::CalcFittedHairpin()
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	int nSlot = m_nSlot;
	int nLayer = m_nLayer;
	HairPinData *pdata = &m_hairpin;
	int idx;
	FitCircleOptions options;
	options.maxIterations = 1000;
	options.minScore = 800;
	options.pixelRadius = 1;
	options.rejectOutliers = 1;
	CircleEquation *circle = new CircleEquation[nLayer];
	CircleEquation  cir_avg;
	CString str;
	cir_avg.cx = 0;
	cir_avg.cy = 0;
	for (int i = 0; i < nLayer; i++) {
		PointFloat *point = new PointFloat[nSlot];
		for (int j = 0; j < nSlot; j++) {
			idx = j * nLayer + i;
			point[j].x = (float)(*pdata)[idx].x;
			point[j].y = (float)(*pdata)[idx].y;

		}
		BestCircle2 *fitRes = imaqFitCircle2(point, nSlot, &options);
		double cx = (double)fitRes->center.x;
		double cy = (double)fitRes->center.y;
		double irad = (double)fitRes->radius;
		circle[i].cx = fitRes->center.x;
		circle[i].cy = fitRes->center.y;
		circle[i].r = fitRes->radius;

		str.Format(L"%.3f", circle[i].cx);
		m_gridCalc.QuickSetText(0, i + 1, str);
		str.Format(L"%.3f", circle[i].cy);
		m_gridCalc.QuickSetText(1, i + 1, str);
		str.Format(L"%.3f", circle[i].r);
		m_gridCalc.QuickSetText(2, i + 1, str);

		cir_avg.cx += circle[i].cx;
		cir_avg.cy += circle[i].cy;

		imaqDispose(fitRes);
		delete[] point;
	}
	str.Format(L"%.3f", cir_avg.cx / double(nLayer));
	m_gridCalc.QuickSetText(0, 0, str);
	str.Format(L"%.3f", cir_avg.cy / double(nLayer));
	m_gridCalc.QuickSetText(1, 0, str);

	delete[] circle;
	m_gridCalc.RedrawAll();
}

void CDlgInspecResluts::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDlgInspecResluts, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_UPDATE, &CDlgInspecResluts::OnBnClickedButtonUpdate)
	ON_BN_CLICKED(IDC_BUTTON_RES_SAVE, &CDlgInspecResluts::OnBnClickedButtonResSave)
	ON_BN_CLICKED(IDC_BUTTON_RES_IMPORT, &CDlgInspecResluts::OnBnClickedButtonResImport)
END_MESSAGE_MAP()


// CDlgInspecResluts 메시지 처리기

BOOL CDlgInspecResluts::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.
	m_grid.AttachGrid(this, IDC_STATIC_GRID_INSPECTRES);
	m_gridCalc.AttachGrid(this, IDC_STATIC_GRID_INSPECTRES2);
	InitGrid();
	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}

void CDlgInspecResluts::InitGrid()
{
	int max_rows = m_nSlot * 4;
	int max_col = 16;
	m_grid.SetNumberCols(max_col);
	m_grid.SetNumberRows(max_rows);
	m_grid.SetHighlightRow(TRUE);
	m_grid.SetSH_NumberCols(2);
	m_grid.SetSH_Width(80);
	CUGCell cell;
	m_grid.GetHeadingDefault(&cell);
	//	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	// 	cell.SetBorder(0);
	// 	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNRIGHT);
	m_grid.SetGridDefault(&cell);
	for (int i = 0; i < 18; i++)
		m_grid.SetColDefault(i, &cell);

	m_grid.SetVScrollMode(UG_SCROLLNORMAL);
	m_grid.SetCurrentCellMode(2);
	m_grid.EnableJoins(TRUE);

	m_grid.QuickSetText(-1, -1, _T("Info"));
	m_grid.QuickSetText(0, -1, _T("Result"));
	m_grid.QuickSetText(1, -1, _T("1H"));
	m_grid.QuickSetText(2, -1, _T("2H"));
	m_grid.QuickSetText(3, -1, _T("3H"));
	m_grid.QuickSetText(4, -1, _T("1A"));
	m_grid.QuickSetText(5, -1, _T("2A"));
	m_grid.QuickSetText(6, -1, _T("aX"));
	m_grid.QuickSetText(7, -1, _T("aY"));
	m_grid.QuickSetText(8, -1, _T("bX"));
	m_grid.QuickSetText(9, -1, _T("bY"));
	m_grid.QuickSetText(10, -1, _T("C"));
	m_grid.QuickSetText(11, -1, _T("Jump"));
	m_grid.QuickSetText(12, -1, _T("theta"));
	m_grid.QuickSetText(13, -1, _T("resv1"));
	m_grid.QuickSetText(14, -1, _T("resv2"));
	m_grid.QuickSetText(15, -1, _T("resv3"));
	int nSlot = m_nSlot;
	CString str;
	int irow = 0;
	int is;
	for (int i = 0; i < nSlot; i++) {
		is = irow;
		for (int j = 0; j < 4; j++) {
			str.Format(L"%d-%d", 2 * j + 1, 2 * j + 2);
			m_grid.QuickSetText(-1, irow, str);
			irow++;
		}
		m_grid.JoinCells(-2, is, -2, irow - 1);
		str.Format(L"%d", i + 1);
		m_grid.QuickSetText(-2, i * 4, str);
	}

	m_gridCalc.SetNumberCols(3);
	m_gridCalc.SetNumberRows(m_nLayer + 1);
	m_gridCalc.SetHighlightRow(TRUE);
	m_gridCalc.SetSH_Width(80);

	m_grid.GetHeadingDefault(&cell);
	//	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	// 	cell.SetBorder(0);
	// 	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	cell.SetAlignment(UG_ALIGNVCENTER | UG_ALIGNRIGHT);
	m_gridCalc.SetGridDefault(&cell);
	for (int i = 0; i < 3; i++)
		m_gridCalc.SetColDefault(i, &cell);

	m_gridCalc.SetVScrollMode(UG_SCROLLNORMAL);
	m_gridCalc.SetCurrentCellMode(2);

	m_gridCalc.QuickSetText(-1, -1, _T("Info"));
	m_gridCalc.QuickSetText(0, -1, _T("cx"));
	m_gridCalc.QuickSetText(1, -1, _T("cy"));
	m_gridCalc.QuickSetText(2, -1, _T("radius"));
	m_gridCalc.QuickSetText(-1, 0, _T("Average"));
	for (int i = 0; i < m_nLayer; i++) {
		str.Format(L"Layer%d", i + 1);
		m_gridCalc.QuickSetText(-1, i + 1, str);
	}
}


void CDlgInspecResluts::UpdateGrid()
{
	short *pData = m_pPLC->m_rDataInsp;
	CString str;
	int irow = 0;
	int nMax = m_nSlot * m_nLayer * 16 / 2;
	for (int i = 0; i < nMax; i += 16)
	{
		str.Format(L"%d", pData[i]);
		m_grid.QuickSetText(0, irow, str);

		for (int j = 1; j < 16; j++) {
			str.Format(L"%.2f", double(pData[i + j]) / 100.0);
			m_grid.QuickSetText(j, irow, str);

		}
		irow++;
	}
	m_grid.RedrawAll();
}

void CDlgInspecResluts::OnBnClickedButtonUpdate()
{
	int nWordCnt = (m_pDoc->m_work.m_hp.nDrawSlot * 16) * (8 / 2);

	int nJig = m_pDoc->m_plc.GetWorkJig();
	int nJigAddress = 0;
	
	if (nJig == 1)
		nJigAddress = PLC_READ_INSPECT_A;
	else if (nJig == 2)
		nJigAddress = PLC_READ_INSPECT_B;
	else
	{
		CString strMsg;
		strMsg.Format(L"Show Inspection Result None Work Jig: %d, StartAddress: %d", nJig, nJigAddress);
		_tracer(TRACE_ERROR, 1, strMsg);
		return;
	}

	if (m_pPLC->UpdateInput_InspData(nJigAddress, nWordCnt)) {
		UpdateGrid();
	}
}

void CDlgInspecResluts::OnBnClickedButtonResSave()
{
	CString fileName;

	CString strFilter;
	strFilter = _T("csv File(*.csv)|*.csv||");
	CString strDefaultFilename = L".csv";
	CFileDialog dlgFile(
		FALSE,														//BOOL bOpenFileDialog,
		_T("*.csv"),												//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES | OFN_CREATEPROMPT,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;
	fileName = dlgFile.GetPathName();

	short *pData = m_pPLC->m_rDataInsp;
	int irow = 0;
	int nMax = m_nSlot * m_nLayer * 16 / 2;
	//for (int i = 0; i < PLC_INSP_MAX_BUFF; i += 16)
	CStdioFile file(fileName, CFile::modeWrite | CFile::typeText | CFile::modeCreate);
	CString str, strtmp;;
	str.Format(_T("slot, layer, Result,1H,2H,3H,1A,2A,aX,aY,bX,bY,C,Jump,theta,resv1,resv2,resv3\n"));
	file.WriteString(str);
	for (int i = 0; i < nMax; i += 16)
	{
		str.Format(L"%d,%d,%d,", irow / 4 + 1, irow % 4 + 1, pData[i]);
		for (int j = 1; j < 16; j++) {
			strtmp.Format(L"%.3f,", double(pData[i + j]) / 100.0);
			str = str + strtmp;
		}
		str = str + _T("\n");
		file.WriteString(str);
		irow++;
	}
	file.Close();
}


void CDlgInspecResluts::OnBnClickedButtonResImport()
{
	CString fileName;

	CString strFilter;
	strFilter = _T("csv File(*.csv)|*.csv||");
	CString strDefaultFilename = L".csv";
	CFileDialog dlgFile(
		TRUE,														//BOOL bOpenFileDialog,
		_T("*.csv"),												//LPCTSTR lpszDefExt = NULL,
		strDefaultFilename,											//LPCTSTR lpszFileName = NULL,												
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_LONGNAMES,		//DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter													//LPCTSTR lpszFilter = NULL,
																	//CWnd* pParentWnd = NULL,
																	//DWORD dwSize = 0,
																	//BOOL bVistaStyle = TRUE
	);

	if (dlgFile.DoModal() != IDOK) return;
	fileName = dlgFile.GetPathName();

	CStdioFile file(fileName, CFile::modeRead | CFile::typeText);
	CString str;
	CString strToken;
	int iPosition = 0;
	int iHeadder = 0;


	if (m_pDoc != NULL)
	{
		for (int i = 0; i < 5; i++)
		{
			file.ReadString(str);

			iPosition = 0;
			strToken = str.Tokenize(L",", iPosition);
			if (strToken == L"Lot ID :")
			{
				if (str.Find(L"JIG-A") > 0)
				{
					m_pDoc->m_plc.SetReadData(PLC_WORD_WORKJIG, 1);
				}
				else if (str.Find(L"JIG-B") > 0)
				{
					m_pDoc->m_plc.SetReadData(PLC_WORD_WORKJIG, 2);
				}
				else
				{
					m_pDoc->m_plc.SetReadData(PLC_WORD_WORKJIG, 0);
				}
				break;
			}
		}
	}


	while (file.ReadString(str))
	{
		iPosition = 0;
		strToken = str.Tokenize(L",", iPosition);
		if (strToken == L"SLOT")
			break;
		iHeadder++;
	}
	if (iHeadder > 20) return;

	double ax, ay, bx, by, h1, h2, h3, theta, jump, c;
	short *pData = m_pPLC->m_rDataInsp;
	int idx = 0;
	BOOL bOK;
	m_hairpin.clear();
	while (file.ReadString(str))
	{
		iPosition = 0;
		strToken = str.Tokenize(L",", iPosition); // slot
		strToken = str.Tokenize(L",", iPosition); // layer
		strToken = str.Tokenize(L",", iPosition); // results (NG, OK)
		bOK = (strToken == L"OK") ? TRUE : FALSE;
		strToken = str.Tokenize(L",", iPosition); // ax
		ax = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // ay
		ay = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // bx
		bx = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // by
		by = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // h1
		h1 = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // h1 NG/OK
		strToken = str.Tokenize(L",", iPosition); // h2
		h2 = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // h2 NG/OK
		strToken = str.Tokenize(L",", iPosition); // h3
		h3 = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // h3 NG/OK
		strToken = str.Tokenize(L",", iPosition); // c
		c = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // c NG/OK
		strToken = str.Tokenize(L",", iPosition); // jump
		jump = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // jump NG/OK


		strToken = str.Tokenize(L",", iPosition); // theta
		theta = _tstof(strToken);
		strToken = str.Tokenize(L",", iPosition); // theta NG/OK

		pData[idx + 0] = short(bOK);
		pData[idx + 1] = short(h1 * 100.0);
		pData[idx + 2] = short(h2 * 100.0);
		pData[idx + 3] = short(h3 * 100.0);
		pData[idx + 4] = 0;
		pData[idx + 5] = 0;
		pData[idx + 6] = short(ax * 100.0);
		pData[idx + 7] = short(ay * 100.0);
		pData[idx + 8] = short(bx * 100.0);
		pData[idx + 9] = short(by * 100.0);
		pData[idx + 10] = short(c * 100.0);
		pData[idx + 11] = short(jump * 100.0);
		pData[idx + 12] = short(theta * 100.0);
		pData[idx + 13] = 0;
		pData[idx + 14] = 0;
		pData[idx + 15] = 0;
		idx += 16;

		HAIRPIN_DATA pin0, pin1;
		pin0.x = ax;
		pin0.y = ay;
		pin0.z = 0.0;
		pin1.x = bx;
		pin1.y = by;
		pin1.z = 0.0;
		m_hairpin.push_back(pin0);
		m_hairpin.push_back(pin1);
	}
	file.Close();

	CalcFittedHairpin();
	UpdateGrid();

	//m_pDoc->m_work.CalcHairpinInfo();
	m_pDoc->m_work.NewCalcHairpinInfo();

	m_pDoc->m_bImportPLCData = TRUE;
}
