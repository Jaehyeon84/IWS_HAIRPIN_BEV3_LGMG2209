#include "stdafx.h"
#include "CGridDIO.h"


CGridDIO::CGridDIO()
{
//#if (LASER_SOURCE == _SPI_LASER)
//	m_strArrInputname.Add(L"Alarm");
//	m_strArrInputname.Add(L"Laser Temp.");
//	m_strArrInputname.Add(L"System Fault");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"Laser On");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//
//	m_strArrOutputname.Add(L"Waveform0");
//	m_strArrOutputname.Add(L"Waveform1");
//	m_strArrOutputname.Add(L"Waveform2");
//	m_strArrOutputname.Add(L"Waveform3");
//	m_strArrOutputname.Add(L"Waveform4");
//	m_strArrOutputname.Add(L"Waveform5");
//	m_strArrOutputname.Add(L"Waveform6");
//	m_strArrOutputname.Add(L"Waveform7");
//	m_strArrOutputname.Add(L"Laser Enable");
//	m_strArrOutputname.Add(L"Laser Disable");
//	m_strArrOutputname.Add(L"Guide Beam");
//	m_strArrOutputname.Add(L"LaserPulse CW");
//#elif (LASER_SOURCE == _TRUMF_LASER)
//	m_strArrInputname.Add(L"Error");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"Laser On");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"Emission On");
//	m_strArrInputname.Add(L"-");		// �߰�
//	m_strArrInputname.Add(L"-");		// �߰�
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	//m_strArrInputname.Add(L"Sik Open");			// ����
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	//m_strArrInputname.Add(L"-");					// ����
//
//	m_strArrOutputname.Add(L"Reset");
//	m_strArrOutputname.Add(L"Ext Activation");
//	m_strArrOutputname.Add(L"Laser On");
//	m_strArrOutputname.Add(L"Request");
//	m_strArrOutputname.Add(L"Pilot");
//	m_strArrOutputname.Add(L"Emission Enable");
//	//m_strArrOutputname.Add(L"Laser Standby");		// ����
//	m_strArrOutputname.Add(L"Pgm. Bit No.0");
//	m_strArrOutputname.Add(L"Pgm. Bit No.1");
//	m_strArrOutputname.Add(L"-");
//	//m_strArrOutputname.Add(L"Pilot Laser");		// ����
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");					// �߰�
//	m_strArrOutputname.Add(L"-");					// �߰�
//
//	//m_strArrInputname.Add(L"Pgm. Complete");
//	//m_strArrInputname.Add(L"Fault Laser");
//	//m_strArrInputname.Add(L"Laser Ready");
//	//m_strArrInputname.Add(L"Pgm. Active");
//	//m_strArrInputname.Add(L"Monitoring");
//	//m_strArrInputname.Add(L"Lgt Path:Bit0");		// �߰�
//	//m_strArrInputname.Add(L"Lgt Path:Bit1");		// �߰�
//	//m_strArrInputname.Add(L"Laser Is On");
//	//m_strArrInputname.Add(L"Ext Activation");
//	////m_strArrInputname.Add(L"Sik Open");			// ����
//	//m_strArrInputname.Add(L"Laser Assigned");
//	//m_strArrInputname.Add(L"-");
//	//m_strArrInputname.Add(L"-");
//	////m_strArrInputname.Add(L"-");					// ����
//
//	//m_strArrOutputname.Add(L"Ext Activation");
//	//m_strArrOutputname.Add(L"Reset");
//	//m_strArrOutputname.Add(L"Laser On");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.0");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.1");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.2");
//	////m_strArrOutputname.Add(L"Laser Standby");		// ����
//	//m_strArrOutputname.Add(L"PStart Statical");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.3");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.4");
//	////m_strArrOutputname.Add(L"Pilot Laser");		// ����
//	//m_strArrOutputname.Add(L"-");
//	//m_strArrOutputname.Add(L"-");					// �߰�
//	//m_strArrOutputname.Add(L"-");					// �߰�
//	//m_strArrInputname.Add(L"Pgm. Complete");
//	//m_strArrInputname.Add(L"Fault Laser");
//	//m_strArrInputname.Add(L"Laser Ready");
//	//m_strArrInputname.Add(L"Pgm. Active");
//	//m_strArrInputname.Add(L"Monitering");
//	//m_strArrInputname.Add(L"Laser On");
//	//m_strArrInputname.Add(L"Ext Activation");
//	//m_strArrInputname.Add(L"Sik Open");
//	//m_strArrInputname.Add(L"Laser Assigned");
//	//m_strArrInputname.Add(L"-");
//	//m_strArrInputname.Add(L"-");
//	//m_strArrInputname.Add(L"-");
//
//	//m_strArrOutputname.Add(L"Ext Activation");
//	//m_strArrOutputname.Add(L"Reset");
//	//m_strArrOutputname.Add(L"Laser On");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.0");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.1");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.2");
//	//m_strArrOutputname.Add(L"Laser Standby");
//	//m_strArrOutputname.Add(L"PStart Statical");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.3");
//	//m_strArrOutputname.Add(L"Pgm. Bit No.4");
//	//m_strArrOutputname.Add(L"Pilot Laser");
//	//m_strArrOutputname.Add(L"-");
//#elif (LASER_SOURCE == _IPG_LASER)
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"Ready");
//	m_strArrInputname.Add(L"Power On");
//	m_strArrInputname.Add(L"Power Active");
//	m_strArrInputname.Add(L"Emission On");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"Err. Reset");
//	m_strArrOutputname.Add(L"Emission Enable");
//	m_strArrOutputname.Add(L"Guide control");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//#elif(LASER_SOURCE == _IPG_SINGLE)
//	m_strArrInputname.Add(L"Error/Ready");
//	m_strArrInputname.Add(L"Power On");
//	m_strArrInputname.Add(L"Power Active");
//	m_strArrInputname.Add(L"Emission On");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"#2 QCW On");
//	m_strArrInputname.Add(L"Error/Ready");
//	m_strArrInputname.Add(L"Power On");
//	m_strArrInputname.Add(L"Pulse mode");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//	m_strArrInputname.Add(L"-");
//
//	m_strArrOutputname.Add(L"Error Reset");
//	m_strArrOutputname.Add(L"Emission Enable");
//	m_strArrOutputname.Add(L"Guide Control");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"Power Supply On");
//	m_strArrOutputname.Add(L"Error Reset");
//	m_strArrOutputname.Add(L"Emission On");
//	m_strArrOutputname.Add(L"Guide Control");
//	m_strArrOutputname.Add(L"Pulse mode On");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//	m_strArrOutputname.Add(L"-");
//#endif // _TRUMF_LASER
	m_errDI = -1;
	m_errDO = -1;
}


CGridDIO::~CGridDIO()
{
}

COLORREF CGridDIO::OnGetDefBackColor(int section)
{
	switch (section)
	{
	case UG_TOPHEADING:
		return RGB(0x38, 0x38, 0x38);
		break;
	case UG_SIDEHEADING:
		return RGB(0x38, 0x38, 0x38);
		break;
	case UG_GRID:
		return RGB(0x38, 0x38, 0x38);
		break;
	}
	return RGB(0x00, 0x00, 0x00);
}

void CGridDIO::OnSetup()
{
	SetNumberRows(MAX_GRID_DIO);
	SetNumberCols(2);
	CUGCell cell;
	GetHeadingDefault(&cell);
	cell.SetBackColor(RGB(0x4D, 0x4D, 0x4D));
	cell.SetBorder(0);
	cell.SetTextColor(RGB(0x99, 0x99, 0x99));
	SetVScrollMode(UG_SCROLLNORMAL);
	SetCurrentCellMode(1);
	this->HideCurrentCell();
	SetHighlightRow(TRUE);

	SetHeadingDefault(&cell);
	QuickSetText(-1, -1, _T("NO"));
	QuickSetText(0, -1, _T("Input"));
	QuickSetText(1, -1, _T("Output"));
	CString str;
	for (int i = 0; i < MAX_GRID_DIO;i++) {
		str.Format(L"%d", i);
		QuickSetText(-1, i, str);
		SetRowHeight(i, 20);
	}
	
	GetGridDefault(&cell);
	cell.SetBackColor(RGB(0x38, 0x38, 0x38));
	cell.SetTextColor(RGB(0xCC, 0xCC, 0xCC));
	SetGridDefault(&cell);

	for (int i = 0; i < m_strArrInputname.GetCount(); i++) {
		SetCell(0, i, &cell);
		SetCell(1, i, &cell);
		QuickSetText(0, i, m_strArrInputname[i]);
		QuickSetText(1, i, m_strArrOutputname[i]);
		SetRowHeight(i, 20);
	}

	CRect rc;
	GetClientRect(rc);
	int iw = int(double(rc.Width())*(1.0 - 0.1) / 2.0);// MAX_GRID_DIO);
	int in = rc.Width() - iw * 2;// MAX_GRID_DIO;
	this->SetColWidth(-1, in);
	this->SetColWidth(0, iw);
	this->SetColWidth(1, iw);
}

void CGridDIO::OnDClicked(int col, long row, RECT * rect, POINT * point, BOOL processed)
{
}

void CGridDIO::OnLClicked(int col, long row, int updn, RECT * rect, POINT * point, int processed)
{
}

void CGridDIO::OnKeyDown(UINT * vcKey, int processed)
{
}

BEGIN_MESSAGE_MAP(CGridDIO, CUGCtrl)
	ON_WM_SIZE()
END_MESSAGE_MAP()


void CGridDIO::OnSize(UINT nType, int cx, int cy)
{
	CUGCtrl::OnSize(nType, cx, cy);
	int iw = int((double(cx)*(1.0 - 0.02) / MAX_GRID_DIO));
	int in = cx - iw * MAX_GRID_DIO;
	this->SetColWidth(-1, in);
	for (int i = 0; i < MAX_GRID_DIO; i++)
		this->SetColWidth(i, iw);
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CGridDIO::SetInOut(int in, int out)
{
	CUGCell cellOn, cellOff, cellErr;
	GetGridDefault(&cellOn);
	GetGridDefault(&cellOff);
	GetGridDefault(&cellErr);
	cellOn.SetTextColor(COLOR_OLIVEDRAB1);
	cellOff.SetTextColor(RGB(0, 128, 0));
	cellErr.SetTextColor(RGB(255, 0, 0));
	for (int i = 0; i < m_numDI; i++) 
	{
#if ( LASER_SOURCE==_TRUMF_LASER)
		if (check_bit(in, i)) {
			if (i == m_errDI)
				SetCell(0, i, &cellErr);
			else
				SetCell(0, i, &cellOn);
		}		
		else {
				SetCell(0, i, &cellOff);
		}
#else
		if (check_bit(in, i))
			SetCell(0, i, &cellOn); 
		else {

			if (i == m_errDI)
				SetCell(0, i, &cellErr);
			else
				SetCell(0, i, &cellOff);

		}
#endif

			
	}
	for (int i = 0; i < m_numDO; i++) {

		if (check_bit(out, i))
			SetCell(1, i, &cellOn);
		else {			
			if (i == m_errDO)
				SetCell(1, i, &cellErr);
			else
				SetCell(1, i, &cellOff);		
		}
			
	}
	for (int i = 0; i < m_strArrInputname.GetCount(); i++) {
		QuickSetText(0, i, m_strArrInputname[i]);
		QuickSetText(1, i, m_strArrOutputname[i]);
		SetRowHeight(i, 20);
	}
	this->RedrawAll();
}

