/*
 *  HistoryEdit.cpp
 *
 *  Description:
 *    CHistoryEdit implementation
 *
 *    A CEdit subclass that allows you to display a text history
 *    of events.
 *
 *  Author:
 *    Ravi Bhavnani (ravib@datablast.net)
 *
 *  Revision History:
 *    15 Mar 1998   rab   Original version
 */

#include "stdafx.h"
#include "HistoryEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistoryEdit

CHistoryEdit::CHistoryEdit()
{
  m_bSelectable = FALSE;
}

CHistoryEdit::~CHistoryEdit()
{
}

BEGIN_MESSAGE_MAP(CHistoryEdit, CEdit)
	//{{AFX_MSG_MAP(CHistoryEdit)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(UWM_TRACE_MSG, &CHistoryEdit::OnUwmTraceMsg)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistoryEdit operations

void CHistoryEdit::AppendString
  (CString str)
//
//  Purpose:
//    Appends a text string to the history buffer.
//
//  Returns:
//    None.
//
{
CString   strBuffer;    // current contents of edit control

  // Append string
  GetWindowText (strBuffer);
  if (!strBuffer.IsEmpty())
     strBuffer += L"\r\n";
  strBuffer += str;
  SetWindowText (strBuffer);

  int nLineCnt = GetLineCount();
  // Scroll the edit control
  LineScroll (nLineCnt, 0);
  if (nLineCnt > 1000)
	  SetWindowText(L"");
}

/////////////////////////////////////////////////////////////////////////////
// CHistoryEdit message handlers

void CHistoryEdit::OnSetFocus(CWnd* pOldWnd) 
{
  // Don't allow user to select text
  if (m_bSelectable)
     CEdit::OnSetFocus (pOldWnd);
  else
     pOldWnd->SetFocus();
}

// End EditHistroy.cpp

afx_msg LRESULT CHistoryEdit::OnUwmTraceMsg(WPARAM wParam, LPARAM lParam)
{
	TRACE_DATA slog = *((TRACE_DATA*)lParam);
	ReplyMessage(0);
	CString str = CString(slog.sMsg.c_str());
	CString strLog;
	strLog.Format(L"[%d:%d:%d.%d]%s", slog.Time.wHour, slog.Time.wMinute,
		slog.Time.wSecond, slog.Time.wMilliseconds, str);
	AppendString(strLog);
	return 0;
}
