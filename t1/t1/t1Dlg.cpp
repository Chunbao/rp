
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
#include <opencv2/imgproc/imgproc_c.h>

//using namespace std;
//using namespace cv;

//price
const int RELATIVE_LEFT = 662;
const int RELATIVE_TOP = 323;
const int RELATIVE_RIGHT = 720;
const int RELATIVE_BOTTOM = 345;
//small price area
const int RELATIVE_LEFT_SMALL = 178;
const int RELATIVE_TOP_SMALL = 469;
const int RELATIVE_RIGHT_SMALL = 226;
const int RELATIVE_BOTTOM_SMALL = 487;
//time area
const int RELATIVE_LEFT_TIME = 149;
const int RELATIVE_TOP_TIME = 455;
const int RELATIVE_RIGHT_TIME = 222;
const int RELATIVE_BOTTOM_TIME = 469;


const cv::Point PRICE_INPUT(754/*right side of input*/, 479);
const cv::Point PRICE_CONFIRM(824, 482);

const cv::Point VALID_BUTTON_AREA_LEFT(468, 305);
const cv::Point VALID_BUTTON_AREA_RIGHT(892, 592);
const cv::Point CAPTCHA_INPUT(818, 478);

const cv::Point REQUEST_TOO_OFTEN_BACKUP(690, 543);

//const int PREDICT_ADD_PRICE = 300;
const int INPUT_PRICE_DELAY = 1; //1 second
const int PERFORM_SEND_PRICE_POINT = 300 + 100;

//@todo, need to be replaced for compatiable problems
//@todo, use getWindowRect instead of getClientRect in Image
// fix offset caused by dialog title and frame area (width=8,height=30)
// initialized in OnInitDialog
static int DIALOG_FRAME_LEFT_WIDTH;
static int DIALOG_FRAME_TOP_HEIGHT;
static utl::PriceFilter PriceOCRFilter;
static utl::TimeFilter TimeAccurateFilter;
static CString EXECUTABLE_PATH_BASE;

const std::string ENHANCED_AREA_BEFORE("TmpPriceClip.bmp");
const std::string ENHANCED_AREA_AFTER("TmpPriceEnhanced.bmp");
//@todo, make it im memory
//@todo, support Chinese

//const std::string     BUTTON_OK_FILE("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\ok.bmp");
//const std::string BUTTON_CANCEL_FILE("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\cancel.bmp");
//const std::string BUTTON_REFRESH_FILE("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\refresh.bmp");
//const std::string DIALOG_CAPTURE_TMP("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t1\\t1\\testdata\\Capture_tmp.bmp");
CString a1 = utl::getWorkingPath() + CString(_T("ok.bmp")); 
std::string     BUTTON_OK_FILE((LPCTSTR)a1);
//const cv::Mat   BUTTON_OK_MAT = cv::imread(BUTTON_OK_FILE);

CString a2 = utl::getWorkingPath() + CString(_T("cancel.bmp"));
std::string BUTTON_CANCEL_FILE((LPCTSTR)a2);
//const cv::Mat   BUTTON_CANCEL_MAT = cv::imread(BUTTON_CANCEL_FILE);

CString a3 = utl::getWorkingPath() + CString(_T("refresh.bmp"));
std::string BUTTON_REFRESH_FILE((LPCTSTR)a3);
//const cv::Mat   BUTTON_REFRESH_MAT = cv::imread(BUTTON_REFRESH_FILE);

CString a4 = utl::getWorkingPath() + CString(_T("Capture_tmp.bmp"));
std::string DIALOG_CAPTURE_TMP((LPCTSTR)a4); 


// CWnd::BringWindowToTop


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
    , m_bidUserFinalPrice(0)
    , m_isInUserInputStage(false)
    , m_useIntelligenceBid(false)
    , m_priceTimer(std::chrono::high_resolution_clock::now())
    , m_okPositionWhenSending(0 , 0)
	, m_timeDiff(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ct1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDHtmlDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXPLORER1, m_pBrowserMy);
    DDX_Control(pDX, IDC_EDIT1, editorMy);
    DDX_Control(pDX, IDC_EDIT2, infoPanelEditor);
    DDX_Control(pDX, IDC_COMBO2, m_confirmPriceSeconds);
    DDX_Control(pDX, IDC_COMBO1, m_confirmPriceAdd);
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
    DIALOG_FRAME_LEFT_WIDTH = utl::getBorderAreaWidth(GetDC()->m_hDC);
    DIALOG_FRAME_TOP_HEIGHT = utl::getCaptionAreaHeight(GetDC()->m_hDC);
    EXECUTABLE_PATH_BASE = utl::getWorkingPath();

    m_confirmPriceSeconds.AddString(_T("42s"));
    m_confirmPriceSeconds.AddString(_T("43s"));
    m_confirmPriceSeconds.AddString(_T("44s"));
    m_confirmPriceSeconds.AddString(_T("45s"));
    m_confirmPriceSeconds.AddString(_T("46s"));
    m_confirmPriceSeconds.AddString(_T("47s"));
    m_confirmPriceSeconds.AddString(_T("48s"));
    m_confirmPriceSeconds.AddString(_T("49s"));
    m_confirmPriceSeconds.AddString(_T("50s"));
    m_confirmPriceSeconds.SetCurSel(3);

    m_confirmPriceAdd.AddString(_T("400"));
    m_confirmPriceAdd.AddString(_T("500"));
    m_confirmPriceAdd.AddString(_T("600"));
    m_confirmPriceAdd.AddString(_T("700"));
    m_confirmPriceAdd.AddString(_T("800"));
    m_confirmPriceAdd.AddString(_T("900"));
    m_confirmPriceAdd.AddString(_T("1000"));
    m_confirmPriceAdd.AddString(_T("1100"));
    m_confirmPriceAdd.AddString(_T("1200"));
    m_confirmPriceAdd.SetCurSel(6);

    //int nIndex = m_cbExample.GetCurSel();
    //CString strCBText;
    //m_cbExample.GetLBText(nIndex, strCBText);

    //@todo, read local file to check if it is registered
	editorMy.SetWindowText(_T("Hello world..."));
    CEdit infoPanelEditor;

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
    // RT display info.
    {
        SYSTEMTIME sys_time;
        GetLocalTime(&sys_time);

        std::tm server = utl::getServerTime(m_timeDiff);
        CString st;
        st.Format(_T("国拍时间 %02d:%02d:%02d "), server.tm_hour, server.tm_min, server.tm_sec);

        CString systemTime;
        systemTime.Format(_T("当前价格%d 距离接受区间 %d, 工作流 %d, %s\n"),
            m_bidPrice,
            m_bidUserFinalPrice - m_bidPrice - 300,
            (int)m_stateMachine,
            st);
        SetWindowText(systemTime);
        /*
        CString systemTime;
        systemTime.Format(_T("%4d/%02d/%02d %02d:%02d:%02d.%03d 星期%1d 当前价格%d 距离受理区间 %d, 工作流 %d, %s, %s\n"),
                            sys_time.wYear,
                            sys_time.wMonth,
                            sys_time.wDay,
                            sys_time.wHour,
                            sys_time.wMinute,
                            sys_time.wSecond,
                            sys_time.wMilliseconds,
                            sys_time.wDayOfWeek,
                            m_bidPrice,
                            m_bidUserFinalPrice - m_bidPrice - 300,
                            (int)m_stateMachine,
                            st,
                            utl::getWorkingPath());
        SetWindowText(systemTime);
        */

        RECT rect;
        GetWindowRect(&rect);
        LONG RELATIVE_HORIZON_DISTANCE = pMsg->pt.x - rect.left;
        LONG RELATIVE_VERTICAL_DISTANCE = pMsg->pt.y - rect.top;

        CString coordinates;
        coordinates.Format(_T("%d, %d"), RELATIVE_HORIZON_DISTANCE, RELATIVE_VERTICAL_DISTANCE);
        infoPanelEditor.SetWindowText(coordinates);
    }

	performTimeRecognition();

    performPriceRecognition();

    if (manageUserEvent(pMsg))
    {
        return true;
    }
	
    automateWorkFlow();

	return CDialog::PreTranslateMessage(pMsg);
}


void Ct1Dlg::performPriceRecognition()
{
    std::chrono::high_resolution_clock::time_point now(std::chrono::high_resolution_clock::now());
    std::chrono::milliseconds elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(now - m_priceTimer);
    if (elapsed.count() >= 200)
    {
        std::string price;
        if (m_stateMachine != STATE_NONE)
        {
            img::writePriceToFile(GetDC()->m_hDC,
                RELATIVE_LEFT_SMALL - DIALOG_FRAME_LEFT_WIDTH,
                RELATIVE_TOP_SMALL - DIALOG_FRAME_TOP_HEIGHT,
                RELATIVE_RIGHT_SMALL - RELATIVE_LEFT_SMALL,
                RELATIVE_BOTTOM_SMALL - RELATIVE_TOP_SMALL,
                ENHANCED_AREA_BEFORE);
            img::enhanceImage(ENHANCED_AREA_BEFORE, ENHANCED_AREA_AFTER);
            price = captureEnhancedText(ENHANCED_AREA_AFTER);
        }
        else
        {
            price = captureText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM);
        }
        m_bidPrice = PriceOCRFilter.process(price);

#if 0 // calculate captureText time
        CString coordinates;
        coordinates.Format(_T("%s, %d, %d"),
            CString(price.c_str()),
            m_bidPrice,
            std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - now).count());
        editorMy.SetWindowText(coordinates);
#endif
        m_priceTimer = std::chrono::high_resolution_clock::now();
    }
}

void Ct1Dlg::performTimeRecognition()
{
	if (m_timeDiff == 0)
	{
		if (!TimeAccurateFilter.ready())
		{
			std::chrono::high_resolution_clock::time_point now(std::chrono::high_resolution_clock::now());
			std::chrono::milliseconds elapsed =
				std::chrono::duration_cast<std::chrono::milliseconds>(now - m_priceTimer);
			if (elapsed.count() >= 100)
			{
				img::writePriceToFile(GetDC()->m_hDC,
					RELATIVE_LEFT_TIME - DIALOG_FRAME_LEFT_WIDTH,
					RELATIVE_TOP_TIME - DIALOG_FRAME_TOP_HEIGHT,
					RELATIVE_RIGHT_TIME - RELATIVE_LEFT_TIME,
					RELATIVE_BOTTOM_TIME - RELATIVE_TOP_TIME,
					ENHANCED_AREA_BEFORE);
				img::enhanceImage(ENHANCED_AREA_BEFORE, ENHANCED_AREA_AFTER);
				std::string time = captureEnhancedText(ENHANCED_AREA_AFTER);
				TimeAccurateFilter.process(time);

				m_priceTimer = std::chrono::high_resolution_clock::now();
			}
		}
		else
		{
			std::time_t t = std::time(NULL);
			std::tm then_tm = *std::localtime(&t);
			then_tm.tm_hour = TimeAccurateFilter.getHour();
			then_tm.tm_min = TimeAccurateFilter.getMinute();
			then_tm.tm_sec = TimeAccurateFilter.getSecond();

			time_t timetThen = mktime(&then_tm);
			std::chrono::time_point<std::chrono::system_clock>
				then_tp = std::chrono::system_clock::from_time_t(timetThen);
			std::chrono::system_clock::time_point local = std::chrono::system_clock::now();
			m_timeDiff = std::chrono::duration_cast<std::chrono::milliseconds>(local - then_tp).count();
            if (m_timeDiff == 0)
            {
                m_timeDiff = -1; // This big change case should never happen in theory. Set 1 milliseconds just in case
            }

			CString coordinates;
			coordinates.Format(_T("%s"), CString(TimeAccurateFilter.getTime().c_str()));
			editorMy.SetWindowText(coordinates);
		}
	}
}

bool Ct1Dlg::manageUserEvent(MSG* pMsg)
{
    //////////
    if (pMsg->message == WM_KEYDOWN)
    {
        if (pMsg->wParam == VK_F1)
        {
            //@todo, check state
            //m_pBrowserMy.GoBack();
        }
        else if (pMsg->wParam == VK_F2)
        {
            //@todo, check state
            //m_pBrowserMy.GoBack();
            m_timeDiff = 0;
            TimeAccurateFilter.reset();
        }
        else if (pMsg->wParam == VK_F3)
        {
            img::writePriceToFile(GetDC()->m_hDC,
                RELATIVE_LEFT_TIME - DIALOG_FRAME_LEFT_WIDTH,
                RELATIVE_TOP_TIME - DIALOG_FRAME_TOP_HEIGHT,
                RELATIVE_RIGHT_TIME - RELATIVE_LEFT_TIME,
                RELATIVE_BOTTOM_TIME - RELATIVE_TOP_TIME,
                ENHANCED_AREA_BEFORE);
            img::enhanceImage(ENHANCED_AREA_BEFORE, ENHANCED_AREA_AFTER);
            std::string time = captureEnhancedText(ENHANCED_AREA_AFTER);

            // ... @todo
            CString coordinates;
            coordinates.Format(_T("%s"), CString(time.c_str()));
            editorMy.SetWindowText(coordinates);

        }
        else if (pMsg->wParam == VK_F5)
        {
            m_pBrowserMy.Refresh();
        }
        else if (pMsg->wParam == VK_F4)
        {
            img::writePriceToFile(GetDC()->m_hDC, 
                                  RELATIVE_LEFT_SMALL - DIALOG_FRAME_LEFT_WIDTH, 
                                  RELATIVE_TOP_SMALL - DIALOG_FRAME_TOP_HEIGHT,
                                  RELATIVE_RIGHT_SMALL - RELATIVE_LEFT_SMALL, 
                                  RELATIVE_BOTTOM_SMALL - RELATIVE_TOP_SMALL,
                                  ENHANCED_AREA_BEFORE);
            img::enhanceImage(ENHANCED_AREA_BEFORE, ENHANCED_AREA_AFTER);
            std::string price = captureEnhancedText(ENHANCED_AREA_AFTER);

            //display result
            CString coordinates;
            coordinates.Format(_T("%s, %s"), 
                               CString(captureText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM).c_str()),
                               CString(price.c_str()));
            editorMy.SetWindowText(coordinates);

            
            //captureEnhancedText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM);
        }
        else if (pMsg->wParam == VK_F6)
        {
            // simplified error handling
            if (m_stateMachine != STATE_NONE)
            {
                return true;
            }

            /// click button
            RECT rect;
            GetWindowRect(&rect);
            ipt::leftButtonClick(rect.left + PRICE_INPUT.x, rect.top + PRICE_INPUT.y);
            ipt::keyboardSendBackspaceKey(7);

            if (!m_useIntelligenceBid)
            {
                const int nIndex = m_confirmPriceAdd.GetCurSel();
                CString strCBText;
                m_confirmPriceAdd.GetLBText(nIndex, strCBText);
                m_bidUserFinalPrice = m_bidPrice + _ttoi(strCBText);
                m_useIntelligenceBid = true;
            }
            else
            {
                m_bidUserFinalPrice = m_bidPrice + prc::getIntelligencePrice(m_timeDiff);
            }
            
            std::string predicatePrice = std::to_string(m_bidUserFinalPrice);
            ipt::keyboardSendUnicodeInput(predicatePrice);

            m_stateMachine = STATE_PRICE_INPUT;
            time(&m_workFlowTimer);
            return true;
        }
        else if (pMsg->wParam == VK_RETURN)
        {
            pMsg->wParam = VK_TAB;
            //m_pBrowserMy.Refresh();
            if (m_stateMachine == STATE_PRICE_CONFIRM)
            {
                time(&m_workFlowTimer);
                m_stateMachine = STATE_PRICE_SEND;
            }
        }
        else if (pMsg->wParam == VK_ESCAPE)
        {
            // Consider moving this functionality to vk_escape
            if (m_stateMachine != STATE_NONE)
            {
                RECT rect;
                GetWindowRect(&rect);
                cv::Point capturedPosition = captureTemplate(BUTTON_CANCEL_FILE);
                if (utl::ifInRange(capturedPosition, VALID_BUTTON_AREA_LEFT, VALID_BUTTON_AREA_RIGHT))
                {
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                }
                else
                {
                    cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                    if (utl::ifInRange(capturedPosition, VALID_BUTTON_AREA_LEFT, VALID_BUTTON_AREA_RIGHT))
                    {
                        ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    }
                }
                m_stateMachine = STATE_NONE;
            }
            
            //Find any cancel click, or find ok to click
            return true;
        }
    }

    return false;
}

void Ct1Dlg::automateWorkFlow() {
    
    //start automatically
    if (m_stateMachine == STATE_NONE)
    {
        const int nIndex = m_confirmPriceSeconds.GetCurSel();
        CString strSecond;
        m_confirmPriceSeconds.GetLBText(nIndex, strSecond);
        const int seconds = _ttoi(strSecond);
        std::tm server = utl::getServerTime(m_timeDiff);
        if (server.tm_hour == 11 && server.tm_min == 29 && server.tm_sec >= seconds)
        {
            ipt::keyboardSendF6Key();
            // No need to record wf time
        }
    }
    
    if (m_stateMachine != STATE_NONE)
    {
        if (m_stateMachine == STATE_PRICE_INPUT)
        {
            time_t now;
            time(&now);
            double seconds = difftime(now, m_workFlowTimer);
            if (seconds > INPUT_PRICE_DELAY)
            {
                //@todo, happy flow case
                RECT rect;
                GetWindowRect(&rect);
                ipt::leftButtonClick(rect.left + PRICE_CONFIRM.x, rect.top + PRICE_CONFIRM.y);
                
                time(&m_workFlowTimer);
                m_isInUserInputStage = true;
                
                m_stateMachine = STATE_PRICE_CONFIRM;
            }
        }
        else if (m_stateMachine == STATE_PRICE_CONFIRM)
        {
            //@todo, refresh if captcha appears
            // need precise match
            // should never appear, promised by deleting
            RECT rect;
            GetWindowRect(&rect);

            time_t now;
            time(&now);
            double seconds = difftime(now, m_workFlowTimer);
            if (seconds >= INPUT_PRICE_DELAY && m_isInUserInputStage)
            {
                //precise match OK button, to make sure no dialog
                cv::Point capturedPosition = captureTemplate(BUTTON_REFRESH_FILE);
                if (utl::ifInRange(capturedPosition, VALID_BUTTON_AREA_LEFT, VALID_BUTTON_AREA_RIGHT))
                {
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    ipt::leftButtonClick(rect.left + CAPTCHA_INPUT.x, rect.top + CAPTCHA_INPUT.y);
                    //click back to input
                }
                else
                {
                    /* need to optimize since this captcha too often cases,
                    a flag to skip time-comsuing cal.
                    */
                    ipt::leftButtonClick(rect.left + REQUEST_TOO_OFTEN_BACKUP.x, 
                                         rect.top + REQUEST_TOO_OFTEN_BACKUP.y);
                    if (m_okPositionWhenSending.x == 0 && m_okPositionWhenSending.y == 0)
                    {
                        cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                        if (utl::ifInRange(capturedPosition, VALID_BUTTON_AREA_LEFT, VALID_BUTTON_AREA_RIGHT))
                        {
                            m_okPositionWhenSending.x = capturedPosition.x;
                            m_okPositionWhenSending.y = capturedPosition.y;
                        }
                    }
                }
                time(&m_workFlowTimer);
                m_isInUserInputStage = false; 
            }

        }
        else if (m_stateMachine == STATE_PRICE_SEND)
        {
            // 
            const bool acceptedPriceRange = (m_bidUserFinalPrice - m_bidPrice <= PERFORM_SEND_PRICE_POINT);
            // deadline 11:29:55 force to send
            std::tm server = utl::getServerTime(m_timeDiff);
            const bool deadlineArrived = (server.tm_hour == 11 && server.tm_min == 29 && server.tm_sec >= 55);
            if (acceptedPriceRange || deadlineArrived)
            {
                RECT rect;
                GetWindowRect(&rect);
                if (m_okPositionWhenSending.x != 0 && m_okPositionWhenSending.y != 0)
                {
                    ipt::leftButtonClick(rect.left + m_okPositionWhenSending.x, rect.top + m_okPositionWhenSending.y);
                }
                else
                {
                    // ideally, should never arrived here
                    assert(false);
                    cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                }
                time(&m_workFlowTimer);
                m_stateMachine = STATE_PRICE_RESULT;
            }
        }
        else if (m_stateMachine == STATE_PRICE_RESULT)
        {
            time_t now;
            time(&now);
            double seconds = difftime(now, m_workFlowTimer);
            if (seconds > INPUT_PRICE_DELAY*2)
            {
                //precise match OK button, to make sure no dialog
                RECT rect;
                GetWindowRect(&rect);
                cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                if (utl::ifInRange(capturedPosition, VALID_BUTTON_AREA_LEFT, VALID_BUTTON_AREA_RIGHT))
                {
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    m_stateMachine = STATE_NONE;
                }
                time(&m_workFlowTimer);
            }
        }
    }

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
	tesseract::TessBaseAPI tesseract_ptr;
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

std::string Ct1Dlg::captureEnhancedText(std::string enhancedFile)
{
    tesseract::TessBaseAPI tesseract_ptr;
    tesseract_ptr.Init("", "eng", tesseract::OEM_DEFAULT);
    tesseract_ptr.SetVariable("classify_bln_numeric_mode", "1");
    tesseract_ptr.SetVariable("tessedit_char_whitelist", "0123456789:");
    tesseract_ptr.SetVariable("tessedit_char_blacklist", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
    tesseract_ptr.SetPageSegMode(tesseract::PSM_SINGLE_LINE);
    //tesseract_ptr.SetImage(Img.GetPixels(), Img.GetWidth(), Img.GetHeight(), Img.GetBytesPerPixel(), Img.GetBytesPerScanLine()); //Fixed this line..


    STRING text_out;
    tesseract_ptr.ProcessPages(enhancedFile.c_str(), NULL, 0, &text_out);
                                                                                                                                 //std::unique_ptr<char[]> utf8_text_ptr(tesseract_ptr->GetUTF8Text());
    char* utf8_text_ptr = tesseract_ptr.GetUTF8Text();
    return std::string(utf8_text_ptr);
}


//Very time consuming, 1.2 seconds uses
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

    int a = img.depth();
    int b = templ.depth();
    int c = img.type();
    int d = templ.type();
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
