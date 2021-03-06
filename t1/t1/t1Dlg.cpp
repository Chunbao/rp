// t1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "t1.h"
#include "t1Dlg.h"
#include "StrategyDlg.h"
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

const cv::Point CAPTCHA_ENLARGE_AREA_LEFT(524 - 7/*border width*/, 468 - 30/*header height*/);
const cv::Point CAPTCHA_ENLARGE_AREA_RIGHT(113, 50);
const cv::Point CAPTCHA_INPUT(818, 478);

const cv::Point REQUEST_TOO_OFTEN_BACKUP(690, 543);

//const int PREDICT_ADD_PRICE = 300;
const int INPUT_PRICE_DELAY = 1; //1 second
const int PERFORM_SEND_PRICE_POINT = 300;

//@todo, need to be replaced for compatiable problems
//@todo, use getWindowRect instead of getClientRect in Image
// fix offset caused by dialog title and frame area (width=8,height=30)
// initialized in OnInitDialog
static int DIALOG_FRAME_LEFT_WIDTH;
static int DIALOG_FRAME_TOP_HEIGHT;
static utl::PriceFilter PriceOCRFilter;
static utl::TimeFilter TimeAccurateFilter;
static StaticImageCtrl staticImageCtrl;
static tim::TimeManager TimeManager;

const std::string ENHANCED_AREA_BEFORE("TmpPriceClip.bmp");
const std::string ENHANCED_AREA_AFTER("TmpPriceEnhanced.bmp");
//@todo, make it im memory
//@todo, support Chinese

const std::string     BUTTON_OK_FILE("..\\t1\\testdata\\ok.bmp");
const std::string BUTTON_CANCEL_FILE("..\\t1\\testdata\\cancel.bmp");
const std::string BUTTON_REFRESH_FILE("..\\t1\\testdata\\refresh.bmp");
const std::string DIALOG_CAPTURE_TMP("..\\t1\\testdata\\Capture_tmp.bmp");
/*
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
*/

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
    , m_okPositionWhenSending(0 , 0)
{
    TimeManager.setLastExecutedTimePoint();
    TimeManager.setServerDiff(0);

    m_hIcon = AfxGetApp()->LoadIcon(/*IDR_MAINFRAME*/IDI_ICON_PANDA);
	if (pParent)
	{
		m_strategyDlg = dynamic_cast<CStrategyDlg*>(pParent);
	}
    logger::log(CString("启动程序 ..."));
}

Ct1Dlg::~Ct1Dlg()
{
    logger::log(CString("退出程序 ..."));
}

void Ct1Dlg::DoDataExchange(CDataExchange* pDX)
{
    CDHtmlDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_EXPLORER1, m_pBrowserMy);
    DDX_Control(pDX, IDC_EDIT1, editorMy);
    DDX_Control(pDX, IDC_EDIT2, infoPanelEditor);
    DDX_Control(pDX, IDC_COMBO2, m_confirmPriceSeconds);
    DDX_Control(pDX, IDC_COMBO1, m_confirmPriceAdd);
    DDX_Control(pDX, IDC_COMBO3, m_webMode);
    DDX_Control(pDX, IDC_CHECK_CAPTCHA_ENLARGE, m_captchaEnlarge);
    DDX_Control(pDX, IDC_CHECK_CAPTCHA_PREVIEW, m_captchaPreview);
    DDX_Control(pDX, IDC_COMBO4, m_forceSendPriceTime);
    DDX_Control(pDX, IDC_COMBO_SEND, m_sendPriceAdd);
}

BEGIN_MESSAGE_MAP(Ct1Dlg, CDHtmlDialog)
    ON_WM_SYSCOMMAND()
    //}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_Refresh, &Ct1Dlg::OnBnClickedButtonRefresh)
    ON_CBN_SELCHANGE(IDC_COMBO3, &Ct1Dlg::OnCbnSelchangeComboMode)
    ON_WM_TIMER()
END_MESSAGE_MAP()


// Ct1Dlg message handlers

BOOL Ct1Dlg::OnInitDialog()
{
    CDHtmlDialog::OnInitDialog();
    SetTimer(tim::TIMERID, 100, nullptr);
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
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(m_hIcon, FALSE);        // Set small icon
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
	ModifyStyle(WS_SYSMENU, 0);

    DIALOG_FRAME_LEFT_WIDTH = utl::getBorderAreaWidth(GetDC()->m_hDC);
    DIALOG_FRAME_TOP_HEIGHT = utl::getCaptionAreaHeight(GetDC()->m_hDC);

    m_confirmPriceSeconds.AddString(_T("50s"));
    m_confirmPriceSeconds.AddString(_T("49s"));
    m_confirmPriceSeconds.AddString(_T("48s"));
    m_confirmPriceSeconds.AddString(_T("47s"));
    m_confirmPriceSeconds.AddString(_T("46s"));
    m_confirmPriceSeconds.AddString(_T("45s"));
    m_confirmPriceSeconds.AddString(_T("44s"));
    m_confirmPriceSeconds.AddString(_T("43s"));
    m_confirmPriceSeconds.AddString(_T("42s"));
    m_confirmPriceSeconds.AddString(_T("41s"));

    m_confirmPriceSeconds.AddString(_T("40s"));
    m_confirmPriceSeconds.AddString(_T("39s"));
    m_confirmPriceSeconds.AddString(_T("38s"));
    m_confirmPriceSeconds.AddString(_T("37s"));
    m_confirmPriceSeconds.AddString(_T("36s"));
    m_confirmPriceSeconds.AddString(_T("35s"));
    m_confirmPriceSeconds.AddString(_T("34s"));
    m_confirmPriceSeconds.AddString(_T("33s"));
    m_confirmPriceSeconds.AddString(_T("32s"));
    m_confirmPriceSeconds.AddString(_T("32s"));
    m_confirmPriceSeconds.AddString(_T("31s"));
    m_confirmPriceSeconds.AddString(_T("30s"));

    m_confirmPriceSeconds.AddString(_T("29s"));
    m_confirmPriceSeconds.AddString(_T("28s"));
    m_confirmPriceSeconds.AddString(_T("27s"));
    m_confirmPriceSeconds.AddString(_T("26s"));
    m_confirmPriceSeconds.AddString(_T("25s"));
    m_confirmPriceSeconds.AddString(_T("24s"));
    m_confirmPriceSeconds.AddString(_T("23s"));
    m_confirmPriceSeconds.AddString(_T("22s"));
    m_confirmPriceSeconds.AddString(_T("21s"));
    m_confirmPriceSeconds.AddString(_T("20s"));

    m_confirmPriceSeconds.AddString(_T("19s"));
    m_confirmPriceSeconds.AddString(_T("18s"));
    m_confirmPriceSeconds.AddString(_T("17s"));
    m_confirmPriceSeconds.AddString(_T("16s"));
    m_confirmPriceSeconds.AddString(_T("15s"));
    m_confirmPriceSeconds.AddString(_T("14s"));
    m_confirmPriceSeconds.AddString(_T("13s"));
    m_confirmPriceSeconds.AddString(_T("12s"));
    m_confirmPriceSeconds.AddString(_T("11s"));
    m_confirmPriceSeconds.AddString(_T("10s"));

    m_confirmPriceSeconds.AddString(_T("9s"));
    m_confirmPriceSeconds.AddString(_T("8s"));
    m_confirmPriceSeconds.AddString(_T("7s"));
    m_confirmPriceSeconds.AddString(_T("6s"));
    m_confirmPriceSeconds.AddString(_T("5s"));
    m_confirmPriceSeconds.AddString(_T("4s"));
    m_confirmPriceSeconds.AddString(_T("3s"));
    m_confirmPriceSeconds.AddString(_T("2s"));
    m_confirmPriceSeconds.AddString(_T("1s"));

    m_confirmPriceSeconds.SetCurSel(5);

    m_confirmPriceAdd.AddString(_T("400"));
    m_confirmPriceAdd.AddString(_T("500"));
    m_confirmPriceAdd.AddString(_T("600"));
    m_confirmPriceAdd.AddString(_T("700"));
    m_confirmPriceAdd.AddString(_T("800"));
    m_confirmPriceAdd.AddString(_T("900"));
    m_confirmPriceAdd.AddString(_T("1000"));
    m_confirmPriceAdd.AddString(_T("1100"));
    m_confirmPriceAdd.AddString(_T("1200"));
    m_confirmPriceAdd.SetCurSel(6); //1000

    //TBD
    //m_webMode.AddString(_T("仿真"));
    //m_webMode.AddString(_T("兼容性"));
    //m_webMode.AddString(_T("实拍"));
    //m_webMode.AddString(_T("百度"));
    m_webMode.SetCurSel(0); //

    m_forceSendPriceTime.SetCurSel(0);

    m_sendPriceAdd.SetCurSel(1);

    //int nIndex = m_cbExample.GetCurSel();
    //CString strCBText;
    //m_cbExample.GetLBText(nIndex, strCBText);

    //@todo, read local file to check if it is registered
    editorMy.SetWindowText(_T("Input URL...F7"));
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

void Ct1Dlg::OnCancel()
{
    // CDialog::OnCancel();  // MUST remove this line
    DestroyWindow();
}

void Ct1Dlg::OnOK()
{
    // CDialog::OnOK(); // MUST remove this line
    DestroyWindow();
}

//void Ct1Dlg::PostNcDestroy()
//{
//    delete this;
//}

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

void Ct1Dlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == tim::TIMERID)
    {
        SYSTEMTIME sys_time;
        GetLocalTime(&sys_time);

        SYSTEMTIME server = TimeManager.getServerTime();
        CString st;
        st.Format(_T("%02d:%02d:%02d "), server.wHour, server.wMinute, server.wSecond);
        m_strategyDlg->m_ocrTime.SetWindowTextW(st);

        CString systemTime;
        systemTime.Format(_T("当前价格%d 距离接受区间 %d, 工作流 %d, 国拍时间 %s\n"),
            m_bidPrice,
            m_bidUserFinalPrice - m_bidPrice - 300,
            (int)m_stateMachine,
            st);
        SetWindowText(systemTime);


        SYSTEMTIME local = TimeManager.getLocalTime();
        st.Format(_T("%02d:%02d:%02d "), local.wHour, local.wMinute, local.wSecond);
        m_strategyDlg->m_localTime.SetWindowTextW(st);

        const int intelligentPriceAdd = prc::getIntelligencePriceBwRelease(TimeManager.getServerTime());
        st.Format(_T("%d "), intelligentPriceAdd + m_bidPrice);
        m_strategyDlg->m_intelligentPrice.SetWindowTextW(st);

        CString price;
        price.Format(_T("%d"), m_bidPrice);
        m_strategyDlg->m_recognizedPrice.SetWindowTextW(price);
        m_strategyDlg->UpdateData(false);

        performTimeRecognition();

        performPriceRecognition();


        performCaptchaProcessing();

        automateWorkFlow();
    }

    if (GetForegroundWindow() != this)
    {
        const SYSTEMTIME server = TimeManager.getServerTime();
        if (server.wHour == 11 && server.wMinute == 29 && server.wSecond >= 20)
        {
            SetForegroundWindow();
        }
    }
}

BOOL Ct1Dlg::PreTranslateMessage(MSG* pMsg)
{
    // coordinate info
    {
        RECT rect;
        GetWindowRect(&rect);
        LONG RELATIVE_HORIZON_DISTANCE = pMsg->pt.x - rect.left;
        LONG RELATIVE_VERTICAL_DISTANCE = pMsg->pt.y - rect.top;

        CString coordinates;
        coordinates.Format(_T("%d, %d"), RELATIVE_HORIZON_DISTANCE, RELATIVE_VERTICAL_DISTANCE);
        infoPanelEditor.SetWindowText(coordinates);
    }

    if (manageUserEvent(pMsg))
    {
        return true;
    }

    return CDialog::PreTranslateMessage(pMsg);
}

void Ct1Dlg::performPriceRecognition()
{
    if (TimeManager.getFreeMilliSeconds() >=200)
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
        TimeManager.setLastExecutedTimePoint();
    }
}

void Ct1Dlg::performTimeRecognition()
{
    if (TimeManager.getServerDiff() == 0)
    {
        if (!TimeAccurateFilter.ready())
        {
            ULONGLONG free = TimeManager.getFreeMilliSeconds();
            if (free > (ULONGLONG)100)
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

                TimeManager.setLastExecutedTimePoint();
            }
        }
        else
        {
            const int hour = TimeAccurateFilter.getHour();
            const int minute = TimeAccurateFilter.getMinute();
            const int second = TimeAccurateFilter.getSecond();
            const ULONGLONG thenTime = TimeManager.getGivenTimePoint(hour, minute, second, 0);
            const ULONGLONG local = TimeManager.currentTime();
            const LONGLONG diff = local - thenTime;
            TimeManager.setServerDiff(diff);
            if (TimeManager.getServerDiff() == 0)
            {
                TimeManager.setServerDiff(10); // This big change case should never happen in theory. Set 1 useconds just in case
            }

            logger::log(CString("校准国拍服务器时间 ..."));
#if 0
            CString coordinates;
            coordinates.Format(_T("%s"), CString(TimeAccurateFilter.getTime().c_str()));
            editorMy.SetWindowText(coordinates);
#endif
        }
    }
}

void Ct1Dlg::performCaptchaProcessing()
{
    if (m_captchaEnlarge.GetCheck() == BST_CHECKED &&
        STATE_CAPTCHA_READY == m_stateMachine &&
        !staticImageCtrl.isCaptchaWorking())
    {
        HBITMAP hBitMap = img::GetSrcBit(GetDC()->m_hDC,
                                         CAPTCHA_ENLARGE_AREA_LEFT.x,
                                         CAPTCHA_ENLARGE_AREA_LEFT.y,
                                         CAPTCHA_ENLARGE_AREA_RIGHT.x,
                                         CAPTCHA_ENLARGE_AREA_RIGHT.y);
        /*m_image = new StaticImageCtrl(this, hBitMap);
        m_image->setVisible(8);*/
        CStatic* pWnd = (CStatic*)GetDlgItem(IDC_STATIC_PIC);
        pWnd->MoveWindow(CAPTCHA_ENLARGE_AREA_LEFT.x - 50 - 40/*40 make the image moves upwards*/,
                         CAPTCHA_ENLARGE_AREA_LEFT.y - 25 - 130 /*130 make the image moves upwards*/,
                         CAPTCHA_ENLARGE_AREA_RIGHT.x * 2,
                         CAPTCHA_ENLARGE_AREA_RIGHT.y * 2);
        pWnd->SetBitmap(hBitMap);
        pWnd->SetWindowPos(NULL,
            0,
            0,
            224,
            98,
            SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOZORDER);
        //pWnd->ShowWindow(SW_SHOW);

        //@todo, add force hide if enter key is pressed
        staticImageCtrl.setVisible(5 /*seconds*/, pWnd);
    }

    if (staticImageCtrl.isTimerWorking())
    {
        if (staticImageCtrl.setInvisibleIfTimeIsup())
        {
            SYSTEMTIME server = TimeManager.getServerTime();
            const bool preview = (server.wHour == 11 && server.wMinute == 29 && server.wSecond <= 30);
            if (preview)
            {
                ipt::keyboardSendKey(VK_ESCAPE);
                logger::log(CString("验证码预览结束 ..."));
            }
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

            return true;
        }
        else if (pMsg->wParam == VK_F2)
        {
            //@todo, check state
            //m_pBrowserMy.GoBack();
            TimeManager.setServerDiff(0);
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
#if 0
            // ... @todo
            CString coordinates;
            coordinates.Format(_T("%s"), CString(time.c_str()));
            editorMy.SetWindowText(coordinates);
#endif
        }
        else if (pMsg->wParam == VK_F5)
        {
            m_pBrowserMy.Refresh();
            logger::log(CString("网页刷新 ..."));
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
            return startWorkflow();
        }
        else if (pMsg->wParam == VK_F7)
        {
            CString strURL;// ("https://paimai.alltobid.com/bid/2016111901/login.htm");
            editorMy.GetWindowText(strURL);
            m_pBrowserMy.Navigate(strURL, NULL, NULL, NULL, NULL);

            CString log(CString("访问用户指向网址："));
            log += strURL;
            logger::log(log);
        }
        else if (pMsg->wParam == VK_RETURN)
        {
            pMsg->wParam = VK_TAB;
            //m_pBrowserMy.Refresh();
            if (m_stateMachine == STATE_CAPTCHA_READY)
            {
                TimeManager.setInputDelayTimer();
                m_stateMachine = STATE_PRICE_SEND;
                logger::log(CString("电脑接管 等待出价时机..."));
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
                if (utl::ifCancelInRange(capturedPosition))
                {
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                }
                else
                {
                    cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                    if (utl::ifOkMiddleInRange(capturedPosition))
                    {
                        ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    }
                }
                m_stateMachine = STATE_NONE;
                logger::log(CString("恢复工作流初始状态..."));
            }

            //Find any cancel click, or find ok to click
            return true;
        }
    }

    return false;
}

bool Ct1Dlg::startWorkflow()
{
    // simplified error handling
    if (m_stateMachine != STATE_NONE)
    {
        logger::log(CString("工作流处于非初始状态无法强制启动 ..."));
        return true;
    }

    /// click button
    RECT rect;
    GetWindowRect(&rect);
    ipt::leftButtonClick(rect.left + PRICE_INPUT.x, rect.top + PRICE_INPUT.y);
    ipt::keyboardSendBackspaceKey(6);

    if (!m_useIntelligenceBid)
    {
        const int nIndex = m_confirmPriceAdd.GetCurSel();
        CString strCBText;
        m_confirmPriceAdd.GetLBText(nIndex, strCBText);
        m_bidUserFinalPrice = m_bidPrice + _ttoi(strCBText);
        m_useIntelligenceBid = true;

        CString log;
        log.Format(_T("启动策略出价 出价：%d 国拍当前价格为：%d 策略加价： %d", m_bidUserFinalPrice, m_bidPrice, _ttoi(strCBText)));
        logger::log(log);
    }
    else
    {
        const int intelligencePrice = prc::getIntelligencePriceBwRelease(TimeManager.getServerTime());
        m_bidUserFinalPrice = m_bidPrice + intelligencePrice;

        CString log;
        log.Format(_T("启动智能出价 出价：%d 国拍当前价格为：%d 策略加价： %d", m_bidUserFinalPrice, m_bidPrice, intelligencePrice));
        logger::log(log);
    }

    std::string predicatePrice = std::to_string(m_bidUserFinalPrice);
    ipt::keyboardSendUnicodeInput(predicatePrice);

    m_stateMachine = STATE_PRICE_INPUT;
    TimeManager.setInputDelayTimer();
    return true;
}

void Ct1Dlg::automateWorkFlow() {
    //start automatically
    if (m_stateMachine == STATE_NONE)
    {
        const int nIndex = m_confirmPriceSeconds.GetCurSel();
        CString strSecond;
        m_confirmPriceSeconds.GetLBText(nIndex, strSecond);
        const int seconds = _ttoi(strSecond);
        SYSTEMTIME server = TimeManager.getServerTime();
        const bool formal = server.wHour == 11 && server.wMinute == 29 && server.wSecond >= seconds;
        const bool preview = (m_captchaPreview.GetCheck() == BST_CHECKED) &&
                             (server.wHour == 11 && server.wMinute == 29 && server.wSecond > 10 && server.wSecond < 15);
        if (formal || preview)
        {
            startWorkflow();
            // No need to record wf time
        }
        if (preview)
        {
            staticImageCtrl.setVisible(5 /*seconds*/);
        }
    }

    if (m_stateMachine != STATE_NONE)
    {
        if (m_stateMachine == STATE_PRICE_INPUT)
        {
            ULONGLONG milliseconds = TimeManager.getInputDelayedInMilliseconds();
            if (milliseconds > (ULONGLONG)INPUT_PRICE_DELAY * 1000)
            {
                //@todo, happy flow case
                RECT rect;
                GetWindowRect(&rect);
                ipt::leftButtonClick(rect.left + PRICE_CONFIRM.x, rect.top + PRICE_CONFIRM.y);

                TimeManager.setInputDelayTimer();
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

            ULONGLONG milliseconds = TimeManager.getInputDelayedInMilliseconds();
            if (milliseconds >= INPUT_PRICE_DELAY * 1000 && m_isInUserInputStage)
            {
                //       !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                // @todo, this is a serious bug, the refresh doesn't function
                //        because it always appear after OK/CANCEL button
                //        Possible solution: add timer to iterate the finding of fresh button
                cv::Point capturedPosition = captureTemplate(BUTTON_REFRESH_FILE);
                if (utl::ifRefreshInRange(capturedPosition))
                {
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    ipt::leftButtonClick(rect.left + CAPTCHA_INPUT.x, rect.top + CAPTCHA_INPUT.y);
                    //click back to input
                    logger::log(CString("验证码已刷新..."));
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
                        if (utl::ifOkNormalInRange(capturedPosition))
                        {
                            m_okPositionWhenSending.x = capturedPosition.x;
                            m_okPositionWhenSending.y = capturedPosition.y;

                            CString log;
                            log.Format(_T("已确定确认按钮坐标：(%d, %d)", m_okPositionWhenSending.x, m_okPositionWhenSending.y));
                            logger::log(log);
                        }
                    }
                    m_stateMachine = STATE_CAPTCHA_READY;
                }

                TimeManager.setInputDelayTimer();
                m_isInUserInputStage = false;
            }
        }
        else if (m_stateMachine == STATE_PRICE_SEND)
        {
            const int seletion = m_sendPriceAdd.GetCurSel();
            CString money;
            m_sendPriceAdd.GetLBText(seletion, money);
            const int sendMoneyAdded = _ttoi(money);

            const bool acceptedPriceRange = (m_bidUserFinalPrice - m_bidPrice <= PERFORM_SEND_PRICE_POINT + sendMoneyAdded);
            const int nIndex = m_forceSendPriceTime.GetCurSel();
            CString strSecond;
            m_forceSendPriceTime.GetLBText(nIndex, strSecond);
            const float seconds = _ttof(strSecond);

            ULONGLONG tmp = TimeManager.timeLeftInMilliseconds();
            const bool deadlineArrived = (tmp < (ULONGLONG)seconds * 1000);
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

                TimeManager.setInputDelayTimer();
                m_stateMachine = STATE_PRICE_RESULT;

                CString log;
                log.Format(_T("正在出价 ... 国拍服务器当前价格：%d 最终出价：%d 出价原因：", m_bidPrice, m_bidUserFinalPrice));
                if (acceptedPriceRange)
                {
                    log += _T("已进入出价区间（100余量）");
                }
                if (deadlineArrived)
                {
                    log += _T("临近出价截止时间还有 %ll毫秒", TimeManager.timeLeftInMilliseconds());
                }
                //log = _T("正在出价 ") + log;
                logger::log(log);
            }
        }
        else if (m_stateMachine == STATE_PRICE_RESULT)
        {
            ULONGLONG milliseconds = TimeManager.getInputDelayedInMilliseconds();
            if (milliseconds > (ULONGLONG)INPUT_PRICE_DELAY * 1000)
            {
                //precise match OK button, to make sure no dialog
                RECT rect;
                GetWindowRect(&rect);
                cv::Point capturedPosition = captureTemplate(BUTTON_OK_FILE);
                if (utl::ifOkMiddleInRange(capturedPosition))
                {
                    ipt::leftButtonClick(rect.left + capturedPosition.x, rect.top + capturedPosition.y);
                    m_stateMachine = STATE_NONE;

                    logger::log(CString("已获取出价返回结果"));
                }

                TimeManager.setInputDelayTimer();
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
    /*const cv::Point templateArea1(454, 228);
    const cv::Point templateArea2(904, 660);
    RECT rect;
    GetWindowRect(&rect);*/

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
    cv::Mat dialogShot = img::hSmallwnd2mat(WindowFromDC(GetDC()->m_hDC), DIALOG_FRAME_LEFT_WIDTH, DIALOG_FRAME_TOP_HEIGHT);

    //@todo, there will be a crash if do NOT save to local disk
    int res = remove(DIALOG_CAPTURE_TMP.c_str());
    cv::imwrite(DIALOG_CAPTURE_TMP, dialogShot);
    cv::Mat img = cv::imread(DIALOG_CAPTURE_TMP, 1);
    remove(DIALOG_CAPTURE_TMP.c_str());

    //cv::Mat templ = cv::imread(templateFile, 1);
    //replace imread to get Mat from stream
    std::ifstream templateStream(templateFile, std::ios::binary);
    std::vector<char> fileContents((std::istreambuf_iterator<char>(templateStream)),
        std::istreambuf_iterator<char>());
    cv::Mat templ = cv::imdecode(cv::Mat(fileContents), 1);

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
    return  cv::Point(TEMPLATE_AREA_LEFT.x + matchLoc.x + templ.cols / 2, TEMPLATE_AREA_LEFT.y + matchLoc.y + templ.rows / 2);
}

void Ct1Dlg::OnCbnSelchangeComboMode()
{
    // TODO: Add your control notification handler code here
    const int nIndex = m_webMode.GetCurSel();
    if (nIndex == 0)
    {
        CString strURL("http://moni.51hupai.org/");
        m_pBrowserMy.Navigate(strURL, NULL, NULL, NULL, NULL);
    }
    else if (nIndex == 1)
    {
        CString strURL("http://test.alltobid.com/");
        m_pBrowserMy.Navigate(strURL, NULL, NULL, NULL, NULL);
    }
    else if (nIndex == 2)
    {
        CString strURL("https://paimai.alltobid.com/bid/2016111901/login.htm");
        m_pBrowserMy.Navigate(strURL, NULL, NULL, NULL, NULL);
    }
    else
    {
        CString strURL("http://www.baidu.com/");
        m_pBrowserMy.Navigate(strURL, NULL, NULL, NULL, NULL);
    }
}