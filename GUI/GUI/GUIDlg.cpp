
// GUIDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GUI.h"
#include "GUIDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CGUIDlg dialog




CGUIDlg::CGUIDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGUIDlg::IDD, pParent)
	, flag(TRUE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGUIDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGUIDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CGUIDlg message handlers

BOOL CGUIDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGUIDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGUIDlg::OnPaint()
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
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGUIDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL CGUIDlg::PreTranslateMessage(MSG* pMsg)
{
    
    if (pMsg->message == WM_KEYDOWN)
	{
        if (pMsg->wParam == VK_F5)
        {
            flag = !flag;
            //pMsg->wParam = VK_TAB;
            //m_pBrowserMy.Refresh();
            flag ? SetWindowText(L"A") : SetWindowText(L"B");
            return true;
        }
        if (pMsg->wParam == VK_RETURN)
        {
            //move the curse to position
            m_hWnd;
            RECT rect;
            GetWindowRect(&rect);
            int x = rect.left + 357;
            int y = rect.top + 325;
            //SentMessage ( m_hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(357, 325) );
            //SentMessage ( m_hWnd, WM_LBUTTONUP, 0, MAKELPARAM(357, 325) );
            
            INPUT input;
            input.type=INPUT_MOUSE;
            input.mi.dx=x;
            input.mi.dy=y;
            input.mi.dwFlags=(MOUSEEVENTF_ABSOLUTE|MOUSEEVENTF_MOVE|MOUSEEVENTF_LEFTDOWN|MOUSEEVENTF_LEFTUP);
            input.mi.mouseData=0;
            input.mi.dwExtraInfo=NULL;
            input.mi.time=0;
            SendInput(1,&input,sizeof(INPUT));
            
            //perform click
        }
	}
	
    if (BN_CLICKED == pMsg->wParam)
    {
        RECT rect;
        GetWindowRect(&rect);
        LONG horizonCoordinate = rect.left;
        LONG verticalCoordinate = rect.top;
        LONG RELATIVE_HORIZON_DISTANCE = pMsg->pt.x - horizonCoordinate;
        LONG RELATIVE_VERTICAL_DISTANCE = pMsg->pt.y - verticalCoordinate;

        wchar_t coordinates[100];
        wsprintf(coordinates, L"%d, %d", RELATIVE_HORIZON_DISTANCE, RELATIVE_VERTICAL_DISTANCE);
        //infoPanelEditor.SetWindowTextA(coordinates);
        SetWindowText(coordinates);
    }
	
    return CDialog::PreTranslateMessage(pMsg);
}


// Mouse click event
// http://stackoverflow.com/questions/5789843/how-i-can-simulate-a-double-mouse-click-on-window-i-khow-handle-on-x-y-coord
// http://www.cplusplus.com/forum/lounge/17053/

//sentInput  GOOD
//sendmessage
//postmessage