// CDlgVScanCalib_ConfGrid.cpp: 구현 파일
//

#include "stdafx.h"
#include "IWS.h"
#include "CDlgVScanCalib_ConfGrid.h"
#include "afxdialogex.h"
#include "./VRSaux/VRS.h"


// CDlgVScanCalib_ConfGrid 대화 상자

IMPLEMENT_DYNAMIC(CDlgVScanCalib_ConfGrid, CDialogEx)

CDlgVScanCalib_ConfGrid::CDlgVScanCalib_ConfGrid(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_DIALOG_VSCANING_CALIB_CONF_GRID, pParent)
{
	m_pImgWnd = NULL;
	m_pImg = NULL;
	m_pResultOut = NULL;
	m_pResultIn = NULL;
	m_fZoomScale = 1.0f;
	m_fZoomScaleUnit = 0.05f;
}

CDlgVScanCalib_ConfGrid::~CDlgVScanCalib_ConfGrid()
{

	if (m_pImgWnd != NULL)
	{
		delete m_pImgWnd;
		m_pImgWnd = NULL;
	}
	if (m_pResultOut != NULL)
	{
		imaqDispose(m_pResultOut);
		m_pResultOut = NULL;
	}
	if (m_pResultIn != NULL)
	{
		imaqDispose(m_pResultIn);
		m_pResultIn = NULL;
	}
}

void CDlgVScanCalib_ConfGrid::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_TARGET_LINE, m_cboTarget);
	DDX_Control(pDX, IDC_EDIT_ROI_LEFT, m_eLeft);
	DDX_Control(pDX, IDC_EDIT_ROI_TOP, m_eTop);
	DDX_Control(pDX, IDC_EDIT_ROI_WIDTH, m_eWidth);
	DDX_Control(pDX, IDC_EDIT_ROI_HEIGHT, m_eHeight);
	DDX_Control(pDX, IDC_EDIT_ROI_EDGE_STRENGTH, m_eStrength);
	DDX_Control(pDX, IDC_STATIC_DIR, m_StDir);
	DDX_Control(pDX, IDC_COMBO_SEARCH_LINE, m_cboSearchLine);
	DDX_Control(pDX, IDC_STATIC_CHART, m_stChart);
}


BEGIN_MESSAGE_MAP(CDlgVScanCalib_ConfGrid, CDialogEx)
	ON_BN_CLICKED(IDC_BUTTON_ROI_APPLY, &CDlgVScanCalib_ConfGrid::OnBnClickedButtonRoiApply)
	ON_BN_CLICKED(IDC_BUTTON_FIND_ALL, &CDlgVScanCalib_ConfGrid::OnBnClickedButtonFindAll)
	ON_CBN_SELCHANGE(IDC_COMBO_TARGET_LINE, &CDlgVScanCalib_ConfGrid::OnCbnSelchangeComboTargetLine)
	ON_CBN_SELCHANGE(IDC_COMBO_SEARCH_LINE, &CDlgVScanCalib_ConfGrid::OnCbnSelchangeComboSearchLine)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_P, &CDlgVScanCalib_ConfGrid::OnBnClickedButtonZoomP)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_FIT, &CDlgVScanCalib_ConfGrid::OnBnClickedButtonZoomFit)
	ON_BN_CLICKED(IDC_BUTTON_ZOOM_N, &CDlgVScanCalib_ConfGrid::OnBnClickedButtonZoomN)
END_MESSAGE_MAP()


// CDlgVScanCalib_ConfGrid 메시지 처리기


BOOL CDlgVScanCalib_ConfGrid::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	m_pImgWnd = new CWorkImg(this);
	m_pImgWnd->AttachToParentWnd(GetDlgItem(IDC_STATIC_IMG_WND), TRUE, { 157, 157,   0, 0 });
	m_pImgWnd->_imaqShowScrollbars(TRUE);

	imaqDisplayImage(m_pImg, m_pImgWnd->GetNiWndId(), FALSE);
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);


	m_cboTarget.AddString(_T("LEFT"));
	m_cboTarget.AddString(_T("TOP"));
	m_cboTarget.AddString(_T("RIGHT"));
	m_cboTarget.AddString(_T("BOTTOM"));
	m_cboTarget.SetCurSel(0);

	ExchangeParam(FALSE, &m_pGridParam.left);
	FindGridLine(m_pGridParam.left);


	return TRUE;  // return TRUE unless you set the focus to a control
				  // 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}
void CDlgVScanCalib_ConfGrid::ExchangeParam(BOOL bLoad, GridLineParam* pLineParam)
{
	if (bLoad == TRUE)
	{
		CString sVal;
		m_eLeft.GetWindowText(sVal);		(*pLineParam).rOut.left = _ttoi(sVal.GetBuffer());
		m_eTop.GetWindowText(sVal);			(*pLineParam).rOut.top = _ttoi(sVal.GetBuffer());
		m_eWidth.GetWindowText(sVal);		(*pLineParam).rOut.width = _ttoi(sVal.GetBuffer());
		m_eHeight.GetWindowText(sVal);		(*pLineParam).rOut.height = _ttoi(sVal.GetBuffer());
		m_eStrength.GetWindowText(sVal);	(*pLineParam).fEdgeStrength = _ttof(sVal.GetBuffer());
	}
	else
	{
		CString sVal;
		sVal.Format(_T("%d"), pLineParam->rOut.left);		m_eLeft.SetWindowText(sVal);
		sVal.Format(_T("%d"), pLineParam->rOut.top);		m_eTop.SetWindowText(sVal);
		sVal.Format(_T("%d"), pLineParam->rOut.width);		m_eWidth.SetWindowText(sVal);
		sVal.Format(_T("%d"), pLineParam->rOut.height);		m_eHeight.SetWindowText(sVal);
		sVal.Format(_T("%.1f"), pLineParam->fEdgeStrength);	m_eStrength.SetWindowText(sVal);

		switch (pLineParam->nFindDir)
		{
		case IMAQ_SEARCH_DIRECTION_LEFT_TO_RIGHT: sVal = _T("Left ▶ Right"); break;
		case IMAQ_SEARCH_DIRECTION_RIGHT_TO_LEFT: sVal = _T("Right ▶ Left"); break;
		case IMAQ_SEARCH_DIRECTION_TOP_TO_BOTTOM: sVal = _T("Top ▶ Bottom"); break;
		case IMAQ_SEARCH_DIRECTION_BOTTOM_TO_TOP: sVal = _T("Bottom ▶ Top"); break;
		}
		m_StDir.SetWindowText(sVal);
	}
}

void CDlgVScanCalib_ConfGrid::OnCbnSelchangeComboTargetLine()
{
	imaqClearOverlay(m_pImg, NULL);
	imaqClearOverlay(m_pImg, "Profile");
	switch (m_cboTarget.GetCurSel())
	{
	case 0:
		ExchangeParam(FALSE, &m_pGridParam.left);
		FindGridLine(m_pGridParam.left);
		break;
	case 1:
		ExchangeParam(FALSE, &m_pGridParam.top);
		FindGridLine(m_pGridParam.top);
		break;
	case 2:
		ExchangeParam(FALSE, &m_pGridParam.right);
		FindGridLine(m_pGridParam.right);
		break;
	case 3:
		ExchangeParam(FALSE, &m_pGridParam.bottom);
		FindGridLine(m_pGridParam.bottom);
		break;
	}
}


void CDlgVScanCalib_ConfGrid::OnBnClickedButtonRoiApply()
{
	imaqClearOverlay(m_pImg, NULL);
	imaqClearOverlay(m_pImg, "Profile");
	switch (m_cboTarget.GetCurSel())
	{
	case 0:
		ExchangeParam(TRUE, &m_pGridParam.left);
		FindGridLine(m_pGridParam.left);
		break;
	case 1:
		ExchangeParam(TRUE, &m_pGridParam.top);
		FindGridLine(m_pGridParam.top);
		break;
	case 2:
		ExchangeParam(TRUE, &m_pGridParam.right);
		FindGridLine(m_pGridParam.right);
		break;
	case 3:
		ExchangeParam(TRUE, &m_pGridParam.bottom);
		FindGridLine(m_pGridParam.bottom);
		break;
	}
	
}
GridLineInfo CDlgVScanCalib_ConfGrid::FindGridLine(GridLineParam pLineParam)
{

	if (m_pResultOut != NULL)
	{
		imaqDispose(m_pResultOut);
		m_pResultOut = NULL;
	}
	if (m_pResultIn != NULL)
	{
		imaqDispose(m_pResultIn);
		m_pResultIn = NULL;
	}
	GridLineInfo pInfo = CImageMerging::FindGridLine3(m_pImg, &pLineParam, &m_pResultOut, &m_pResultIn);


	RGBValue IMAQ_RGB_RED = RGB_RED;
	RGBValue IMAQ_RGB_BLUE = RGB_BLUE;
	RGBValue IMAQ_RGB_GREEN = RGB_GREEN;
	RGBValue IMAQ_RGB_YELLOW = RGB_YELLOW;
	RGBValue IMAQ_RGB_CYAN = RGB_CYAN;
	
	imaqOverlayRect(m_pImg, pLineParam.rOut, &IMAQ_RGB_BLUE, IMAQ_DRAW_VALUE, NULL);


	if (pInfo.bFindOut)
	{
		imaqOverlayLine(m_pImg, PointFloat2Point(pInfo.edgeOut.start),
			PointFloat2Point(pInfo.edgeOut.end), &IMAQ_RGB_CYAN, NULL);
		if (pInfo.bFindIn)
		{
			imaqOverlayLine(m_pImg, PointFloat2Point(pInfo.edgeIn.start),
				PointFloat2Point(pInfo.edgeIn.end), &IMAQ_RGB_CYAN, NULL);

			imaqOverlayLine(m_pImg, PointFloat2Point(pInfo.edgeCen.start),
				PointFloat2Point(pInfo.edgeCen.end), &IMAQ_RGB_RED, NULL);
		}
	}
	
	m_cboSearchLine.ResetContent();
	if (m_pResultOut != NULL)
	{

		for (int i = 0; i < m_pResultOut->numSearchLines; i++)
		{
			imaqOverlayLine(m_pImg, PointFloat2Point(m_pResultOut->searchLines[i].lineCoordinates.start),
				PointFloat2Point(m_pResultOut->searchLines[i].lineCoordinates.end), &IMAQ_RGB_YELLOW, NULL);
			
			int nEdge = m_pResultOut->searchLines[i].edgeReport.numEdges;
			if (nEdge > 0)
			{
				Point* pEdgePos = new Point[nEdge];
				for (int j = 0; j < nEdge; j++)
				{
					pEdgePos[j] = PointFloat2Point(m_pResultOut->searchLines[i].edgeReport.edges[j].position);
				}
				RGBValue pCol[1];
				pCol[0] = IMAQ_RGB_RED;
				imaqOverlayPoints(m_pImg, pEdgePos, nEdge, pCol, 1, IMAQ_POINT_AS_CROSS, NULL, NULL);


				delete[] pEdgePos;
			}

			CString sVal;
			sVal.Format(_T("%d"), i);
			m_cboSearchLine.AddString(sVal);
		}
	}
	if (m_cboSearchLine.GetCount() > 0)
	{
		m_cboSearchLine.SetCurSel(0);
		OnCbnSelchangeComboSearchLine();
	}
	if (m_pResultIn != NULL)
	{
		for (int i = 0; i < m_pResultIn->numSearchLines; i++)
		{

			int nEdge = m_pResultIn->searchLines[i].edgeReport.numEdges;
			if (nEdge > 0)
			{
				Point* pEdgePos = new Point[nEdge];
				for (int j = 0; j < nEdge; j++)
				{
					pEdgePos[j] = PointFloat2Point(m_pResultIn->searchLines[i].edgeReport.edges[j].position);
				}
				imaqOverlayPoints(m_pImg, pEdgePos, nEdge, &IMAQ_RGB_RED, 1, IMAQ_POINT_AS_CROSS, NULL, NULL);


				delete [] pEdgePos;
			}


		}
	}
	imaqDisplayImage(m_pImg, m_pImgWnd->GetNiWndId(), FALSE);


	return pInfo;
}


void CDlgVScanCalib_ConfGrid::OnBnClickedButtonFindAll()
{
	imaqClearOverlay(m_pImg, NULL);
	imaqClearOverlay(m_pImg, "Profile");

	GridLineInfo pLine[4];

	pLine[0] = FindGridLine(m_pGridParam.left);
	pLine[1] = FindGridLine(m_pGridParam.top);
	pLine[2] = FindGridLine(m_pGridParam.right);
	pLine[3] = FindGridLine(m_pGridParam.bottom);


	int iFindLine = 0;
	for (int i = 0; i < 4; i++)
	{
		if (pLine[i].bFindOut && pLine[i].bFindIn)
		{
			iFindLine++;
		}
	}
	if (iFindLine == 4)
	{
		PointFloat pITSPos[4];
		imaqFindIntersection(pLine[0].edgeCen, pLine[1].edgeCen, &(pITSPos[0]));
		imaqFindIntersection(pLine[2].edgeCen, pLine[1].edgeCen, &(pITSPos[1]));
		imaqFindIntersection(pLine[2].edgeCen, pLine[3].edgeCen, &(pITSPos[2]));
		imaqFindIntersection(pLine[0].edgeCen, pLine[3].edgeCen, &(pITSPos[3]));

		RGBValue IMAQ_RGB_RED = RGB_RED;
		Rect circle[4];
		int iR = 10;
		for (int i = 0; i < 4; i++)
		{
			circle[i].left = (int)(pITSPos[i].x + 0.5) - iR;
			circle[i].top = (int)(pITSPos[i].y + 0.5) - iR;
			circle[i].width = iR + iR + 1;
			circle[i].height = iR + iR + 1;
			imaqOverlayOval(m_pImg, circle[i], &IMAQ_RGB_RED, IMAQ_PAINT_VALUE, NULL);
		}
		imaqDisplayImage(m_pImg, m_pImgWnd->GetNiWndId(), FALSE);
	}
}



void CDlgVScanCalib_ConfGrid::OnOK()
{
	imaqClearOverlay(m_pImg, NULL);

	CDialogEx::OnOK();
}


void CDlgVScanCalib_ConfGrid::OnCancel()
{
	imaqClearOverlay(m_pImg, NULL);

	CDialogEx::OnCancel();
}


void CDlgVScanCalib_ConfGrid::OnCbnSelchangeComboSearchLine()
{
	int iSel = m_cboSearchLine.GetCurSel();

	int nData = m_pResultOut->searchLines[iSel].edgeReport.numGradientInfo;
	if (nData < 1)
	{
		AfxMessageBox(_T("Error(Empty)"));
		return;
	}

	RGBValue IMAQ_RGB_RED = RGB_RED;
	imaqClearOverlay(m_pImg, "Profile");
	imaqOverlayLine(m_pImg, PointFloat2Point(m_pResultOut->searchLines[iSel].lineCoordinates.start),
		PointFloat2Point(m_pResultOut->searchLines[iSel].lineCoordinates.end), &IMAQ_RGB_RED, "Profile");
	imaqDisplayImage(m_pImg, m_pImgWnd->GetNiWndId(), FALSE);



	double* px = new double[nData];
	double* py = new double[nData];
	double* gradientInfo = m_pResultOut->searchLines[iSel].edgeReport.gradientInfo;
	for (int i = 0; i < nData; i++)
	{
		px[i] = (double)i;
		py[i] = gradientInfo[i];
		//TRACE("\n%d, %f,\t\t", i, m_pResultOut->searchLines[iSel].edgeReport.gradientInfo[i]);
	}
	m_stChart.SetData(px, py, nData);
	delete[] px;
	delete[] py;
}



void CDlgVScanCalib_ConfGrid::OnBnClickedButtonZoomP()
{

	//if (m_chkShowMerge.GetCheck())
	//{
	//	m_fZoomScaleMerge += m_fZoomScaleUnit;
	//
	//	if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_nBlockX &&
	//		m_pSelBlock.y >= 0 && m_pSelBlock.y < m_nBlockY)
	//	{
	//		CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
	//		m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
	//		m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
	//	}
	//	else
	//	{
	//		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	}
	//
	//	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	//}
	//else
	//{
	m_fZoomScale += m_fZoomScaleUnit;

	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	//}
}


void CDlgVScanCalib_ConfGrid::OnBnClickedButtonZoomFit()
{
	imaqSetWindowZoomToFit(m_pImgWnd->GetNiWndId(), TRUE);

	//if (m_chkShowMerge.GetCheck())
	//{
	//	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScaleMerge, &m_fZoomScaleMerge);
	//}
	//else
	//{

	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqGetWindowZoom2(m_pImgWnd->GetNiWndId(), &m_fZoomScale, &m_fZoomScale);
	//}



}


void CDlgVScanCalib_ConfGrid::OnBnClickedButtonZoomN()
{
	//if (m_chkShowMerge.GetCheck())
	//{
	//
	//	if ((m_fZoomScaleMerge - m_fZoomScaleUnit) < 0.01)
	//	{
	//	}
	//	else
	//	{
	//		m_fZoomScaleMerge -= m_fZoomScaleUnit;
	//	}
	//
	//
	//	if (m_pSelBlock.x >= 0 && m_pSelBlock.x < m_nBlockX &&
	//		m_pSelBlock.y >= 0 && m_pSelBlock.y < m_nBlockY)
	//	{
	//		CRect cr = m_pMergeCtrl->GetBlock(m_pSelBlock)->rBlockArea;
	//		m_fZoomCenMerge.x = cr.CenterPoint().x;// r.left + r.width / 2;
	//		m_fZoomCenMerge.y = cr.CenterPoint().y;//r.top+r.height/2;
	//	}
	//	else
	//	{
	//		imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCenMerge);
	//	}
	//
	//	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScaleMerge, m_fZoomScaleMerge, m_fZoomCenMerge);
	//}
	//else
	//{

	if ((m_fZoomScale - m_fZoomScaleUnit) < 0.01)
	{
	}
	else
	{
		m_fZoomScale -= m_fZoomScaleUnit;
	}
	imaqGetWindowCenterPos(m_pImgWnd->GetNiWndId(), &m_fZoomCen);
	imaqZoomWindow2(m_pImgWnd->GetNiWndId(), m_fZoomScale, m_fZoomScale, m_fZoomCen);
	//}
}


