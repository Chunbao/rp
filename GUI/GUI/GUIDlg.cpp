
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
    DDX_Control(pDX, IDC_EDIT1, textEditor);
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

void mouseMove(int x, int y)
{
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
    double fx = x*(65535.0f / fScreenWidth);
    double fy = y*(65535.0f / fScreenHeight);
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1, &Input, sizeof(INPUT));
}

void leftButtonClick(int x, int y)
{
    double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
    double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
    double fx = x*(65535.0f / fScreenWidth);
    double fy = y*(65535.0f / fScreenHeight);
    INPUT  Input = { 0 };
    Input.type = INPUT_MOUSE;
    Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
    Input.mi.dx = fx;
    Input.mi.dy = fy;
    ::SendInput(1, &Input, sizeof(INPUT));
}

void keyboardSendBackspaceKey()
{
    INPUT input_down = { 0 };
    input_down.type = INPUT_KEYBOARD;
    input_down.ki.dwFlags = 0;
    input_down.ki.wScan = 0;
    input_down.ki.wVk = VK_BACK;
    SendInput(1, &input_down, sizeof(input_down));//keydown     
    INPUT input_up = { 0 };
    input_up.type = INPUT_KEYBOARD;
    input_up.ki.wScan = 0;
    input_up.ki.wVk = VK_BACK;
    input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP);
    SendInput(1, &input_up, sizeof(input_up));//keyup 
}

void keyboardSendDeleteKey()
{
    INPUT input_down = { 0 };
    input_down.type = INPUT_KEYBOARD;
    input_down.ki.dwFlags = 0;
    input_down.ki.wScan = 0;
    input_down.ki.wVk = VK_DELETE;
    SendInput(1, &input_down, sizeof(input_down));//keydown     
    INPUT input_up = { 0 };
    input_up.type = INPUT_KEYBOARD;
    input_up.ki.wScan = 0;
    input_up.ki.wVk = VK_DELETE;
    input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP);
    SendInput(1, &input_up, sizeof(input_up));//keyup 
}


#include <string>
void keyboardSendUnicodeInput(std::string message)
{
    for (int i = 0; i < message.size(); i++)
    {
        INPUT input_down = { 0 };
        input_down.type = INPUT_KEYBOARD;
        input_down.ki.dwFlags = KEYEVENTF_UNICODE;
        input_down.ki.wScan = (short)message.at(i);
        input_down.ki.wVk = 0;
        SendInput(1, &input_down, sizeof(input_down));//keydown     
        INPUT input_up = { 0 };
        input_up.type = INPUT_KEYBOARD;
        input_up.ki.wScan = (short)message.at(i);
        input_up.ki.wVk = 0;
        input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP | KEYEVENTF_UNICODE);
        SendInput(1, &input_up, sizeof(input_up));//keyup      
    }
}

BOOL CGUIDlg::PreTranslateMessage(MSG* pMsg)
{
#if 0
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
            //move the curse to position //perform click
            RECT rect;
            GetWindowRect(&rect);

            //  @todo, !!!!!! provide inputs
            //mouseMove(rect.left + 168, rect.top + 88);

            // @todo, good to have, check number pad is on
            leftButtonClick(rect.left + 168, rect.top + 88);
 
            // error handling, might not needed
            //for (size_t i = 0; i < 50; i++)
            //{
            //    keyboardSendBackspaceKey();
            //    keyboardSendDeleteKey();
            //}

            //another solution is stopping user input anything to that field

            keyboardSendUnicodeInput("85900");
            leftButtonClick(rect.left + 359, rect.top + 330);
            return true;
        }
	}
#endif
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