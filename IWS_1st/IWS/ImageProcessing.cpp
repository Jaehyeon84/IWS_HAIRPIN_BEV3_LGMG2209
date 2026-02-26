#include "stdafx.h"

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ImageProcessing.h"
#include "Ini.h"

#ifdef _WIN32
#include <windows.h>
#endif

CImageProcessing::CImageProcessing()
{
	//
	m_scale = 1.0;
	m_radius = 0.0;
}
CImageProcessing::~CImageProcessing()
{
	//
}

// 105 x 105 mm - Hole ( 24 point )
int CImageProcessing::ProcessLineEdge(Image *image, POINTFLOAT pPos[], float resolX, float resolY)
{
	int success = 1;

	Point piMake = imaqMakePoint(m_pcc.cen.x, m_pcc.cen.y);  //imaqMakePoint(4282, 4278);
	int    nInnerR = m_pcc.nInnerR; // 3160;
	int    nOuterR = m_pcc.nOuterR; // 3550;
	double dsAngle = 0;
	double deAngle = 0;

	// Scale 계산 ( 센터 -> 홀 : 86.5mm Diameter 173mm )
	m_resX = resolX * m_pcc.dScale;
	m_resY = resolY * m_pcc.dScale;

	cenCircle = FindCircular(image, piMake, nInnerR, nOuterR, dsAngle, deAngle, 10);

	double dcx, dcy;
	////dcx = (m_pcc.cen.x - cenCircle.cen.x) * PX_RESOLUTION;
	////dcy = (m_pcc.cen.y - cenCircle.cen.y) * PX_RESOLUTION;
	dcx = (m_pcc.cen.x - cenCircle.cen.x) * m_resX;
	dcy = (m_pcc.cen.y - cenCircle.cen.y) * m_resY;
	m_radius = cenCircle.radius * ((m_resX + m_resY)/2.0);
	m_scale = m_pcc.dRadius / m_radius;
	_tracer(TRACE_NORMAL, 3000, L"Center Position - X : %.3f, Y : %.3f, Radius : %.3f(%.2f)", dcx, dcy, m_radius, m_pcc.dRadius);
	_tracer(TRACE_NORMAL, 3000, L"Scale factor - %.5f", m_scale);

	// Find Cross Point
	double dAngle = 0.0;
	int inx = 0;
	for (int i = 0; i < PCC_POINTS; i++)
	{
		FindCrossPoint(i+1, image, m_pcc.pEdge[i].y, m_pcc.pEdge[i].x, (int)m_pcc.nRcpHeight, (int)m_pcc.nRcpWidth, dAngle, pPos);
		dAngle += m_pcc.dEdgeAngle;
	}	
	return success;
}

BOOL CImageProcessing::FindCrossPoint(int no, Image* img, 
									  int nx, int ny, int nheight, int nwidth, double dangle,
									  POINTFLOAT pPos[])
{
	LineEdge ResEdge1, ResEdge2;
	RotatedRect rotRect;

	int n = 0;
	int ntop, nleft;

	// Find Edge1
	ntop  = ny - (int)(nheight / 2.0f);
	nleft = nx - (int)(nwidth  / 2.0f);
	rotRect = imaqMakeRotatedRect(ntop, nleft, nheight, nwidth, dangle);
	ResEdge1 = FindLine1(img, rotRect);

	// Find Edge2
	ntop  = ny - (int)(nwidth  / 2.0f);
	nleft = nx - (int)(nheight / 2.0f);
	rotRect = imaqMakeRotatedRect(ntop, nleft, nwidth, nheight, dangle);
	ResEdge2 = FindLine2(img, rotRect);

	// cross point check
	POINTFLOAT Ln1st, Ln1en, Ln2st, Ln2en;
	Ln1st.x = ResEdge1.edge.start.x	; Ln1st.y = ResEdge1.edge.start.y;
	Ln1en.x = ResEdge1.edge.end.x	; Ln1en.y = ResEdge1.edge.end.y	;
	Ln2st.x = ResEdge2.edge.start.x	; Ln2st.y = ResEdge2.edge.start.y;
	Ln2en.x = ResEdge2.edge.end.x	; Ln2en.y = ResEdge2.edge.end.y	;

	POINTFLOAT pt = CrossPoint(Ln1st, Ln1en, Ln2st, Ln2en);
	if (pt.x == 0 && pt.y == 0.0) {
		pPos[no-1].x = 0.0;
		pPos[no-1].y = 0.0;

		return FALSE;
	}
	else
	{
		pPos[no-1].x = (pt.x - cenCircle.cen.x) * m_resX * m_scale;
		pPos[no-1].y = (cenCircle.cen.y - pt.y) * m_resY * m_scale;
	}
	Logging(no, pPos[no-1], pt);

	return TRUE;
}

BOOL CImageProcessing::Logging(int no, POINTFLOAT pt1, POINTFLOAT pt2)
{

	_tracer(TRACE_NORMAL, 3000, L"Point %02d - x : %8.3f, y : %8.3f", no, pt1.x, pt1.y);

	return TRUE;
}

CircularEdge CImageProcessing::FindCircular(Image* img, Point piMake, 
									int nInnerRadius, int nOuterRadius, 
									double dStartAngle, double dEndAngle, unsigned int nStepSize /*= 10*/)
{
	int success = 1;
	ROI* roi ;
	CircularEdge *cirEdge1 = NULL, *cirEdge2 = NULL;
	CircularEdge cenCircle = CircularEdge();

	// Creates a new, empty region of interest.
	VisionErrChk(roi = imaqCreateROI());

	// Creates an new ROI contour that represents an annulus and then adds it to the provided ROI.
	VisionErrChk(imaqAddAnnulusContour(roi, imaqMakeAnnulus(piMake, nInnerRadius, nOuterRadius, dStartAngle, dEndAngle)));
	
	cirEdge1 = new CircularEdge();
	cirEdge2 = new CircularEdge();

	VisionErrChk(FindCircularEdge(img, roi, IMAQ_INSIDE_TO_OUTSIDE, nStepSize, cirEdge1));
	VisionErrChk(FindCircularEdge(img, roi, IMAQ_OUTSIDE_TO_INSIDE, nStepSize, cirEdge2));

	if ((cirEdge1->circleFound) && (cirEdge2->circleFound)) {		

		cenCircle.cen.x    = (cirEdge1->cen.x	 + cirEdge2->cen.x	  ) / 2.0f;
		cenCircle.cen.y    = (cirEdge1->cen.y	 + cirEdge2->cen.y	  ) / 2.0f;
		cenCircle.radius   = (cirEdge1->radius   + cirEdge2->radius   ) / 2.0f;
		cenCircle.roundness= (cirEdge1->roundness+ cirEdge2->roundness) / 2.0f;
	}
	else
	{
		_tracer(TRACE_NORMAL, 3000, L"Find Circular Error.");
	}

	// Cleans up resources associated with the object
	imaqDispose(roi);

	delete cirEdge1;
	delete cirEdge2;

Error:
	return cenCircle;
}

LineEdge CImageProcessing::FindLine1(Image* img, RotatedRect rotRect)
{
	int success = 1;
	ROI* roi;

	LineEdge *lnEdge1 = NULL, *lnEdge2 = NULL;
	LineEdge cenLine = LineEdge();

	// Creates a new, empty region of interest.
	VisionErrChk(roi = imaqCreateROI());

	// Creates a new rectangle ROI contour and adds the rectangle to the provided ROI.
	VisionErrChk(imaqAddRotatedRectContour(roi, rotRect));

	lnEdge1 = new LineEdge();
	lnEdge2 = new LineEdge();

	VisionErrChk(FindEdge(img, roi, IMAQ_LEFT_TO_RIGHT, EDGE_STEP_SIZE, lnEdge1));
	VisionErrChk(FindEdge(img, roi, IMAQ_RIGHT_TO_LEFT, EDGE_STEP_SIZE, lnEdge2));

	if ((lnEdge1->numUsedEdges > 0) && (lnEdge2->numUsedEdges > 0)) 
	{
		cenLine.edge.start.x = (lnEdge1->edge.start.x + lnEdge2->edge.start.x) / 2.0f;
		cenLine.edge.start.y = (lnEdge1->edge.start.y + lnEdge1->edge.start.y) / 2.0f;
		cenLine.edge.end.x	 = (lnEdge1->edge.end.x   + lnEdge2->edge.end.x  ) / 2.0f;
		cenLine.edge.end.y	 = (lnEdge1->edge.end.y   + lnEdge2->edge.end.y  ) / 2.0f;
		cenLine.angle		 = (lnEdge1->angle        + lnEdge2->angle       ) / 2.0f;
		cenLine.score		 = (lnEdge1->score        + lnEdge2->score       ) / 2.0f;
		cenLine.straightness = (lnEdge1->straightness + lnEdge2->straightness) / 2.0f;
	}
	else
	{
		_tracer(TRACE_NORMAL, 3000, L"Find Line Error.");
	}

	// Cleans up resources associated with the object
	imaqDispose(roi);

	delete lnEdge1;
	delete lnEdge2;

Error:
	return cenLine;
}

LineEdge CImageProcessing::FindLine2(Image* img, RotatedRect rotRect)
{
	int success = 1;
	ROI* roi;

	LineEdge *lnEdge1 = NULL, *lnEdge2 = NULL;
	LineEdge cenLine = LineEdge();

	// Creates a new, empty region of interest.
	VisionErrChk(roi = imaqCreateROI());

	// Creates a new rectangle ROI contour and adds the rectangle to the provided ROI.
	VisionErrChk(imaqAddRotatedRectContour(roi, rotRect));

	lnEdge1 = new LineEdge();
	lnEdge2 = new LineEdge();

	VisionErrChk(FindEdge(img, roi, IMAQ_TOP_TO_BOTTOM, EDGE_STEP_SIZE, lnEdge1));
	VisionErrChk(FindEdge(img, roi, IMAQ_BOTTOM_TO_TOP, EDGE_STEP_SIZE, lnEdge2));

	if ((lnEdge1->numUsedEdges > 0) && (lnEdge2->numUsedEdges > 0))
	{
		cenLine.edge.start.x = (lnEdge1->edge.start.x + lnEdge2->edge.start.x) / 2.0f;
		cenLine.edge.start.y = (lnEdge1->edge.start.y + lnEdge1->edge.start.y) / 2.0f;
		cenLine.edge.end.x	 = (lnEdge1->edge.end.x   + lnEdge2->edge.end.x  ) / 2.0f;
		cenLine.edge.end.y	 = (lnEdge1->edge.end.y   + lnEdge2->edge.end.y  ) / 2.0f;
		cenLine.angle		 = (lnEdge1->angle        + lnEdge2->angle       ) / 2.0f;
		cenLine.score		 = (lnEdge1->score        + lnEdge2->score       ) / 2.0f;
		cenLine.straightness = (lnEdge1->straightness + lnEdge2->straightness) / 2.0f;
	}
	else
	{
		_tracer(TRACE_NORMAL, 3000, L"Find Line Error.");
	}

	// Cleans up resources associated with the object
	imaqDispose(roi);

	delete lnEdge1;
	delete lnEdge2;

Error:
	return cenLine;
}

////////////////////////////////////////////////////////////////////////////////
//
// Function Name: FindCircularEdge
//
// Description  : Locates a circular edge in an annular search area.
//
// Parameters   : image              -  Input image.
//                roi                -  Region of Interest
//                pThreshold         -  Threshold for the contrast of an edge.
//                pFilterWidth       -  Number of pixels that the function averages
//                                      to find the contrast at either side of the edge.
//                pSteepness         -  Span, in pixels, of the slope of the edge
//                                      projected along the path specified by the input points.
//                pSubsamplingRatio  -  Number of pixels that separates two
//                                      consecutive search lines.
//                stepIndex          -  Step index (index at which to store
//                                      the results in the resuts array)
//                ivaData            -  Internal Data structure
//
// Return Value : success
//
////////////////////////////////////////////////////////////////////////////////
int CImageProcessing::FindCircularEdge(Image* image, ROI* roi, int pDirection, unsigned int pStepSize, CircularEdge *cirEdge)
{
    int success = TRUE;
    EdgeOptions2 edgeOptions;
    FindCircularEdgeOptions findEdgeOptions;
    CircleFitOptions fitOptions;
    FindCircularEdgeReport* circularEdgeReport = NULL;
    //IVA_Result* edgeResults;
    //unsigned int visionInfo;

    //-------------------------------------------------------------------//
    //                         Find Circular Edge                        //
    //-------------------------------------------------------------------//

    edgeOptions.polarity			= CIRLCE_POLARITY;
    edgeOptions.kernelSize			= CIRLCE_KERNEL_SIZE;
    edgeOptions.width				= CIRLCE_WIDTH;
    //edgeOptions.minThreshold		= CIRLCE_THRESHOLD;
	edgeOptions.minThreshold		= m_pcc.nCirThreshold;
    edgeOptions.interpolationType	= CIRCLE_INTERPOLATION_TYPE;
    edgeOptions.columnProcessingMode= CIRCLE_PROCESSING_MODE;
    
    findEdgeOptions.direction		= (SpokeDirection)pDirection;
    findEdgeOptions.showSearchArea	= TRUE;
    findEdgeOptions.showSearchLines = TRUE;
    findEdgeOptions.showEdgesFound	= TRUE;

    findEdgeOptions.showResult		= TRUE;
    findEdgeOptions.searchAreaColor = IMAQ_RGB_GREEN;
    findEdgeOptions.searchLinesColor= IMAQ_RGB_BLUE;
    findEdgeOptions.searchEdgesColor= IMAQ_RGB_YELLOW;
    findEdgeOptions.resultColor		= IMAQ_RGB_RED;
    findEdgeOptions.overlayGroupName= NULL;
    findEdgeOptions.edgeOptions		= edgeOptions;

    fitOptions.maxRadius			= 3;
    fitOptions.stepSize				= pStepSize;
    fitOptions.processType			= (RakeProcessType)CIRCLE_PROCESS;

    // Locates a circular edge in the annular search area.
    VisionErrChk(circularEdgeReport = imaqFindCircularEdge2(image, roi, NULL, NULL, &findEdgeOptions,  &fitOptions));

    // ////////////////////////////////////////
    // Store the results in the data structure.
    // ////////////////////////////////////////
    if (circularEdgeReport->circleFound)
    {
		cirEdge->circleFound = circularEdgeReport->circleFound;
		cirEdge->cen		 = circularEdgeReport->center;
		cirEdge->radius		 = circularEdgeReport->radius;
		cirEdge->roundness	 = circularEdgeReport->roundness;
    }

Error:

	if (!circularEdgeReport->circleFound) {
		cirEdge->circleFound = 0;
		cirEdge->cen = { 0,0 };
		cirEdge->radius = 0.0;
		cirEdge->roundness = 0.0;
	}

    // Disposes the edge report
    imaqDispose(circularEdgeReport);
    
    return success;
}



////////////////////////////////////////////////////////////////////////////////
//
// Function Name: FindEdge
//
// Description  : Locates a straight edge in a rectangular search area.
//
// Parameters   : image                  -  Input image
//                roi                    -  Region of interest
//                pDirection             -
//                pStepSize              -  Number of pixels that separates two
//                                          consecutive search lines.
//                ivaData                -  Internal Data structure
//                stepIndex              -  Step index (index at which to store
//                                          the results in the resuts array)
//
// Return Value : success
//
////////////////////////////////////////////////////////////////////////////////
int CImageProcessing::FindEdge(Image* image, ROI* roi, int pDirection, unsigned int pStepSize, LineEdge* lineEdge)
{
    int success = TRUE;
    EdgeOptions2 edgeOptions;
    FindEdgeOptions2 findEdgeOptions;
    StraightEdgeOptions straightEdgeOptions;
    FindEdgeReport* findEdgeReport = NULL;
    
    //-------------------------------------------------------------------//
    //                         Find Straight Edge                        //
    //-------------------------------------------------------------------//

    edgeOptions.polarity			= EDGE_POLARITY;
    edgeOptions.kernelSize			= EDGE_KERNEL_SIZE;
    edgeOptions.width				= EDGE_WIDTH;
    //edgeOptions.minThreshold		= EDGE_THRESHOLD;
	edgeOptions.minThreshold		= m_pcc.nEdgeThreshold;
    edgeOptions.interpolationType	= EDGE_INTERPOLATION_TYPE;
    edgeOptions.columnProcessingMode= EDGE_PROCESSING_MODE;
    
    findEdgeOptions.direction		= (RakeDirection)pDirection;
    findEdgeOptions.showSearchArea	= TRUE;
    findEdgeOptions.showSearchLines = TRUE;
    findEdgeOptions.showEdgesFound	= TRUE;
    findEdgeOptions.showResult		= TRUE;
    findEdgeOptions.searchAreaColor = IMAQ_RGB_GREEN;
    findEdgeOptions.searchLinesColor= IMAQ_RGB_BLUE;
    findEdgeOptions.searchEdgesColor= IMAQ_RGB_YELLOW;
    findEdgeOptions.resultColor		= IMAQ_RGB_RED;
    findEdgeOptions.overlayGroupName= NULL;
    findEdgeOptions.edgeOptions		= edgeOptions;
    
    straightEdgeOptions.numLines	= 1;
    straightEdgeOptions.searchMode	= EDGE_SEARCH_MODE;
    straightEdgeOptions.minScore	= 10;
    straightEdgeOptions.maxScore	= 1000;
    straightEdgeOptions.orientation = 0;
    straightEdgeOptions.angleRange	= 45;
    straightEdgeOptions.angleTolerance= 1;
    straightEdgeOptions.stepSize	= pStepSize;
    straightEdgeOptions.minSignalToNoiseRatio = 0;
    straightEdgeOptions.minCoverage	= 25;
    straightEdgeOptions.houghIterations = 5;
    
    // Locates a straight edge in the rectangular search area.
    VisionErrChk(findEdgeReport = imaqFindEdge2(image, roi, NULL, NULL, &findEdgeOptions, &straightEdgeOptions));

    // ////////////////////////////////////////
    // Store the results in the data structure.
    // ////////////////////////////////////////
	if (findEdgeReport == NULL) goto Error;

    if (findEdgeReport->numStraightEdges > 0)
    {
		lineEdge->numUsedEdges = findEdgeReport->straightEdges->numUsedEdges;
		
		lineEdge->edge.start.x = findEdgeReport->straightEdges->straightEdgeCoordinates.start.x;
		lineEdge->edge.start.y = findEdgeReport->straightEdges->straightEdgeCoordinates.start.y;
		lineEdge->edge.end.x   = findEdgeReport->straightEdges->straightEdgeCoordinates.end.x;
		lineEdge->edge.end.y   = findEdgeReport->straightEdges->straightEdgeCoordinates.end.y;

		lineEdge->angle = findEdgeReport->straightEdges->angle;
		lineEdge->straightness = findEdgeReport->straightEdges->straightness;
		lineEdge->score = findEdgeReport->straightEdges->score;

    }
    
Error:

	//if (findEdgeReport->numStraightEdges <= 0) {
	if (findEdgeReport == NULL) {
		lineEdge->numUsedEdges = 0;

		lineEdge->edge.start.x = 0.0;
		lineEdge->edge.start.y = 0.0;
		lineEdge->edge.end.x = 0.0;
		lineEdge->edge.end.y = 0.0;

		lineEdge->angle = 0.0;
		lineEdge->straightness = 0.0;
		lineEdge->score = 0.0;
	}

    // Disposes the edge report
    imaqDispose(findEdgeReport);

    return success;
}

POINTFLOAT CImageProcessing::CrossPoint(POINTFLOAT p1, POINTFLOAT p2, POINTFLOAT p3, POINTFLOAT p4)
{
	POINTFLOAT resPoint = { 0.0f, 0.0f };

	//float fIncrease1, fConstant1, fSaneValue1;
	//float fIncrease2, fConstant2, fSaneValue2;
	//// 직선 1.. 
	//if (p1.x == p2.x) 	// y축과 평행한 경우..
	//	fSaneValue1 = p1.x;
	//else { 				// x축과 평행한 경우와 아닌 경우 포함..
	//	fIncrease1 = (float)(p2.y - p1.y) / (p2.x - p1.x);	// 기울기 구하기
	//	fConstant1 = p1.y - fIncrease1 * p1.x;			// 상수값 구하기.
	//}

	//// 직선 2.. 
	//if (p3.x == p4.x) 	// y축과 평행한 경우..
	//	fSaneValue2 = p3.x;
	//else { 				// x축과 평행한 경우와 아닌 경우 포함..
	//	fIncrease2 = (float)(p4.y - p3.y) / (p4.x - p3.x); 	// 기울기 구하기.
	//	fConstant2 = p3.y - fIncrease2 * p3.x;			// 상수값 구하기.
	//}

	//// 교점..
	//CPoint pResult;
	//if (p1.x == p2.x && p3.x == p4.x) {		// 평행할 경우 교점 없음
	//	resPoint.x = resPoint.y = 0.0f;
	//}
	//else {

	//	if (p1.x = p2.x) {					// 직선 100 축에 평행할 경우 교점.
	//		resPoint.x = fSaneValue1;
	//		resPoint.y = fIncrease2 * fSaneValue1 + fConstant2;
	//	} 
	//	else if (p3.x == p4.x) {			// 직선 2가 축에 평행할 경우 교점 .
	//		resPoint.x = fSaneValue2;
	//		resPoint.y = fIncrease1 * fSaneValue2 + fConstant1;
	//	}
	//	else {
	//		pResult.x - -(fConstant1 - fConstant2) / (fIncrease1 - fIncrease2); 	// 교점의 x 좌표.. 
	//		pResult.y - fIncrease1 * pResult.x + fConstant1;						// 교점의 y 좌표.. 
	//	}
	//}

	float d = (p1.x - p2.x) * (p3.y - p4.y) - (p1.y - p2.y) * (p3.x - p4.x);

	// If d is zero, there is no intersection
	if (d == 0) return resPoint;

	// Get the x and y
	double pre = (p1.x * p2.y - p1.y * p2.x);
	double post = (p3.x * p4.y - p3.y * p4.x);
	float x = (pre * (p3.x - p4.x) - (p1.x - p2.x) * post) / d;
	float y = (pre * (p3.y - p4.y) - (p1.y - p2.y) * post) / d;

	resPoint = { x,y };

	// Return the point of intersection
	return resPoint;
}

void CImageProcessing::Config(BOOL bFlag, CString sIniFile)
{
	CIni ini;
	ini.SetPathName(sIniFile);
	CString section;

	if (bFlag) {	// Save
	}
	else {			// Load
		
		// IMAGE
		int icx, icy;
		icx = ini.GetInt(_T("IMAGE"), _T("CenterX"), 4300);
		icy = ini.GetInt(_T("IMAGE"), _T("CenterY"), 4270);
		m_pcc.cen.x = icx;
		m_pcc.cen.y = icy;

		// Scale
		m_pcc.dScale = ini.GetDouble(_T("IMAGE"), _T("Scale"), 1.0);

		// Find Circular Edge
		m_pcc.dRadius = ini.GetInt(_T("CIRCULAR"), _T("Radius"), 220 );
		m_pcc.nInnerR = ini.GetInt(_T("CIRCULAR"), _T("InnerR"), 3150);
		m_pcc.nOuterR = ini.GetInt(_T("CIRCULAR"), _T("OuterR"), 3600);
		m_pcc.nCirThreshold = ini.GetInt(_T("CIRCULAR"), _T("Threshold"), 35);

		// Edge
		m_pcc.nRcpHeight = ini.GetInt(_T("RCP"), _T("Height"), 560);
		m_pcc.nRcpWidth  = ini.GetInt(_T("RCP"), _T("Width" ), 20);

		m_pcc.dEdgeAngle = ini.GetDouble(_T("EDGE"), _T("Angle"), 15);
		m_pcc.nEdgeThreshold = ini.GetInt(_T("EDGE"), _T("Threshold"), 30);

		CString sKey, sValue, sTop, sLeft;
		int iPos=0;
		Point po;
		for (int i = 0; i < PCC_POINTS; i++)
		{
			sKey.Format(_T("L%02d"), i + 1);
			sValue = ini.GetString(_T("EDGE"), sKey, _T("0,0"));
			iPos = 0;
			sTop   = sValue.Tokenize(_T(","), iPos);
			sLeft  = sValue.Tokenize(_T(" "), iPos);
			po.x = _ttoi(sLeft);
			po.y = _ttoi(sTop);
			m_pcc.pEdge[i] = po;
		}
	}
}

// 2024.02.10 jh.kim 추가
int  CImageProcessing::OverlayText(Image* image, int originX, int originY, char* text, unsigned char colors[], char* fName, int textOptions[], int alignmentH, int alignmentV, double ang)
{
	Point    origin;
	RGBValue color;
	RGBValue background;
	OverlayTextOptions overlayTextOptions;

	origin.x = originX;
	origin.y = originY;

	color.R = colors[0];
	color.G = colors[1];
	color.B = colors[2];
	color.alpha = 0;

	background.R = colors[3];
	background.G = colors[4];
	background.B = colors[5];
	background.alpha = colors[6];

	overlayTextOptions.fontName = fName;
	overlayTextOptions.fontSize = textOptions[0];
	overlayTextOptions.bold = textOptions[1];
	overlayTextOptions.italic = textOptions[2];
	overlayTextOptions.underline = textOptions[3];
	overlayTextOptions.strikeout = textOptions[4];
	overlayTextOptions.horizontalTextAlignment = TextAlignment(alignmentH);
	overlayTextOptions.verticalTextAlignment = VerticalTextAlignment(alignmentV);
	overlayTextOptions.backgroundColor = background;
	overlayTextOptions.angle = ang;

	// Overlays the string of text onto the image.
	imaqOverlayText(image, origin, text, &color, &overlayTextOptions, NULL);

	return TRUE;
}
