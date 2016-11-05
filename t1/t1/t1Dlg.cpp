
// t1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "t1.h"
#include "t1Dlg.h"
#include "ImgUtil.h"

#include <Windows.h>
#include <iostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//FOR OCR
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <memory>
#include <cstring>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#if defined _WIN32 || defined _WIN64
#include <windows.h>
#endif

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2\imgproc\imgproc_c.h>

//using namespace std;
//using namespace cv;

//price
//const int RELATIVE_LEFT = 750;
//const int RELATIVE_TOP = 335;
//const int RELATIVE_RIGHT = 809;
//const int RELATIVE_BOTTOM = 357;

//backup price
const int RELATIVE_LEFT = 267;
const int RELATIVE_TOP = 480;
const int RELATIVE_RIGHT = 314;
const int RELATIVE_BOTTOM = 495;


//@todo, need to be replaced for compatiable problems
//@todo, use getWindowRect instead of getClientRect in Image

// @todo, fix offset caused by dialog title and frame area (width=8,height=30)
// @todo, retrived from this function -> GetSystemMetrics
const int DIALOG_FRAME_LEFT_WIDTH = 8;
const int DIALOG_FRAME_TOP_HEIGHT = 30;

const cv::Point PRICE_INPUT(792, 491);
const cv::Point PRICE_CONFIRM(910, 492);

const std::string     BUTTON_OK_FILE("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\ok.png");
const std::string BUTTON_CANCEL_FILE("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\cancel.png");
const std::string DIALOG_CAPTURE_TMP("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\Capture_tmp.jpg");
//@todo, make it im memory
//@todo, support Chinese
//const std::string BUTTON_OK_FILE("D:\\workspace\\test\\bmatch\\文件夹\\GUIXX\\ok.png");
//const std::string BUTTON_CANCEL_FILE("D:\\workspace\\test\\bmatch\\文件夹\\GUIXX\\cancel.png");
//const std::string DIALOG_CAPTURE_TMP("D:\\workspace\\test\\bmatch\\文件夹\\GUIXX\\Capture_tmp.jpg");



void GetScreenShot(int absoluteLeft, int absoluteTop, int relativeWidth, int relativeHeight)
{
	int x1, y1, x2, y2, w, h;

	// get screen dimensions
	x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
	y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
	x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
	y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
	//w = x2 - x1;
	//h = y2 - y1;
	w = relativeWidth;
	h = relativeHeight;

	// copy screen to bitmap
	HDC     hScreen = GetDC(NULL);
	HDC     hDC = CreateCompatibleDC(hScreen);
	HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
	HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
	BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, absoluteLeft, absoluteTop, SRCCOPY);

	// save bitmap to clipboard
	OpenClipboard(NULL);
	EmptyClipboard();
	SetClipboardData(CF_BITMAP, hBitmap);
	CloseClipboard();

	// clean up
	SelectObject(hDC, old_obj);
	DeleteDC(hDC);
	ReleaseDC(NULL, hScreen);
	DeleteObject(hBitmap);
}


void GetScreenShotDialog(HDC h1) // h1 will be changed
{
    int x1, y1, x2, y2, w, h;

    // get screen dimensions
    x1 = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1 = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2 = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2 = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    
    RECT rect;
    GetWindowRect(WindowFromDC(h1), &rect);
    //w = x2 - x1;
    //h = y2 - y1;
    w = rect.right - rect.left;
    h = rect.bottom - rect.top;

    // copy screen to bitmap
    //HDC     hScreen = GetDC(NULL);
    HDC     hScreen = h1;
    HDC     hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, -DIALOG_FRAME_LEFT_WIDTH, -DIALOG_FRAME_TOP_HEIGHT, SRCCOPY);

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}


namespace IPT { // Virtual input of mouse of keyboard
    // Mouse click event
    // http://stackoverflow.com/questions/5789843/how-i-can-simulate-a-double-mouse-click-on-window-i-khow-handle-on-x-y-coord
    // http://www.cplusplus.com/forum/lounge/17053/
    //sentInput
    //sendmessage
    //postmessage
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

    void keyboardSendUnicodeInput(std::string message)
    {
        for (int i = 0; i < message.size()-2; i++)
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

}

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


// Ct1Dlg dialog

BEGIN_DHTML_EVENT_MAP(Ct1Dlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
	DHTML_EVENT_ONCLICK(_T("ButtonHelp"), OnButtonHelp)
END_DHTML_EVENT_MAP()



Ct1Dlg::Ct1Dlg(CWnd* pParent /*=NULL*/)
	: CDHtmlDialog(Ct1Dlg::IDD, Ct1Dlg::IDH, pParent)
    , m_stateMachine(STATE_NONE)
    , m_bidPrice(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ct1Dlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EXPLORER1, m_pBrowserMy);
	DDX_Control(pDX, IDC_EDIT1, editorMy);
	DDX_Control(pDX, IDC_EDIT2, infoPanelEditor);
}

BEGIN_MESSAGE_MAP(Ct1Dlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_Refresh, &Ct1Dlg::OnBnClickedButtonRefresh)
END_MESSAGE_MAP()


// Ct1Dlg message handlers

BOOL Ct1Dlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

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
/*
	// TODO: Add extra initialization here
    // Create the control.
    //m_pBrowserMy = new CWebBrowser2;
    //ASSERT (m_pBrowser);
    if (!m_pBrowserMy.Create(NULL, NULL, WS_VISIBLE, CRect(0,0,100,100), this, NULL))
    {
        TRACE("failed to create browser\n");
        //delete m_pBrowserMy;
        //m_pBrowserMy = NULL;
        return 0;
    }

    // Initialize the first URL.
    COleVariant noArg;
    CString strURL("www.microsoft.com");
    m_pBrowserMy.Navigate(strURL, &noArg, &noArg, &noArg, &noArg);
*/
    //@todo, read local file to check if it is registered
	editorMy.SetWindowTextA("Hello world...");
    
    //CString strURL("http://www.baidu.com");
	CString strURL("http://moni.51hupai.org/");
	if(false/*if registered*/)
    {
        strURL= "http://test.alltobid.com/";
    }
    m_pBrowserMy.Navigate(strURL, NULL, NULL, NULL, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Ct1Dlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Ct1Dlg::OnPaint()
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
		CDHtmlDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR Ct1Dlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT Ct1Dlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT Ct1Dlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}

HRESULT Ct1Dlg::OnButtonHelp(IHTMLElement* /*pElement*/)
{
	OnHelp();
	return S_OK;
}

void Ct1Dlg::OnBnClickedButtonRefresh()
{
    // TODO: Add your control notification handler code here
    m_pBrowserMy.Refresh();
}

BOOL Ct1Dlg::PreTranslateMessage(MSG* pMsg)
{
	SYSTEMTIME sys_time;
	GetLocalTime(&sys_time);
	char systemTime[150];
	sprintf(systemTime, "%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d\n", sys_time.wYear,
		sys_time.wMonth,
		sys_time.wDay,
		sys_time.wHour,
		sys_time.wMinute,
		sys_time.wSecond,
		sys_time.wMilliseconds,
		sys_time.wDayOfWeek);
	//editorMy.SetWindowTextA(systemTime);
	//system("time");
	//////////
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_RETURN)
		{
			pMsg->wParam = VK_TAB;
			//m_pBrowserMy.Refresh();
            //cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
            //IPT::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
            //wait to click send
            if (m_stateMachine == STATE_PRICE_CONFIRM)
            {
                m_stateMachine = STATE_PRICE_SEND;
            }
		}
		else if (pMsg->wParam == VK_F5)
		{
			m_pBrowserMy.Refresh();
		}
		else if (pMsg->wParam == VK_F4)
		{
			RECT rect;
			GetWindowRect(&rect);

            RECT rect1;
            GetClientRect(&rect1);

			char coordinates[60];

			LONG horizonCoordinate = rect.left;
			LONG verticalCoordinate = rect.top;

			LONG RELATIVE_HORIZON_DISTANCE = pMsg->pt.x - horizonCoordinate;
			LONG RELATIVE_VERTICAL_DISTANCE = pMsg->pt.y - verticalCoordinate;

            //display result
			sprintf(coordinates, "%d, %d, %d, %d, %s", RELATIVE_LEFT,RELATIVE_TOP,RELATIVE_RIGHT,RELATIVE_BOTTOM,
				    captureText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM).c_str());
			editorMy.SetWindowTextA(coordinates);
		}
        else if (pMsg->wParam == VK_F6)
        {
            /// Create windows
            //std::string image_window("XXXXXX");
            //cv::namedWindow(image_window, CV_WINDOW_AUTOSIZE);
            //namedWindow(result_window, CV_WINDOW_AUTOSIZE);
            /// Create Trackbar
            //int match_method(0);
            //char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
            //cv::createTrackbar(trackbar_label, image_window, &match_method, 5, nullptr);

            //cv::Point capturedPosition = captureTemplate(BUTTON_CANCEL_FILE);

            //display result
            //char coordinates[60];
            //sprintf(coordinates, "%d, %d", capturedPosition.x, capturedPosition.y);
            //editorMy.SetWindowTextA(coordinates);

            //click button
            RECT rect;
            GetWindowRect(&rect);
            IPT::leftButtonClick(rect.left + PRICE_INPUT.x, rect.top + PRICE_INPUT.y);
            std::string price = captureText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM).c_str();
            if (price.empty())
            {
                return true;
            }

            IPT::keyboardSendUnicodeInput(price);
            m_bidPrice = atof(price.c_str());
            m_stateMachine = STATE_PRICE_INPUT;
            time(&m_timer);
            return true;
        }
	}

    if (m_stateMachine != STATE_NONE)
    {
        if (m_stateMachine == STATE_PRICE_INPUT)
        {
            time_t now;
            time(&now);
            double seconds = difftime(now, m_timer);
            if (seconds > 3)
            {
                RECT rect;
                GetWindowRect(&rect);
                IPT::leftButtonClick(rect.left + PRICE_CONFIRM.x, rect.top + PRICE_CONFIRM.y);
                m_stateMachine = STATE_PRICE_CONFIRM;
            }
        }

        if (m_stateMachine == STATE_PRICE_SEND)
        {
            while (true)
            {
                std::string price = captureText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM).c_str();
                int currentPrice = atof(price.c_str());
                if (currentPrice >= m_bidPrice + 300)
                {
                    RECT rect;
                    GetWindowRect(&rect);
                    cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                    IPT::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    m_stateMachine = STATE_NONE;
                    break;
                }
                continue;
            }
            
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

		char coordinates[30];
		sprintf(coordinates, "%d, %d", RELATIVE_HORIZON_DISTANCE, RELATIVE_VERTICAL_DISTANCE);
		infoPanelEditor.SetWindowTextA(coordinates);
	}

	return CDialog::PreTranslateMessage(pMsg);
}




std::string Ct1Dlg::captureText(int relativeLeft, int relativeTop, int relativeRight, int relativeBottom)
{
#if defined _WIN32 || defined _WIN64
	
	//HWND SomeWindowHandle = GetDesktopWindow();
	HWND SomeWindowHandle = GetDesktopWindow()->m_hWnd;
	//HDC DC = GetDC(SomeWindowHandle);
	HDC DC = GetDC()->m_hDC;
    
    // @todo, current dialog's client area
    //GetScreenShotDialog(DC);
    //return std::string("test");

	img::Image Img = img::Image(DC, relativeLeft - DIALOG_FRAME_LEFT_WIDTH, relativeTop - DIALOG_FRAME_TOP_HEIGHT,
                      relativeRight - relativeLeft, relativeBottom - relativeTop); //screenshot of 0, 0, 200, 200..
	
	//ReleaseDC(SomeWindowHandle, DC);
	ReleaseDC(GetDC());
#else
	Image Img = Image(some_pixel_pointer, 200, 200); //pointer to pixels..
#endif

    // For testing purpose
    //RECT rect;
    //GetWindowRect(&rect);
	//GetScreenShot(rect.left + relativeLeft, rect.top + relativeTop, relativeRight - relativeLeft, relativeBottom - relativeTop);

	//std::unique_ptr<tesseract::TessBaseAPI> tesseract_ptr(new tesseract::TessBaseAPI());
	tesseract::TessBaseAPI tesseract_ptr;

	//tesseract_ptr->Init("/tesseract/tessdata", "eng");
	tesseract_ptr.Init("", "eng", tesseract::OEM_DEFAULT);
	tesseract_ptr.SetVariable("classify_bln_numeric_mode", "1");
	tesseract_ptr.SetVariable("tessedit_char_whitelist", "0123456789:");
	tesseract_ptr.SetVariable("tessedit_char_blacklist", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	tesseract_ptr.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
	tesseract_ptr.SetImage(Img.GetPixels(), Img.GetWidth(), Img.GetHeight(), Img.GetBytesPerPixel(), Img.GetBytesPerScanLine()); //Fixed this line..

    //std::unique_ptr<char[]> utf8_text_ptr(tesseract_ptr->GetUTF8Text());
	char* utf8_text_ptr = tesseract_ptr.GetUTF8Text();
	return std::string(utf8_text_ptr);
}


cv::Point Ct1Dlg::captureTemplate(const std::string& templateFile)
{
    cv::Mat dialogShot = img::hwnd2mat(WindowFromDC(GetDC()->m_hDC), DIALOG_FRAME_LEFT_WIDTH, DIALOG_FRAME_TOP_HEIGHT);

    //@todo, there will be a crash if do NOT save to local disk
    int res = remove(DIALOG_CAPTURE_TMP.c_str());
    cv::imwrite(DIALOG_CAPTURE_TMP, dialogShot);
    cv::Mat img = cv::imread(DIALOG_CAPTURE_TMP, 1);
    remove(DIALOG_CAPTURE_TMP.c_str());

    cv::Mat templ = cv::imread(templateFile, 1);
    cv::Mat result;
    int match_method = 0;
    int max_Trackbar = 5;

    //char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
    //createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);
    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    /// Do the Matching and Normalize
    matchTemplate(img, templ, result, match_method);
    normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
    cv::Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }

    /// Show me what you got
    //rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
    //rectangle(result, matchLoc, cv::Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), cv::Scalar::all(0), 2, 8, 0);
    return  cv::Point(matchLoc.x + templ.cols / 2, matchLoc.y + templ.rows / 2);
}
