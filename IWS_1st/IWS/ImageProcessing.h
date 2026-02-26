#pragma once

#include "./VRSaux/NIVision/Include/nivision.h"

//  Defines
#define DISPLAY_WINDOW 0

#define PCC_POINTS 24

#define IVA_MAX_BUFFERS 20
#define IVA_STORE_RESULT_NAMES

#define VisionErrChk(Function) {if (!(Function)) {success = 0; goto Error;}}

typedef struct CircularEdge_struct {
	int        circleFound;                  //Specifies if the circular edge was found.
	PointFloat cen;                          //Pixel coordinates of the center.
	double     radius;                       //Radius in pixels of the circular edge found.
	double     roundness;                    //The roundness of the calculated circular edge.
	CircularEdge_struct() 
	{
		circleFound = 0;
		cen = { 0.0, 0.0 };
		radius = 0.0;
		roundness = 0.0;
	}
} CircularEdge;

typedef struct LineEdge_struct {
	LineFloat    edge;           //End points of the detected straight edge in pixel coordinates.
	double       angle;          //Angle of the found edge using the pixel coordinates.
	double       score;          //Describes the score of the detected edge.
	double       straightness;   //The straightness value of the detected straight edge.
	unsigned int numUsedEdges;   //Indicates the number of edges in the usedEdges array.
	LineEdge_struct()
	{
		edge.start = { 0.0, 0.0 };
		edge.end = { 0.0, 0.0 };
		angle = 0.0;
		score = 0.0;
		straightness = 0.0;
		numUsedEdges = 0;
	}
} LineEdge;

typedef struct _PCC_INFO
{
	Point cen;
	double dScale;

	double dRadius;
	int nInnerR, nOuterR;
	int nCirThreshold;

	double nRcpHeight, nRcpWidth;
	double dEdgeAngle;
	int nEdgeThreshold;

	Point pEdge[PCC_POINTS];

	_PCC_INFO()
	{
		cen = { 0, 0 };
		dScale = 1.0;

		dRadius = 0.0;
		nInnerR = 0;
		nOuterR = 0;
		nCirThreshold = 30;

		nRcpHeight = 0;
		nRcpWidth = 0;
		nEdgeThreshold = 30;

		for (int i = 0; i < PCC_POINTS; i++)
			pEdge[i] = { 0, 0 };
	}
} PCC_INFO;

// Find Circulator
#define CIRLCE_POLARITY				IMAQ_SEARCH_FOR_RISING_EDGES
#define CIRLCE_KERNEL_SIZE			3
#define CIRLCE_WIDTH				3
//#define CIRLCE_THRESHOLD			35
#define CIRCLE_INTERPOLATION_TYPE	IMAQ_BILINEAR_FIXED
#define CIRCLE_PROCESSING_MODE		IMAQ_AVERAGE_COLUMNS
#define CIRCLE_PROCESS				IMAQ_FIRST
#define CIRCLE_STEP_SIZE			10

// Find Edge
#define EDGE_POLARITY				IMAQ_SEARCH_FOR_RISING_EDGES
#define EDGE_KERNEL_SIZE			3
#define EDGE_WIDTH					3
//#define EDGE_THRESHOLD			65
//#define EDGE_THRESHOLD			30
#define EDGE_INTERPOLATION_TYPE		IMAQ_BILINEAR_FIXED
#define EDGE_PROCESSING_MODE		IMAQ_AVERAGE_COLUMNS
#define EDGE_SEARCH_MODE			IMAQ_USE_FIRST_RAKE_EDGES
#define EDGE_STEP_SIZE				10

//
class CImageProcessing
{
// 생성입니다.
public:
	CImageProcessing();
	virtual ~CImageProcessing();

protected:

	PCC_INFO m_pcc;

	CircularEdge cenCircle;
	LineEdge	 cenEdge;

	float m_resX, m_resY;
	double m_radius, m_scale;


public:

	int ProcessLineEdge	(Image *image, POINTFLOAT pPos[], float resolX, float resolY);

	CircularEdge FindCircular(Image* img, Point piMake, int nInnerRadius, int nOuterRadius, double dStartAngle, double dEndAngle, unsigned int nStepSize = 10);
	int FindCircularEdge(Image* image, ROI* roi, int pDirection, unsigned int pStepSize, CircularEdge *cirEdge);

	LineEdge FindLine1(Image* img, RotatedRect rotRect);
	LineEdge FindLine2(Image* img, RotatedRect rotRect);
	int FindEdge(Image* image, ROI* roi, int pDirection, unsigned int pStepSize, LineEdge* lineEdge);

	BOOL FindCrossPoint(int no, Image* img, int nx, int ny, int nheight, int nwidth, double dangle, POINTFLOAT pPos[]);
	POINTFLOAT CrossPoint(POINTFLOAT p1, POINTFLOAT p2, POINTFLOAT p3, POINTFLOAT p4);

	BOOL Logging(int no, POINTFLOAT pt1, POINTFLOAT pt2);

	void Config(BOOL bFlag, CString sIniFile);

	// 2024.02.10 jh.kim 추가
	int OverlayText(Image* image, int originX, int originY, char* text, unsigned char colors[], char* fName, int textOptions[], int alignmentH, int alignmentV, double ang);


};
