
// GPSLocationDlg.h : header file
//

#pragma once

#include <atlcomcli.h>
#include <LocationAPI.h>
#pragma comment(lib,"LocationAPI.lib")
#pragma warning(disable:4995)

// CGPSLocationDlg dialog
class CGPSLocationDlg : public CDialogEx
{
// Construction
public:
	CGPSLocationDlg(CWnd* pParent = nullptr);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GPSLOCATION_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;
	LOCATION_REPORT_STATUS status;
	CComPtr<ILocation> spLoc; // This is the main Location interface
	double lfPrevLat, lfPrevLng;
	double lfThisLat, lfThisLng;
	double lfDistance;
	CString sPrevPos, sStatus, sThisPos, sDistance;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	void SetStatusString();
	double CalcDistance();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
