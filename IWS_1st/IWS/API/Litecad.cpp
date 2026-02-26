/********************************************************************
* LiteCAD DLL version 2.0
* Copyright (c) 2009-2013, Oleg Kolbaskin.
* All rights reserved.
*
* This file must be included in C project
* that uses library Litecad.dll
* This file makes dynamic linking with LiteCAD functions.
*
* Visual C++ applications can not include this file,
* but use litecad.lib file instead
********************************************************************/

#include <windows.h>

#include "Litecad.h"


static HINSTANCE ghLibInst=0;

BOOL lcLoadLib()
{
  ghLibInst = LoadLibraryA( "litecad.dll" );
  return (ghLibInst)? TRUE : FALSE;
}

VOID lcFreeLib()
{
  FreeLibrary( ghLibInst );
  ghLibInst = 0;
}

typedef BOOL (LCAPI *tflcInitialize)();
typedef BOOL (LCAPI *tflcUninitialize)(BOOL bSaveConfig);
typedef HANDLE (LCAPI *tflcAddFontRes)(HANDLE hModule, int idResource, LPCWSTR szFontName);
typedef HANDLE (LCAPI *tflcAddFontFile)(LPCWSTR szFilename, LPCWSTR szFontName);
typedef HANDLE (LCAPI *tflcAddFontBin)(HANDLE hData, LPCWSTR szFontName);
typedef BOOL (LCAPI *tflcStrAdd)(LPCWSTR szTag, LPCWSTR szText);
typedef BOOL (LCAPI *tflcStrSet)(LPCWSTR szTag, LPCWSTR szText);
typedef LPCWSTR (LCAPI *tflcStrGet)(LPCWSTR szTag);
typedef BOOL (LCAPI *tflcStrFileLoad)(LPCWSTR szFileName);
typedef BOOL (LCAPI *tflcStrFileSave)(LPCWSTR szFileName, LPCWSTR szLanguage, BOOL bInit);
typedef BOOL (LCAPI *tflcPropGetBool)(HANDLE hObject, int idProp);
typedef int (LCAPI *tflcPropGetInt)(HANDLE hObject, int idProp);
typedef double (LCAPI *tflcPropGetFloat)(HANDLE hObject, int idProp);
typedef LPCWSTR (LCAPI *tflcPropGetStr)(HANDLE hObject, int idProp);
typedef int (LCAPI *tflcPropGetStrA)(HANDLE hObject, int idProp, char* szBuf, int BufSize);
typedef HANDLE (LCAPI *tflcPropGetHandle)(HANDLE hObject, int idProp);
typedef BOOL (LCAPI *tflcPropPutBool)(HANDLE hObject, int idProp, BOOL bValue);
typedef BOOL (LCAPI *tflcPropPutInt)(HANDLE hObject, int idProp, int Value);
typedef BOOL (LCAPI *tflcPropPutFloat)(HANDLE hObject, int idProp, double Value);
typedef BOOL (LCAPI *tflcPropPutStr)(HANDLE hObject, int idProp, LPCWSTR szValue);
typedef BOOL (LCAPI *tflcPropPutHandle)(HANDLE hObject, int idProp, HANDLE hValue);
typedef HANDLE (LCAPI *tflcCreateDrawing)();
typedef BOOL (LCAPI *tflcDeleteDrawing)(HANDLE hDrw);
typedef BOOL (LCAPI *tflcDrwNew)(HANDLE hDrw, LPCWSTR szFileName, HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcDrwNewT)(HANDLE hDrw, LPCWSTR szFileName, LPCWSTR szTemplate, HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcDrwLoad)(HANDLE hDrw, LPCWSTR szFileName, HWND hWnd, HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcDrwLoadMem)(HANDLE hDrw, HANDLE hMem, HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcDrwInsert)(HANDLE hDrw, LPCWSTR szFileName, int Overwrite, HWND hWnd);
typedef BOOL (LCAPI *tflcDrwSave)(HANDLE hDrw, LPCWSTR szFileName, BOOL bBak, HWND hWnd);
typedef int (LCAPI *tflcDrwSaveMem)(HANDLE hDrw, HANDLE hMem, int MemSize);
typedef BOOL (LCAPI *tflcDrwSaveExploded)(HANDLE hDrw, LPCWSTR szFileName, BOOL bBak, HWND hWnd);
typedef BOOL (LCAPI *tflcDrwPurge)(HANDLE hDrw);
typedef BOOL (LCAPI *tflcDrwRegenViews)(HANDLE hDrw, HANDLE hEnt);
typedef BOOL (LCAPI *tflcDrwSortObjects)(HANDLE hDrw, int ObjType);
typedef BOOL (LCAPI *tflcDrwClearXData)(HANDLE hDrw, int ObjType, int Mode);
typedef BOOL (LCAPI *tflcDrwResolveBlocks)(HANDLE hDrw, WCHAR* szError);
typedef HANDLE (LCAPI *tflcDrwAddLayer)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szColor, HANDLE hLtype, int Lweight);
typedef HANDLE (LCAPI *tflcDrwAddLinetype)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szDefinition);
typedef HANDLE (LCAPI *tflcDrwAddLinetypeF)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName, LPCWSTR szLtypeName);
typedef HANDLE (LCAPI *tflcDrwAddTextStyle)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szFontName);
typedef HANDLE (LCAPI *tflcDrwAddDimStyle)(HANDLE hDrw, LPCWSTR szName);
typedef HANDLE (LCAPI *tflcDrwAddMlineStyle)(HANDLE hDrw, LPCWSTR szName);
typedef HANDLE (LCAPI *tflcDrwAddImage)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName);
typedef HANDLE (LCAPI *tflcDrwAddImage2)(HANDLE hDrw, LPCWSTR szName, int Width, int Height, int nBits);
typedef HANDLE (LCAPI *tflcDrwAddBlock)(HANDLE hDrw, LPCWSTR szName, double X, double Y);
typedef HANDLE (LCAPI *tflcDrwAddBlockFromFile)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName, int Overwrite, HWND hwParent);
typedef HANDLE (LCAPI *tflcDrwAddBlockFromDrw)(HANDLE hDrw, LPCWSTR szName, HANDLE hDrw2, int Overwrite, HWND hwParent);
typedef HANDLE (LCAPI *tflcDrwAddBlockFile)(HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName, int Overwrite, HWND hwParent);
typedef HANDLE (LCAPI *tflcDrwAddViewPaper)(HANDLE hDrw, LPCWSTR szName, int PaperSize, int Orient, double Width, double Height);
typedef BOOL (LCAPI *tflcDrwDeleteObject)(HANDLE hDrw, HANDLE hObject);
typedef int (LCAPI *tflcDrwDeleteUnusedObjects)(HANDLE hDrw, int ObjType);
typedef HANDLE (LCAPI *tflcDrwGetFirstObject)(HANDLE hDrw, int ObjType);
typedef HANDLE (LCAPI *tflcDrwGetNextObject)(HANDLE hDrw, HANDLE hObject);
typedef HANDLE (LCAPI *tflcDrwGetObjectByID)(HANDLE hDrw, int ObjType, int Id);
typedef HANDLE (LCAPI *tflcDrwGetObjectByIDH)(HANDLE hDrw, int ObjType, LPCWSTR szId);
typedef HANDLE (LCAPI *tflcDrwGetObjectByName)(HANDLE hDrw, int ObjType, LPCWSTR szName);
typedef int (LCAPI *tflcDrwCountObjects)(HANDLE hDrw, int ObjType);
typedef HANDLE (LCAPI *tflcDrwGetViewByBlock)(HANDLE hDrw, HANDLE hBlock);
typedef HANDLE (LCAPI *tflcDrwGetEntByID)(HANDLE hDrw, int Id);
typedef HANDLE (LCAPI *tflcDrwGetEntByIDH)(HANDLE hDrw, LPCWSTR szId);
typedef HANDLE (LCAPI *tflcDrwGetEntByKey)(HANDLE hDrw, int Key);
typedef BOOL (LCAPI *tflcViewSetRect)(HANDLE hView, double Xcen, double Ycen, double Width, double Height);
typedef BOOL (LCAPI *tflcViewSetRect2)(HANDLE hView, double Lef, double Bot, double Rig, double Top);
typedef BOOL (LCAPI *tflcViewSetLimits)(HANDLE hView, double Xmin, double Ymin, double Xmax, double Ymax);
typedef BOOL (LCAPI *tflcViewSetGrid)(HANDLE hView, double X0, double Y0, double dX, double dY);
typedef BOOL (LCAPI *tflcViewRegen)(HANDLE hView, HANDLE hEnt);
typedef BOOL (LCAPI *tflcViewSetPaperSize)(HANDLE hView, int PaperSize, int Orient, double Width, double Height);
typedef HANDLE (LCAPI *tflcViewGetEntByPoint)(HANDLE hView, double X, double Y, double Delta, int Mode);
typedef int (LCAPI *tflcViewGetEntsByRect)(HANDLE hView, double Lef, double Bot, double Rig, double Top, BOOL bCross, int MaxNum);
typedef HANDLE (LCAPI *tflcViewGetEntity)(int iEnt);
typedef BOOL (LCAPI *tflcViewRasterize)(HANDLE hView, LPCWSTR szFileName, double Lef, double Bot, double Rig, double Top, double Scal);
typedef int (LCAPI *tflcViewRasterizeMem)(HANDLE hView, HANDLE hMem, double Lef, double Bot, double Rig, double Top, double Scal);
typedef BOOL (LCAPI *tflcViewDrawMarker)(HANDLE hView, double X, double Y);
typedef BOOL (LCAPI *tflcViewDrawLine)(HANDLE hView, double X0, double Y0, double X1, double Y1);
typedef BOOL (LCAPI *tflcViewDrawXline)(HANDLE hView, double X0, double Y0, double X1, double Y1, BOOL bRay);
typedef BOOL (LCAPI *tflcViewDrawRect)(HANDLE hView, double X0, double Y0, double X1, double Y1);
typedef BOOL (LCAPI *tflcViewPrint)(HANDLE hView, HDC hPrintDC, double Lef, double Bot, double Rig, double Top, double Scal, double PapLef, double PapTop);
typedef BOOL (LCAPI *tflcMLStyleAddLine)(HANDLE hStyle, double Offset, LPCWSTR szColor, HANDLE hLtype);
typedef BOOL (LCAPI *tflcMLStyleDelLine)(HANDLE hStyle, int iLine);
typedef BOOL (LCAPI *tflcMLStyleSortLines)(HANDLE hStyle);
typedef BOOL (LCAPI *tflcImageSetPixelRGB)(HANDLE hImage, int X, int Y, int Red, int Green, int Blue);
typedef BOOL (LCAPI *tflcImageSetPixelI)(HANDLE hImage, int X, int Y, int iColor);
typedef BOOL (LCAPI *tflcImageGetPixelRGB)(HANDLE hImage, int X, int Y, int* pRed, int* pGreen, int* pBlue);
typedef BOOL (LCAPI *tflcImageGetPixelI)(HANDLE hImage, int X, int Y, int* piColor);
typedef BOOL (LCAPI *tflcImageSetPalColor)(HANDLE hImage, int iColor, int Red, int Green, int Blue);
typedef BOOL (LCAPI *tflcImageGetPalColor)(HANDLE hImage, int iColor, int* pRed, int* pGreen, int* pBlue);
typedef BOOL (LCAPI *tflcImageSetTranColor)(HANDLE hImage, BOOL bTransparent, int Red, int Green, int Blue);
typedef BOOL (LCAPI *tflcImageResize)(HANDLE hImage, int Width, int Height);
typedef BOOL (LCAPI *tflcLayerClear)(HANDLE hLayer, HANDLE hBlock);
typedef BOOL (LCAPI *tflcBlockClear)(HANDLE hBlock, HANDLE hLayer);
typedef BOOL (LCAPI *tflcBlockPurge)(HANDLE hBlock);
typedef BOOL (LCAPI *tflcBlockSortEnts)(HANDLE hBlock, BOOL bByLayers);
typedef HANDLE (LCAPI *tflcBlockAddPoint)(HANDLE hBlock, double X, double Y);
typedef HANDLE (LCAPI *tflcBlockAddPoint2)(HANDLE hBlock, double X, double Y, int PtMode, double PtSize);
typedef HANDLE (LCAPI *tflcBlockAddXline)(HANDLE hBlock, double X, double Y, double Angle, BOOL bRay);
typedef HANDLE (LCAPI *tflcBlockAddXline2P)(HANDLE hBlock, double X, double Y, double X2, double Y2, BOOL bRay);
typedef HANDLE (LCAPI *tflcBlockAddLine)(HANDLE hBlock, double X1, double Y1, double X2, double Y2);
typedef HANDLE (LCAPI *tflcBlockAddLineDir)(HANDLE hBlock, double X, double Y, double Angle, double Dist);
typedef HANDLE (LCAPI *tflcBlockAddLineTan)(HANDLE hBlock, HANDLE hEnt1, HANDLE hEnt2, int Mode);
typedef HANDLE (LCAPI *tflcBlockAddPolyline)(HANDLE hBlock, int FitType, BOOL bClosed, BOOL bFilled);
typedef HANDLE (LCAPI *tflcBlockAddSpline)(HANDLE hBlock, BOOL bClosed, BOOL bFilled);
typedef HANDLE (LCAPI *tflcBlockAddMline)(HANDLE hBlock, int FitType, BOOL bClosed);
typedef HANDLE (LCAPI *tflcBlockAddRect)(HANDLE hBlock, double Xc, double Yc, double Width, double Height, double Angle, BOOL bFilled);
typedef HANDLE (LCAPI *tflcBlockAddRect2)(HANDLE hBlock, double Left, double Bottom, double Width, double Height, double Rad, BOOL bFilled);
typedef HANDLE (LCAPI *tflcBlockAddCircle)(HANDLE hBlock, double X, double Y, double Radius, BOOL bFilled);
typedef HANDLE (LCAPI *tflcBlockAddArc)(HANDLE hBlock, double X, double Y, double Radius, double StartAngle, double ArcAngle);
typedef HANDLE (LCAPI *tflcBlockAddArc3P)(HANDLE hBlock, double X1, double Y1, double X2, double Y2, double X3, double Y3);
typedef HANDLE (LCAPI *tflcBlockAddEllipse)(HANDLE hBlock, double X, double Y, double R1, double R2, double RotAngle, double StartAngle, double ArcAngle);
typedef HANDLE (LCAPI *tflcBlockAddText)(HANDLE hBlock, LPCWSTR szText, double X, double Y);
typedef HANDLE (LCAPI *tflcBlockAddText2)(HANDLE hBlock, LPCWSTR szText, double X, double Y, int Align, double H, double WScale, double RotAngle, double Oblique);
typedef HANDLE (LCAPI *tflcBlockAddTextWin)(HANDLE hBlock, LPCWSTR szText, double X, double Y);
typedef HANDLE (LCAPI *tflcBlockAddTextWin2)(HANDLE hBlock, LPCWSTR szText, double X, double Y, int Align, double H, double WScale, double RotAngle, double Oblique);
typedef HANDLE (LCAPI *tflcBlockAddMText)(HANDLE hBlock, LPCWSTR szText, double X, double Y, double WrapWidth, int Align, double RotAngle, double H, double WScale);
typedef HANDLE (LCAPI *tflcBlockAddArcText)(HANDLE hBlock, LPCWSTR szText, double X, double Y, double Radius, double StartAngle, BOOL bClockwise, double H, double WScale, int Align);
typedef HANDLE (LCAPI *tflcBlockAddBlockRef)(HANDLE hBlock, HANDLE hRefBlock, double X, double Y, double Scal, double Angle);
typedef HANDLE (LCAPI *tflcBlockAddBlockRefID)(HANDLE hBlock, int idRefBlock, double X, double Y, double Scal, double Angle);
typedef HANDLE (LCAPI *tflcBlockAddBlockRefIDH)(HANDLE hBlock, LPCWSTR szIdRefBlock, double X, double Y, double Scal, double Angle);
typedef HANDLE (LCAPI *tflcBlockAddImageRef)(HANDLE hBlock, HANDLE hImage, double X, double Y, double Width, double Height, BOOL bBorder);
typedef HANDLE (LCAPI *tflcBlockAddEcw)(HANDLE hBlock, LPCWSTR szFileName);
typedef HANDLE (LCAPI *tflcBlockAddAttDef)(HANDLE hBlock, int Mode, LPCWSTR szTag, LPCWSTR szPrompt, LPCWSTR szDefVal, double X, double Y, int Align, double H, double WScale, double RotAngle, double Oblique);
typedef HANDLE (LCAPI *tflcBlockAddHatch)(HANDLE hBlock, LPCWSTR szFileName, LPCWSTR szPatName, double Scal, double Angle);
typedef HANDLE (LCAPI *tflcBlockAddViewport)(HANDLE hBlock, double Lef, double Bot, double Width, double Height, double DrwPntX, double DrwPntY, double DrwScale, double DrwAngle);
typedef HANDLE (LCAPI *tflcBlockAddFace)(HANDLE hBlock, int Flags, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2);
typedef HANDLE (LCAPI *tflcBlockAddFace4)(HANDLE hBlock, int Flags, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3);
typedef HANDLE (LCAPI *tflcBlockAddLeader)(HANDLE hBlock, LPCWSTR szText, double Xt, double Yt, double LandDist, double Xa, double Ya, int Attach, int Align);
typedef HANDLE (LCAPI *tflcBlockAddDimRot)(HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Xt, double Yt, double Angle, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimHor)(HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Yt, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimVer)(HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Xt, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimAli)(HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Xt, double Yt, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimAli2)(HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Dt, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimOrd)(HANDLE hBlock, double Xd, double Yd, double Xt, double Yt, bool bX, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimRad)(HANDLE hBlock, double Xc, double Yc, double Xr, double Yr, double Xt, double Yt, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimRad2)(HANDLE hBlock, double Xc, double Yc, double R, double Angle, double TextOff, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimDia)(HANDLE hBlock, double Xc, double Yc, double Xr, double Yr, double Xt, double Yt, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimDia2)(HANDLE hBlock, double Xc, double Yc, double R, double Angle, double TextOff, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimAng)(HANDLE hBlock, double Xc, double Yc, double X1, double Y1, double X2, double Y2, double Xa, double Ya, double TextPos, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddDimAng2)(HANDLE hBlock, double X1, double Y1, double X2, double Y2, double X3, double Y3, double X4, double Y4, double Xa, double Ya, double TextPos, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcBlockAddRPlan)(HANDLE hBlock);
typedef HANDLE (LCAPI *tflcBlockAddBezier)(HANDLE hBlock, double X1, double Y1, double Angle1, double X2, double Y2, double Angle2);
typedef HANDLE (LCAPI *tflcBlockAddTIN)(HANDLE hBlock);
typedef HANDLE (LCAPI *tflcBlockAddClone)(HANDLE hBlock, HANDLE hEnt);
typedef HANDLE (LCAPI *tflcBlockGetFirstEnt)(HANDLE hBlock);
typedef HANDLE (LCAPI *tflcBlockGetNextEnt)(HANDLE hBlock, HANDLE hEnt);
typedef HANDLE (LCAPI *tflcBlockGetLastEnt)(HANDLE hBlock);
typedef HANDLE (LCAPI *tflcBlockGetPrevEnt)(HANDLE hBlock, HANDLE hEnt);
typedef HANDLE (LCAPI *tflcBlockGetEntByID)(HANDLE hBlock, int Id);
typedef HANDLE (LCAPI *tflcBlockGetEntByIDH)(HANDLE hBlock, LPCWSTR szId);
typedef HANDLE (LCAPI *tflcBlockGetEntByKey)(HANDLE hBlock, int Key);
typedef HANDLE (LCAPI *tflcBlockGetTIN)(HANDLE hBlock, int Index);
typedef BOOL (LCAPI *tflcBlockUnselect)(HANDLE hBlock);
typedef BOOL (LCAPI *tflcBlockSelectEnt)(HANDLE hBlock, HANDLE hEntity, BOOL bSelect);
typedef BOOL (LCAPI *tflcBlockSelErase)(HANDLE hBlock);
typedef BOOL (LCAPI *tflcBlockSelMove)(HANDLE hBlock, double dX, double dY, BOOL bCopy, BOOL bNewSelect);
typedef BOOL (LCAPI *tflcBlockSelScale)(HANDLE hBlock, double X0, double Y0, double Scal, BOOL bCopy, BOOL bNewSelect);
typedef BOOL (LCAPI *tflcBlockSelRotate)(HANDLE hBlock, double X0, double Y0, double Angle, BOOL bCopy, BOOL bNewSelect);
typedef BOOL (LCAPI *tflcBlockSelMirror)(HANDLE hBlock, double X1, double Y1, double X2, double Y2, BOOL bCopy, BOOL bNewSelect);
typedef BOOL (LCAPI *tflcBlockSelExplode)(HANDLE hBlock);
typedef HANDLE (LCAPI *tflcBlockSelJoin)(HANDLE hBlock, double Delta);
typedef HANDLE (LCAPI *tflcBlockGetFirstSel)(HANDLE hBlock);
typedef HANDLE (LCAPI *tflcBlockGetNextSel)(HANDLE hBlock);
typedef BOOL (LCAPI *tflcEntErase)(HANDLE hEnt, BOOL bErase);
typedef BOOL (LCAPI *tflcEntMove)(HANDLE hEnt, double dX, double dY);
typedef BOOL (LCAPI *tflcEntScale)(HANDLE hEnt, double X0, double Y0, double Scal);
typedef BOOL (LCAPI *tflcEntRotate)(HANDLE hEnt, double X0, double Y0, double Angle);
typedef BOOL (LCAPI *tflcEntMirror)(HANDLE hEnt, double X1, double Y1, double X2, double Y2);
typedef BOOL (LCAPI *tflcEntExplode)(HANDLE hEnt, BOOL bSelect);
typedef BOOL (LCAPI *tflcEntOffset)(HANDLE hEnt, double Dist);
typedef BOOL (LCAPI *tflcEntToTop)(HANDLE hEnt);
typedef BOOL (LCAPI *tflcEntToBottom)(HANDLE hEnt);
typedef BOOL (LCAPI *tflcEntToAbove)(HANDLE hEnt, HANDLE hEnt2);
typedef BOOL (LCAPI *tflcEntToUnder)(HANDLE hEnt, HANDLE hEnt2);
typedef BOOL (LCAPI *tflcEntGetGrip)(HANDLE hEnt, int iGrip, double* pX, double* pY);
typedef BOOL (LCAPI *tflcEntPutGrip)(HANDLE hEnt, int iGrip, double X, double Y);
typedef BOOL (LCAPI *tflcEntUpdate)(HANDLE hEnt);
typedef BOOL (LCAPI *tflcEntCopyBase)(HANDLE hEnt, HANDLE hEntFrom);
typedef int (LCAPI *tflcIntersection)(HANDLE hEnt, HANDLE hEnt2);
typedef BOOL (LCAPI *tflcInterGetPoint)(int iPoint, double* pX, double* pY);
typedef HANDLE (LCAPI *tflcPlineAddVer)(HANDLE hPline, HANDLE hVer, double X, double Y);
typedef HANDLE (LCAPI *tflcPlineAddVer2)(HANDLE hPline, HANDLE hVer, double X, double Y, double Param, double W0, double W1);
typedef HANDLE (LCAPI *tflcPlineAddVerDir)(HANDLE hPline, HANDLE hVer, double Ang, double Length);
typedef BOOL (LCAPI *tflcPlineDeleteVer)(HANDLE hPline, HANDLE hVer);
typedef HANDLE (LCAPI *tflcPlineGetFirstVer)(HANDLE hPline);
typedef HANDLE (LCAPI *tflcPlineGetNextVer)(HANDLE hPline, HANDLE hVer);
typedef HANDLE (LCAPI *tflcPlineGetLastVer)(HANDLE hPline);
typedef HANDLE (LCAPI *tflcPlineGetPrevVer)(HANDLE hPline, HANDLE hVer);
typedef HANDLE (LCAPI *tflcPlineGetVer)(HANDLE hPline, int Index);
typedef HANDLE (LCAPI *tflcPlineGetVerPt)(HANDLE hPline, double X, double Y, double Delta);
typedef HANDLE (LCAPI *tflcPlineGetSeg)(HANDLE hPline, double X, double Y, double Delta);
typedef BOOL (LCAPI *tflcPlineReverse)(HANDLE hPline);
typedef BOOL (LCAPI *tflcPlineSimple)(HANDLE hPline, BOOL bSimple, int nLines);
typedef int (LCAPI *tflcPlineContainPoint)(HANDLE hPline, double X, double Y);
typedef BOOL (LCAPI *tflcPlineGetRoundPrm)(HANDLE hPline, HANDLE hVer, double* px0, double* py0, double* pBulge, double* px1, double* py1);
typedef HANDLE (LCAPI *tflcMlineAddVer)(HANDLE hMline, HANDLE hVer, double X, double Y);
typedef HANDLE (LCAPI *tflcMlineAddVerDir)(HANDLE hMline, HANDLE hVer, double Ang, double Length);
typedef BOOL (LCAPI *tflcMlineDeleteVer)(HANDLE hMline, HANDLE hVer);
typedef HANDLE (LCAPI *tflcMlineGetFirstVer)(HANDLE hMline);
typedef HANDLE (LCAPI *tflcMlineGetNextVer)(HANDLE hMline, HANDLE hVer);
typedef HANDLE (LCAPI *tflcMlineGetLastVer)(HANDLE hMline);
typedef HANDLE (LCAPI *tflcMlineGetPrevVer)(HANDLE hMline, HANDLE hVer);
typedef HANDLE (LCAPI *tflcMlineGetVer)(HANDLE hMline, int Index);
typedef HANDLE (LCAPI *tflcMlineGetVerPt)(HANDLE hMline, double X, double Y, double Delta);
typedef HANDLE (LCAPI *tflcMlineGetSeg)(HANDLE hMline, double X, double Y, double Delta);
typedef BOOL (LCAPI *tflcMlineReverse)(HANDLE hMline);
typedef HANDLE (LCAPI *tflcRPlanAddVer)(HANDLE hRPlan, double X, double Y);
typedef BOOL (LCAPI *tflcRPlanSetCurve)(HANDLE hVer, double Radius, double LenClot1, double LenClot2);
typedef BOOL (LCAPI *tflcRPlanSetPos)(HANDLE hVer, double X, double Y);
typedef BOOL (LCAPI *tflcRPlanDeleteVer)(HANDLE hRPlan, HANDLE hVer);
typedef HANDLE (LCAPI *tflcRPlanGetFirstVer)(HANDLE hRPlan);
typedef HANDLE (LCAPI *tflcRPlanGetNextVer)(HANDLE hRPlan, HANDLE hVer);
typedef HANDLE (LCAPI *tflcRPlanGetLastVer)(HANDLE hRPlan);
typedef HANDLE (LCAPI *tflcRPlanGetPrevVer)(HANDLE hRPlan, HANDLE hVer);
typedef HANDLE (LCAPI *tflcRPlanGetVer)(HANDLE hRPlan, int Index);
typedef BOOL (LCAPI *tflcRPlanGetPoint)(HANDLE hRPlan, double Dist, double* pX, double* pY, double* pAngle, int* pSide);
typedef BOOL (LCAPI *tflcRPlanGetDist)(HANDLE hRPlan, double X, double Y, double* pX2, double* pY2, double* pDist, double* pOffset);
typedef BOOL (LCAPI *tflcVportSetView)(HANDLE hVport, double Xcen, double Ycen, double Scal, double Angle);
typedef BOOL (LCAPI *tflcVportFrolClear)(HANDLE hVport);
typedef BOOL (LCAPI *tflcVportFrolAdd)(HANDLE hVport, HANDLE hLayer);
typedef HANDLE (LCAPI *tflcVportFrolGet)(HANDLE hVport, int iLayer);
typedef BOOL (LCAPI *tflcVportIsFrol)(HANDLE hVport, HANDLE hLayer);
typedef HANDLE (LCAPI *tflcBlkRefAddAtt)(HANDLE hBlockRef, LPCWSTR szTag, LPCWSTR szValue);
typedef HANDLE (LCAPI *tflcBlkRefGetFirstAtt)(HANDLE hBlockRef);
typedef HANDLE (LCAPI *tflcBlkRefGetNextAtt)(HANDLE hBlockRef, HANDLE hAttrib);
typedef HANDLE (LCAPI *tflcBlkRefGetAtt)(HANDLE hBlockRef, LPCWSTR szTag);
typedef BOOL (LCAPI *tflcHatchSetPattern)(HANDLE hHatch, LPCWSTR szFileName, LPCWSTR szPatName, double Scal, double Angle);
typedef BOOL (LCAPI *tflcHatchBoundStart)(HANDLE hHatch);
typedef BOOL (LCAPI *tflcHatchBoundPoint)(HANDLE hHatch, double X, double Y);
typedef BOOL (LCAPI *tflcHatchBoundEndLoop)(HANDLE hHatch);
typedef BOOL (LCAPI *tflcHatchBoundEnd)(HANDLE hHatch);
typedef BOOL (LCAPI *tflcHatchPatStart)(HANDLE hHatch);
typedef BOOL (LCAPI *tflcHatchPatLine)(HANDLE hHatch, double Angle, double x0, double y0, double dx, double dy, int nDash, double L0, double L1, double L2, double L3, double L4, double L5, double L6, double L7);
typedef BOOL (LCAPI *tflcHatchPatEnd)(HANDLE hHatch);
typedef int (LCAPI *tflcHatchGetLoopSize)(HANDLE hHatch, int iLoop);
typedef BOOL (LCAPI *tflcHatchGetPoint)(HANDLE hHatch, int iPnt, double* pX, double* pY);
typedef int (LCAPI *tflcRectGetPolyline)(HANDLE hRect, double* pX, double* pY, double* pBulge);
typedef BOOL (LCAPI *tflcXlinePutDir)(HANDLE hXline, double X, double Y);
typedef BOOL (LCAPI *tflcTinClear)(HANDLE hTin, BOOL bOnlyTrians);
typedef BOOL (LCAPI *tflcTinLoad)(HANDLE hTin, LPCWSTR szFileName, BOOL bEmbed);
typedef BOOL (LCAPI *tflcTinSave)(HANDLE hTin, LPCWSTR szFileName, BOOL bOverwriteExist);
typedef HANDLE (LCAPI *tflcTinAddPtype)(HANDLE hTin, LPCWSTR szName, LPCWSTR szNotes, int Color);
typedef HANDLE (LCAPI *tflcTinAddPoint)(HANDLE hTin, HANDLE hPtype, double X, double Y, double Z);
typedef HANDLE (LCAPI *tflcTinAddTrian)(HANDLE hTin, HANDLE hPt0, HANDLE hPt1, HANDLE hPt2);
typedef BOOL (LCAPI *tflcTinNormalize)(HANDLE hTin);
typedef BOOL (LCAPI *tflcTinGetZ)(HANDLE hTin, double X, double Y, double* pZ);
typedef int (LCAPI *tflcTinInterLine)(HANDLE hTin, double X0, double Y0, double X1, double Y1);
typedef BOOL (LCAPI *tflcTinInterGetPoint)(HANDLE hTin, int iPnt, double* pX, double* pY, double* pZ);
typedef BOOL (LCAPI *tflcTinDelPtype)(HANDLE hTin, HANDLE hPtype);
typedef BOOL (LCAPI *tflcTinDelPoint)(HANDLE hTin, HANDLE hPoint);
typedef BOOL (LCAPI *tflcTinDelTrian)(HANDLE hTin, HANDLE hTrian);
typedef HANDLE (LCAPI *tflcTinPtypeGetFirst)(HANDLE hTin);
typedef HANDLE (LCAPI *tflcTinPtypeGetNext)(HANDLE hTin, HANDLE hPtype);
typedef HANDLE (LCAPI *tflcTinPtypeGetByName)(HANDLE hTin, LPCWSTR szName);
typedef BOOL (LCAPI *tflcTinPtypeDialog)(HANDLE hTin, HWND hWnd, BOOL bReadOnly);
typedef HANDLE (LCAPI *tflcTinPointGetFirst)(HANDLE hTin);
typedef HANDLE (LCAPI *tflcTinPointGetNext)(HANDLE hTin, HANDLE hPoint);
typedef HANDLE (LCAPI *tflcTinPointGetByPos)(HANDLE hTin, double X, double Y, double Delta);
typedef HANDLE (LCAPI *tflcTinTrianGetFirst)(HANDLE hTin);
typedef HANDLE (LCAPI *tflcTinTrianGetNext)(HANDLE hTin, HANDLE hTrian);
typedef HANDLE (LCAPI *tflcTinTrianGetByPos)(HANDLE hTin, double X, double Y);
typedef HANDLE (LCAPI *tflcCreateWindow)(HWND hWndParent, int Style, int Left, int Top, int Width, int Height);
typedef BOOL (LCAPI *tflcDeleteWindow)(HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcWndResize)(HANDLE hLcWnd, int Left, int Top, int Width, int Height);
typedef BOOL (LCAPI *tflcWndSelectView)(HANDLE hLcWnd, HANDLE hView);
typedef BOOL (LCAPI *tflcWndZoomRect)(HANDLE hLcWnd, double Left, double Bottom, double Right, double Top);
typedef BOOL (LCAPI *tflcWndZoomScale)(HANDLE hLcWnd, double Scal);
typedef BOOL (LCAPI *tflcWndZoomMove)(HANDLE hLcWnd, double DX, double DY);
typedef BOOL (LCAPI *tflcWndRedraw)(HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcWndSetFocus)(HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcWndExeCommand)(HANDLE hLcWnd, int Command, int CmdParam);
typedef BOOL (LCAPI *tflcWndCoordFromDrw)(HANDLE hLcWnd, double Xdrw, double Ydrw, int* pXwin, int* pYwin);
typedef BOOL (LCAPI *tflcWndCoordToDrw)(HANDLE hLcWnd, int Xwin, int Ywin, double* pXdrw, double* pYdrw);
typedef BOOL (LCAPI *tflcWndGetCursorCoord)(HANDLE hLcWnd, int* pXwin, int* pYwin, double* pXdrw, double* pYdrw);
typedef HANDLE (LCAPI *tflcWndGetEntByPoint)(HANDLE hLcWnd, int Xwin, int Ywin);
typedef BOOL (LCAPI *tflcWndSetCmdwin)(HANDLE hLcWnd, HANDLE hCmdLine);
typedef BOOL (LCAPI *tflcWndSetPropwin)(HANDLE hLcWnd, HANDLE hPropWnd);
typedef BOOL (LCAPI *tflcWndStopCommand)(HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcWndGetPoint)(HANDLE hLcWnd, LPCWSTR szPrompt, int SnapMode, int Index, double* pX, double* pY);
typedef BOOL (LCAPI *tflcWndGetPointBuf)(HANDLE hLcWnd, int Index, double* pX, double* pY);
typedef BOOL (LCAPI *tflcWndUpdateTabs)(HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcCoordDrwToWnd)(HANDLE hLcWnd, double Xdrw, double Ydrw, int* pXwin, int* pYwin);
typedef BOOL (LCAPI *tflcCoordWndToDrw)(HANDLE hLcWnd, int Xwin, int Ywin, double* pXdrw, double* pYdrw);
typedef BOOL (LCAPI *tflcColorIsRGB)(LPCWSTR szColor);
typedef int (LCAPI *tflcColorGetRed)(LPCWSTR szColor);
typedef int (LCAPI *tflcColorGetGreen)(LPCWSTR szColor);
typedef int (LCAPI *tflcColorGetBlue)(LPCWSTR szColor);
typedef int (LCAPI *tflcColorGetIndex)(LPCWSTR szColor);
typedef BOOL (LCAPI *tflcColorToVal)(LPCWSTR szColor, int* pbRGB, int* pIndex, int* pR, int* pG, int* pB);
typedef BOOL (LCAPI *tflcColorSetPalette)(int Index, int R, int G, int B);
typedef BOOL (LCAPI *tflcColorGetPalette)(int Index, int* pR, int* pG, int* pB);
typedef HANDLE (LCAPI *tflcCreateCmdwin)(HWND hWndParent, int Left, int Top, int Width, int Height);
typedef BOOL (LCAPI *tflcCmdwinResize)(HANDLE hCmdLine, int Left, int Top, int Width, int Height);
typedef BOOL (LCAPI *tflcCmdwinUpdate)(HANDLE hCmdLine);
typedef HANDLE (LCAPI *tflcCreatePropwin)(HWND hWndParent, int Left, int Top, int Width, int Height);
typedef BOOL (LCAPI *tflcPropwinResize)(HANDLE hPropWnd, int Left, int Top, int Width, int Height);
typedef BOOL (LCAPI *tflcPropwinUpdate)(HANDLE hPropWnd, BOOL bSelChanged);
typedef int (LCAPI *tflcGetErrorCode)();
typedef LPCWSTR (LCAPI *tflcGetErrorStr)();
typedef int (LCAPI *tflcExtractPreview)(LPCWSTR szFileName, HGLOBAL hGlob, int MaxSize);
typedef BOOL (LCAPI *tflcExtractFileInfo)(LPCWSTR szFileName, WCHAR* szGuid, WCHAR* szComment, int* pInt0, int* pInt1);
typedef BOOL (LCAPI *tflcFilletSetLines)(double L1x0, double L1y0, double L1x1, double L1y1, double L2x0, double L2y0, double L2x1, double L2y1);
typedef BOOL (LCAPI *tflcFillet)(double Rad, double Bis, double Tang);
typedef BOOL (LCAPI *tflcFilletGetPoint)(int iPnt, double* pX, double* pY);
typedef LPCWSTR (LCAPI *tflcDgGetFileName)(HWND hWnd, int Mode);
typedef LPCWSTR (LCAPI *tflcDgGetValue)(HWND hWnd, int Lef, int Top, LPCWSTR szTitle, LPCWSTR szPrompt, LPCWSTR szValue);
typedef BOOL (LCAPI *tflcDgGetValue2)(HWND hWnd, int Lef, int Top, LPCWSTR szTitle, LPCWSTR szPrompt, WCHAR* szValue, int MaxChar);
typedef LPCWSTR (LCAPI *tflcPlugGetOption)(LPCWSTR szFileName, LPCWSTR szKey);
typedef BOOL (LCAPI *tflcPlugSetOption)(LPCWSTR szFileName, LPCWSTR szKey, LPCWSTR szValue, BOOL bSave);
typedef void (LCAPI *tflcGetPolarPoint)(double X, double Y, double Ang, double Dist, double* pX, double* pY);
typedef BOOL (LCAPI *tflcGetClientSize)(HWND hWnd, int* pWidth, int* pHeight);
typedef void (LCAPI *tflcTextOut)(HDC hDC, int x, int y, LPCWSTR szText, int ColorRGB);
typedef HANDLE (LCAPI *tflcCreateCommand)(HANDLE hLcWnd, int Id, LPCWSTR szName, BOOL bNeedSel);
typedef BOOL (LCAPI *tflcCmdExit)();
typedef BOOL (LCAPI *tflcCmdPrompt)(HANDLE hCmd, LPCWSTR szText);
typedef HANDLE (LCAPI *tflcCmdGetEntByPoint)(HANDLE hCmd, int Xwnd, int Ywnd);
typedef BOOL (LCAPI *tflcCmdSelectEnt)(HANDLE hCmd, HANDLE hEntity, BOOL bSelect);
typedef BOOL (LCAPI *tflcCmdRedraw)(HANDLE hCmd);
typedef BOOL (LCAPI *tflcCmdSetFocus)(HANDLE hCmd);
typedef BOOL (LCAPI *tflcCmdRegen)(HANDLE hCmd, HANDLE hEnt);
typedef BOOL (LCAPI *tflcCmdClearPoints)(HANDLE hCmd);
typedef BOOL (LCAPI *tflcCmdAddPoint)(HANDLE hCmd, int Id, double X, double Y);
typedef BOOL (LCAPI *tflcCmdGetPoint)(HANDLE hCmd, int Id, double* pX, double* pY);
typedef BOOL (LCAPI *tflcCmdSetBasePoint)(HANDLE hCmd, BOOL bEnable, double X, double Y);
typedef BOOL (LCAPI *tflcCmdDrawLine)(HANDLE hCmd, double X0, double Y0, double X1, double Y1);
typedef BOOL (LCAPI *tflcCmdDrawPickbox)(HANDLE hCmd, int Xwnd, int Ywnd);
typedef BOOL (LCAPI *tflcCmdEqualPoints)(HANDLE hCmd, double X0, double Y0, double X1, double Y1, int Delta);
typedef BOOL (LCAPI *tflcPaintBegin)(HANDLE hLcWnd);
typedef BOOL (LCAPI *tflcPaintSetPen)(int Style, int Width, int ColorRed, int ColorGreen, int ColorBlue);
typedef BOOL (LCAPI *tflcPaintSetBrush)(int ColorRed, int ColorGreen, int ColorBlue);
typedef BOOL (LCAPI *tflcPaintPixel)(double X, double Y, int Size, int ColorRed, int ColorGreen, int ColorBlue);
typedef BOOL (LCAPI *tflcPaintPoint)(double X, double Y, int PtMode, double PtSize);
typedef BOOL (LCAPI *tflcPaintLine)(double X0, double Y0, double X1, double Y1);
typedef BOOL (LCAPI *tflcPaintPlineVer)(double X, double Y);
typedef BOOL (LCAPI *tflcPaintPline)(BOOL bClosed, BOOL bFilled, BOOL bBorder);
typedef BOOL (LCAPI *tflcPaintCircle)(double X, double Y, double R, BOOL bFilled, BOOL bBorder);
typedef BOOL (LCAPI *tflcPaintArc)(double X, double Y, double R, double Ang0, double AngArc);
typedef BOOL (LCAPI *tflcPaintSetFont)(LPCWSTR szFontName, double Height, double Angle, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut);
typedef BOOL (LCAPI *tflcPaintSetTextColor)(int ColorRed, int ColorGreen, int ColorBlue);
typedef BOOL (LCAPI *tflcPaintText)(LPCWSTR szText, double X, double Y, int Align);
typedef BOOL (LCAPI *tflcPaintEnd)();
typedef void (LCAPI *tflcSurfInitialize)(int MaxSectPnt, double AngStep);
typedef void (LCAPI *tflcSurfUnInitialize)();
typedef void (LCAPI *tflcSurfClearTriangles)();
typedef BOOL (LCAPI *tflcSurfAddTriangle)(double X0, double Y0, double Z0, double X1, double Y1, double Z1, double X2, double Y2, double Z2);
typedef BOOL (LCAPI *tflcSurfGetZ)(double X, double Y, double* pZ);
typedef BOOL (LCAPI *tflcSurfGetZ_dbg)(double X, double Y, double* pZ, double* pX0, double* pY0, double* pZ0, double* pX1, double* pY1, double* pZ1, double* pX2, double* pY2, double* pZ2);
typedef void (LCAPI *tflcSurfClearPlan)();
typedef BOOL (LCAPI *tflcSurfAddPlanVer)(double X, double Y, double Bulge);
typedef int (LCAPI *tflcSurfMakeSection)();
typedef BOOL (LCAPI *tflcSurfGetSectPoint)(int iPnt, double* pX, double* pY, double* pZ);
typedef BOOL (LCAPI *tflcCSectClear)();
typedef BOOL (LCAPI *tflcCSectBegin)(int PlineType);
typedef BOOL (LCAPI *tflcCSectAddPoint)(double X, double Y);
typedef BOOL (LCAPI *tflcCSectEnd)();
typedef int (LCAPI *tflcCSectCalcArea)();
typedef BOOL (LCAPI *tflcCSectGetArea)(int iArea, double* pAreaVal, int* pNumPnts);
typedef BOOL (LCAPI *tflcCSectGetAreaPoint)(int iArea, int iPnt, double* pX, double* pY);
typedef void (LCAPI *tflcEventReturnCode)(int code);
typedef void (LCAPI *tflcEventsEnable)(BOOL bEnable);
typedef void (LCAPI *tflcOnEventMouseMove)(F_MOUSEMOVE pFunc);
typedef void (LCAPI *tflcOnEventMouseDown)(F_MOUSEDOWN pFunc);
typedef void (LCAPI *tflcOnEventMouseUp)(F_MOUSEUP pFunc);
typedef void (LCAPI *tflcOnEventMouseDblClk)(F_MOUSEDBLCLK pFunc);
typedef void (LCAPI *tflcOnEventMouseSnap)(F_MOUSESNAP pFunc);
typedef void (LCAPI *tflcOnEventMouseLeave)(F_MOUSELEAVE pFunc);
typedef void (LCAPI *tflcOnEventMouseWheel)(F_MOUSEWHEEL pFunc);
typedef void (LCAPI *tflcOnEventKeyDown)(F_KEYDOWN pFunc);
typedef void (LCAPI *tflcOnEventPaint)(F_PAINT pFunc);
typedef void (LCAPI *tflcOnEventZoom)(F_ZOOM pFunc);
typedef void (LCAPI *tflcOnEventSelectView)(F_SELECTVIEW pFunc);
typedef void (LCAPI *tflcOnEventGetPoint)(F_GETPOINT pFunc);
typedef void (LCAPI *tflcOnEventAddCommand)(F_ADDCOMMAND pFunc);
typedef void (LCAPI *tflcOnEventCmdStart)(F_CMD_START pFunc);
typedef void (LCAPI *tflcOnEventCmdFinish)(F_CMD_FINISH pFunc);
typedef void (LCAPI *tflcOnEventCmdMouseDown)(F_CMD_MOUSEDOWN pFunc);
typedef void (LCAPI *tflcOnEventCmdMouseUp)(F_CMD_MOUSEUP pFunc);
typedef void (LCAPI *tflcOnEventCmdMouseMove)(F_CMD_MOUSEMOVE pFunc);
typedef void (LCAPI *tflcOnEventCmdString)(F_CMD_STRING pFunc);
typedef void (LCAPI *tflcOnEventAddEntity)(F_ADDENTITY pFunc);
typedef void (LCAPI *tflcOnEventRegen)(F_REGEN pFunc);
typedef void (LCAPI *tflcOnEventFioProgress)(F_FIOPROGRESS pFunc);
typedef void (LCAPI *tflcOnEventSelection)(F_SELECTION pFunc);
typedef void (LCAPI *tflcOnEventPropChanged)(F_PROPCHANGED pFunc);
typedef void (LCAPI *tflcOnEventGripSelect)(F_GRIPSELECT pFunc);
typedef void (LCAPI *tflcOnEventGripMove)(F_GRIPMOVE pFunc);
typedef void (LCAPI *tflcOnEventEntErase)(F_ENTERASE pFunc);
typedef void (LCAPI *tflcOnEventEntMove)(F_ENTMOVE pFunc);
typedef void (LCAPI *tflcOnEventEntRotate)(F_ENTROTATE pFunc);
typedef void (LCAPI *tflcOnEventEntScale)(F_ENTSCALE pFunc);
typedef void (LCAPI *tflcOnEventEntMirror)(F_ENTMIRROR pFunc);
typedef void (LCAPI *tflcOnEventEntProp)(F_ENTPROP pFunc);
typedef void (LCAPI *tflcOnEventHelp)(F_HELP pFunc);
typedef void (LCAPI *tflcOnEventAddStr)(F_ADDSTR pFunc);
typedef void (LCAPI *tflcOnEventFontReplace)(F_FONTREPLACE pFunc);

//--------------
BOOL LCAPI lcInitialize ()
{
  tflcInitialize pflcInitialize;
  pflcInitialize = (tflcInitialize)GetProcAddress( ghLibInst, "lcInitialize" );
  if (pflcInitialize){
    return (*pflcInitialize)();
  }
  return 0;
}


//--------------
BOOL LCAPI lcUninitialize (BOOL bSaveConfig)
{
  tflcUninitialize pflcUninitialize;
  pflcUninitialize = (tflcUninitialize)GetProcAddress( ghLibInst, "lcUninitialize" );
  if (pflcUninitialize){
    return (*pflcUninitialize)(bSaveConfig);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcAddFontRes (HANDLE hModule, int idResource, LPCWSTR szFontName)
{
  tflcAddFontRes pflcAddFontRes;
  pflcAddFontRes = (tflcAddFontRes)GetProcAddress( ghLibInst, "lcAddFontRes" );
  if (pflcAddFontRes){
    return (*pflcAddFontRes)(hModule, idResource, szFontName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcAddFontFile (LPCWSTR szFilename, LPCWSTR szFontName)
{
  tflcAddFontFile pflcAddFontFile;
  pflcAddFontFile = (tflcAddFontFile)GetProcAddress( ghLibInst, "lcAddFontFile" );
  if (pflcAddFontFile){
    return (*pflcAddFontFile)(szFilename, szFontName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcAddFontBin (HANDLE hData, LPCWSTR szFontName)
{
  tflcAddFontBin pflcAddFontBin;
  pflcAddFontBin = (tflcAddFontBin)GetProcAddress( ghLibInst, "lcAddFontBin" );
  if (pflcAddFontBin){
    return (*pflcAddFontBin)(hData, szFontName);
  }
  return 0;
}


//--------------
BOOL LCAPI lcStrAdd (LPCWSTR szTag, LPCWSTR szText)
{
  tflcStrAdd pflcStrAdd;
  pflcStrAdd = (tflcStrAdd)GetProcAddress( ghLibInst, "lcStrAdd" );
  if (pflcStrAdd){
    return (*pflcStrAdd)(szTag, szText);
  }
  return 0;
}


//--------------
BOOL LCAPI lcStrSet (LPCWSTR szTag, LPCWSTR szText)
{
  tflcStrSet pflcStrSet;
  pflcStrSet = (tflcStrSet)GetProcAddress( ghLibInst, "lcStrSet" );
  if (pflcStrSet){
    return (*pflcStrSet)(szTag, szText);
  }
  return 0;
}


//--------------
LPCWSTR LCAPI lcStrGet (LPCWSTR szTag)
{
  tflcStrGet pflcStrGet;
  pflcStrGet = (tflcStrGet)GetProcAddress( ghLibInst, "lcStrGet" );
  if (pflcStrGet){
    return (*pflcStrGet)(szTag);
  }
  return 0;
}


//--------------
BOOL LCAPI lcStrFileLoad (LPCWSTR szFileName)
{
  tflcStrFileLoad pflcStrFileLoad;
  pflcStrFileLoad = (tflcStrFileLoad)GetProcAddress( ghLibInst, "lcStrFileLoad" );
  if (pflcStrFileLoad){
    return (*pflcStrFileLoad)(szFileName);
  }
  return 0;
}


//--------------
BOOL LCAPI lcStrFileSave (LPCWSTR szFileName, LPCWSTR szLanguage, BOOL bInit)
{
  tflcStrFileSave pflcStrFileSave;
  pflcStrFileSave = (tflcStrFileSave)GetProcAddress( ghLibInst, "lcStrFileSave" );
  if (pflcStrFileSave){
    return (*pflcStrFileSave)(szFileName, szLanguage, bInit);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropGetBool (HANDLE hObject, int idProp)
{
  tflcPropGetBool pflcPropGetBool;
  pflcPropGetBool = (tflcPropGetBool)GetProcAddress( ghLibInst, "lcPropGetBool" );
  if (pflcPropGetBool){
    return (*pflcPropGetBool)(hObject, idProp);
  }
  return 0;
}


//--------------
int LCAPI lcPropGetInt (HANDLE hObject, int idProp)
{
  tflcPropGetInt pflcPropGetInt;
  pflcPropGetInt = (tflcPropGetInt)GetProcAddress( ghLibInst, "lcPropGetInt" );
  if (pflcPropGetInt){
    return (*pflcPropGetInt)(hObject, idProp);
  }
  return 0;
}


//--------------
double LCAPI lcPropGetFloat (HANDLE hObject, int idProp)
{
  tflcPropGetFloat pflcPropGetFloat;
  pflcPropGetFloat = (tflcPropGetFloat)GetProcAddress( ghLibInst, "lcPropGetFloat" );
  if (pflcPropGetFloat){
    return (*pflcPropGetFloat)(hObject, idProp);
  }
  return 0;
}


//--------------
LPCWSTR LCAPI lcPropGetStr (HANDLE hObject, int idProp)
{
  tflcPropGetStr pflcPropGetStr;
  pflcPropGetStr = (tflcPropGetStr)GetProcAddress( ghLibInst, "lcPropGetStr" );
  if (pflcPropGetStr){
    return (*pflcPropGetStr)(hObject, idProp);
  }
  return 0;
}


//--------------
int LCAPI lcPropGetStrA (HANDLE hObject, int idProp, char* szBuf, int BufSize)
{
  tflcPropGetStrA pflcPropGetStrA;
  pflcPropGetStrA = (tflcPropGetStrA)GetProcAddress( ghLibInst, "lcPropGetStrA" );
  if (pflcPropGetStrA){
    return (*pflcPropGetStrA)(hObject, idProp, szBuf, BufSize);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPropGetHandle (HANDLE hObject, int idProp)
{
  tflcPropGetHandle pflcPropGetHandle;
  pflcPropGetHandle = (tflcPropGetHandle)GetProcAddress( ghLibInst, "lcPropGetHandle" );
  if (pflcPropGetHandle){
    return (*pflcPropGetHandle)(hObject, idProp);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropPutBool (HANDLE hObject, int idProp, BOOL bValue)
{
  tflcPropPutBool pflcPropPutBool;
  pflcPropPutBool = (tflcPropPutBool)GetProcAddress( ghLibInst, "lcPropPutBool" );
  if (pflcPropPutBool){
    return (*pflcPropPutBool)(hObject, idProp, bValue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropPutInt (HANDLE hObject, int idProp, int Value)
{
  tflcPropPutInt pflcPropPutInt;
  pflcPropPutInt = (tflcPropPutInt)GetProcAddress( ghLibInst, "lcPropPutInt" );
  if (pflcPropPutInt){
    return (*pflcPropPutInt)(hObject, idProp, Value);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropPutFloat (HANDLE hObject, int idProp, double Value)
{
  tflcPropPutFloat pflcPropPutFloat;
  pflcPropPutFloat = (tflcPropPutFloat)GetProcAddress( ghLibInst, "lcPropPutFloat" );
  if (pflcPropPutFloat){
    return (*pflcPropPutFloat)(hObject, idProp, Value);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropPutStr (HANDLE hObject, int idProp, LPCWSTR szValue)
{
  tflcPropPutStr pflcPropPutStr;
  pflcPropPutStr = (tflcPropPutStr)GetProcAddress( ghLibInst, "lcPropPutStr" );
  if (pflcPropPutStr){
    return (*pflcPropPutStr)(hObject, idProp, szValue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropPutHandle (HANDLE hObject, int idProp, HANDLE hValue)
{
  tflcPropPutHandle pflcPropPutHandle;
  pflcPropPutHandle = (tflcPropPutHandle)GetProcAddress( ghLibInst, "lcPropPutHandle" );
  if (pflcPropPutHandle){
    return (*pflcPropPutHandle)(hObject, idProp, hValue);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcCreateDrawing ()
{
  tflcCreateDrawing pflcCreateDrawing;
  pflcCreateDrawing = (tflcCreateDrawing)GetProcAddress( ghLibInst, "lcCreateDrawing" );
  if (pflcCreateDrawing){
    return (*pflcCreateDrawing)();
  }
  return 0;
}


//--------------
BOOL LCAPI lcDeleteDrawing (HANDLE hDrw)
{
  tflcDeleteDrawing pflcDeleteDrawing;
  pflcDeleteDrawing = (tflcDeleteDrawing)GetProcAddress( ghLibInst, "lcDeleteDrawing" );
  if (pflcDeleteDrawing){
    return (*pflcDeleteDrawing)(hDrw);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwNew (HANDLE hDrw, LPCWSTR szFileName, HANDLE hLcWnd)
{
  tflcDrwNew pflcDrwNew;
  pflcDrwNew = (tflcDrwNew)GetProcAddress( ghLibInst, "lcDrwNew" );
  if (pflcDrwNew){
    return (*pflcDrwNew)(hDrw, szFileName, hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwNewT (HANDLE hDrw, LPCWSTR szFileName, LPCWSTR szTemplate, HANDLE hLcWnd)
{
  tflcDrwNewT pflcDrwNewT;
  pflcDrwNewT = (tflcDrwNewT)GetProcAddress( ghLibInst, "lcDrwNewT" );
  if (pflcDrwNewT){
    return (*pflcDrwNewT)(hDrw, szFileName, szTemplate, hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwLoad (HANDLE hDrw, LPCWSTR szFileName, HWND hWnd, HANDLE hLcWnd)
{
  tflcDrwLoad pflcDrwLoad;
  pflcDrwLoad = (tflcDrwLoad)GetProcAddress( ghLibInst, "lcDrwLoad" );
  if (pflcDrwLoad){
    return (*pflcDrwLoad)(hDrw, szFileName, hWnd, hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwLoadMem (HANDLE hDrw, HANDLE hMem, HANDLE hLcWnd)
{
  tflcDrwLoadMem pflcDrwLoadMem;
  pflcDrwLoadMem = (tflcDrwLoadMem)GetProcAddress( ghLibInst, "lcDrwLoadMem" );
  if (pflcDrwLoadMem){
    return (*pflcDrwLoadMem)(hDrw, hMem, hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwInsert (HANDLE hDrw, LPCWSTR szFileName, int Overwrite, HWND hWnd)
{
  tflcDrwInsert pflcDrwInsert;
  pflcDrwInsert = (tflcDrwInsert)GetProcAddress( ghLibInst, "lcDrwInsert" );
  if (pflcDrwInsert){
    return (*pflcDrwInsert)(hDrw, szFileName, Overwrite, hWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwSave (HANDLE hDrw, LPCWSTR szFileName, BOOL bBak, HWND hWnd)
{
  tflcDrwSave pflcDrwSave;
  pflcDrwSave = (tflcDrwSave)GetProcAddress( ghLibInst, "lcDrwSave" );
  if (pflcDrwSave){
    return (*pflcDrwSave)(hDrw, szFileName, bBak, hWnd);
  }
  return 0;
}


//--------------
int LCAPI lcDrwSaveMem (HANDLE hDrw, HANDLE hMem, int MemSize)
{
  tflcDrwSaveMem pflcDrwSaveMem;
  pflcDrwSaveMem = (tflcDrwSaveMem)GetProcAddress( ghLibInst, "lcDrwSaveMem" );
  if (pflcDrwSaveMem){
    return (*pflcDrwSaveMem)(hDrw, hMem, MemSize);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwSaveExploded (HANDLE hDrw, LPCWSTR szFileName, BOOL bBak, HWND hWnd)
{
  tflcDrwSaveExploded pflcDrwSaveExploded;
  pflcDrwSaveExploded = (tflcDrwSaveExploded)GetProcAddress( ghLibInst, "lcDrwSaveExploded" );
  if (pflcDrwSaveExploded){
    return (*pflcDrwSaveExploded)(hDrw, szFileName, bBak, hWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwPurge (HANDLE hDrw)
{
  tflcDrwPurge pflcDrwPurge;
  pflcDrwPurge = (tflcDrwPurge)GetProcAddress( ghLibInst, "lcDrwPurge" );
  if (pflcDrwPurge){
    return (*pflcDrwPurge)(hDrw);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwRegenViews (HANDLE hDrw, HANDLE hEnt)
{
  tflcDrwRegenViews pflcDrwRegenViews;
  pflcDrwRegenViews = (tflcDrwRegenViews)GetProcAddress( ghLibInst, "lcDrwRegenViews" );
  if (pflcDrwRegenViews){
    return (*pflcDrwRegenViews)(hDrw, hEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwSortObjects (HANDLE hDrw, int ObjType)
{
  tflcDrwSortObjects pflcDrwSortObjects;
  pflcDrwSortObjects = (tflcDrwSortObjects)GetProcAddress( ghLibInst, "lcDrwSortObjects" );
  if (pflcDrwSortObjects){
    return (*pflcDrwSortObjects)(hDrw, ObjType);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwClearXData (HANDLE hDrw, int ObjType, int Mode)
{
  tflcDrwClearXData pflcDrwClearXData;
  pflcDrwClearXData = (tflcDrwClearXData)GetProcAddress( ghLibInst, "lcDrwClearXData" );
  if (pflcDrwClearXData){
    return (*pflcDrwClearXData)(hDrw, ObjType, Mode);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwResolveBlocks (HANDLE hDrw, WCHAR* szError)
{
  tflcDrwResolveBlocks pflcDrwResolveBlocks;
  pflcDrwResolveBlocks = (tflcDrwResolveBlocks)GetProcAddress( ghLibInst, "lcDrwResolveBlocks" );
  if (pflcDrwResolveBlocks){
    return (*pflcDrwResolveBlocks)(hDrw, szError);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddLayer (HANDLE hDrw, LPCWSTR szName, LPCWSTR szColor, HANDLE hLtype, int Lweight)
{
  tflcDrwAddLayer pflcDrwAddLayer;
  pflcDrwAddLayer = (tflcDrwAddLayer)GetProcAddress( ghLibInst, "lcDrwAddLayer" );
  if (pflcDrwAddLayer){
    return (*pflcDrwAddLayer)(hDrw, szName, szColor, hLtype, Lweight);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddLinetype (HANDLE hDrw, LPCWSTR szName, LPCWSTR szDefinition)
{
  tflcDrwAddLinetype pflcDrwAddLinetype;
  pflcDrwAddLinetype = (tflcDrwAddLinetype)GetProcAddress( ghLibInst, "lcDrwAddLinetype" );
  if (pflcDrwAddLinetype){
    return (*pflcDrwAddLinetype)(hDrw, szName, szDefinition);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddLinetypeF (HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName, LPCWSTR szLtypeName)
{
  tflcDrwAddLinetypeF pflcDrwAddLinetypeF;
  pflcDrwAddLinetypeF = (tflcDrwAddLinetypeF)GetProcAddress( ghLibInst, "lcDrwAddLinetypeF" );
  if (pflcDrwAddLinetypeF){
    return (*pflcDrwAddLinetypeF)(hDrw, szName, szFileName, szLtypeName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddTextStyle (HANDLE hDrw, LPCWSTR szName, LPCWSTR szFontName)
{
  tflcDrwAddTextStyle pflcDrwAddTextStyle;
  pflcDrwAddTextStyle = (tflcDrwAddTextStyle)GetProcAddress( ghLibInst, "lcDrwAddTextStyle" );
  if (pflcDrwAddTextStyle){
    return (*pflcDrwAddTextStyle)(hDrw, szName, szFontName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddDimStyle (HANDLE hDrw, LPCWSTR szName)
{
  tflcDrwAddDimStyle pflcDrwAddDimStyle;
  pflcDrwAddDimStyle = (tflcDrwAddDimStyle)GetProcAddress( ghLibInst, "lcDrwAddDimStyle" );
  if (pflcDrwAddDimStyle){
    return (*pflcDrwAddDimStyle)(hDrw, szName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddMlineStyle (HANDLE hDrw, LPCWSTR szName)
{
  tflcDrwAddMlineStyle pflcDrwAddMlineStyle;
  pflcDrwAddMlineStyle = (tflcDrwAddMlineStyle)GetProcAddress( ghLibInst, "lcDrwAddMlineStyle" );
  if (pflcDrwAddMlineStyle){
    return (*pflcDrwAddMlineStyle)(hDrw, szName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddImage (HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName)
{
  tflcDrwAddImage pflcDrwAddImage;
  pflcDrwAddImage = (tflcDrwAddImage)GetProcAddress( ghLibInst, "lcDrwAddImage" );
  if (pflcDrwAddImage){
    return (*pflcDrwAddImage)(hDrw, szName, szFileName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddImage2 (HANDLE hDrw, LPCWSTR szName, int Width, int Height, int nBits)
{
  tflcDrwAddImage2 pflcDrwAddImage2;
  pflcDrwAddImage2 = (tflcDrwAddImage2)GetProcAddress( ghLibInst, "lcDrwAddImage2" );
  if (pflcDrwAddImage2){
    return (*pflcDrwAddImage2)(hDrw, szName, Width, Height, nBits);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddBlock (HANDLE hDrw, LPCWSTR szName, double X, double Y)
{
  tflcDrwAddBlock pflcDrwAddBlock;
  pflcDrwAddBlock = (tflcDrwAddBlock)GetProcAddress( ghLibInst, "lcDrwAddBlock" );
  if (pflcDrwAddBlock){
    return (*pflcDrwAddBlock)(hDrw, szName, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddBlockFromFile (HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName, int Overwrite, HWND hwParent)
{
  tflcDrwAddBlockFromFile pflcDrwAddBlockFromFile;
  pflcDrwAddBlockFromFile = (tflcDrwAddBlockFromFile)GetProcAddress( ghLibInst, "lcDrwAddBlockFromFile" );
  if (pflcDrwAddBlockFromFile){
    return (*pflcDrwAddBlockFromFile)(hDrw, szName, szFileName, Overwrite, hwParent);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddBlockFromDrw (HANDLE hDrw, LPCWSTR szName, HANDLE hDrw2, int Overwrite, HWND hwParent)
{
  tflcDrwAddBlockFromDrw pflcDrwAddBlockFromDrw;
  pflcDrwAddBlockFromDrw = (tflcDrwAddBlockFromDrw)GetProcAddress( ghLibInst, "lcDrwAddBlockFromDrw" );
  if (pflcDrwAddBlockFromDrw){
    return (*pflcDrwAddBlockFromDrw)(hDrw, szName, hDrw2, Overwrite, hwParent);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddBlockFile (HANDLE hDrw, LPCWSTR szName, LPCWSTR szFileName, int Overwrite, HWND hwParent)
{
  tflcDrwAddBlockFile pflcDrwAddBlockFile;
  pflcDrwAddBlockFile = (tflcDrwAddBlockFile)GetProcAddress( ghLibInst, "lcDrwAddBlockFile" );
  if (pflcDrwAddBlockFile){
    return (*pflcDrwAddBlockFile)(hDrw, szName, szFileName, Overwrite, hwParent);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwAddViewPaper (HANDLE hDrw, LPCWSTR szName, int PaperSize, int Orient, double Width, double Height)
{
  tflcDrwAddViewPaper pflcDrwAddViewPaper;
  pflcDrwAddViewPaper = (tflcDrwAddViewPaper)GetProcAddress( ghLibInst, "lcDrwAddViewPaper" );
  if (pflcDrwAddViewPaper){
    return (*pflcDrwAddViewPaper)(hDrw, szName, PaperSize, Orient, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDrwDeleteObject (HANDLE hDrw, HANDLE hObject)
{
  tflcDrwDeleteObject pflcDrwDeleteObject;
  pflcDrwDeleteObject = (tflcDrwDeleteObject)GetProcAddress( ghLibInst, "lcDrwDeleteObject" );
  if (pflcDrwDeleteObject){
    return (*pflcDrwDeleteObject)(hDrw, hObject);
  }
  return 0;
}


//--------------
int LCAPI lcDrwDeleteUnusedObjects (HANDLE hDrw, int ObjType)
{
  tflcDrwDeleteUnusedObjects pflcDrwDeleteUnusedObjects;
  pflcDrwDeleteUnusedObjects = (tflcDrwDeleteUnusedObjects)GetProcAddress( ghLibInst, "lcDrwDeleteUnusedObjects" );
  if (pflcDrwDeleteUnusedObjects){
    return (*pflcDrwDeleteUnusedObjects)(hDrw, ObjType);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetFirstObject (HANDLE hDrw, int ObjType)
{
  tflcDrwGetFirstObject pflcDrwGetFirstObject;
  pflcDrwGetFirstObject = (tflcDrwGetFirstObject)GetProcAddress( ghLibInst, "lcDrwGetFirstObject" );
  if (pflcDrwGetFirstObject){
    return (*pflcDrwGetFirstObject)(hDrw, ObjType);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetNextObject (HANDLE hDrw, HANDLE hObject)
{
  tflcDrwGetNextObject pflcDrwGetNextObject;
  pflcDrwGetNextObject = (tflcDrwGetNextObject)GetProcAddress( ghLibInst, "lcDrwGetNextObject" );
  if (pflcDrwGetNextObject){
    return (*pflcDrwGetNextObject)(hDrw, hObject);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetObjectByID (HANDLE hDrw, int ObjType, int Id)
{
  tflcDrwGetObjectByID pflcDrwGetObjectByID;
  pflcDrwGetObjectByID = (tflcDrwGetObjectByID)GetProcAddress( ghLibInst, "lcDrwGetObjectByID" );
  if (pflcDrwGetObjectByID){
    return (*pflcDrwGetObjectByID)(hDrw, ObjType, Id);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetObjectByIDH (HANDLE hDrw, int ObjType, LPCWSTR szId)
{
  tflcDrwGetObjectByIDH pflcDrwGetObjectByIDH;
  pflcDrwGetObjectByIDH = (tflcDrwGetObjectByIDH)GetProcAddress( ghLibInst, "lcDrwGetObjectByIDH" );
  if (pflcDrwGetObjectByIDH){
    return (*pflcDrwGetObjectByIDH)(hDrw, ObjType, szId);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetObjectByName (HANDLE hDrw, int ObjType, LPCWSTR szName)
{
  tflcDrwGetObjectByName pflcDrwGetObjectByName;
  pflcDrwGetObjectByName = (tflcDrwGetObjectByName)GetProcAddress( ghLibInst, "lcDrwGetObjectByName" );
  if (pflcDrwGetObjectByName){
    return (*pflcDrwGetObjectByName)(hDrw, ObjType, szName);
  }
  return 0;
}


//--------------
int LCAPI lcDrwCountObjects (HANDLE hDrw, int ObjType)
{
  tflcDrwCountObjects pflcDrwCountObjects;
  pflcDrwCountObjects = (tflcDrwCountObjects)GetProcAddress( ghLibInst, "lcDrwCountObjects" );
  if (pflcDrwCountObjects){
    return (*pflcDrwCountObjects)(hDrw, ObjType);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetViewByBlock (HANDLE hDrw, HANDLE hBlock)
{
  tflcDrwGetViewByBlock pflcDrwGetViewByBlock;
  pflcDrwGetViewByBlock = (tflcDrwGetViewByBlock)GetProcAddress( ghLibInst, "lcDrwGetViewByBlock" );
  if (pflcDrwGetViewByBlock){
    return (*pflcDrwGetViewByBlock)(hDrw, hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetEntByID (HANDLE hDrw, int Id)
{
  tflcDrwGetEntByID pflcDrwGetEntByID;
  pflcDrwGetEntByID = (tflcDrwGetEntByID)GetProcAddress( ghLibInst, "lcDrwGetEntByID" );
  if (pflcDrwGetEntByID){
    return (*pflcDrwGetEntByID)(hDrw, Id);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetEntByIDH (HANDLE hDrw, LPCWSTR szId)
{
  tflcDrwGetEntByIDH pflcDrwGetEntByIDH;
  pflcDrwGetEntByIDH = (tflcDrwGetEntByIDH)GetProcAddress( ghLibInst, "lcDrwGetEntByIDH" );
  if (pflcDrwGetEntByIDH){
    return (*pflcDrwGetEntByIDH)(hDrw, szId);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcDrwGetEntByKey (HANDLE hDrw, int Key)
{
  tflcDrwGetEntByKey pflcDrwGetEntByKey;
  pflcDrwGetEntByKey = (tflcDrwGetEntByKey)GetProcAddress( ghLibInst, "lcDrwGetEntByKey" );
  if (pflcDrwGetEntByKey){
    return (*pflcDrwGetEntByKey)(hDrw, Key);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewSetRect (HANDLE hView, double Xcen, double Ycen, double Width, double Height)
{
  tflcViewSetRect pflcViewSetRect;
  pflcViewSetRect = (tflcViewSetRect)GetProcAddress( ghLibInst, "lcViewSetRect" );
  if (pflcViewSetRect){
    return (*pflcViewSetRect)(hView, Xcen, Ycen, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewSetRect2 (HANDLE hView, double Lef, double Bot, double Rig, double Top)
{
  tflcViewSetRect2 pflcViewSetRect2;
  pflcViewSetRect2 = (tflcViewSetRect2)GetProcAddress( ghLibInst, "lcViewSetRect2" );
  if (pflcViewSetRect2){
    return (*pflcViewSetRect2)(hView, Lef, Bot, Rig, Top);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewSetLimits (HANDLE hView, double Xmin, double Ymin, double Xmax, double Ymax)
{
  tflcViewSetLimits pflcViewSetLimits;
  pflcViewSetLimits = (tflcViewSetLimits)GetProcAddress( ghLibInst, "lcViewSetLimits" );
  if (pflcViewSetLimits){
    return (*pflcViewSetLimits)(hView, Xmin, Ymin, Xmax, Ymax);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewSetGrid (HANDLE hView, double X0, double Y0, double dX, double dY)
{
  tflcViewSetGrid pflcViewSetGrid;
  pflcViewSetGrid = (tflcViewSetGrid)GetProcAddress( ghLibInst, "lcViewSetGrid" );
  if (pflcViewSetGrid){
    return (*pflcViewSetGrid)(hView, X0, Y0, dX, dY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewRegen (HANDLE hView, HANDLE hEnt)
{
  tflcViewRegen pflcViewRegen;
  pflcViewRegen = (tflcViewRegen)GetProcAddress( ghLibInst, "lcViewRegen" );
  if (pflcViewRegen){
    return (*pflcViewRegen)(hView, hEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewSetPaperSize (HANDLE hView, int PaperSize, int Orient, double Width, double Height)
{
  tflcViewSetPaperSize pflcViewSetPaperSize;
  pflcViewSetPaperSize = (tflcViewSetPaperSize)GetProcAddress( ghLibInst, "lcViewSetPaperSize" );
  if (pflcViewSetPaperSize){
    return (*pflcViewSetPaperSize)(hView, PaperSize, Orient, Width, Height);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcViewGetEntByPoint (HANDLE hView, double X, double Y, double Delta, int Mode)
{
  tflcViewGetEntByPoint pflcViewGetEntByPoint;
  pflcViewGetEntByPoint = (tflcViewGetEntByPoint)GetProcAddress( ghLibInst, "lcViewGetEntByPoint" );
  if (pflcViewGetEntByPoint){
    return (*pflcViewGetEntByPoint)(hView, X, Y, Delta, Mode);
  }
  return 0;
}


//--------------
int LCAPI lcViewGetEntsByRect (HANDLE hView, double Lef, double Bot, double Rig, double Top, BOOL bCross, int MaxNum)
{
  tflcViewGetEntsByRect pflcViewGetEntsByRect;
  pflcViewGetEntsByRect = (tflcViewGetEntsByRect)GetProcAddress( ghLibInst, "lcViewGetEntsByRect" );
  if (pflcViewGetEntsByRect){
    return (*pflcViewGetEntsByRect)(hView, Lef, Bot, Rig, Top, bCross, MaxNum);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcViewGetEntity (int iEnt)
{
  tflcViewGetEntity pflcViewGetEntity;
  pflcViewGetEntity = (tflcViewGetEntity)GetProcAddress( ghLibInst, "lcViewGetEntity" );
  if (pflcViewGetEntity){
    return (*pflcViewGetEntity)(iEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewRasterize (HANDLE hView, LPCWSTR szFileName, double Lef, double Bot, double Rig, double Top, double Scal)
{
  tflcViewRasterize pflcViewRasterize;
  pflcViewRasterize = (tflcViewRasterize)GetProcAddress( ghLibInst, "lcViewRasterize" );
  if (pflcViewRasterize){
    return (*pflcViewRasterize)(hView, szFileName, Lef, Bot, Rig, Top, Scal);
  }
  return 0;
}


//--------------
int LCAPI lcViewRasterizeMem (HANDLE hView, HANDLE hMem, double Lef, double Bot, double Rig, double Top, double Scal)
{
  tflcViewRasterizeMem pflcViewRasterizeMem;
  pflcViewRasterizeMem = (tflcViewRasterizeMem)GetProcAddress( ghLibInst, "lcViewRasterizeMem" );
  if (pflcViewRasterizeMem){
    return (*pflcViewRasterizeMem)(hView, hMem, Lef, Bot, Rig, Top, Scal);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewDrawMarker (HANDLE hView, double X, double Y)
{
  tflcViewDrawMarker pflcViewDrawMarker;
  pflcViewDrawMarker = (tflcViewDrawMarker)GetProcAddress( ghLibInst, "lcViewDrawMarker" );
  if (pflcViewDrawMarker){
    return (*pflcViewDrawMarker)(hView, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewDrawLine (HANDLE hView, double X0, double Y0, double X1, double Y1)
{
  tflcViewDrawLine pflcViewDrawLine;
  pflcViewDrawLine = (tflcViewDrawLine)GetProcAddress( ghLibInst, "lcViewDrawLine" );
  if (pflcViewDrawLine){
    return (*pflcViewDrawLine)(hView, X0, Y0, X1, Y1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewDrawXline (HANDLE hView, double X0, double Y0, double X1, double Y1, BOOL bRay)
{
  tflcViewDrawXline pflcViewDrawXline;
  pflcViewDrawXline = (tflcViewDrawXline)GetProcAddress( ghLibInst, "lcViewDrawXline" );
  if (pflcViewDrawXline){
    return (*pflcViewDrawXline)(hView, X0, Y0, X1, Y1, bRay);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewDrawRect (HANDLE hView, double X0, double Y0, double X1, double Y1)
{
  tflcViewDrawRect pflcViewDrawRect;
  pflcViewDrawRect = (tflcViewDrawRect)GetProcAddress( ghLibInst, "lcViewDrawRect" );
  if (pflcViewDrawRect){
    return (*pflcViewDrawRect)(hView, X0, Y0, X1, Y1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcViewPrint (HANDLE hView, HDC hPrintDC, double Lef, double Bot, double Rig, double Top, double Scal, double PapLef, double PapTop)
{
  tflcViewPrint pflcViewPrint;
  pflcViewPrint = (tflcViewPrint)GetProcAddress( ghLibInst, "lcViewPrint" );
  if (pflcViewPrint){
    return (*pflcViewPrint)(hView, hPrintDC, Lef, Bot, Rig, Top, Scal, PapLef, PapTop);
  }
  return 0;
}


//--------------
BOOL LCAPI lcMLStyleAddLine (HANDLE hStyle, double Offset, LPCWSTR szColor, HANDLE hLtype)
{
  tflcMLStyleAddLine pflcMLStyleAddLine;
  pflcMLStyleAddLine = (tflcMLStyleAddLine)GetProcAddress( ghLibInst, "lcMLStyleAddLine" );
  if (pflcMLStyleAddLine){
    return (*pflcMLStyleAddLine)(hStyle, Offset, szColor, hLtype);
  }
  return 0;
}


//--------------
BOOL LCAPI lcMLStyleDelLine (HANDLE hStyle, int iLine)
{
  tflcMLStyleDelLine pflcMLStyleDelLine;
  pflcMLStyleDelLine = (tflcMLStyleDelLine)GetProcAddress( ghLibInst, "lcMLStyleDelLine" );
  if (pflcMLStyleDelLine){
    return (*pflcMLStyleDelLine)(hStyle, iLine);
  }
  return 0;
}


//--------------
BOOL LCAPI lcMLStyleSortLines (HANDLE hStyle)
{
  tflcMLStyleSortLines pflcMLStyleSortLines;
  pflcMLStyleSortLines = (tflcMLStyleSortLines)GetProcAddress( ghLibInst, "lcMLStyleSortLines" );
  if (pflcMLStyleSortLines){
    return (*pflcMLStyleSortLines)(hStyle);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageSetPixelRGB (HANDLE hImage, int X, int Y, int Red, int Green, int Blue)
{
  tflcImageSetPixelRGB pflcImageSetPixelRGB;
  pflcImageSetPixelRGB = (tflcImageSetPixelRGB)GetProcAddress( ghLibInst, "lcImageSetPixelRGB" );
  if (pflcImageSetPixelRGB){
    return (*pflcImageSetPixelRGB)(hImage, X, Y, Red, Green, Blue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageSetPixelI (HANDLE hImage, int X, int Y, int iColor)
{
  tflcImageSetPixelI pflcImageSetPixelI;
  pflcImageSetPixelI = (tflcImageSetPixelI)GetProcAddress( ghLibInst, "lcImageSetPixelI" );
  if (pflcImageSetPixelI){
    return (*pflcImageSetPixelI)(hImage, X, Y, iColor);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageGetPixelRGB (HANDLE hImage, int X, int Y, int* pRed, int* pGreen, int* pBlue)
{
  tflcImageGetPixelRGB pflcImageGetPixelRGB;
  pflcImageGetPixelRGB = (tflcImageGetPixelRGB)GetProcAddress( ghLibInst, "lcImageGetPixelRGB" );
  if (pflcImageGetPixelRGB){
    return (*pflcImageGetPixelRGB)(hImage, X, Y, pRed, pGreen, pBlue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageGetPixelI (HANDLE hImage, int X, int Y, int* piColor)
{
  tflcImageGetPixelI pflcImageGetPixelI;
  pflcImageGetPixelI = (tflcImageGetPixelI)GetProcAddress( ghLibInst, "lcImageGetPixelI" );
  if (pflcImageGetPixelI){
    return (*pflcImageGetPixelI)(hImage, X, Y, piColor);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageSetPalColor (HANDLE hImage, int iColor, int Red, int Green, int Blue)
{
  tflcImageSetPalColor pflcImageSetPalColor;
  pflcImageSetPalColor = (tflcImageSetPalColor)GetProcAddress( ghLibInst, "lcImageSetPalColor" );
  if (pflcImageSetPalColor){
    return (*pflcImageSetPalColor)(hImage, iColor, Red, Green, Blue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageGetPalColor (HANDLE hImage, int iColor, int* pRed, int* pGreen, int* pBlue)
{
  tflcImageGetPalColor pflcImageGetPalColor;
  pflcImageGetPalColor = (tflcImageGetPalColor)GetProcAddress( ghLibInst, "lcImageGetPalColor" );
  if (pflcImageGetPalColor){
    return (*pflcImageGetPalColor)(hImage, iColor, pRed, pGreen, pBlue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageSetTranColor (HANDLE hImage, BOOL bTransparent, int Red, int Green, int Blue)
{
  tflcImageSetTranColor pflcImageSetTranColor;
  pflcImageSetTranColor = (tflcImageSetTranColor)GetProcAddress( ghLibInst, "lcImageSetTranColor" );
  if (pflcImageSetTranColor){
    return (*pflcImageSetTranColor)(hImage, bTransparent, Red, Green, Blue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcImageResize (HANDLE hImage, int Width, int Height)
{
  tflcImageResize pflcImageResize;
  pflcImageResize = (tflcImageResize)GetProcAddress( ghLibInst, "lcImageResize" );
  if (pflcImageResize){
    return (*pflcImageResize)(hImage, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcLayerClear (HANDLE hLayer, HANDLE hBlock)
{
  tflcLayerClear pflcLayerClear;
  pflcLayerClear = (tflcLayerClear)GetProcAddress( ghLibInst, "lcLayerClear" );
  if (pflcLayerClear){
    return (*pflcLayerClear)(hLayer, hBlock);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockClear (HANDLE hBlock, HANDLE hLayer)
{
  tflcBlockClear pflcBlockClear;
  pflcBlockClear = (tflcBlockClear)GetProcAddress( ghLibInst, "lcBlockClear" );
  if (pflcBlockClear){
    return (*pflcBlockClear)(hBlock, hLayer);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockPurge (HANDLE hBlock)
{
  tflcBlockPurge pflcBlockPurge;
  pflcBlockPurge = (tflcBlockPurge)GetProcAddress( ghLibInst, "lcBlockPurge" );
  if (pflcBlockPurge){
    return (*pflcBlockPurge)(hBlock);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSortEnts (HANDLE hBlock, BOOL bByLayers)
{
  tflcBlockSortEnts pflcBlockSortEnts;
  pflcBlockSortEnts = (tflcBlockSortEnts)GetProcAddress( ghLibInst, "lcBlockSortEnts" );
  if (pflcBlockSortEnts){
    return (*pflcBlockSortEnts)(hBlock, bByLayers);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddPoint (HANDLE hBlock, double X, double Y)
{
  tflcBlockAddPoint pflcBlockAddPoint;
  pflcBlockAddPoint = (tflcBlockAddPoint)GetProcAddress( ghLibInst, "lcBlockAddPoint" );
  if (pflcBlockAddPoint){
    return (*pflcBlockAddPoint)(hBlock, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddPoint2 (HANDLE hBlock, double X, double Y, int PtMode, double PtSize)
{
  tflcBlockAddPoint2 pflcBlockAddPoint2;
  pflcBlockAddPoint2 = (tflcBlockAddPoint2)GetProcAddress( ghLibInst, "lcBlockAddPoint2" );
  if (pflcBlockAddPoint2){
    return (*pflcBlockAddPoint2)(hBlock, X, Y, PtMode, PtSize);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddXline (HANDLE hBlock, double X, double Y, double Angle, BOOL bRay)
{
  tflcBlockAddXline pflcBlockAddXline;
  pflcBlockAddXline = (tflcBlockAddXline)GetProcAddress( ghLibInst, "lcBlockAddXline" );
  if (pflcBlockAddXline){
    return (*pflcBlockAddXline)(hBlock, X, Y, Angle, bRay);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddXline2P (HANDLE hBlock, double X, double Y, double X2, double Y2, BOOL bRay)
{
  tflcBlockAddXline2P pflcBlockAddXline2P;
  pflcBlockAddXline2P = (tflcBlockAddXline2P)GetProcAddress( ghLibInst, "lcBlockAddXline2P" );
  if (pflcBlockAddXline2P){
    return (*pflcBlockAddXline2P)(hBlock, X, Y, X2, Y2, bRay);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddLine (HANDLE hBlock, double X1, double Y1, double X2, double Y2)
{
  tflcBlockAddLine pflcBlockAddLine;
  pflcBlockAddLine = (tflcBlockAddLine)GetProcAddress( ghLibInst, "lcBlockAddLine" );
  if (pflcBlockAddLine){
    return (*pflcBlockAddLine)(hBlock, X1, Y1, X2, Y2);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddLineDir (HANDLE hBlock, double X, double Y, double Angle, double Dist)
{
  tflcBlockAddLineDir pflcBlockAddLineDir;
  pflcBlockAddLineDir = (tflcBlockAddLineDir)GetProcAddress( ghLibInst, "lcBlockAddLineDir" );
  if (pflcBlockAddLineDir){
    return (*pflcBlockAddLineDir)(hBlock, X, Y, Angle, Dist);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddLineTan (HANDLE hBlock, HANDLE hEnt1, HANDLE hEnt2, int Mode)
{
  tflcBlockAddLineTan pflcBlockAddLineTan;
  pflcBlockAddLineTan = (tflcBlockAddLineTan)GetProcAddress( ghLibInst, "lcBlockAddLineTan" );
  if (pflcBlockAddLineTan){
    return (*pflcBlockAddLineTan)(hBlock, hEnt1, hEnt2, Mode);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddPolyline (HANDLE hBlock, int FitType, BOOL bClosed, BOOL bFilled)
{
  tflcBlockAddPolyline pflcBlockAddPolyline;
  pflcBlockAddPolyline = (tflcBlockAddPolyline)GetProcAddress( ghLibInst, "lcBlockAddPolyline" );
  if (pflcBlockAddPolyline){
    return (*pflcBlockAddPolyline)(hBlock, FitType, bClosed, bFilled);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddSpline (HANDLE hBlock, BOOL bClosed, BOOL bFilled)
{
  tflcBlockAddSpline pflcBlockAddSpline;
  pflcBlockAddSpline = (tflcBlockAddSpline)GetProcAddress( ghLibInst, "lcBlockAddSpline" );
  if (pflcBlockAddSpline){
    return (*pflcBlockAddSpline)(hBlock, bClosed, bFilled);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddMline (HANDLE hBlock, int FitType, BOOL bClosed)
{
  tflcBlockAddMline pflcBlockAddMline;
  pflcBlockAddMline = (tflcBlockAddMline)GetProcAddress( ghLibInst, "lcBlockAddMline" );
  if (pflcBlockAddMline){
    return (*pflcBlockAddMline)(hBlock, FitType, bClosed);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddRect (HANDLE hBlock, double Xc, double Yc, double Width, double Height, double Angle, BOOL bFilled)
{
  tflcBlockAddRect pflcBlockAddRect;
  pflcBlockAddRect = (tflcBlockAddRect)GetProcAddress( ghLibInst, "lcBlockAddRect" );
  if (pflcBlockAddRect){
    return (*pflcBlockAddRect)(hBlock, Xc, Yc, Width, Height, Angle, bFilled);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddRect2 (HANDLE hBlock, double Left, double Bottom, double Width, double Height, double Rad, BOOL bFilled)
{
  tflcBlockAddRect2 pflcBlockAddRect2;
  pflcBlockAddRect2 = (tflcBlockAddRect2)GetProcAddress( ghLibInst, "lcBlockAddRect2" );
  if (pflcBlockAddRect2){
    return (*pflcBlockAddRect2)(hBlock, Left, Bottom, Width, Height, Rad, bFilled);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddCircle (HANDLE hBlock, double X, double Y, double Radius, BOOL bFilled)
{
  tflcBlockAddCircle pflcBlockAddCircle;
  pflcBlockAddCircle = (tflcBlockAddCircle)GetProcAddress( ghLibInst, "lcBlockAddCircle" );
  if (pflcBlockAddCircle){
    return (*pflcBlockAddCircle)(hBlock, X, Y, Radius, bFilled);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddArc (HANDLE hBlock, double X, double Y, double Radius, double StartAngle, double ArcAngle)
{
  tflcBlockAddArc pflcBlockAddArc;
  pflcBlockAddArc = (tflcBlockAddArc)GetProcAddress( ghLibInst, "lcBlockAddArc" );
  if (pflcBlockAddArc){
    return (*pflcBlockAddArc)(hBlock, X, Y, Radius, StartAngle, ArcAngle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddArc3P (HANDLE hBlock, double X1, double Y1, double X2, double Y2, double X3, double Y3)
{
  tflcBlockAddArc3P pflcBlockAddArc3P;
  pflcBlockAddArc3P = (tflcBlockAddArc3P)GetProcAddress( ghLibInst, "lcBlockAddArc3P" );
  if (pflcBlockAddArc3P){
    return (*pflcBlockAddArc3P)(hBlock, X1, Y1, X2, Y2, X3, Y3);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddEllipse (HANDLE hBlock, double X, double Y, double R1, double R2, double RotAngle, double StartAngle, double ArcAngle)
{
  tflcBlockAddEllipse pflcBlockAddEllipse;
  pflcBlockAddEllipse = (tflcBlockAddEllipse)GetProcAddress( ghLibInst, "lcBlockAddEllipse" );
  if (pflcBlockAddEllipse){
    return (*pflcBlockAddEllipse)(hBlock, X, Y, R1, R2, RotAngle, StartAngle, ArcAngle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddText (HANDLE hBlock, LPCWSTR szText, double X, double Y)
{
  tflcBlockAddText pflcBlockAddText;
  pflcBlockAddText = (tflcBlockAddText)GetProcAddress( ghLibInst, "lcBlockAddText" );
  if (pflcBlockAddText){
    return (*pflcBlockAddText)(hBlock, szText, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddText2 (HANDLE hBlock, LPCWSTR szText, double X, double Y, int Align, double H, double WScale, double RotAngle, double Oblique)
{
  tflcBlockAddText2 pflcBlockAddText2;
  pflcBlockAddText2 = (tflcBlockAddText2)GetProcAddress( ghLibInst, "lcBlockAddText2" );
  if (pflcBlockAddText2){
    return (*pflcBlockAddText2)(hBlock, szText, X, Y, Align, H, WScale, RotAngle, Oblique);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddTextWin (HANDLE hBlock, LPCWSTR szText, double X, double Y)
{
  tflcBlockAddTextWin pflcBlockAddTextWin;
  pflcBlockAddTextWin = (tflcBlockAddTextWin)GetProcAddress( ghLibInst, "lcBlockAddTextWin" );
  if (pflcBlockAddTextWin){
    return (*pflcBlockAddTextWin)(hBlock, szText, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddTextWin2 (HANDLE hBlock, LPCWSTR szText, double X, double Y, int Align, double H, double WScale, double RotAngle, double Oblique)
{
  tflcBlockAddTextWin2 pflcBlockAddTextWin2;
  pflcBlockAddTextWin2 = (tflcBlockAddTextWin2)GetProcAddress( ghLibInst, "lcBlockAddTextWin2" );
  if (pflcBlockAddTextWin2){
    return (*pflcBlockAddTextWin2)(hBlock, szText, X, Y, Align, H, WScale, RotAngle, Oblique);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddMText (HANDLE hBlock, LPCWSTR szText, double X, double Y, double WrapWidth, int Align, double RotAngle, double H, double WScale)
{
  tflcBlockAddMText pflcBlockAddMText;
  pflcBlockAddMText = (tflcBlockAddMText)GetProcAddress( ghLibInst, "lcBlockAddMText" );
  if (pflcBlockAddMText){
    return (*pflcBlockAddMText)(hBlock, szText, X, Y, WrapWidth, Align, RotAngle, H, WScale);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddArcText (HANDLE hBlock, LPCWSTR szText, double X, double Y, double Radius, double StartAngle, BOOL bClockwise, double H, double WScale, int Align)
{
  tflcBlockAddArcText pflcBlockAddArcText;
  pflcBlockAddArcText = (tflcBlockAddArcText)GetProcAddress( ghLibInst, "lcBlockAddArcText" );
  if (pflcBlockAddArcText){
    return (*pflcBlockAddArcText)(hBlock, szText, X, Y, Radius, StartAngle, bClockwise, H, WScale, Align);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddBlockRef (HANDLE hBlock, HANDLE hRefBlock, double X, double Y, double Scal, double Angle)
{
  tflcBlockAddBlockRef pflcBlockAddBlockRef;
  pflcBlockAddBlockRef = (tflcBlockAddBlockRef)GetProcAddress( ghLibInst, "lcBlockAddBlockRef" );
  if (pflcBlockAddBlockRef){
    return (*pflcBlockAddBlockRef)(hBlock, hRefBlock, X, Y, Scal, Angle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddBlockRefID (HANDLE hBlock, int idRefBlock, double X, double Y, double Scal, double Angle)
{
  tflcBlockAddBlockRefID pflcBlockAddBlockRefID;
  pflcBlockAddBlockRefID = (tflcBlockAddBlockRefID)GetProcAddress( ghLibInst, "lcBlockAddBlockRefID" );
  if (pflcBlockAddBlockRefID){
    return (*pflcBlockAddBlockRefID)(hBlock, idRefBlock, X, Y, Scal, Angle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddBlockRefIDH (HANDLE hBlock, LPCWSTR szIdRefBlock, double X, double Y, double Scal, double Angle)
{
  tflcBlockAddBlockRefIDH pflcBlockAddBlockRefIDH;
  pflcBlockAddBlockRefIDH = (tflcBlockAddBlockRefIDH)GetProcAddress( ghLibInst, "lcBlockAddBlockRefIDH" );
  if (pflcBlockAddBlockRefIDH){
    return (*pflcBlockAddBlockRefIDH)(hBlock, szIdRefBlock, X, Y, Scal, Angle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddImageRef (HANDLE hBlock, HANDLE hImage, double X, double Y, double Width, double Height, BOOL bBorder)
{
  tflcBlockAddImageRef pflcBlockAddImageRef;
  pflcBlockAddImageRef = (tflcBlockAddImageRef)GetProcAddress( ghLibInst, "lcBlockAddImageRef" );
  if (pflcBlockAddImageRef){
    return (*pflcBlockAddImageRef)(hBlock, hImage, X, Y, Width, Height, bBorder);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddEcw (HANDLE hBlock, LPCWSTR szFileName)
{
  tflcBlockAddEcw pflcBlockAddEcw;
  pflcBlockAddEcw = (tflcBlockAddEcw)GetProcAddress( ghLibInst, "lcBlockAddEcw" );
  if (pflcBlockAddEcw){
    return (*pflcBlockAddEcw)(hBlock, szFileName);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddAttDef (HANDLE hBlock, int Mode, LPCWSTR szTag, LPCWSTR szPrompt, LPCWSTR szDefVal, double X, double Y, int Align, double H, double WScale, double RotAngle, double Oblique)
{
  tflcBlockAddAttDef pflcBlockAddAttDef;
  pflcBlockAddAttDef = (tflcBlockAddAttDef)GetProcAddress( ghLibInst, "lcBlockAddAttDef" );
  if (pflcBlockAddAttDef){
    return (*pflcBlockAddAttDef)(hBlock, Mode, szTag, szPrompt, szDefVal, X, Y, Align, H, WScale, RotAngle, Oblique);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddHatch (HANDLE hBlock, LPCWSTR szFileName, LPCWSTR szPatName, double Scal, double Angle)
{
  tflcBlockAddHatch pflcBlockAddHatch;
  pflcBlockAddHatch = (tflcBlockAddHatch)GetProcAddress( ghLibInst, "lcBlockAddHatch" );
  if (pflcBlockAddHatch){
    return (*pflcBlockAddHatch)(hBlock, szFileName, szPatName, Scal, Angle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddViewport (HANDLE hBlock, double Lef, double Bot, double Width, double Height, double DrwPntX, double DrwPntY, double DrwScale, double DrwAngle)
{
  tflcBlockAddViewport pflcBlockAddViewport;
  pflcBlockAddViewport = (tflcBlockAddViewport)GetProcAddress( ghLibInst, "lcBlockAddViewport" );
  if (pflcBlockAddViewport){
    return (*pflcBlockAddViewport)(hBlock, Lef, Bot, Width, Height, DrwPntX, DrwPntY, DrwScale, DrwAngle);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddFace (HANDLE hBlock, int Flags, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2)
{
  tflcBlockAddFace pflcBlockAddFace;
  pflcBlockAddFace = (tflcBlockAddFace)GetProcAddress( ghLibInst, "lcBlockAddFace" );
  if (pflcBlockAddFace){
    return (*pflcBlockAddFace)(hBlock, Flags, x0, y0, z0, x1, y1, z1, x2, y2, z2);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddFace4 (HANDLE hBlock, int Flags, double x0, double y0, double z0, double x1, double y1, double z1, double x2, double y2, double z2, double x3, double y3, double z3)
{
  tflcBlockAddFace4 pflcBlockAddFace4;
  pflcBlockAddFace4 = (tflcBlockAddFace4)GetProcAddress( ghLibInst, "lcBlockAddFace4" );
  if (pflcBlockAddFace4){
    return (*pflcBlockAddFace4)(hBlock, Flags, x0, y0, z0, x1, y1, z1, x2, y2, z2, x3, y3, z3);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddLeader (HANDLE hBlock, LPCWSTR szText, double Xt, double Yt, double LandDist, double Xa, double Ya, int Attach, int Align)
{
  tflcBlockAddLeader pflcBlockAddLeader;
  pflcBlockAddLeader = (tflcBlockAddLeader)GetProcAddress( ghLibInst, "lcBlockAddLeader" );
  if (pflcBlockAddLeader){
    return (*pflcBlockAddLeader)(hBlock, szText, Xt, Yt, LandDist, Xa, Ya, Attach, Align);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimRot (HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Xt, double Yt, double Angle, LPCWSTR szText)
{
  tflcBlockAddDimRot pflcBlockAddDimRot;
  pflcBlockAddDimRot = (tflcBlockAddDimRot)GetProcAddress( ghLibInst, "lcBlockAddDimRot" );
  if (pflcBlockAddDimRot){
    return (*pflcBlockAddDimRot)(hBlock, X0, Y0, X1, Y1, Xt, Yt, Angle, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimHor (HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Yt, LPCWSTR szText)
{
  tflcBlockAddDimHor pflcBlockAddDimHor;
  pflcBlockAddDimHor = (tflcBlockAddDimHor)GetProcAddress( ghLibInst, "lcBlockAddDimHor" );
  if (pflcBlockAddDimHor){
    return (*pflcBlockAddDimHor)(hBlock, X0, Y0, X1, Y1, Yt, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimVer (HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Xt, LPCWSTR szText)
{
  tflcBlockAddDimVer pflcBlockAddDimVer;
  pflcBlockAddDimVer = (tflcBlockAddDimVer)GetProcAddress( ghLibInst, "lcBlockAddDimVer" );
  if (pflcBlockAddDimVer){
    return (*pflcBlockAddDimVer)(hBlock, X0, Y0, X1, Y1, Xt, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimAli (HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Xt, double Yt, LPCWSTR szText)
{
  tflcBlockAddDimAli pflcBlockAddDimAli;
  pflcBlockAddDimAli = (tflcBlockAddDimAli)GetProcAddress( ghLibInst, "lcBlockAddDimAli" );
  if (pflcBlockAddDimAli){
    return (*pflcBlockAddDimAli)(hBlock, X0, Y0, X1, Y1, Xt, Yt, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimAli2 (HANDLE hBlock, double X0, double Y0, double X1, double Y1, double Dt, LPCWSTR szText)
{
  tflcBlockAddDimAli2 pflcBlockAddDimAli2;
  pflcBlockAddDimAli2 = (tflcBlockAddDimAli2)GetProcAddress( ghLibInst, "lcBlockAddDimAli2" );
  if (pflcBlockAddDimAli2){
    return (*pflcBlockAddDimAli2)(hBlock, X0, Y0, X1, Y1, Dt, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimOrd (HANDLE hBlock, double Xd, double Yd, double Xt, double Yt, bool bX, LPCWSTR szText)
{
  tflcBlockAddDimOrd pflcBlockAddDimOrd;
  pflcBlockAddDimOrd = (tflcBlockAddDimOrd)GetProcAddress( ghLibInst, "lcBlockAddDimOrd" );
  if (pflcBlockAddDimOrd){
    return (*pflcBlockAddDimOrd)(hBlock, Xd, Yd, Xt, Yt, bX, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimRad (HANDLE hBlock, double Xc, double Yc, double Xr, double Yr, double Xt, double Yt, LPCWSTR szText)
{
  tflcBlockAddDimRad pflcBlockAddDimRad;
  pflcBlockAddDimRad = (tflcBlockAddDimRad)GetProcAddress( ghLibInst, "lcBlockAddDimRad" );
  if (pflcBlockAddDimRad){
    return (*pflcBlockAddDimRad)(hBlock, Xc, Yc, Xr, Yr, Xt, Yt, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimRad2 (HANDLE hBlock, double Xc, double Yc, double R, double Angle, double TextOff, LPCWSTR szText)
{
  tflcBlockAddDimRad2 pflcBlockAddDimRad2;
  pflcBlockAddDimRad2 = (tflcBlockAddDimRad2)GetProcAddress( ghLibInst, "lcBlockAddDimRad2" );
  if (pflcBlockAddDimRad2){
    return (*pflcBlockAddDimRad2)(hBlock, Xc, Yc, R, Angle, TextOff, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimDia (HANDLE hBlock, double Xc, double Yc, double Xr, double Yr, double Xt, double Yt, LPCWSTR szText)
{
  tflcBlockAddDimDia pflcBlockAddDimDia;
  pflcBlockAddDimDia = (tflcBlockAddDimDia)GetProcAddress( ghLibInst, "lcBlockAddDimDia" );
  if (pflcBlockAddDimDia){
    return (*pflcBlockAddDimDia)(hBlock, Xc, Yc, Xr, Yr, Xt, Yt, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimDia2 (HANDLE hBlock, double Xc, double Yc, double R, double Angle, double TextOff, LPCWSTR szText)
{
  tflcBlockAddDimDia2 pflcBlockAddDimDia2;
  pflcBlockAddDimDia2 = (tflcBlockAddDimDia2)GetProcAddress( ghLibInst, "lcBlockAddDimDia2" );
  if (pflcBlockAddDimDia2){
    return (*pflcBlockAddDimDia2)(hBlock, Xc, Yc, R, Angle, TextOff, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimAng (HANDLE hBlock, double Xc, double Yc, double X1, double Y1, double X2, double Y2, double Xa, double Ya, double TextPos, LPCWSTR szText)
{
  tflcBlockAddDimAng pflcBlockAddDimAng;
  pflcBlockAddDimAng = (tflcBlockAddDimAng)GetProcAddress( ghLibInst, "lcBlockAddDimAng" );
  if (pflcBlockAddDimAng){
    return (*pflcBlockAddDimAng)(hBlock, Xc, Yc, X1, Y1, X2, Y2, Xa, Ya, TextPos, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddDimAng2 (HANDLE hBlock, double X1, double Y1, double X2, double Y2, double X3, double Y3, double X4, double Y4, double Xa, double Ya, double TextPos, LPCWSTR szText)
{
  tflcBlockAddDimAng2 pflcBlockAddDimAng2;
  pflcBlockAddDimAng2 = (tflcBlockAddDimAng2)GetProcAddress( ghLibInst, "lcBlockAddDimAng2" );
  if (pflcBlockAddDimAng2){
    return (*pflcBlockAddDimAng2)(hBlock, X1, Y1, X2, Y2, X3, Y3, X4, Y4, Xa, Ya, TextPos, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddRPlan (HANDLE hBlock)
{
  tflcBlockAddRPlan pflcBlockAddRPlan;
  pflcBlockAddRPlan = (tflcBlockAddRPlan)GetProcAddress( ghLibInst, "lcBlockAddRPlan" );
  if (pflcBlockAddRPlan){
    return (*pflcBlockAddRPlan)(hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddBezier (HANDLE hBlock, double X1, double Y1, double Angle1, double X2, double Y2, double Angle2)
{
  tflcBlockAddBezier pflcBlockAddBezier;
  pflcBlockAddBezier = (tflcBlockAddBezier)GetProcAddress( ghLibInst, "lcBlockAddBezier" );
  if (pflcBlockAddBezier){
    return (*pflcBlockAddBezier)(hBlock, X1, Y1, Angle1, X2, Y2, Angle2);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddTIN (HANDLE hBlock)
{
  tflcBlockAddTIN pflcBlockAddTIN;
  pflcBlockAddTIN = (tflcBlockAddTIN)GetProcAddress( ghLibInst, "lcBlockAddTIN" );
  if (pflcBlockAddTIN){
    return (*pflcBlockAddTIN)(hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockAddClone (HANDLE hBlock, HANDLE hEnt)
{
  tflcBlockAddClone pflcBlockAddClone;
  pflcBlockAddClone = (tflcBlockAddClone)GetProcAddress( ghLibInst, "lcBlockAddClone" );
  if (pflcBlockAddClone){
    return (*pflcBlockAddClone)(hBlock, hEnt);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetFirstEnt (HANDLE hBlock)
{
  tflcBlockGetFirstEnt pflcBlockGetFirstEnt;
  pflcBlockGetFirstEnt = (tflcBlockGetFirstEnt)GetProcAddress( ghLibInst, "lcBlockGetFirstEnt" );
  if (pflcBlockGetFirstEnt){
    return (*pflcBlockGetFirstEnt)(hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetNextEnt (HANDLE hBlock, HANDLE hEnt)
{
  tflcBlockGetNextEnt pflcBlockGetNextEnt;
  pflcBlockGetNextEnt = (tflcBlockGetNextEnt)GetProcAddress( ghLibInst, "lcBlockGetNextEnt" );
  if (pflcBlockGetNextEnt){
    return (*pflcBlockGetNextEnt)(hBlock, hEnt);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetLastEnt (HANDLE hBlock)
{
  tflcBlockGetLastEnt pflcBlockGetLastEnt;
  pflcBlockGetLastEnt = (tflcBlockGetLastEnt)GetProcAddress( ghLibInst, "lcBlockGetLastEnt" );
  if (pflcBlockGetLastEnt){
    return (*pflcBlockGetLastEnt)(hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetPrevEnt (HANDLE hBlock, HANDLE hEnt)
{
  tflcBlockGetPrevEnt pflcBlockGetPrevEnt;
  pflcBlockGetPrevEnt = (tflcBlockGetPrevEnt)GetProcAddress( ghLibInst, "lcBlockGetPrevEnt" );
  if (pflcBlockGetPrevEnt){
    return (*pflcBlockGetPrevEnt)(hBlock, hEnt);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetEntByID (HANDLE hBlock, int Id)
{
  tflcBlockGetEntByID pflcBlockGetEntByID;
  pflcBlockGetEntByID = (tflcBlockGetEntByID)GetProcAddress( ghLibInst, "lcBlockGetEntByID" );
  if (pflcBlockGetEntByID){
    return (*pflcBlockGetEntByID)(hBlock, Id);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetEntByIDH (HANDLE hBlock, LPCWSTR szId)
{
  tflcBlockGetEntByIDH pflcBlockGetEntByIDH;
  pflcBlockGetEntByIDH = (tflcBlockGetEntByIDH)GetProcAddress( ghLibInst, "lcBlockGetEntByIDH" );
  if (pflcBlockGetEntByIDH){
    return (*pflcBlockGetEntByIDH)(hBlock, szId);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetEntByKey (HANDLE hBlock, int Key)
{
  tflcBlockGetEntByKey pflcBlockGetEntByKey;
  pflcBlockGetEntByKey = (tflcBlockGetEntByKey)GetProcAddress( ghLibInst, "lcBlockGetEntByKey" );
  if (pflcBlockGetEntByKey){
    return (*pflcBlockGetEntByKey)(hBlock, Key);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetTIN (HANDLE hBlock, int Index)
{
  tflcBlockGetTIN pflcBlockGetTIN;
  pflcBlockGetTIN = (tflcBlockGetTIN)GetProcAddress( ghLibInst, "lcBlockGetTIN" );
  if (pflcBlockGetTIN){
    return (*pflcBlockGetTIN)(hBlock, Index);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockUnselect (HANDLE hBlock)
{
  tflcBlockUnselect pflcBlockUnselect;
  pflcBlockUnselect = (tflcBlockUnselect)GetProcAddress( ghLibInst, "lcBlockUnselect" );
  if (pflcBlockUnselect){
    return (*pflcBlockUnselect)(hBlock);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelectEnt (HANDLE hBlock, HANDLE hEntity, BOOL bSelect)
{
  tflcBlockSelectEnt pflcBlockSelectEnt;
  pflcBlockSelectEnt = (tflcBlockSelectEnt)GetProcAddress( ghLibInst, "lcBlockSelectEnt" );
  if (pflcBlockSelectEnt){
    return (*pflcBlockSelectEnt)(hBlock, hEntity, bSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelErase (HANDLE hBlock)
{
  tflcBlockSelErase pflcBlockSelErase;
  pflcBlockSelErase = (tflcBlockSelErase)GetProcAddress( ghLibInst, "lcBlockSelErase" );
  if (pflcBlockSelErase){
    return (*pflcBlockSelErase)(hBlock);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelMove (HANDLE hBlock, double dX, double dY, BOOL bCopy, BOOL bNewSelect)
{
  tflcBlockSelMove pflcBlockSelMove;
  pflcBlockSelMove = (tflcBlockSelMove)GetProcAddress( ghLibInst, "lcBlockSelMove" );
  if (pflcBlockSelMove){
    return (*pflcBlockSelMove)(hBlock, dX, dY, bCopy, bNewSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelScale (HANDLE hBlock, double X0, double Y0, double Scal, BOOL bCopy, BOOL bNewSelect)
{
  tflcBlockSelScale pflcBlockSelScale;
  pflcBlockSelScale = (tflcBlockSelScale)GetProcAddress( ghLibInst, "lcBlockSelScale" );
  if (pflcBlockSelScale){
    return (*pflcBlockSelScale)(hBlock, X0, Y0, Scal, bCopy, bNewSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelRotate (HANDLE hBlock, double X0, double Y0, double Angle, BOOL bCopy, BOOL bNewSelect)
{
  tflcBlockSelRotate pflcBlockSelRotate;
  pflcBlockSelRotate = (tflcBlockSelRotate)GetProcAddress( ghLibInst, "lcBlockSelRotate" );
  if (pflcBlockSelRotate){
    return (*pflcBlockSelRotate)(hBlock, X0, Y0, Angle, bCopy, bNewSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelMirror (HANDLE hBlock, double X1, double Y1, double X2, double Y2, BOOL bCopy, BOOL bNewSelect)
{
  tflcBlockSelMirror pflcBlockSelMirror;
  pflcBlockSelMirror = (tflcBlockSelMirror)GetProcAddress( ghLibInst, "lcBlockSelMirror" );
  if (pflcBlockSelMirror){
    return (*pflcBlockSelMirror)(hBlock, X1, Y1, X2, Y2, bCopy, bNewSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcBlockSelExplode (HANDLE hBlock)
{
  tflcBlockSelExplode pflcBlockSelExplode;
  pflcBlockSelExplode = (tflcBlockSelExplode)GetProcAddress( ghLibInst, "lcBlockSelExplode" );
  if (pflcBlockSelExplode){
    return (*pflcBlockSelExplode)(hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockSelJoin (HANDLE hBlock, double Delta)
{
  tflcBlockSelJoin pflcBlockSelJoin;
  pflcBlockSelJoin = (tflcBlockSelJoin)GetProcAddress( ghLibInst, "lcBlockSelJoin" );
  if (pflcBlockSelJoin){
    return (*pflcBlockSelJoin)(hBlock, Delta);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetFirstSel (HANDLE hBlock)
{
  tflcBlockGetFirstSel pflcBlockGetFirstSel;
  pflcBlockGetFirstSel = (tflcBlockGetFirstSel)GetProcAddress( ghLibInst, "lcBlockGetFirstSel" );
  if (pflcBlockGetFirstSel){
    return (*pflcBlockGetFirstSel)(hBlock);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlockGetNextSel (HANDLE hBlock)
{
  tflcBlockGetNextSel pflcBlockGetNextSel;
  pflcBlockGetNextSel = (tflcBlockGetNextSel)GetProcAddress( ghLibInst, "lcBlockGetNextSel" );
  if (pflcBlockGetNextSel){
    return (*pflcBlockGetNextSel)(hBlock);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntErase (HANDLE hEnt, BOOL bErase)
{
  tflcEntErase pflcEntErase;
  pflcEntErase = (tflcEntErase)GetProcAddress( ghLibInst, "lcEntErase" );
  if (pflcEntErase){
    return (*pflcEntErase)(hEnt, bErase);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntMove (HANDLE hEnt, double dX, double dY)
{
  tflcEntMove pflcEntMove;
  pflcEntMove = (tflcEntMove)GetProcAddress( ghLibInst, "lcEntMove" );
  if (pflcEntMove){
    return (*pflcEntMove)(hEnt, dX, dY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntScale (HANDLE hEnt, double X0, double Y0, double Scal)
{
  tflcEntScale pflcEntScale;
  pflcEntScale = (tflcEntScale)GetProcAddress( ghLibInst, "lcEntScale" );
  if (pflcEntScale){
    return (*pflcEntScale)(hEnt, X0, Y0, Scal);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntRotate (HANDLE hEnt, double X0, double Y0, double Angle)
{
  tflcEntRotate pflcEntRotate;
  pflcEntRotate = (tflcEntRotate)GetProcAddress( ghLibInst, "lcEntRotate" );
  if (pflcEntRotate){
    return (*pflcEntRotate)(hEnt, X0, Y0, Angle);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntMirror (HANDLE hEnt, double X1, double Y1, double X2, double Y2)
{
  tflcEntMirror pflcEntMirror;
  pflcEntMirror = (tflcEntMirror)GetProcAddress( ghLibInst, "lcEntMirror" );
  if (pflcEntMirror){
    return (*pflcEntMirror)(hEnt, X1, Y1, X2, Y2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntExplode (HANDLE hEnt, BOOL bSelect)
{
  tflcEntExplode pflcEntExplode;
  pflcEntExplode = (tflcEntExplode)GetProcAddress( ghLibInst, "lcEntExplode" );
  if (pflcEntExplode){
    return (*pflcEntExplode)(hEnt, bSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntOffset (HANDLE hEnt, double Dist)
{
  tflcEntOffset pflcEntOffset;
  pflcEntOffset = (tflcEntOffset)GetProcAddress( ghLibInst, "lcEntOffset" );
  if (pflcEntOffset){
    return (*pflcEntOffset)(hEnt, Dist);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntToTop (HANDLE hEnt)
{
  tflcEntToTop pflcEntToTop;
  pflcEntToTop = (tflcEntToTop)GetProcAddress( ghLibInst, "lcEntToTop" );
  if (pflcEntToTop){
    return (*pflcEntToTop)(hEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntToBottom (HANDLE hEnt)
{
  tflcEntToBottom pflcEntToBottom;
  pflcEntToBottom = (tflcEntToBottom)GetProcAddress( ghLibInst, "lcEntToBottom" );
  if (pflcEntToBottom){
    return (*pflcEntToBottom)(hEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntToAbove (HANDLE hEnt, HANDLE hEnt2)
{
  tflcEntToAbove pflcEntToAbove;
  pflcEntToAbove = (tflcEntToAbove)GetProcAddress( ghLibInst, "lcEntToAbove" );
  if (pflcEntToAbove){
    return (*pflcEntToAbove)(hEnt, hEnt2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntToUnder (HANDLE hEnt, HANDLE hEnt2)
{
  tflcEntToUnder pflcEntToUnder;
  pflcEntToUnder = (tflcEntToUnder)GetProcAddress( ghLibInst, "lcEntToUnder" );
  if (pflcEntToUnder){
    return (*pflcEntToUnder)(hEnt, hEnt2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntGetGrip (HANDLE hEnt, int iGrip, double* pX, double* pY)
{
  tflcEntGetGrip pflcEntGetGrip;
  pflcEntGetGrip = (tflcEntGetGrip)GetProcAddress( ghLibInst, "lcEntGetGrip" );
  if (pflcEntGetGrip){
    return (*pflcEntGetGrip)(hEnt, iGrip, pX, pY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntPutGrip (HANDLE hEnt, int iGrip, double X, double Y)
{
  tflcEntPutGrip pflcEntPutGrip;
  pflcEntPutGrip = (tflcEntPutGrip)GetProcAddress( ghLibInst, "lcEntPutGrip" );
  if (pflcEntPutGrip){
    return (*pflcEntPutGrip)(hEnt, iGrip, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntUpdate (HANDLE hEnt)
{
  tflcEntUpdate pflcEntUpdate;
  pflcEntUpdate = (tflcEntUpdate)GetProcAddress( ghLibInst, "lcEntUpdate" );
  if (pflcEntUpdate){
    return (*pflcEntUpdate)(hEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcEntCopyBase (HANDLE hEnt, HANDLE hEntFrom)
{
  tflcEntCopyBase pflcEntCopyBase;
  pflcEntCopyBase = (tflcEntCopyBase)GetProcAddress( ghLibInst, "lcEntCopyBase" );
  if (pflcEntCopyBase){
    return (*pflcEntCopyBase)(hEnt, hEntFrom);
  }
  return 0;
}


//--------------
int LCAPI lcIntersection (HANDLE hEnt, HANDLE hEnt2)
{
  tflcIntersection pflcIntersection;
  pflcIntersection = (tflcIntersection)GetProcAddress( ghLibInst, "lcIntersection" );
  if (pflcIntersection){
    return (*pflcIntersection)(hEnt, hEnt2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcInterGetPoint (int iPoint, double* pX, double* pY)
{
  tflcInterGetPoint pflcInterGetPoint;
  pflcInterGetPoint = (tflcInterGetPoint)GetProcAddress( ghLibInst, "lcInterGetPoint" );
  if (pflcInterGetPoint){
    return (*pflcInterGetPoint)(iPoint, pX, pY);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineAddVer (HANDLE hPline, HANDLE hVer, double X, double Y)
{
  tflcPlineAddVer pflcPlineAddVer;
  pflcPlineAddVer = (tflcPlineAddVer)GetProcAddress( ghLibInst, "lcPlineAddVer" );
  if (pflcPlineAddVer){
    return (*pflcPlineAddVer)(hPline, hVer, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineAddVer2 (HANDLE hPline, HANDLE hVer, double X, double Y, double Param, double W0, double W1)
{
  tflcPlineAddVer2 pflcPlineAddVer2;
  pflcPlineAddVer2 = (tflcPlineAddVer2)GetProcAddress( ghLibInst, "lcPlineAddVer2" );
  if (pflcPlineAddVer2){
    return (*pflcPlineAddVer2)(hPline, hVer, X, Y, Param, W0, W1);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineAddVerDir (HANDLE hPline, HANDLE hVer, double Ang, double Length)
{
  tflcPlineAddVerDir pflcPlineAddVerDir;
  pflcPlineAddVerDir = (tflcPlineAddVerDir)GetProcAddress( ghLibInst, "lcPlineAddVerDir" );
  if (pflcPlineAddVerDir){
    return (*pflcPlineAddVerDir)(hPline, hVer, Ang, Length);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPlineDeleteVer (HANDLE hPline, HANDLE hVer)
{
  tflcPlineDeleteVer pflcPlineDeleteVer;
  pflcPlineDeleteVer = (tflcPlineDeleteVer)GetProcAddress( ghLibInst, "lcPlineDeleteVer" );
  if (pflcPlineDeleteVer){
    return (*pflcPlineDeleteVer)(hPline, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetFirstVer (HANDLE hPline)
{
  tflcPlineGetFirstVer pflcPlineGetFirstVer;
  pflcPlineGetFirstVer = (tflcPlineGetFirstVer)GetProcAddress( ghLibInst, "lcPlineGetFirstVer" );
  if (pflcPlineGetFirstVer){
    return (*pflcPlineGetFirstVer)(hPline);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetNextVer (HANDLE hPline, HANDLE hVer)
{
  tflcPlineGetNextVer pflcPlineGetNextVer;
  pflcPlineGetNextVer = (tflcPlineGetNextVer)GetProcAddress( ghLibInst, "lcPlineGetNextVer" );
  if (pflcPlineGetNextVer){
    return (*pflcPlineGetNextVer)(hPline, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetLastVer (HANDLE hPline)
{
  tflcPlineGetLastVer pflcPlineGetLastVer;
  pflcPlineGetLastVer = (tflcPlineGetLastVer)GetProcAddress( ghLibInst, "lcPlineGetLastVer" );
  if (pflcPlineGetLastVer){
    return (*pflcPlineGetLastVer)(hPline);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetPrevVer (HANDLE hPline, HANDLE hVer)
{
  tflcPlineGetPrevVer pflcPlineGetPrevVer;
  pflcPlineGetPrevVer = (tflcPlineGetPrevVer)GetProcAddress( ghLibInst, "lcPlineGetPrevVer" );
  if (pflcPlineGetPrevVer){
    return (*pflcPlineGetPrevVer)(hPline, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetVer (HANDLE hPline, int Index)
{
  tflcPlineGetVer pflcPlineGetVer;
  pflcPlineGetVer = (tflcPlineGetVer)GetProcAddress( ghLibInst, "lcPlineGetVer" );
  if (pflcPlineGetVer){
    return (*pflcPlineGetVer)(hPline, Index);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetVerPt (HANDLE hPline, double X, double Y, double Delta)
{
  tflcPlineGetVerPt pflcPlineGetVerPt;
  pflcPlineGetVerPt = (tflcPlineGetVerPt)GetProcAddress( ghLibInst, "lcPlineGetVerPt" );
  if (pflcPlineGetVerPt){
    return (*pflcPlineGetVerPt)(hPline, X, Y, Delta);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcPlineGetSeg (HANDLE hPline, double X, double Y, double Delta)
{
  tflcPlineGetSeg pflcPlineGetSeg;
  pflcPlineGetSeg = (tflcPlineGetSeg)GetProcAddress( ghLibInst, "lcPlineGetSeg" );
  if (pflcPlineGetSeg){
    return (*pflcPlineGetSeg)(hPline, X, Y, Delta);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPlineReverse (HANDLE hPline)
{
  tflcPlineReverse pflcPlineReverse;
  pflcPlineReverse = (tflcPlineReverse)GetProcAddress( ghLibInst, "lcPlineReverse" );
  if (pflcPlineReverse){
    return (*pflcPlineReverse)(hPline);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPlineSimple (HANDLE hPline, BOOL bSimple, int nLines)
{
  tflcPlineSimple pflcPlineSimple;
  pflcPlineSimple = (tflcPlineSimple)GetProcAddress( ghLibInst, "lcPlineSimple" );
  if (pflcPlineSimple){
    return (*pflcPlineSimple)(hPline, bSimple, nLines);
  }
  return 0;
}


//--------------
int LCAPI lcPlineContainPoint (HANDLE hPline, double X, double Y)
{
  tflcPlineContainPoint pflcPlineContainPoint;
  pflcPlineContainPoint = (tflcPlineContainPoint)GetProcAddress( ghLibInst, "lcPlineContainPoint" );
  if (pflcPlineContainPoint){
    return (*pflcPlineContainPoint)(hPline, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPlineGetRoundPrm (HANDLE hPline, HANDLE hVer, double* px0, double* py0, double* pBulge, double* px1, double* py1)
{
  tflcPlineGetRoundPrm pflcPlineGetRoundPrm;
  pflcPlineGetRoundPrm = (tflcPlineGetRoundPrm)GetProcAddress( ghLibInst, "lcPlineGetRoundPrm" );
  if (pflcPlineGetRoundPrm){
    return (*pflcPlineGetRoundPrm)(hPline, hVer, px0, py0, pBulge, px1, py1);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineAddVer (HANDLE hMline, HANDLE hVer, double X, double Y)
{
  tflcMlineAddVer pflcMlineAddVer;
  pflcMlineAddVer = (tflcMlineAddVer)GetProcAddress( ghLibInst, "lcMlineAddVer" );
  if (pflcMlineAddVer){
    return (*pflcMlineAddVer)(hMline, hVer, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineAddVerDir (HANDLE hMline, HANDLE hVer, double Ang, double Length)
{
  tflcMlineAddVerDir pflcMlineAddVerDir;
  pflcMlineAddVerDir = (tflcMlineAddVerDir)GetProcAddress( ghLibInst, "lcMlineAddVerDir" );
  if (pflcMlineAddVerDir){
    return (*pflcMlineAddVerDir)(hMline, hVer, Ang, Length);
  }
  return 0;
}


//--------------
BOOL LCAPI lcMlineDeleteVer (HANDLE hMline, HANDLE hVer)
{
  tflcMlineDeleteVer pflcMlineDeleteVer;
  pflcMlineDeleteVer = (tflcMlineDeleteVer)GetProcAddress( ghLibInst, "lcMlineDeleteVer" );
  if (pflcMlineDeleteVer){
    return (*pflcMlineDeleteVer)(hMline, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetFirstVer (HANDLE hMline)
{
  tflcMlineGetFirstVer pflcMlineGetFirstVer;
  pflcMlineGetFirstVer = (tflcMlineGetFirstVer)GetProcAddress( ghLibInst, "lcMlineGetFirstVer" );
  if (pflcMlineGetFirstVer){
    return (*pflcMlineGetFirstVer)(hMline);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetNextVer (HANDLE hMline, HANDLE hVer)
{
  tflcMlineGetNextVer pflcMlineGetNextVer;
  pflcMlineGetNextVer = (tflcMlineGetNextVer)GetProcAddress( ghLibInst, "lcMlineGetNextVer" );
  if (pflcMlineGetNextVer){
    return (*pflcMlineGetNextVer)(hMline, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetLastVer (HANDLE hMline)
{
  tflcMlineGetLastVer pflcMlineGetLastVer;
  pflcMlineGetLastVer = (tflcMlineGetLastVer)GetProcAddress( ghLibInst, "lcMlineGetLastVer" );
  if (pflcMlineGetLastVer){
    return (*pflcMlineGetLastVer)(hMline);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetPrevVer (HANDLE hMline, HANDLE hVer)
{
  tflcMlineGetPrevVer pflcMlineGetPrevVer;
  pflcMlineGetPrevVer = (tflcMlineGetPrevVer)GetProcAddress( ghLibInst, "lcMlineGetPrevVer" );
  if (pflcMlineGetPrevVer){
    return (*pflcMlineGetPrevVer)(hMline, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetVer (HANDLE hMline, int Index)
{
  tflcMlineGetVer pflcMlineGetVer;
  pflcMlineGetVer = (tflcMlineGetVer)GetProcAddress( ghLibInst, "lcMlineGetVer" );
  if (pflcMlineGetVer){
    return (*pflcMlineGetVer)(hMline, Index);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetVerPt (HANDLE hMline, double X, double Y, double Delta)
{
  tflcMlineGetVerPt pflcMlineGetVerPt;
  pflcMlineGetVerPt = (tflcMlineGetVerPt)GetProcAddress( ghLibInst, "lcMlineGetVerPt" );
  if (pflcMlineGetVerPt){
    return (*pflcMlineGetVerPt)(hMline, X, Y, Delta);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcMlineGetSeg (HANDLE hMline, double X, double Y, double Delta)
{
  tflcMlineGetSeg pflcMlineGetSeg;
  pflcMlineGetSeg = (tflcMlineGetSeg)GetProcAddress( ghLibInst, "lcMlineGetSeg" );
  if (pflcMlineGetSeg){
    return (*pflcMlineGetSeg)(hMline, X, Y, Delta);
  }
  return 0;
}


//--------------
BOOL LCAPI lcMlineReverse (HANDLE hMline)
{
  tflcMlineReverse pflcMlineReverse;
  pflcMlineReverse = (tflcMlineReverse)GetProcAddress( ghLibInst, "lcMlineReverse" );
  if (pflcMlineReverse){
    return (*pflcMlineReverse)(hMline);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcRPlanAddVer (HANDLE hRPlan, double X, double Y)
{
  tflcRPlanAddVer pflcRPlanAddVer;
  pflcRPlanAddVer = (tflcRPlanAddVer)GetProcAddress( ghLibInst, "lcRPlanAddVer" );
  if (pflcRPlanAddVer){
    return (*pflcRPlanAddVer)(hRPlan, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcRPlanSetCurve (HANDLE hVer, double Radius, double LenClot1, double LenClot2)
{
  tflcRPlanSetCurve pflcRPlanSetCurve;
  pflcRPlanSetCurve = (tflcRPlanSetCurve)GetProcAddress( ghLibInst, "lcRPlanSetCurve" );
  if (pflcRPlanSetCurve){
    return (*pflcRPlanSetCurve)(hVer, Radius, LenClot1, LenClot2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcRPlanSetPos (HANDLE hVer, double X, double Y)
{
  tflcRPlanSetPos pflcRPlanSetPos;
  pflcRPlanSetPos = (tflcRPlanSetPos)GetProcAddress( ghLibInst, "lcRPlanSetPos" );
  if (pflcRPlanSetPos){
    return (*pflcRPlanSetPos)(hVer, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcRPlanDeleteVer (HANDLE hRPlan, HANDLE hVer)
{
  tflcRPlanDeleteVer pflcRPlanDeleteVer;
  pflcRPlanDeleteVer = (tflcRPlanDeleteVer)GetProcAddress( ghLibInst, "lcRPlanDeleteVer" );
  if (pflcRPlanDeleteVer){
    return (*pflcRPlanDeleteVer)(hRPlan, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcRPlanGetFirstVer (HANDLE hRPlan)
{
  tflcRPlanGetFirstVer pflcRPlanGetFirstVer;
  pflcRPlanGetFirstVer = (tflcRPlanGetFirstVer)GetProcAddress( ghLibInst, "lcRPlanGetFirstVer" );
  if (pflcRPlanGetFirstVer){
    return (*pflcRPlanGetFirstVer)(hRPlan);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcRPlanGetNextVer (HANDLE hRPlan, HANDLE hVer)
{
  tflcRPlanGetNextVer pflcRPlanGetNextVer;
  pflcRPlanGetNextVer = (tflcRPlanGetNextVer)GetProcAddress( ghLibInst, "lcRPlanGetNextVer" );
  if (pflcRPlanGetNextVer){
    return (*pflcRPlanGetNextVer)(hRPlan, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcRPlanGetLastVer (HANDLE hRPlan)
{
  tflcRPlanGetLastVer pflcRPlanGetLastVer;
  pflcRPlanGetLastVer = (tflcRPlanGetLastVer)GetProcAddress( ghLibInst, "lcRPlanGetLastVer" );
  if (pflcRPlanGetLastVer){
    return (*pflcRPlanGetLastVer)(hRPlan);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcRPlanGetPrevVer (HANDLE hRPlan, HANDLE hVer)
{
  tflcRPlanGetPrevVer pflcRPlanGetPrevVer;
  pflcRPlanGetPrevVer = (tflcRPlanGetPrevVer)GetProcAddress( ghLibInst, "lcRPlanGetPrevVer" );
  if (pflcRPlanGetPrevVer){
    return (*pflcRPlanGetPrevVer)(hRPlan, hVer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcRPlanGetVer (HANDLE hRPlan, int Index)
{
  tflcRPlanGetVer pflcRPlanGetVer;
  pflcRPlanGetVer = (tflcRPlanGetVer)GetProcAddress( ghLibInst, "lcRPlanGetVer" );
  if (pflcRPlanGetVer){
    return (*pflcRPlanGetVer)(hRPlan, Index);
  }
  return 0;
}


//--------------
BOOL LCAPI lcRPlanGetPoint (HANDLE hRPlan, double Dist, double* pX, double* pY, double* pAngle, int* pSide)
{
  tflcRPlanGetPoint pflcRPlanGetPoint;
  pflcRPlanGetPoint = (tflcRPlanGetPoint)GetProcAddress( ghLibInst, "lcRPlanGetPoint" );
  if (pflcRPlanGetPoint){
    return (*pflcRPlanGetPoint)(hRPlan, Dist, pX, pY, pAngle, pSide);
  }
  return 0;
}


//--------------
BOOL LCAPI lcRPlanGetDist (HANDLE hRPlan, double X, double Y, double* pX2, double* pY2, double* pDist, double* pOffset)
{
  tflcRPlanGetDist pflcRPlanGetDist;
  pflcRPlanGetDist = (tflcRPlanGetDist)GetProcAddress( ghLibInst, "lcRPlanGetDist" );
  if (pflcRPlanGetDist){
    return (*pflcRPlanGetDist)(hRPlan, X, Y, pX2, pY2, pDist, pOffset);
  }
  return 0;
}


//--------------
BOOL LCAPI lcVportSetView (HANDLE hVport, double Xcen, double Ycen, double Scal, double Angle)
{
  tflcVportSetView pflcVportSetView;
  pflcVportSetView = (tflcVportSetView)GetProcAddress( ghLibInst, "lcVportSetView" );
  if (pflcVportSetView){
    return (*pflcVportSetView)(hVport, Xcen, Ycen, Scal, Angle);
  }
  return 0;
}


//--------------
BOOL LCAPI lcVportFrolClear (HANDLE hVport)
{
  tflcVportFrolClear pflcVportFrolClear;
  pflcVportFrolClear = (tflcVportFrolClear)GetProcAddress( ghLibInst, "lcVportFrolClear" );
  if (pflcVportFrolClear){
    return (*pflcVportFrolClear)(hVport);
  }
  return 0;
}


//--------------
BOOL LCAPI lcVportFrolAdd (HANDLE hVport, HANDLE hLayer)
{
  tflcVportFrolAdd pflcVportFrolAdd;
  pflcVportFrolAdd = (tflcVportFrolAdd)GetProcAddress( ghLibInst, "lcVportFrolAdd" );
  if (pflcVportFrolAdd){
    return (*pflcVportFrolAdd)(hVport, hLayer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcVportFrolGet (HANDLE hVport, int iLayer)
{
  tflcVportFrolGet pflcVportFrolGet;
  pflcVportFrolGet = (tflcVportFrolGet)GetProcAddress( ghLibInst, "lcVportFrolGet" );
  if (pflcVportFrolGet){
    return (*pflcVportFrolGet)(hVport, iLayer);
  }
  return 0;
}


//--------------
BOOL LCAPI lcVportIsFrol (HANDLE hVport, HANDLE hLayer)
{
  tflcVportIsFrol pflcVportIsFrol;
  pflcVportIsFrol = (tflcVportIsFrol)GetProcAddress( ghLibInst, "lcVportIsFrol" );
  if (pflcVportIsFrol){
    return (*pflcVportIsFrol)(hVport, hLayer);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlkRefAddAtt (HANDLE hBlockRef, LPCWSTR szTag, LPCWSTR szValue)
{
  tflcBlkRefAddAtt pflcBlkRefAddAtt;
  pflcBlkRefAddAtt = (tflcBlkRefAddAtt)GetProcAddress( ghLibInst, "lcBlkRefAddAtt" );
  if (pflcBlkRefAddAtt){
    return (*pflcBlkRefAddAtt)(hBlockRef, szTag, szValue);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlkRefGetFirstAtt (HANDLE hBlockRef)
{
  tflcBlkRefGetFirstAtt pflcBlkRefGetFirstAtt;
  pflcBlkRefGetFirstAtt = (tflcBlkRefGetFirstAtt)GetProcAddress( ghLibInst, "lcBlkRefGetFirstAtt" );
  if (pflcBlkRefGetFirstAtt){
    return (*pflcBlkRefGetFirstAtt)(hBlockRef);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlkRefGetNextAtt (HANDLE hBlockRef, HANDLE hAttrib)
{
  tflcBlkRefGetNextAtt pflcBlkRefGetNextAtt;
  pflcBlkRefGetNextAtt = (tflcBlkRefGetNextAtt)GetProcAddress( ghLibInst, "lcBlkRefGetNextAtt" );
  if (pflcBlkRefGetNextAtt){
    return (*pflcBlkRefGetNextAtt)(hBlockRef, hAttrib);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcBlkRefGetAtt (HANDLE hBlockRef, LPCWSTR szTag)
{
  tflcBlkRefGetAtt pflcBlkRefGetAtt;
  pflcBlkRefGetAtt = (tflcBlkRefGetAtt)GetProcAddress( ghLibInst, "lcBlkRefGetAtt" );
  if (pflcBlkRefGetAtt){
    return (*pflcBlkRefGetAtt)(hBlockRef, szTag);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchSetPattern (HANDLE hHatch, LPCWSTR szFileName, LPCWSTR szPatName, double Scal, double Angle)
{
  tflcHatchSetPattern pflcHatchSetPattern;
  pflcHatchSetPattern = (tflcHatchSetPattern)GetProcAddress( ghLibInst, "lcHatchSetPattern" );
  if (pflcHatchSetPattern){
    return (*pflcHatchSetPattern)(hHatch, szFileName, szPatName, Scal, Angle);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchBoundStart (HANDLE hHatch)
{
  tflcHatchBoundStart pflcHatchBoundStart;
  pflcHatchBoundStart = (tflcHatchBoundStart)GetProcAddress( ghLibInst, "lcHatchBoundStart" );
  if (pflcHatchBoundStart){
    return (*pflcHatchBoundStart)(hHatch);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchBoundPoint (HANDLE hHatch, double X, double Y)
{
  tflcHatchBoundPoint pflcHatchBoundPoint;
  pflcHatchBoundPoint = (tflcHatchBoundPoint)GetProcAddress( ghLibInst, "lcHatchBoundPoint" );
  if (pflcHatchBoundPoint){
    return (*pflcHatchBoundPoint)(hHatch, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchBoundEndLoop (HANDLE hHatch)
{
  tflcHatchBoundEndLoop pflcHatchBoundEndLoop;
  pflcHatchBoundEndLoop = (tflcHatchBoundEndLoop)GetProcAddress( ghLibInst, "lcHatchBoundEndLoop" );
  if (pflcHatchBoundEndLoop){
    return (*pflcHatchBoundEndLoop)(hHatch);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchBoundEnd (HANDLE hHatch)
{
  tflcHatchBoundEnd pflcHatchBoundEnd;
  pflcHatchBoundEnd = (tflcHatchBoundEnd)GetProcAddress( ghLibInst, "lcHatchBoundEnd" );
  if (pflcHatchBoundEnd){
    return (*pflcHatchBoundEnd)(hHatch);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchPatStart (HANDLE hHatch)
{
  tflcHatchPatStart pflcHatchPatStart;
  pflcHatchPatStart = (tflcHatchPatStart)GetProcAddress( ghLibInst, "lcHatchPatStart" );
  if (pflcHatchPatStart){
    return (*pflcHatchPatStart)(hHatch);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchPatLine (HANDLE hHatch, double Angle, double x0, double y0, double dx, double dy, int nDash, double L0, double L1, double L2, double L3, double L4, double L5, double L6, double L7)
{
  tflcHatchPatLine pflcHatchPatLine;
  pflcHatchPatLine = (tflcHatchPatLine)GetProcAddress( ghLibInst, "lcHatchPatLine" );
  if (pflcHatchPatLine){
    return (*pflcHatchPatLine)(hHatch, Angle, x0, y0, dx, dy, nDash, L0, L1, L2, L3, L4, L5, L6, L7);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchPatEnd (HANDLE hHatch)
{
  tflcHatchPatEnd pflcHatchPatEnd;
  pflcHatchPatEnd = (tflcHatchPatEnd)GetProcAddress( ghLibInst, "lcHatchPatEnd" );
  if (pflcHatchPatEnd){
    return (*pflcHatchPatEnd)(hHatch);
  }
  return 0;
}


//--------------
int LCAPI lcHatchGetLoopSize (HANDLE hHatch, int iLoop)
{
  tflcHatchGetLoopSize pflcHatchGetLoopSize;
  pflcHatchGetLoopSize = (tflcHatchGetLoopSize)GetProcAddress( ghLibInst, "lcHatchGetLoopSize" );
  if (pflcHatchGetLoopSize){
    return (*pflcHatchGetLoopSize)(hHatch, iLoop);
  }
  return 0;
}


//--------------
BOOL LCAPI lcHatchGetPoint (HANDLE hHatch, int iPnt, double* pX, double* pY)
{
  tflcHatchGetPoint pflcHatchGetPoint;
  pflcHatchGetPoint = (tflcHatchGetPoint)GetProcAddress( ghLibInst, "lcHatchGetPoint" );
  if (pflcHatchGetPoint){
    return (*pflcHatchGetPoint)(hHatch, iPnt, pX, pY);
  }
  return 0;
}


//--------------
int LCAPI lcRectGetPolyline (HANDLE hRect, double* pX, double* pY, double* pBulge)
{
  tflcRectGetPolyline pflcRectGetPolyline;
  pflcRectGetPolyline = (tflcRectGetPolyline)GetProcAddress( ghLibInst, "lcRectGetPolyline" );
  if (pflcRectGetPolyline){
    return (*pflcRectGetPolyline)(hRect, pX, pY, pBulge);
  }
  return 0;
}


//--------------
BOOL LCAPI lcXlinePutDir (HANDLE hXline, double X, double Y)
{
  tflcXlinePutDir pflcXlinePutDir;
  pflcXlinePutDir = (tflcXlinePutDir)GetProcAddress( ghLibInst, "lcXlinePutDir" );
  if (pflcXlinePutDir){
    return (*pflcXlinePutDir)(hXline, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinClear (HANDLE hTin, BOOL bOnlyTrians)
{
  tflcTinClear pflcTinClear;
  pflcTinClear = (tflcTinClear)GetProcAddress( ghLibInst, "lcTinClear" );
  if (pflcTinClear){
    return (*pflcTinClear)(hTin, bOnlyTrians);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinLoad (HANDLE hTin, LPCWSTR szFileName, BOOL bEmbed)
{
  tflcTinLoad pflcTinLoad;
  pflcTinLoad = (tflcTinLoad)GetProcAddress( ghLibInst, "lcTinLoad" );
  if (pflcTinLoad){
    return (*pflcTinLoad)(hTin, szFileName, bEmbed);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinSave (HANDLE hTin, LPCWSTR szFileName, BOOL bOverwriteExist)
{
  tflcTinSave pflcTinSave;
  pflcTinSave = (tflcTinSave)GetProcAddress( ghLibInst, "lcTinSave" );
  if (pflcTinSave){
    return (*pflcTinSave)(hTin, szFileName, bOverwriteExist);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinAddPtype (HANDLE hTin, LPCWSTR szName, LPCWSTR szNotes, int Color)
{
  tflcTinAddPtype pflcTinAddPtype;
  pflcTinAddPtype = (tflcTinAddPtype)GetProcAddress( ghLibInst, "lcTinAddPtype" );
  if (pflcTinAddPtype){
    return (*pflcTinAddPtype)(hTin, szName, szNotes, Color);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinAddPoint (HANDLE hTin, HANDLE hPtype, double X, double Y, double Z)
{
  tflcTinAddPoint pflcTinAddPoint;
  pflcTinAddPoint = (tflcTinAddPoint)GetProcAddress( ghLibInst, "lcTinAddPoint" );
  if (pflcTinAddPoint){
    return (*pflcTinAddPoint)(hTin, hPtype, X, Y, Z);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinAddTrian (HANDLE hTin, HANDLE hPt0, HANDLE hPt1, HANDLE hPt2)
{
  tflcTinAddTrian pflcTinAddTrian;
  pflcTinAddTrian = (tflcTinAddTrian)GetProcAddress( ghLibInst, "lcTinAddTrian" );
  if (pflcTinAddTrian){
    return (*pflcTinAddTrian)(hTin, hPt0, hPt1, hPt2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinNormalize (HANDLE hTin)
{
  tflcTinNormalize pflcTinNormalize;
  pflcTinNormalize = (tflcTinNormalize)GetProcAddress( ghLibInst, "lcTinNormalize" );
  if (pflcTinNormalize){
    return (*pflcTinNormalize)(hTin);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinGetZ (HANDLE hTin, double X, double Y, double* pZ)
{
  tflcTinGetZ pflcTinGetZ;
  pflcTinGetZ = (tflcTinGetZ)GetProcAddress( ghLibInst, "lcTinGetZ" );
  if (pflcTinGetZ){
    return (*pflcTinGetZ)(hTin, X, Y, pZ);
  }
  return 0;
}


//--------------
int LCAPI lcTinInterLine (HANDLE hTin, double X0, double Y0, double X1, double Y1)
{
  tflcTinInterLine pflcTinInterLine;
  pflcTinInterLine = (tflcTinInterLine)GetProcAddress( ghLibInst, "lcTinInterLine" );
  if (pflcTinInterLine){
    return (*pflcTinInterLine)(hTin, X0, Y0, X1, Y1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinInterGetPoint (HANDLE hTin, int iPnt, double* pX, double* pY, double* pZ)
{
  tflcTinInterGetPoint pflcTinInterGetPoint;
  pflcTinInterGetPoint = (tflcTinInterGetPoint)GetProcAddress( ghLibInst, "lcTinInterGetPoint" );
  if (pflcTinInterGetPoint){
    return (*pflcTinInterGetPoint)(hTin, iPnt, pX, pY, pZ);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinDelPtype (HANDLE hTin, HANDLE hPtype)
{
  tflcTinDelPtype pflcTinDelPtype;
  pflcTinDelPtype = (tflcTinDelPtype)GetProcAddress( ghLibInst, "lcTinDelPtype" );
  if (pflcTinDelPtype){
    return (*pflcTinDelPtype)(hTin, hPtype);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinDelPoint (HANDLE hTin, HANDLE hPoint)
{
  tflcTinDelPoint pflcTinDelPoint;
  pflcTinDelPoint = (tflcTinDelPoint)GetProcAddress( ghLibInst, "lcTinDelPoint" );
  if (pflcTinDelPoint){
    return (*pflcTinDelPoint)(hTin, hPoint);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinDelTrian (HANDLE hTin, HANDLE hTrian)
{
  tflcTinDelTrian pflcTinDelTrian;
  pflcTinDelTrian = (tflcTinDelTrian)GetProcAddress( ghLibInst, "lcTinDelTrian" );
  if (pflcTinDelTrian){
    return (*pflcTinDelTrian)(hTin, hTrian);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinPtypeGetFirst (HANDLE hTin)
{
  tflcTinPtypeGetFirst pflcTinPtypeGetFirst;
  pflcTinPtypeGetFirst = (tflcTinPtypeGetFirst)GetProcAddress( ghLibInst, "lcTinPtypeGetFirst" );
  if (pflcTinPtypeGetFirst){
    return (*pflcTinPtypeGetFirst)(hTin);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinPtypeGetNext (HANDLE hTin, HANDLE hPtype)
{
  tflcTinPtypeGetNext pflcTinPtypeGetNext;
  pflcTinPtypeGetNext = (tflcTinPtypeGetNext)GetProcAddress( ghLibInst, "lcTinPtypeGetNext" );
  if (pflcTinPtypeGetNext){
    return (*pflcTinPtypeGetNext)(hTin, hPtype);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinPtypeGetByName (HANDLE hTin, LPCWSTR szName)
{
  tflcTinPtypeGetByName pflcTinPtypeGetByName;
  pflcTinPtypeGetByName = (tflcTinPtypeGetByName)GetProcAddress( ghLibInst, "lcTinPtypeGetByName" );
  if (pflcTinPtypeGetByName){
    return (*pflcTinPtypeGetByName)(hTin, szName);
  }
  return 0;
}


//--------------
BOOL LCAPI lcTinPtypeDialog (HANDLE hTin, HWND hWnd, BOOL bReadOnly)
{
  tflcTinPtypeDialog pflcTinPtypeDialog;
  pflcTinPtypeDialog = (tflcTinPtypeDialog)GetProcAddress( ghLibInst, "lcTinPtypeDialog" );
  if (pflcTinPtypeDialog){
    return (*pflcTinPtypeDialog)(hTin, hWnd, bReadOnly);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinPointGetFirst (HANDLE hTin)
{
  tflcTinPointGetFirst pflcTinPointGetFirst;
  pflcTinPointGetFirst = (tflcTinPointGetFirst)GetProcAddress( ghLibInst, "lcTinPointGetFirst" );
  if (pflcTinPointGetFirst){
    return (*pflcTinPointGetFirst)(hTin);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinPointGetNext (HANDLE hTin, HANDLE hPoint)
{
  tflcTinPointGetNext pflcTinPointGetNext;
  pflcTinPointGetNext = (tflcTinPointGetNext)GetProcAddress( ghLibInst, "lcTinPointGetNext" );
  if (pflcTinPointGetNext){
    return (*pflcTinPointGetNext)(hTin, hPoint);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinPointGetByPos (HANDLE hTin, double X, double Y, double Delta)
{
  tflcTinPointGetByPos pflcTinPointGetByPos;
  pflcTinPointGetByPos = (tflcTinPointGetByPos)GetProcAddress( ghLibInst, "lcTinPointGetByPos" );
  if (pflcTinPointGetByPos){
    return (*pflcTinPointGetByPos)(hTin, X, Y, Delta);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinTrianGetFirst (HANDLE hTin)
{
  tflcTinTrianGetFirst pflcTinTrianGetFirst;
  pflcTinTrianGetFirst = (tflcTinTrianGetFirst)GetProcAddress( ghLibInst, "lcTinTrianGetFirst" );
  if (pflcTinTrianGetFirst){
    return (*pflcTinTrianGetFirst)(hTin);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinTrianGetNext (HANDLE hTin, HANDLE hTrian)
{
  tflcTinTrianGetNext pflcTinTrianGetNext;
  pflcTinTrianGetNext = (tflcTinTrianGetNext)GetProcAddress( ghLibInst, "lcTinTrianGetNext" );
  if (pflcTinTrianGetNext){
    return (*pflcTinTrianGetNext)(hTin, hTrian);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcTinTrianGetByPos (HANDLE hTin, double X, double Y)
{
  tflcTinTrianGetByPos pflcTinTrianGetByPos;
  pflcTinTrianGetByPos = (tflcTinTrianGetByPos)GetProcAddress( ghLibInst, "lcTinTrianGetByPos" );
  if (pflcTinTrianGetByPos){
    return (*pflcTinTrianGetByPos)(hTin, X, Y);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcCreateWindow (HWND hWndParent, int Style, int Left, int Top, int Width, int Height)
{
  tflcCreateWindow pflcCreateWindow;
  pflcCreateWindow = (tflcCreateWindow)GetProcAddress( ghLibInst, "lcCreateWindow" );
  if (pflcCreateWindow){
    return (*pflcCreateWindow)(hWndParent, Style, Left, Top, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDeleteWindow (HANDLE hLcWnd)
{
  tflcDeleteWindow pflcDeleteWindow;
  pflcDeleteWindow = (tflcDeleteWindow)GetProcAddress( ghLibInst, "lcDeleteWindow" );
  if (pflcDeleteWindow){
    return (*pflcDeleteWindow)(hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndResize (HANDLE hLcWnd, int Left, int Top, int Width, int Height)
{
  tflcWndResize pflcWndResize;
  pflcWndResize = (tflcWndResize)GetProcAddress( ghLibInst, "lcWndResize" );
  if (pflcWndResize){
    return (*pflcWndResize)(hLcWnd, Left, Top, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndSelectView (HANDLE hLcWnd, HANDLE hView)
{
  tflcWndSelectView pflcWndSelectView;
  pflcWndSelectView = (tflcWndSelectView)GetProcAddress( ghLibInst, "lcWndSelectView" );
  if (pflcWndSelectView){
    return (*pflcWndSelectView)(hLcWnd, hView);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndZoomRect (HANDLE hLcWnd, double Left, double Bottom, double Right, double Top)
{
  tflcWndZoomRect pflcWndZoomRect;
  pflcWndZoomRect = (tflcWndZoomRect)GetProcAddress( ghLibInst, "lcWndZoomRect" );
  if (pflcWndZoomRect){
    return (*pflcWndZoomRect)(hLcWnd, Left, Bottom, Right, Top);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndZoomScale (HANDLE hLcWnd, double Scal)
{
  tflcWndZoomScale pflcWndZoomScale;
  pflcWndZoomScale = (tflcWndZoomScale)GetProcAddress( ghLibInst, "lcWndZoomScale" );
  if (pflcWndZoomScale){
    return (*pflcWndZoomScale)(hLcWnd, Scal);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndZoomMove (HANDLE hLcWnd, double DX, double DY)
{
  tflcWndZoomMove pflcWndZoomMove;
  pflcWndZoomMove = (tflcWndZoomMove)GetProcAddress( ghLibInst, "lcWndZoomMove" );
  if (pflcWndZoomMove){
    return (*pflcWndZoomMove)(hLcWnd, DX, DY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndRedraw (HANDLE hLcWnd)
{
  tflcWndRedraw pflcWndRedraw;
  pflcWndRedraw = (tflcWndRedraw)GetProcAddress( ghLibInst, "lcWndRedraw" );
  if (pflcWndRedraw){
    return (*pflcWndRedraw)(hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndSetFocus (HANDLE hLcWnd)
{
  tflcWndSetFocus pflcWndSetFocus;
  pflcWndSetFocus = (tflcWndSetFocus)GetProcAddress( ghLibInst, "lcWndSetFocus" );
  if (pflcWndSetFocus){
    return (*pflcWndSetFocus)(hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndExeCommand (HANDLE hLcWnd, int Command, int CmdParam)
{
  tflcWndExeCommand pflcWndExeCommand;
  pflcWndExeCommand = (tflcWndExeCommand)GetProcAddress( ghLibInst, "lcWndExeCommand" );
  if (pflcWndExeCommand){
    return (*pflcWndExeCommand)(hLcWnd, Command, CmdParam);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndCoordFromDrw (HANDLE hLcWnd, double Xdrw, double Ydrw, int* pXwin, int* pYwin)
{
  tflcWndCoordFromDrw pflcWndCoordFromDrw;
  pflcWndCoordFromDrw = (tflcWndCoordFromDrw)GetProcAddress( ghLibInst, "lcWndCoordFromDrw" );
  if (pflcWndCoordFromDrw){
    return (*pflcWndCoordFromDrw)(hLcWnd, Xdrw, Ydrw, pXwin, pYwin);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndCoordToDrw (HANDLE hLcWnd, int Xwin, int Ywin, double* pXdrw, double* pYdrw)
{
  tflcWndCoordToDrw pflcWndCoordToDrw;
  pflcWndCoordToDrw = (tflcWndCoordToDrw)GetProcAddress( ghLibInst, "lcWndCoordToDrw" );
  if (pflcWndCoordToDrw){
    return (*pflcWndCoordToDrw)(hLcWnd, Xwin, Ywin, pXdrw, pYdrw);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndGetCursorCoord (HANDLE hLcWnd, int* pXwin, int* pYwin, double* pXdrw, double* pYdrw)
{
  tflcWndGetCursorCoord pflcWndGetCursorCoord;
  pflcWndGetCursorCoord = (tflcWndGetCursorCoord)GetProcAddress( ghLibInst, "lcWndGetCursorCoord" );
  if (pflcWndGetCursorCoord){
    return (*pflcWndGetCursorCoord)(hLcWnd, pXwin, pYwin, pXdrw, pYdrw);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcWndGetEntByPoint (HANDLE hLcWnd, int Xwin, int Ywin)
{
  tflcWndGetEntByPoint pflcWndGetEntByPoint;
  pflcWndGetEntByPoint = (tflcWndGetEntByPoint)GetProcAddress( ghLibInst, "lcWndGetEntByPoint" );
  if (pflcWndGetEntByPoint){
    return (*pflcWndGetEntByPoint)(hLcWnd, Xwin, Ywin);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndSetCmdwin (HANDLE hLcWnd, HANDLE hCmdLine)
{
  tflcWndSetCmdwin pflcWndSetCmdwin;
  pflcWndSetCmdwin = (tflcWndSetCmdwin)GetProcAddress( ghLibInst, "lcWndSetCmdwin" );
  if (pflcWndSetCmdwin){
    return (*pflcWndSetCmdwin)(hLcWnd, hCmdLine);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndSetPropwin (HANDLE hLcWnd, HANDLE hPropWnd)
{
  tflcWndSetPropwin pflcWndSetPropwin;
  pflcWndSetPropwin = (tflcWndSetPropwin)GetProcAddress( ghLibInst, "lcWndSetPropwin" );
  if (pflcWndSetPropwin){
    return (*pflcWndSetPropwin)(hLcWnd, hPropWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndStopCommand (HANDLE hLcWnd)
{
  tflcWndStopCommand pflcWndStopCommand;
  pflcWndStopCommand = (tflcWndStopCommand)GetProcAddress( ghLibInst, "lcWndStopCommand" );
  if (pflcWndStopCommand){
    return (*pflcWndStopCommand)(hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndGetPoint (HANDLE hLcWnd, LPCWSTR szPrompt, int SnapMode, int Index, double* pX, double* pY)
{
  tflcWndGetPoint pflcWndGetPoint;
  pflcWndGetPoint = (tflcWndGetPoint)GetProcAddress( ghLibInst, "lcWndGetPoint" );
  if (pflcWndGetPoint){
    return (*pflcWndGetPoint)(hLcWnd, szPrompt, SnapMode, Index, pX, pY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndGetPointBuf (HANDLE hLcWnd, int Index, double* pX, double* pY)
{
  tflcWndGetPointBuf pflcWndGetPointBuf;
  pflcWndGetPointBuf = (tflcWndGetPointBuf)GetProcAddress( ghLibInst, "lcWndGetPointBuf" );
  if (pflcWndGetPointBuf){
    return (*pflcWndGetPointBuf)(hLcWnd, Index, pX, pY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcWndUpdateTabs (HANDLE hLcWnd)
{
  tflcWndUpdateTabs pflcWndUpdateTabs;
  pflcWndUpdateTabs = (tflcWndUpdateTabs)GetProcAddress( ghLibInst, "lcWndUpdateTabs" );
  if (pflcWndUpdateTabs){
    return (*pflcWndUpdateTabs)(hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCoordDrwToWnd (HANDLE hLcWnd, double Xdrw, double Ydrw, int* pXwin, int* pYwin)
{
  tflcCoordDrwToWnd pflcCoordDrwToWnd;
  pflcCoordDrwToWnd = (tflcCoordDrwToWnd)GetProcAddress( ghLibInst, "lcCoordDrwToWnd" );
  if (pflcCoordDrwToWnd){
    return (*pflcCoordDrwToWnd)(hLcWnd, Xdrw, Ydrw, pXwin, pYwin);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCoordWndToDrw (HANDLE hLcWnd, int Xwin, int Ywin, double* pXdrw, double* pYdrw)
{
  tflcCoordWndToDrw pflcCoordWndToDrw;
  pflcCoordWndToDrw = (tflcCoordWndToDrw)GetProcAddress( ghLibInst, "lcCoordWndToDrw" );
  if (pflcCoordWndToDrw){
    return (*pflcCoordWndToDrw)(hLcWnd, Xwin, Ywin, pXdrw, pYdrw);
  }
  return 0;
}


//--------------
BOOL LCAPI lcColorIsRGB (LPCWSTR szColor)
{
  tflcColorIsRGB pflcColorIsRGB;
  pflcColorIsRGB = (tflcColorIsRGB)GetProcAddress( ghLibInst, "lcColorIsRGB" );
  if (pflcColorIsRGB){
    return (*pflcColorIsRGB)(szColor);
  }
  return 0;
}


//--------------
int LCAPI lcColorGetRed (LPCWSTR szColor)
{
  tflcColorGetRed pflcColorGetRed;
  pflcColorGetRed = (tflcColorGetRed)GetProcAddress( ghLibInst, "lcColorGetRed" );
  if (pflcColorGetRed){
    return (*pflcColorGetRed)(szColor);
  }
  return 0;
}


//--------------
int LCAPI lcColorGetGreen (LPCWSTR szColor)
{
  tflcColorGetGreen pflcColorGetGreen;
  pflcColorGetGreen = (tflcColorGetGreen)GetProcAddress( ghLibInst, "lcColorGetGreen" );
  if (pflcColorGetGreen){
    return (*pflcColorGetGreen)(szColor);
  }
  return 0;
}


//--------------
int LCAPI lcColorGetBlue (LPCWSTR szColor)
{
  tflcColorGetBlue pflcColorGetBlue;
  pflcColorGetBlue = (tflcColorGetBlue)GetProcAddress( ghLibInst, "lcColorGetBlue" );
  if (pflcColorGetBlue){
    return (*pflcColorGetBlue)(szColor);
  }
  return 0;
}


//--------------
int LCAPI lcColorGetIndex (LPCWSTR szColor)
{
  tflcColorGetIndex pflcColorGetIndex;
  pflcColorGetIndex = (tflcColorGetIndex)GetProcAddress( ghLibInst, "lcColorGetIndex" );
  if (pflcColorGetIndex){
    return (*pflcColorGetIndex)(szColor);
  }
  return 0;
}


//--------------
BOOL LCAPI lcColorToVal (LPCWSTR szColor, int* pbRGB, int* pIndex, int* pR, int* pG, int* pB)
{
  tflcColorToVal pflcColorToVal;
  pflcColorToVal = (tflcColorToVal)GetProcAddress( ghLibInst, "lcColorToVal" );
  if (pflcColorToVal){
    return (*pflcColorToVal)(szColor, pbRGB, pIndex, pR, pG, pB);
  }
  return 0;
}


//--------------
BOOL LCAPI lcColorSetPalette (int Index, int R, int G, int B)
{
  tflcColorSetPalette pflcColorSetPalette;
  pflcColorSetPalette = (tflcColorSetPalette)GetProcAddress( ghLibInst, "lcColorSetPalette" );
  if (pflcColorSetPalette){
    return (*pflcColorSetPalette)(Index, R, G, B);
  }
  return 0;
}


//--------------
BOOL LCAPI lcColorGetPalette (int Index, int* pR, int* pG, int* pB)
{
  tflcColorGetPalette pflcColorGetPalette;
  pflcColorGetPalette = (tflcColorGetPalette)GetProcAddress( ghLibInst, "lcColorGetPalette" );
  if (pflcColorGetPalette){
    return (*pflcColorGetPalette)(Index, pR, pG, pB);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcCreateCmdwin (HWND hWndParent, int Left, int Top, int Width, int Height)
{
  tflcCreateCmdwin pflcCreateCmdwin;
  pflcCreateCmdwin = (tflcCreateCmdwin)GetProcAddress( ghLibInst, "lcCreateCmdwin" );
  if (pflcCreateCmdwin){
    return (*pflcCreateCmdwin)(hWndParent, Left, Top, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdwinResize (HANDLE hCmdLine, int Left, int Top, int Width, int Height)
{
  tflcCmdwinResize pflcCmdwinResize;
  pflcCmdwinResize = (tflcCmdwinResize)GetProcAddress( ghLibInst, "lcCmdwinResize" );
  if (pflcCmdwinResize){
    return (*pflcCmdwinResize)(hCmdLine, Left, Top, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdwinUpdate (HANDLE hCmdLine)
{
  tflcCmdwinUpdate pflcCmdwinUpdate;
  pflcCmdwinUpdate = (tflcCmdwinUpdate)GetProcAddress( ghLibInst, "lcCmdwinUpdate" );
  if (pflcCmdwinUpdate){
    return (*pflcCmdwinUpdate)(hCmdLine);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcCreatePropwin (HWND hWndParent, int Left, int Top, int Width, int Height)
{
  tflcCreatePropwin pflcCreatePropwin;
  pflcCreatePropwin = (tflcCreatePropwin)GetProcAddress( ghLibInst, "lcCreatePropwin" );
  if (pflcCreatePropwin){
    return (*pflcCreatePropwin)(hWndParent, Left, Top, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropwinResize (HANDLE hPropWnd, int Left, int Top, int Width, int Height)
{
  tflcPropwinResize pflcPropwinResize;
  pflcPropwinResize = (tflcPropwinResize)GetProcAddress( ghLibInst, "lcPropwinResize" );
  if (pflcPropwinResize){
    return (*pflcPropwinResize)(hPropWnd, Left, Top, Width, Height);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPropwinUpdate (HANDLE hPropWnd, BOOL bSelChanged)
{
  tflcPropwinUpdate pflcPropwinUpdate;
  pflcPropwinUpdate = (tflcPropwinUpdate)GetProcAddress( ghLibInst, "lcPropwinUpdate" );
  if (pflcPropwinUpdate){
    return (*pflcPropwinUpdate)(hPropWnd, bSelChanged);
  }
  return 0;
}


//--------------
int LCAPI lcGetErrorCode ()
{
  tflcGetErrorCode pflcGetErrorCode;
  pflcGetErrorCode = (tflcGetErrorCode)GetProcAddress( ghLibInst, "lcGetErrorCode" );
  if (pflcGetErrorCode){
    return (*pflcGetErrorCode)();
  }
  return 0;
}


//--------------
LPCWSTR LCAPI lcGetErrorStr ()
{
  tflcGetErrorStr pflcGetErrorStr;
  pflcGetErrorStr = (tflcGetErrorStr)GetProcAddress( ghLibInst, "lcGetErrorStr" );
  if (pflcGetErrorStr){
    return (*pflcGetErrorStr)();
  }
  return 0;
}


//--------------
int LCAPI lcExtractPreview (LPCWSTR szFileName, HGLOBAL hGlob, int MaxSize)
{
  tflcExtractPreview pflcExtractPreview;
  pflcExtractPreview = (tflcExtractPreview)GetProcAddress( ghLibInst, "lcExtractPreview" );
  if (pflcExtractPreview){
    return (*pflcExtractPreview)(szFileName, hGlob, MaxSize);
  }
  return 0;
}


//--------------
BOOL LCAPI lcExtractFileInfo (LPCWSTR szFileName, WCHAR* szGuid, WCHAR* szComment, int* pInt0, int* pInt1)
{
  tflcExtractFileInfo pflcExtractFileInfo;
  pflcExtractFileInfo = (tflcExtractFileInfo)GetProcAddress( ghLibInst, "lcExtractFileInfo" );
  if (pflcExtractFileInfo){
    return (*pflcExtractFileInfo)(szFileName, szGuid, szComment, pInt0, pInt1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcFilletSetLines (double L1x0, double L1y0, double L1x1, double L1y1, double L2x0, double L2y0, double L2x1, double L2y1)
{
  tflcFilletSetLines pflcFilletSetLines;
  pflcFilletSetLines = (tflcFilletSetLines)GetProcAddress( ghLibInst, "lcFilletSetLines" );
  if (pflcFilletSetLines){
    return (*pflcFilletSetLines)(L1x0, L1y0, L1x1, L1y1, L2x0, L2y0, L2x1, L2y1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcFillet (double Rad, double Bis, double Tang)
{
  tflcFillet pflcFillet;
  pflcFillet = (tflcFillet)GetProcAddress( ghLibInst, "lcFillet" );
  if (pflcFillet){
    return (*pflcFillet)(Rad, Bis, Tang);
  }
  return 0;
}


//--------------
BOOL LCAPI lcFilletGetPoint (int iPnt, double* pX, double* pY)
{
  tflcFilletGetPoint pflcFilletGetPoint;
  pflcFilletGetPoint = (tflcFilletGetPoint)GetProcAddress( ghLibInst, "lcFilletGetPoint" );
  if (pflcFilletGetPoint){
    return (*pflcFilletGetPoint)(iPnt, pX, pY);
  }
  return 0;
}


//--------------
LPCWSTR LCAPI lcDgGetFileName (HWND hWnd, int Mode)
{
  tflcDgGetFileName pflcDgGetFileName;
  pflcDgGetFileName = (tflcDgGetFileName)GetProcAddress( ghLibInst, "lcDgGetFileName" );
  if (pflcDgGetFileName){
    return (*pflcDgGetFileName)(hWnd, Mode);
  }
  return 0;
}


//--------------
LPCWSTR LCAPI lcDgGetValue (HWND hWnd, int Lef, int Top, LPCWSTR szTitle, LPCWSTR szPrompt, LPCWSTR szValue)
{
  tflcDgGetValue pflcDgGetValue;
  pflcDgGetValue = (tflcDgGetValue)GetProcAddress( ghLibInst, "lcDgGetValue" );
  if (pflcDgGetValue){
    return (*pflcDgGetValue)(hWnd, Lef, Top, szTitle, szPrompt, szValue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcDgGetValue2 (HWND hWnd, int Lef, int Top, LPCWSTR szTitle, LPCWSTR szPrompt, WCHAR* szValue, int MaxChar)
{
  tflcDgGetValue2 pflcDgGetValue2;
  pflcDgGetValue2 = (tflcDgGetValue2)GetProcAddress( ghLibInst, "lcDgGetValue2" );
  if (pflcDgGetValue2){
    return (*pflcDgGetValue2)(hWnd, Lef, Top, szTitle, szPrompt, szValue, MaxChar);
  }
  return 0;
}


//--------------
LPCWSTR LCAPI lcPlugGetOption (LPCWSTR szFileName, LPCWSTR szKey)
{
  tflcPlugGetOption pflcPlugGetOption;
  pflcPlugGetOption = (tflcPlugGetOption)GetProcAddress( ghLibInst, "lcPlugGetOption" );
  if (pflcPlugGetOption){
    return (*pflcPlugGetOption)(szFileName, szKey);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPlugSetOption (LPCWSTR szFileName, LPCWSTR szKey, LPCWSTR szValue, BOOL bSave)
{
  tflcPlugSetOption pflcPlugSetOption;
  pflcPlugSetOption = (tflcPlugSetOption)GetProcAddress( ghLibInst, "lcPlugSetOption" );
  if (pflcPlugSetOption){
    return (*pflcPlugSetOption)(szFileName, szKey, szValue, bSave);
  }
  return 0;
}


//--------------
void LCAPI lcGetPolarPoint (double X, double Y, double Ang, double Dist, double* pX, double* pY)
{
  tflcGetPolarPoint pflcGetPolarPoint;
  pflcGetPolarPoint = (tflcGetPolarPoint)GetProcAddress( ghLibInst, "lcGetPolarPoint" );
  if (pflcGetPolarPoint){
    (*pflcGetPolarPoint)(X, Y, Ang, Dist, pX, pY);
  }
}


//--------------
BOOL LCAPI lcGetClientSize (HWND hWnd, int* pWidth, int* pHeight)
{
  tflcGetClientSize pflcGetClientSize;
  pflcGetClientSize = (tflcGetClientSize)GetProcAddress( ghLibInst, "lcGetClientSize" );
  if (pflcGetClientSize){
    return (*pflcGetClientSize)(hWnd, pWidth, pHeight);
  }
  return 0;
}


//--------------
void LCAPI lcTextOut (HDC hDC, int x, int y, LPCWSTR szText, int ColorRGB)
{
  tflcTextOut pflcTextOut;
  pflcTextOut = (tflcTextOut)GetProcAddress( ghLibInst, "lcTextOut" );
  if (pflcTextOut){
    (*pflcTextOut)(hDC, x, y, szText, ColorRGB);
  }
}


//--------------
HANDLE LCAPI lcCreateCommand (HANDLE hLcWnd, int Id, LPCWSTR szName, BOOL bNeedSel)
{
  tflcCreateCommand pflcCreateCommand;
  pflcCreateCommand = (tflcCreateCommand)GetProcAddress( ghLibInst, "lcCreateCommand" );
  if (pflcCreateCommand){
    return (*pflcCreateCommand)(hLcWnd, Id, szName, bNeedSel);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdExit ()
{
  tflcCmdExit pflcCmdExit;
  pflcCmdExit = (tflcCmdExit)GetProcAddress( ghLibInst, "lcCmdExit" );
  if (pflcCmdExit){
    return (*pflcCmdExit)();
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdPrompt (HANDLE hCmd, LPCWSTR szText)
{
  tflcCmdPrompt pflcCmdPrompt;
  pflcCmdPrompt = (tflcCmdPrompt)GetProcAddress( ghLibInst, "lcCmdPrompt" );
  if (pflcCmdPrompt){
    return (*pflcCmdPrompt)(hCmd, szText);
  }
  return 0;
}


//--------------
HANDLE LCAPI lcCmdGetEntByPoint (HANDLE hCmd, int Xwnd, int Ywnd)
{
  tflcCmdGetEntByPoint pflcCmdGetEntByPoint;
  pflcCmdGetEntByPoint = (tflcCmdGetEntByPoint)GetProcAddress( ghLibInst, "lcCmdGetEntByPoint" );
  if (pflcCmdGetEntByPoint){
    return (*pflcCmdGetEntByPoint)(hCmd, Xwnd, Ywnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdSelectEnt (HANDLE hCmd, HANDLE hEntity, BOOL bSelect)
{
  tflcCmdSelectEnt pflcCmdSelectEnt;
  pflcCmdSelectEnt = (tflcCmdSelectEnt)GetProcAddress( ghLibInst, "lcCmdSelectEnt" );
  if (pflcCmdSelectEnt){
    return (*pflcCmdSelectEnt)(hCmd, hEntity, bSelect);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdRedraw (HANDLE hCmd)
{
  tflcCmdRedraw pflcCmdRedraw;
  pflcCmdRedraw = (tflcCmdRedraw)GetProcAddress( ghLibInst, "lcCmdRedraw" );
  if (pflcCmdRedraw){
    return (*pflcCmdRedraw)(hCmd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdSetFocus (HANDLE hCmd)
{
  tflcCmdSetFocus pflcCmdSetFocus;
  pflcCmdSetFocus = (tflcCmdSetFocus)GetProcAddress( ghLibInst, "lcCmdSetFocus" );
  if (pflcCmdSetFocus){
    return (*pflcCmdSetFocus)(hCmd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdRegen (HANDLE hCmd, HANDLE hEnt)
{
  tflcCmdRegen pflcCmdRegen;
  pflcCmdRegen = (tflcCmdRegen)GetProcAddress( ghLibInst, "lcCmdRegen" );
  if (pflcCmdRegen){
    return (*pflcCmdRegen)(hCmd, hEnt);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdClearPoints (HANDLE hCmd)
{
  tflcCmdClearPoints pflcCmdClearPoints;
  pflcCmdClearPoints = (tflcCmdClearPoints)GetProcAddress( ghLibInst, "lcCmdClearPoints" );
  if (pflcCmdClearPoints){
    return (*pflcCmdClearPoints)(hCmd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdAddPoint (HANDLE hCmd, int Id, double X, double Y)
{
  tflcCmdAddPoint pflcCmdAddPoint;
  pflcCmdAddPoint = (tflcCmdAddPoint)GetProcAddress( ghLibInst, "lcCmdAddPoint" );
  if (pflcCmdAddPoint){
    return (*pflcCmdAddPoint)(hCmd, Id, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdGetPoint (HANDLE hCmd, int Id, double* pX, double* pY)
{
  tflcCmdGetPoint pflcCmdGetPoint;
  pflcCmdGetPoint = (tflcCmdGetPoint)GetProcAddress( ghLibInst, "lcCmdGetPoint" );
  if (pflcCmdGetPoint){
    return (*pflcCmdGetPoint)(hCmd, Id, pX, pY);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdSetBasePoint (HANDLE hCmd, BOOL bEnable, double X, double Y)
{
  tflcCmdSetBasePoint pflcCmdSetBasePoint;
  pflcCmdSetBasePoint = (tflcCmdSetBasePoint)GetProcAddress( ghLibInst, "lcCmdSetBasePoint" );
  if (pflcCmdSetBasePoint){
    return (*pflcCmdSetBasePoint)(hCmd, bEnable, X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdDrawLine (HANDLE hCmd, double X0, double Y0, double X1, double Y1)
{
  tflcCmdDrawLine pflcCmdDrawLine;
  pflcCmdDrawLine = (tflcCmdDrawLine)GetProcAddress( ghLibInst, "lcCmdDrawLine" );
  if (pflcCmdDrawLine){
    return (*pflcCmdDrawLine)(hCmd, X0, Y0, X1, Y1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdDrawPickbox (HANDLE hCmd, int Xwnd, int Ywnd)
{
  tflcCmdDrawPickbox pflcCmdDrawPickbox;
  pflcCmdDrawPickbox = (tflcCmdDrawPickbox)GetProcAddress( ghLibInst, "lcCmdDrawPickbox" );
  if (pflcCmdDrawPickbox){
    return (*pflcCmdDrawPickbox)(hCmd, Xwnd, Ywnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCmdEqualPoints (HANDLE hCmd, double X0, double Y0, double X1, double Y1, int Delta)
{
  tflcCmdEqualPoints pflcCmdEqualPoints;
  pflcCmdEqualPoints = (tflcCmdEqualPoints)GetProcAddress( ghLibInst, "lcCmdEqualPoints" );
  if (pflcCmdEqualPoints){
    return (*pflcCmdEqualPoints)(hCmd, X0, Y0, X1, Y1, Delta);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintBegin (HANDLE hLcWnd)
{
  tflcPaintBegin pflcPaintBegin;
  pflcPaintBegin = (tflcPaintBegin)GetProcAddress( ghLibInst, "lcPaintBegin" );
  if (pflcPaintBegin){
    return (*pflcPaintBegin)(hLcWnd);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintSetPen (int Style, int Width, int ColorRed, int ColorGreen, int ColorBlue)
{
  tflcPaintSetPen pflcPaintSetPen;
  pflcPaintSetPen = (tflcPaintSetPen)GetProcAddress( ghLibInst, "lcPaintSetPen" );
  if (pflcPaintSetPen){
    return (*pflcPaintSetPen)(Style, Width, ColorRed, ColorGreen, ColorBlue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintSetBrush (int ColorRed, int ColorGreen, int ColorBlue)
{
  tflcPaintSetBrush pflcPaintSetBrush;
  pflcPaintSetBrush = (tflcPaintSetBrush)GetProcAddress( ghLibInst, "lcPaintSetBrush" );
  if (pflcPaintSetBrush){
    return (*pflcPaintSetBrush)(ColorRed, ColorGreen, ColorBlue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintPixel (double X, double Y, int Size, int ColorRed, int ColorGreen, int ColorBlue)
{
  tflcPaintPixel pflcPaintPixel;
  pflcPaintPixel = (tflcPaintPixel)GetProcAddress( ghLibInst, "lcPaintPixel" );
  if (pflcPaintPixel){
    return (*pflcPaintPixel)(X, Y, Size, ColorRed, ColorGreen, ColorBlue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintPoint (double X, double Y, int PtMode, double PtSize)
{
  tflcPaintPoint pflcPaintPoint;
  pflcPaintPoint = (tflcPaintPoint)GetProcAddress( ghLibInst, "lcPaintPoint" );
  if (pflcPaintPoint){
    return (*pflcPaintPoint)(X, Y, PtMode, PtSize);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintLine (double X0, double Y0, double X1, double Y1)
{
  tflcPaintLine pflcPaintLine;
  pflcPaintLine = (tflcPaintLine)GetProcAddress( ghLibInst, "lcPaintLine" );
  if (pflcPaintLine){
    return (*pflcPaintLine)(X0, Y0, X1, Y1);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintPlineVer (double X, double Y)
{
  tflcPaintPlineVer pflcPaintPlineVer;
  pflcPaintPlineVer = (tflcPaintPlineVer)GetProcAddress( ghLibInst, "lcPaintPlineVer" );
  if (pflcPaintPlineVer){
    return (*pflcPaintPlineVer)(X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintPline (BOOL bClosed, BOOL bFilled, BOOL bBorder)
{
  tflcPaintPline pflcPaintPline;
  pflcPaintPline = (tflcPaintPline)GetProcAddress( ghLibInst, "lcPaintPline" );
  if (pflcPaintPline){
    return (*pflcPaintPline)(bClosed, bFilled, bBorder);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintCircle (double X, double Y, double R, BOOL bFilled, BOOL bBorder)
{
  tflcPaintCircle pflcPaintCircle;
  pflcPaintCircle = (tflcPaintCircle)GetProcAddress( ghLibInst, "lcPaintCircle" );
  if (pflcPaintCircle){
    return (*pflcPaintCircle)(X, Y, R, bFilled, bBorder);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintArc (double X, double Y, double R, double Ang0, double AngArc)
{
  tflcPaintArc pflcPaintArc;
  pflcPaintArc = (tflcPaintArc)GetProcAddress( ghLibInst, "lcPaintArc" );
  if (pflcPaintArc){
    return (*pflcPaintArc)(X, Y, R, Ang0, AngArc);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintSetFont (LPCWSTR szFontName, double Height, double Angle, bool bBold, bool bItalic, bool bUnderline, bool bStrikeOut)
{
  tflcPaintSetFont pflcPaintSetFont;
  pflcPaintSetFont = (tflcPaintSetFont)GetProcAddress( ghLibInst, "lcPaintSetFont" );
  if (pflcPaintSetFont){
    return (*pflcPaintSetFont)(szFontName, Height, Angle, bBold, bItalic, bUnderline, bStrikeOut);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintSetTextColor (int ColorRed, int ColorGreen, int ColorBlue)
{
  tflcPaintSetTextColor pflcPaintSetTextColor;
  pflcPaintSetTextColor = (tflcPaintSetTextColor)GetProcAddress( ghLibInst, "lcPaintSetTextColor" );
  if (pflcPaintSetTextColor){
    return (*pflcPaintSetTextColor)(ColorRed, ColorGreen, ColorBlue);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintText (LPCWSTR szText, double X, double Y, int Align)
{
  tflcPaintText pflcPaintText;
  pflcPaintText = (tflcPaintText)GetProcAddress( ghLibInst, "lcPaintText" );
  if (pflcPaintText){
    return (*pflcPaintText)(szText, X, Y, Align);
  }
  return 0;
}


//--------------
BOOL LCAPI lcPaintEnd ()
{
  tflcPaintEnd pflcPaintEnd;
  pflcPaintEnd = (tflcPaintEnd)GetProcAddress( ghLibInst, "lcPaintEnd" );
  if (pflcPaintEnd){
    return (*pflcPaintEnd)();
  }
  return 0;
}


//--------------
void LCAPI lcSurfInitialize (int MaxSectPnt, double AngStep)
{
  tflcSurfInitialize pflcSurfInitialize;
  pflcSurfInitialize = (tflcSurfInitialize)GetProcAddress( ghLibInst, "lcSurfInitialize" );
  if (pflcSurfInitialize){
    (*pflcSurfInitialize)(MaxSectPnt, AngStep);
  }
}


//--------------
void LCAPI lcSurfUnInitialize ()
{
  tflcSurfUnInitialize pflcSurfUnInitialize;
  pflcSurfUnInitialize = (tflcSurfUnInitialize)GetProcAddress( ghLibInst, "lcSurfUnInitialize" );
  if (pflcSurfUnInitialize){
    (*pflcSurfUnInitialize)();
  }
}


//--------------
void LCAPI lcSurfClearTriangles ()
{
  tflcSurfClearTriangles pflcSurfClearTriangles;
  pflcSurfClearTriangles = (tflcSurfClearTriangles)GetProcAddress( ghLibInst, "lcSurfClearTriangles" );
  if (pflcSurfClearTriangles){
    (*pflcSurfClearTriangles)();
  }
}


//--------------
BOOL LCAPI lcSurfAddTriangle (double X0, double Y0, double Z0, double X1, double Y1, double Z1, double X2, double Y2, double Z2)
{
  tflcSurfAddTriangle pflcSurfAddTriangle;
  pflcSurfAddTriangle = (tflcSurfAddTriangle)GetProcAddress( ghLibInst, "lcSurfAddTriangle" );
  if (pflcSurfAddTriangle){
    return (*pflcSurfAddTriangle)(X0, Y0, Z0, X1, Y1, Z1, X2, Y2, Z2);
  }
  return 0;
}


//--------------
BOOL LCAPI lcSurfGetZ (double X, double Y, double* pZ)
{
  tflcSurfGetZ pflcSurfGetZ;
  pflcSurfGetZ = (tflcSurfGetZ)GetProcAddress( ghLibInst, "lcSurfGetZ" );
  if (pflcSurfGetZ){
    return (*pflcSurfGetZ)(X, Y, pZ);
  }
  return 0;
}


//--------------
BOOL LCAPI lcSurfGetZ_dbg (double X, double Y, double* pZ, double* pX0, double* pY0, double* pZ0, double* pX1, double* pY1, double* pZ1, double* pX2, double* pY2, double* pZ2)
{
  tflcSurfGetZ_dbg pflcSurfGetZ_dbg;
  pflcSurfGetZ_dbg = (tflcSurfGetZ_dbg)GetProcAddress( ghLibInst, "lcSurfGetZ_dbg" );
  if (pflcSurfGetZ_dbg){
    return (*pflcSurfGetZ_dbg)(X, Y, pZ, pX0, pY0, pZ0, pX1, pY1, pZ1, pX2, pY2, pZ2);
  }
  return 0;
}


//--------------
void LCAPI lcSurfClearPlan ()
{
  tflcSurfClearPlan pflcSurfClearPlan;
  pflcSurfClearPlan = (tflcSurfClearPlan)GetProcAddress( ghLibInst, "lcSurfClearPlan" );
  if (pflcSurfClearPlan){
    (*pflcSurfClearPlan)();
  }
}


//--------------
BOOL LCAPI lcSurfAddPlanVer (double X, double Y, double Bulge)
{
  tflcSurfAddPlanVer pflcSurfAddPlanVer;
  pflcSurfAddPlanVer = (tflcSurfAddPlanVer)GetProcAddress( ghLibInst, "lcSurfAddPlanVer" );
  if (pflcSurfAddPlanVer){
    return (*pflcSurfAddPlanVer)(X, Y, Bulge);
  }
  return 0;
}


//--------------
int LCAPI lcSurfMakeSection ()
{
  tflcSurfMakeSection pflcSurfMakeSection;
  pflcSurfMakeSection = (tflcSurfMakeSection)GetProcAddress( ghLibInst, "lcSurfMakeSection" );
  if (pflcSurfMakeSection){
    return (*pflcSurfMakeSection)();
  }
  return 0;
}


//--------------
BOOL LCAPI lcSurfGetSectPoint (int iPnt, double* pX, double* pY, double* pZ)
{
  tflcSurfGetSectPoint pflcSurfGetSectPoint;
  pflcSurfGetSectPoint = (tflcSurfGetSectPoint)GetProcAddress( ghLibInst, "lcSurfGetSectPoint" );
  if (pflcSurfGetSectPoint){
    return (*pflcSurfGetSectPoint)(iPnt, pX, pY, pZ);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCSectClear ()
{
  tflcCSectClear pflcCSectClear;
  pflcCSectClear = (tflcCSectClear)GetProcAddress( ghLibInst, "lcCSectClear" );
  if (pflcCSectClear){
    return (*pflcCSectClear)();
  }
  return 0;
}


//--------------
BOOL LCAPI lcCSectBegin (int PlineType)
{
  tflcCSectBegin pflcCSectBegin;
  pflcCSectBegin = (tflcCSectBegin)GetProcAddress( ghLibInst, "lcCSectBegin" );
  if (pflcCSectBegin){
    return (*pflcCSectBegin)(PlineType);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCSectAddPoint (double X, double Y)
{
  tflcCSectAddPoint pflcCSectAddPoint;
  pflcCSectAddPoint = (tflcCSectAddPoint)GetProcAddress( ghLibInst, "lcCSectAddPoint" );
  if (pflcCSectAddPoint){
    return (*pflcCSectAddPoint)(X, Y);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCSectEnd ()
{
  tflcCSectEnd pflcCSectEnd;
  pflcCSectEnd = (tflcCSectEnd)GetProcAddress( ghLibInst, "lcCSectEnd" );
  if (pflcCSectEnd){
    return (*pflcCSectEnd)();
  }
  return 0;
}


//--------------
int LCAPI lcCSectCalcArea ()
{
  tflcCSectCalcArea pflcCSectCalcArea;
  pflcCSectCalcArea = (tflcCSectCalcArea)GetProcAddress( ghLibInst, "lcCSectCalcArea" );
  if (pflcCSectCalcArea){
    return (*pflcCSectCalcArea)();
  }
  return 0;
}


//--------------
BOOL LCAPI lcCSectGetArea (int iArea, double* pAreaVal, int* pNumPnts)
{
  tflcCSectGetArea pflcCSectGetArea;
  pflcCSectGetArea = (tflcCSectGetArea)GetProcAddress( ghLibInst, "lcCSectGetArea" );
  if (pflcCSectGetArea){
    return (*pflcCSectGetArea)(iArea, pAreaVal, pNumPnts);
  }
  return 0;
}


//--------------
BOOL LCAPI lcCSectGetAreaPoint (int iArea, int iPnt, double* pX, double* pY)
{
  tflcCSectGetAreaPoint pflcCSectGetAreaPoint;
  pflcCSectGetAreaPoint = (tflcCSectGetAreaPoint)GetProcAddress( ghLibInst, "lcCSectGetAreaPoint" );
  if (pflcCSectGetAreaPoint){
    return (*pflcCSectGetAreaPoint)(iArea, iPnt, pX, pY);
  }
  return 0;
}


//--------------
void LCAPI lcEventReturnCode (int code)
{
  tflcEventReturnCode pflcEventReturnCode;
  pflcEventReturnCode = (tflcEventReturnCode)GetProcAddress( ghLibInst, "lcEventReturnCode" );
  if (pflcEventReturnCode){
    (*pflcEventReturnCode)(code);
  }
}


//--------------
void LCAPI lcEventsEnable (BOOL bEnable)
{
  tflcEventsEnable pflcEventsEnable;
  pflcEventsEnable = (tflcEventsEnable)GetProcAddress( ghLibInst, "lcEventsEnable" );
  if (pflcEventsEnable){
    (*pflcEventsEnable)(bEnable);
  }
}


//--------------
void LCAPI lcOnEventMouseMove (F_MOUSEMOVE pFunc)
{
  tflcOnEventMouseMove pflcOnEventMouseMove;
  pflcOnEventMouseMove = (tflcOnEventMouseMove)GetProcAddress( ghLibInst, "lcOnEventMouseMove" );
  if (pflcOnEventMouseMove){
    (*pflcOnEventMouseMove)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventMouseDown (F_MOUSEDOWN pFunc)
{
  tflcOnEventMouseDown pflcOnEventMouseDown;
  pflcOnEventMouseDown = (tflcOnEventMouseDown)GetProcAddress( ghLibInst, "lcOnEventMouseDown" );
  if (pflcOnEventMouseDown){
    (*pflcOnEventMouseDown)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventMouseUp (F_MOUSEUP pFunc)
{
  tflcOnEventMouseUp pflcOnEventMouseUp;
  pflcOnEventMouseUp = (tflcOnEventMouseUp)GetProcAddress( ghLibInst, "lcOnEventMouseUp" );
  if (pflcOnEventMouseUp){
    (*pflcOnEventMouseUp)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventMouseDblClk (F_MOUSEDBLCLK pFunc)
{
  tflcOnEventMouseDblClk pflcOnEventMouseDblClk;
  pflcOnEventMouseDblClk = (tflcOnEventMouseDblClk)GetProcAddress( ghLibInst, "lcOnEventMouseDblClk" );
  if (pflcOnEventMouseDblClk){
    (*pflcOnEventMouseDblClk)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventMouseSnap (F_MOUSESNAP pFunc)
{
  tflcOnEventMouseSnap pflcOnEventMouseSnap;
  pflcOnEventMouseSnap = (tflcOnEventMouseSnap)GetProcAddress( ghLibInst, "lcOnEventMouseSnap" );
  if (pflcOnEventMouseSnap){
    (*pflcOnEventMouseSnap)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventMouseLeave (F_MOUSELEAVE pFunc)
{
  tflcOnEventMouseLeave pflcOnEventMouseLeave;
  pflcOnEventMouseLeave = (tflcOnEventMouseLeave)GetProcAddress( ghLibInst, "lcOnEventMouseLeave" );
  if (pflcOnEventMouseLeave){
    (*pflcOnEventMouseLeave)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventMouseWheel (F_MOUSEWHEEL pFunc)
{
  tflcOnEventMouseWheel pflcOnEventMouseWheel;
  pflcOnEventMouseWheel = (tflcOnEventMouseWheel)GetProcAddress( ghLibInst, "lcOnEventMouseWheel" );
  if (pflcOnEventMouseWheel){
    (*pflcOnEventMouseWheel)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventKeyDown (F_KEYDOWN pFunc)
{
  tflcOnEventKeyDown pflcOnEventKeyDown;
  pflcOnEventKeyDown = (tflcOnEventKeyDown)GetProcAddress( ghLibInst, "lcOnEventKeyDown" );
  if (pflcOnEventKeyDown){
    (*pflcOnEventKeyDown)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventPaint (F_PAINT pFunc)
{
  tflcOnEventPaint pflcOnEventPaint;
  pflcOnEventPaint = (tflcOnEventPaint)GetProcAddress( ghLibInst, "lcOnEventPaint" );
  if (pflcOnEventPaint){
    (*pflcOnEventPaint)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventZoom (F_ZOOM pFunc)
{
  tflcOnEventZoom pflcOnEventZoom;
  pflcOnEventZoom = (tflcOnEventZoom)GetProcAddress( ghLibInst, "lcOnEventZoom" );
  if (pflcOnEventZoom){
    (*pflcOnEventZoom)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventSelectView (F_SELECTVIEW pFunc)
{
  tflcOnEventSelectView pflcOnEventSelectView;
  pflcOnEventSelectView = (tflcOnEventSelectView)GetProcAddress( ghLibInst, "lcOnEventSelectView" );
  if (pflcOnEventSelectView){
    (*pflcOnEventSelectView)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventGetPoint (F_GETPOINT pFunc)
{
  tflcOnEventGetPoint pflcOnEventGetPoint;
  pflcOnEventGetPoint = (tflcOnEventGetPoint)GetProcAddress( ghLibInst, "lcOnEventGetPoint" );
  if (pflcOnEventGetPoint){
    (*pflcOnEventGetPoint)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventAddCommand (F_ADDCOMMAND pFunc)
{
  tflcOnEventAddCommand pflcOnEventAddCommand;
  pflcOnEventAddCommand = (tflcOnEventAddCommand)GetProcAddress( ghLibInst, "lcOnEventAddCommand" );
  if (pflcOnEventAddCommand){
    (*pflcOnEventAddCommand)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventCmdStart (F_CMD_START pFunc)
{
  tflcOnEventCmdStart pflcOnEventCmdStart;
  pflcOnEventCmdStart = (tflcOnEventCmdStart)GetProcAddress( ghLibInst, "lcOnEventCmdStart" );
  if (pflcOnEventCmdStart){
    (*pflcOnEventCmdStart)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventCmdFinish (F_CMD_FINISH pFunc)
{
  tflcOnEventCmdFinish pflcOnEventCmdFinish;
  pflcOnEventCmdFinish = (tflcOnEventCmdFinish)GetProcAddress( ghLibInst, "lcOnEventCmdFinish" );
  if (pflcOnEventCmdFinish){
    (*pflcOnEventCmdFinish)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventCmdMouseDown (F_CMD_MOUSEDOWN pFunc)
{
  tflcOnEventCmdMouseDown pflcOnEventCmdMouseDown;
  pflcOnEventCmdMouseDown = (tflcOnEventCmdMouseDown)GetProcAddress( ghLibInst, "lcOnEventCmdMouseDown" );
  if (pflcOnEventCmdMouseDown){
    (*pflcOnEventCmdMouseDown)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventCmdMouseUp (F_CMD_MOUSEUP pFunc)
{
  tflcOnEventCmdMouseUp pflcOnEventCmdMouseUp;
  pflcOnEventCmdMouseUp = (tflcOnEventCmdMouseUp)GetProcAddress( ghLibInst, "lcOnEventCmdMouseUp" );
  if (pflcOnEventCmdMouseUp){
    (*pflcOnEventCmdMouseUp)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventCmdMouseMove (F_CMD_MOUSEMOVE pFunc)
{
  tflcOnEventCmdMouseMove pflcOnEventCmdMouseMove;
  pflcOnEventCmdMouseMove = (tflcOnEventCmdMouseMove)GetProcAddress( ghLibInst, "lcOnEventCmdMouseMove" );
  if (pflcOnEventCmdMouseMove){
    (*pflcOnEventCmdMouseMove)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventCmdString (F_CMD_STRING pFunc)
{
  tflcOnEventCmdString pflcOnEventCmdString;
  pflcOnEventCmdString = (tflcOnEventCmdString)GetProcAddress( ghLibInst, "lcOnEventCmdString" );
  if (pflcOnEventCmdString){
    (*pflcOnEventCmdString)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventAddEntity (F_ADDENTITY pFunc)
{
  tflcOnEventAddEntity pflcOnEventAddEntity;
  pflcOnEventAddEntity = (tflcOnEventAddEntity)GetProcAddress( ghLibInst, "lcOnEventAddEntity" );
  if (pflcOnEventAddEntity){
    (*pflcOnEventAddEntity)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventRegen (F_REGEN pFunc)
{
  tflcOnEventRegen pflcOnEventRegen;
  pflcOnEventRegen = (tflcOnEventRegen)GetProcAddress( ghLibInst, "lcOnEventRegen" );
  if (pflcOnEventRegen){
    (*pflcOnEventRegen)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventFioProgress (F_FIOPROGRESS pFunc)
{
  tflcOnEventFioProgress pflcOnEventFioProgress;
  pflcOnEventFioProgress = (tflcOnEventFioProgress)GetProcAddress( ghLibInst, "lcOnEventFioProgress" );
  if (pflcOnEventFioProgress){
    (*pflcOnEventFioProgress)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventSelection (F_SELECTION pFunc)
{
  tflcOnEventSelection pflcOnEventSelection;
  pflcOnEventSelection = (tflcOnEventSelection)GetProcAddress( ghLibInst, "lcOnEventSelection" );
  if (pflcOnEventSelection){
    (*pflcOnEventSelection)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventPropChanged (F_PROPCHANGED pFunc)
{
  tflcOnEventPropChanged pflcOnEventPropChanged;
  pflcOnEventPropChanged = (tflcOnEventPropChanged)GetProcAddress( ghLibInst, "lcOnEventPropChanged" );
  if (pflcOnEventPropChanged){
    (*pflcOnEventPropChanged)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventGripSelect (F_GRIPSELECT pFunc)
{
  tflcOnEventGripSelect pflcOnEventGripSelect;
  pflcOnEventGripSelect = (tflcOnEventGripSelect)GetProcAddress( ghLibInst, "lcOnEventGripSelect" );
  if (pflcOnEventGripSelect){
    (*pflcOnEventGripSelect)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventGripMove (F_GRIPMOVE pFunc)
{
  tflcOnEventGripMove pflcOnEventGripMove;
  pflcOnEventGripMove = (tflcOnEventGripMove)GetProcAddress( ghLibInst, "lcOnEventGripMove" );
  if (pflcOnEventGripMove){
    (*pflcOnEventGripMove)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventEntErase (F_ENTERASE pFunc)
{
  tflcOnEventEntErase pflcOnEventEntErase;
  pflcOnEventEntErase = (tflcOnEventEntErase)GetProcAddress( ghLibInst, "lcOnEventEntErase" );
  if (pflcOnEventEntErase){
    (*pflcOnEventEntErase)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventEntMove (F_ENTMOVE pFunc)
{
  tflcOnEventEntMove pflcOnEventEntMove;
  pflcOnEventEntMove = (tflcOnEventEntMove)GetProcAddress( ghLibInst, "lcOnEventEntMove" );
  if (pflcOnEventEntMove){
    (*pflcOnEventEntMove)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventEntRotate (F_ENTROTATE pFunc)
{
  tflcOnEventEntRotate pflcOnEventEntRotate;
  pflcOnEventEntRotate = (tflcOnEventEntRotate)GetProcAddress( ghLibInst, "lcOnEventEntRotate" );
  if (pflcOnEventEntRotate){
    (*pflcOnEventEntRotate)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventEntScale (F_ENTSCALE pFunc)
{
  tflcOnEventEntScale pflcOnEventEntScale;
  pflcOnEventEntScale = (tflcOnEventEntScale)GetProcAddress( ghLibInst, "lcOnEventEntScale" );
  if (pflcOnEventEntScale){
    (*pflcOnEventEntScale)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventEntMirror (F_ENTMIRROR pFunc)
{
  tflcOnEventEntMirror pflcOnEventEntMirror;
  pflcOnEventEntMirror = (tflcOnEventEntMirror)GetProcAddress( ghLibInst, "lcOnEventEntMirror" );
  if (pflcOnEventEntMirror){
    (*pflcOnEventEntMirror)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventEntProp (F_ENTPROP pFunc)
{
  tflcOnEventEntProp pflcOnEventEntProp;
  pflcOnEventEntProp = (tflcOnEventEntProp)GetProcAddress( ghLibInst, "lcOnEventEntProp" );
  if (pflcOnEventEntProp){
    (*pflcOnEventEntProp)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventHelp (F_HELP pFunc)
{
  tflcOnEventHelp pflcOnEventHelp;
  pflcOnEventHelp = (tflcOnEventHelp)GetProcAddress( ghLibInst, "lcOnEventHelp" );
  if (pflcOnEventHelp){
    (*pflcOnEventHelp)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventAddStr (F_ADDSTR pFunc)
{
  tflcOnEventAddStr pflcOnEventAddStr;
  pflcOnEventAddStr = (tflcOnEventAddStr)GetProcAddress( ghLibInst, "lcOnEventAddStr" );
  if (pflcOnEventAddStr){
    (*pflcOnEventAddStr)(pFunc);
  }
}


//--------------
void LCAPI lcOnEventFontReplace (F_FONTREPLACE pFunc)
{
  tflcOnEventFontReplace pflcOnEventFontReplace;
  pflcOnEventFontReplace = (tflcOnEventFontReplace)GetProcAddress( ghLibInst, "lcOnEventFontReplace" );
  if (pflcOnEventFontReplace){
    (*pflcOnEventFontReplace)(pFunc);
  }
}

