#pragma once
// VRS: "Vision Recognition Suite"
#include <vector>
#include <string>
#include "./NIVision/Include/nivision.h"

using namespace std;

extern HANDLE	g_hEventPopThreadSrt;
extern HANDLE	g_hEventPushQueueDone;
extern HANDLE	g_hEventSrtCopyDispData;
extern HANDLE	g_hEventDisplayStart;

extern CString	g_strLogPath;
extern CString	g_strSimulImagePath;

extern CFont	g_textFont;
extern HBRUSH	g_hBrushBk;

//extern int		g_nCPUCore;
extern double	g_AvgFrmRate;
extern BOOL		g_bLiveDisplay;
extern BOOL		g_bGrabThreadAlive;

extern CRITICAL_SECTION G_cs;
extern ImageType g_ImgType;
extern BOOL		g_bCameraStateOK;


// default & definition
#define SCANNER_SCAN_RANGE_X	100.f		// in mm
#define SCANNER_SCAN_RANGE_Y    100.f		// in mm
#define NUM_CAM_PIXEL_X			1280			// in pixels
#define NUM_CAM_PIXEL_Y			1024			// in pixels
#define CAM_RESOL_X				10.f		// in um
#define CAM_RESOL_Y				10.f		// in um
#define CAM_LOCAL_FOV_X			(NUM_CAM_PIXEL_X*CAM_RESOL_X/1000.f)	// in mm
#define CAM_LOCAL_FOV_Y			(NUM_CAM_PIXEL_Y*CAM_RESOL_Y/1000.f)	// in mm
#define CAM_FULL_FOV_X			(SCANNER_SCAN_RANGE_X+CAM_LOCAL_FOV_X)	// in mm		
#define CAM_FULL_FOV_Y			(SCANNER_SCAN_RANGE_Y+CAM_LOCAL_FOV_Y)	// in mm		
#define SCAN_SRT_POS_X			(-SCANNER_SCAN_RANGE_X/2.f)					
#define SCAN_SRT_POS_Y			(SCANNER_SCAN_RANGE_Y/2.f)
#define SCAN_END_POS_X			(SCANNER_SCAN_RANGE_X/2.f)	
#define SCAN_END_POS_Y			(-SCANNER_SCAN_RANGE_Y/2.f)
#define OVERLAP_X				0	// 0 % overlap between frame
#define OVERLAP_Y				0   // 0 % overlap between frame

//
#define NUM_MAX_QUEUE_BUFF		100

// image type
#define IMG_GRAY_CHAN			1
#define IMG_RGB_CHAN			4

#define FRAME_IMG_CH			IMG_GRAY_CHAN
#define FRAME_IMG_WD			NUM_CAM_PIXEL_X
#define FRAME_IMG_HT			NUM_CAM_PIXEL_Y

#define NUM_IMG_2_SCAN_X		((int)(CAM_FULL_FOV_X/CAM_LOCAL_FOV_X) + 1)
#define NUM_IMG_2_SCAN_Y		((int)(CAM_FULL_FOV_Y/CAM_LOCAL_FOV_Y) + 1)
#define TOT_NUM_SCAN_IMG		(NUM_IMG_2_SCAN_Y * NUM_IMG_2_SCAN_X)
#define NUM_SCAN_QUEUE_BUF		((int)((float)TOT_NUM_SCAN_IMG * 1.5f))

#define WIDTHBYTES_VRS(w,bitcount) ( (((w)*(bitcount)+31) & ~31) >> 3)

// Image type and necessary path
#define JPG_IMG					(L".jpg")
#define ALL_JPG_IMG				(L"*.jpg")
#define LOG_PATH				(L"Vision\\Log")
#define SIMUL_IMG_PATH			(L"Vision\\Simulation")
#define SIMUL_IMG_NAME			(L"simul.jpg")


/* ---------------- Basler camara ----------------------------- */
#define NUM_INTERNAL_CAMBUF		3
#define DISP_IMG_WD				(FRAME_IMG_WD/2)
#define DISP_IMG_HT				(FRAME_IMG_HT/2)
#define DISP_IMG_CH				(FRAME_IMG_CH)

/*--- Color Definition --- {B, G, R, alpa} */
#define RGB_WHITE			{255, 255, 255, 0}
#define RGB_GREEN			{0, 255, 0, 0}
#define RGB_BLACK			{0, 0, 0, 0}
#define RGB_GRAY			{128, 128, 128, 0}
#define RGB_BRIGHT_GRAY		{180, 180, 180, 0}
#define RGB_VIOLET			{0, 196, 98, 0}
#define RGB_MAGENTA			{255, 0, 255, 0}
#define RGB_RED				{0, 0, 255, 0}
#define RGB_BLUE			{255, 0, 0, 0}
#define RGB_YELLOW			{0, 255, 255, 0}
#define RGB_BACK			{80, 80, 80, 0}
#define RGB_BACK1			{162, 162, 82, 0}
#define RGB_BACK2			{231, 210, 121, 0}
#define RGB_PALEGREEN		{153, 218, 190, 0}
#define RGB_ORANGE			{100, 170, 255, 0}
#define RGB_CYAN			{231, 210, 121, 0}
#define RGB_DARKCYAN		{157, 157,   0, 0}
#define RGB_LIGHT_GREEN		{127, 253, 203, 0}
#define	RGB_GREENISH		{170, 247, 199, 0 } 


#define RED					RGB(255,0,0)
#define GREEN				RGB(0, 255,0)
#define BLACK				RGB(0, 0, 0)
#define YELLOW				RGB(255,255,0)
#define MAGENTA				RGB(255,0,255)
#define WHITE				RGB(255,255,255)
#define BLUE				RGB(30,150,249)
#define CYAN				RGB(121, 210, 231)
#define DARKCYAN			RGB(0, 157, 157)
#define BACK				RGB(80, 80, 80)
#define BACK1				RGB(82, 162, 162)
#define GREENBLUE			RGB(121, 210, 231)
#define GRAY				RGB(80, 80, 80)
#define ORANGE				RGB(255, 170, 100)
#define PALEGREEN			RGB(190, 218, 153)

/* ------------------------ */
//#define NUM_MAX_CPU_CORE	4
#define	BORDER_SIZE			2

//
typedef struct _effectivelocalfov {
	float rtoX;				// % of used cam area along x dir when overlap is applied,	(100 - overlapX)/100
	float rtoY;				// % of used cam area along y dir when overlap is applied,	(100 - overlapY)/100
	Rect  eLocalFovRect;	// rect have info about which part of image is extracted
	float elfX;				// effective local fov on x direction with above condition. 
	float elfY;				// effective local fov on y direction with above condition.

}EFFECTIVE_LOCAL_FOV;	// Effective Local FOV


typedef struct _scaninfo {
	float	fRange_X;					// Laser Scanner Scan Range in X(mm)
	float	fRange_Y;					// Laser Scanner Scan Range in Y(mm)

	POINTF	ptSrtPos;					// VRS scan start position X, Y(mm)
	POINTF	ptCurSrtPos;				// VRS scan start position X, Y(mm)

	POINTF	ptEndPos;					// VRS scan end position X, Y(mm)
	POINTF	ptCurEndPos;				// VRS scan end position X, Y(mm)

	int		nOverlap_X;					// % overlap ratio between camera frame on X Dir
	int		nOverlap_Y;					// % overlap ratio between camera frame on Y Dir
	int		nCurOverlap_X;				// % Cur overlap ratio between camera frame on X Dir
	int		nCurOverlap_Y;				// % Cur overlap ratio between camera frame on Y Dir

	std::vector<POINTF> ptScanCoord;	// Scanner scan coordinate
	std::vector<BOOL> bSkipScanCoord;	// flag whether a certain scan position will skip
	std::vector<Rect> rectScanCoord;	// rect for scan position
	int		nBlockX;
	int		nBlockY;

	EFFECTIVE_LOCAL_FOV eLocalFovInfo;	// effetive local fov
}SCAN_INFO;

typedef struct _lightinfo {
	int		nIntensity;				// External scout light intensity
	bool	bConnected;				// Flag for connection
	int		nComPort;				// Serial Comport for light control

}LIGHT_INFO;

typedef struct _caminfo {
	int		nBinning;				// Binning ratio( 1, 2, 4..)
	int		nFRate;					// VRS camera frame rate
	int		nChan;					// Number of image channel
	int		nWidth;					// VRS camera number of pixel in x(horizontal)
	int		nHeight;				// VRS camera number of pixel in Y(vertical)
	float   fResol_X;				// VRS camera pixel resolution in X(um~mm/pxl)
	float	fResol_Y;				// VRS camera pixel resolution in Y(um~mm/pxl)
	float	fLocalFov_X;			// VRS camera's local FOV in X(mm)
	float	fLocalFov_Y;			// VRS camera's local FOV in Y(mm)
	float	fFullFov_X;				// VRS camera's full-field FOV in X(mm)
	float	fFullFov_Y;				// VRS camera's full-field FOV in Y(mm)
	float	fCurFullFov_X;			// Cur full-field FOV calculated by cur scan setting in X(mm)
	float	fCurFullFov_Y;			// Cur full-field FOV calculated by cur scan setting in Y(mm)
	CString strIpAddress;			// Ethernet type ip address

}CAM_INFO;

// Action Purpose
enum _actpurpose {
	ACT_NONE = -1,
	ACT_LOAD_IMG_FROM_DISK,
	ACT_CALIB_DISTO_ONLY,			// only disto calib (No scan)
	ACT_CALIB_BRIGH_ONLY,			// only brightness calib(No scan)
	ACT_DETECT_ONLY,				// only detect pin(No scan, No calib)
	ACT_SCAN_ONLY,					// just scan (no calibration)
	ACT_SCAN4_CALIB_DISTO,			// only scan for calib disto
	ACT_SCAN4_CALIB_BRIGH,			// only scan for calib brightness
	ACT_SCAN_AND_CALIB,				// scan and calib(disto & brightness)
	ACT_SCAN_AND_CALIB_AND_DETECT	// scan -> calib -> detect pin
};

typedef struct _stitch_img {
	// memory to make a large stitch-image from each scanned local cam image
//	unsigned char*	pMosData;

	Image*	pMosaicImg;			// One Large stitched image
	Image*	pScanImg;			// Use for on scan process (push: ccd data -> QBuffer, pop:QBuffer -> image -> copy to Mosaic image)
								// Use for on load sequential patch image (file -> scan img -> copy to Mosaic image

	int		iMosaicNiWindow;
	//
	Image*  pLoadImg;			// For loading image 
	Image*  pProcessImg;		// For processing stitch image 

	int		nHt;
	int		nWd;
	int		nChMosaic;
	int		iActionPurpose;			// construction purpose
	bool	bMosaicImgAvailable;	// stitched mos img is available by loading patches or scanned

	int		iSubAction;
	CString strName4Save;			//  full path & name to save stitched image
	
	// 2019.09.26 
	BYTE** ppScanImgData;			// Original scanned image data are stored at this variables
									// but when scan take places for calibration, those are stored at 
									// CALIB_INFO struct Image** ppCalibImg;
	int		iStitchMethod;

	//  add 2019.10.02     
	// reason: In processing origianl stitch imag. it's too big to handle
	Image* pDownMosaicImg;

	// info for VRS to CAD conversion
	PointDouble downImgCen;			// in pixel
	int	downImgWd;				// in pixel
	int downImgHt;				// in pixel

	// mm/piexl, conversion factor for image coord to CAD(IMAGE_ON_CAD_OVERLAY_DIM_X X IMAGE_ON_CAD_OVERLAY_DIM_Y)
	double fX;
	double fY;
	double offsetX;
	double offsetY;

	//DHChoi 20200629
	PointFloat pfStartPos_mm;
	PointFloat pfCenterPos_mm;
}STITCH_IMG;

enum {
	CALLBACK_PURPOSE_NONE = -1,
	CALLBACK_PURPOSE_EXTRACT,
};

typedef struct _callbackInfo {
	WindowEventType eventType = IMAQ_NO_EVENT;
	int windowNumber = -1;
	Tool tool = IMAQ_NO_TOOL;
	Rect cbRect = Rect();
	int  callbackPurpose = CALLBACK_PURPOSE_NONE;
	bool bRoiCreated = false;
	Rect curRect = Rect();
	
//	Point auxPt;			// auxillary points for mouse's selection process
	bool bAuxOp = false;	// auxillary operation check flag
//	int keyIdx = -1;		// left:0, bot:1, right:2, top:3, cen:4 
}CALLBACKINFO;


#define CROSS_HAIR_SIZE		300		// in pixel 
#define MPI					3.14159265358979f 

// USER MESSAGE DEF
#define	UWM_MOUSE_EVENT			(WM_USER+101)
#define	UWM_LOGGING				(WM_USER+201)
//#define	UWM_DISPIMG			(WM_USER+301)
#define UWM_DISPHISTO			(WM_USER+401)
#define UWM_PROGRESS_BAR		(WM_USER+501)

// DRAW HISTOGRAM
#define BOTTOM_FUDGE_FACTOR		-3

#define EFF_AREA_RATIO(x)		((float)((100.f - (float)x) / 100.f))

#define NUM_HISTO_DIV	256
typedef struct _histoinfo {
	int* pHisto;
	int minCnt;
	int maxCnt;
	int grayAtMax;

}HISTO_INFO;


enum {
	MOVE_TO_LEFT = 0,
	MOVE_TO_RIGHT,
	MOVE_TO_UP,
	MOVE_TO_DOWN
};


// # of sub block should be a odd number( i.e. 1, 3, 5, 7, 9, etc)
#define NUM_SUB_IMAGE_BLOCK_X	9		
#define NUM_SUB_IMAGE_BLOCK_Y	9

enum _subaction {
	SUB_ACT_NONE = -1,
	SUB_ACT_SAVE,	// save stitched image
};


enum _stitchmethod {
	STITCH_SIMPLE = 0,
	STITCH_1ST_ORDER,
	STITCH_2ND_ORDER  
};

#define NUM_PIXEL_STITCH_CHECK 60	// # of pixel range to search from orginal stitch position
#define CONV_ERROR_LIMIT 1.0f

#define IMAGE_ON_CAD_OVERLAY_DIM_X	197.5		// in mm
#define IMAGE_ON_CAD_OVERLAY_DIM_Y  202.5		// in mm
#define IMAGE_ON_CAD_OVERLAY_OFFSET_X	0.1		// in mm
#define IMAGE_ON_CAD_OVERLAY_OFFSET_Y   0.15		// in mm

#define DOWN_SAMPLE_FACTOR			2