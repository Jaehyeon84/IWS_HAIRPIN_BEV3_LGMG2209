#pragma once
#include <afxwin.h>
class CColorComboBox :
	public CComboBox
{
public:
	CColorComboBox();
	~CColorComboBox();
	
	COLORREF GetSelectedColourValue();
	void AddStringC(CString str, COLORREF clr);
	virtual void PreSubclassWindow();


private:
	void Initialise();
public:
	virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
	DECLARE_MESSAGE_MAP()
/*	afx_msg void OnCbnSelchange();*/
};

