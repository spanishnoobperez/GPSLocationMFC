
// GPSLocationDlg.cpp : implementation file
//

#include "pch.h"
#include "framework.h"
#include "GPSLocation.h"
#include "GPSLocationDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnTimer(UINT_PTR nIDEvent);
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
//	ON_WM_TIMER()
END_MESSAGE_MAP()


// CGPSLocationDlg dialog



CGPSLocationDlg::CGPSLocationDlg(CWnd* pParent /*=nullptr*/)
	: CDialogEx(IDD_GPSLOCATION_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGPSLocationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_STATUS, sStatus);
	DDX_Text(pDX, IDC_PREVPOS, sPrevPos);
	DDX_Text(pDX, IDC_THISPOS, sThisPos);
	DDX_Text(pDX, IDC_DISTANCE, sDistance);
}

BEGIN_MESSAGE_MAP(CGPSLocationDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDOK, &CGPSLocationDlg::OnBnClickedOk)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CGPSLocationDlg message handlers

void CGPSLocationDlg::SetStatusString() {
	switch (status) // If there is an error, print the error
	{
	case REPORT_RUNNING:
		sStatus = "Report received okay";
		break;
	case REPORT_NOT_SUPPORTED:
		sStatus = "No devices detected.";
		break;
	case REPORT_ERROR:
		sStatus = "Report error.";
		break;
	case REPORT_ACCESS_DENIED:
		sStatus = "Access denied to reports.";
		break;
	case REPORT_INITIALIZING:
		sStatus = "Report is initializing.";
		break;
	}
}

BOOL CGPSLocationDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	// Our code starts here
	lfDistance = 0.0;

	// Create the Location object
	status = REPORT_NOT_SUPPORTED;
	if (SUCCEEDED(spLoc.CoCreateInstance(CLSID_Location)))
	{
		// Array of report types of interest.
		  // Civic addresses/etc also supported
		IID REPORT_TYPES[] = { IID_ILatLongReport };

		// Request permissions for this user account to receive location
		// data for all the types defined in REPORT_TYPES
		// The final parameter (TRUE) indicates a synchronous request
		// use FALSE to request asynchronous calls
		if (FAILED(spLoc->RequestPermissions(NULL, REPORT_TYPES, ARRAYSIZE(REPORT_TYPES), TRUE)))
		{
			AfxMessageBox(L"Warning: Unable to request permissions.\n");
		}

		// Get the report status
		if (SUCCEEDED(spLoc->GetReportStatus(IID_ILatLongReport, &status)))
		{
			// Map status enum to a text label for display
			SetStatusString();

			// Next define our report objects
			// These store the reports we request from spLoc
			CComPtr<ILocationReport> spLocationReport;
			CComPtr<ILatLongReport> spLatLongReport;

			// Get the current location report (ILocationReport) and
			// then get a ILatLongReport from it
			// Check they are are reported okay and not null
			if ((SUCCEEDED(spLoc->GetReport(IID_ILatLongReport, &spLocationReport)))
				&& (SUCCEEDED(spLocationReport->QueryInterface(&spLatLongReport))))
			{
				lfThisLat = 0;
				lfThisLng = 0;

				// Fetch the latitude & longitude
				spLatLongReport->GetLatitude(&lfThisLat);
				spLatLongReport->GetLongitude(&lfThisLng);

				// Format them into a label for display
				sPrevPos.Format(L"Lat: %.6f,  Lng:%.6f", lfThisLat, lfThisLng);

				// set some sensible initial values
				lfPrevLat = lfThisLat;
				lfPrevLng = lfThisLng;
				sDistance = "0.0";
			}

		}

		// We have completed our call
		// but keep the spLoc pointer for future use
		// Start the timer (Timer #1) with a 500ms (0.5s) interval
		SetTimer(1, 500, 0);

	}

	// Update labels with their new information
	UpdateData(FALSE);

	if (status == REPORT_NOT_SUPPORTED) {
		AfxMessageBox(L"¡Su dispositivo no es compatible con GPS!");
		PostQuitMessage(0);
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGPSLocationDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGPSLocationDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGPSLocationDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPSLocationDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	KillTimer(1);
	CDialogEx::OnOK();
}

#define DEG2RAD (3.14159265 / 180.0)
#define RAD2DEG (180.0 / 3.14159265)
#define RAD2NM  ( RAD2DEG * 60.0 )
#define RAD2KM  ( RAD2NM * 1.852 )

double CGPSLocationDlg::CalcDistance()
{
	double lf = sin(lfPrevLat * DEG2RAD) * sin(lfThisLat * DEG2RAD);
	lf += cos(lfPrevLat * DEG2RAD) * cos(lfThisLat * DEG2RAD) * cos((lfPrevLng - lfThisLng) * DEG2RAD);
	double d = acos(lf) * RAD2KM;	// distance in kilometers
	return (d > 0.001) ? d : 0.0;  // only return distance if >1m
}

void CGPSLocationDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (SUCCEEDED(spLoc->GetReportStatus(IID_ILatLongReport, &status)))
	{
		SetStatusString();

		CComPtr<ILocationReport> spLocationReport;
		CComPtr<ILatLongReport> spLatLongReport;

		if ((SUCCEEDED(spLoc->GetReport(IID_ILatLongReport, &spLocationReport)))
			&& (SUCCEEDED(spLocationReport->QueryInterface(&spLatLongReport))))
		{
			lfPrevLat = lfThisLat;
			lfPrevLng = lfThisLng;

			// Fetch the new latitude & longitude
			spLatLongReport->GetLatitude(&lfThisLat);
			spLatLongReport->GetLongitude(&lfThisLng);

			// Format coords into a label for display
			sThisPos.Format(L"Lat: %.6f,  Lng:%.6f", lfThisLat, lfThisLng);
			sPrevPos.Format(L"Lat: %.6f,  Lng:%.6f", lfPrevLat, lfPrevLng);

			// Calculate the new cumulative distance, and update label
			lfDistance += CalcDistance();
			sDistance.Format(L"%.3f km", lfDistance);

			// update the display
			UpdateData(FALSE);
		}
	}
	CDialogEx::OnTimer(nIDEvent);
}
