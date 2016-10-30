
// t1Dlg.cpp : implementation file
//

#include "stdafx.h"
#include "t1.h"
#include "t1Dlg.h"

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

//using namespace std;
//using namespace cv;

//price
const int RELATIVE_LEFT = 750;
const int RELATIVE_TOP = 335;
const int RELATIVE_RIGHT = 809;
const int RELATIVE_BOTTOM = 357;

//@todo, need to be replaced for compatiable problems
//@todo, use getWindowRect instead of getClientRect in Image
const int DIALOG_FRAME_LEFT_WIDTH = 8;
const int DIALOG_FRAME_TOP_HEIGHT = 30;

//time
//const int RELATIVE_LEFT = 266;
//const int RELATIVE_TOP = 482;
//const int RELATIVE_RIGHT = 312;
//const int RELATIVE_BOTTOM = 495;


class Image
{
private:
	std::vector<std::uint8_t> Pixels;
	std::uint32_t width, height;
	std::uint16_t BitsPerPixel;

	void Flip(void* In, void* Out, int width, int height, unsigned int Bpp);

public:
#if defined _WIN32 || defined _WIN64
	explicit Image(HDC DC, int X, int Y, int Width, int Height);
#endif

	inline std::uint16_t GetBitsPerPixel() { return this->BitsPerPixel; }
	inline std::uint16_t GetBytesPerPixel() { return this->BitsPerPixel / 8; }
	inline std::uint16_t GetBytesPerScanLine() { return (this->BitsPerPixel / 8) * this->width; }
	inline int GetWidth() const { return this->width; }
	inline int GetHeight() const { return this->height; }
	inline const std::uint8_t* GetPixels() { return this->Pixels.data(); }
};

void Image::Flip(void* In, void* Out, int width, int height, unsigned int Bpp)
{
	unsigned long Chunk = (Bpp > 24 ? width * 4 : width * 3 + width % 4);
	unsigned char* Destination = static_cast<unsigned char*>(Out);
	unsigned char* Source = static_cast<unsigned char*>(In) + Chunk * (height - 1);

	while (Source != In)
	{
		std::memcpy(Destination, Source, Chunk);
		Destination += Chunk;
		Source -= Chunk;
	}
}

#if defined _WIN32 || defined _WIN64
Image::Image(HDC DC, int X, int Y, int Width, int Height) : Pixels(), width(Width), height(Height), BitsPerPixel(32)
{
	BITMAP Bmp = { 0 };
	HBITMAP hBmp = reinterpret_cast<HBITMAP>(GetCurrentObject(DC, OBJ_BITMAP));

	if (GetObject(hBmp, sizeof(BITMAP), &Bmp) == 0)
		throw std::runtime_error("BITMAP DC NOT FOUND.");

	RECT area = { X, Y, X + Width, Y + Height };
	HWND Window = WindowFromDC(DC);
	GetClientRect(Window, &area);

	HDC MemDC = GetDC(nullptr);
	HDC SDC = CreateCompatibleDC(MemDC);
	HBITMAP hSBmp = CreateCompatibleBitmap(MemDC, width, height);
	DeleteObject(SelectObject(SDC, hSBmp));

	BitBlt(SDC, 0, 0, width, height, DC, X, Y, SRCCOPY);
	unsigned int data_size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
	std::vector<std::uint8_t> Data(data_size);
	this->Pixels.resize(data_size);

	BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(width), static_cast<long>(height), 1, BitsPerPixel, BI_RGB, data_size, 0, 0, 0, 0 };
	GetDIBits(SDC, hSBmp, 0, height, &Data[0], &Info, DIB_RGB_COLORS);
	this->Flip(&Data[0], &Pixels[0], width, height, BitsPerPixel);

	DeleteDC(SDC);
	DeleteObject(hSBmp);
	ReleaseDC(nullptr, MemDC);
}
#endif



void GetScreenShot(int relativeWidth, int relativeHeight, int absoluteLeft, int absoluteTop)
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
    w = x2 - x1;
    h = y2 - y1;

    // copy screen to bitmap
    //HDC     hScreen = GetDC(NULL);
    HDC     hScreen = h1;
    HDC     hDC = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

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


//@todo, the screen got is black
cv::Mat hwnd2mat(HWND hwnd) {

    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    cv::Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;

    //height = windowsize.bottom / 2;  //change this to whatever size you want to resize to
    //width = windowsize.right / 2;
    height = windowsize.bottom/* - windowsize.top*/;  //change this to whatever size you want to resize to
    width = windowsize.right/* - windowsize.left*/;

    //src.create(height, width, CV_8UC4);
    src.create(height, width, CV_32FC1);

    // create a bitmap
    hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
    bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
    bi.biWidth = width;
    bi.biHeight = -height;  //this is the line that makes it draw upside down or not
    bi.biPlanes = 1;
    bi.biBitCount = 32;

    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrUsed = 0;
    bi.biClrImportant = 0;

    // use the previously created device context with the bitmap
    SelectObject(hwindowCompatibleDC, hbwindow);
    
    // copy from the window device context to the bitmap device context
    //StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, width, height, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

                                                                                                       // avoid memory leak
    DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

    return src;
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
        CString strURL("http://test.alltobid.com/");
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
	sprintf(systemTime, "%4d/%02d/%02d %02d:%02d:%02d.%03d ÐÇÆÚ%1d\n", sys_time.wYear,
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
			m_pBrowserMy.Refresh();
		}
		else if (pMsg->wParam == VK_F5)
		{
			m_pBrowserMy.Refresh();
		}
		else if (pMsg->wParam == VK_F4)
		{
			RECT rect;
			GetWindowRect(&rect);
			char coordinates[60];
			//sprintf(coordinates, "%d, %d, %d, %d",rect.left, rect.top, rect.right, rect.bottom);

			LONG horizonCoordinate = rect.left;
			LONG verticalCoordinate = rect.top;

			LONG RELATIVE_HORIZON_DISTANCE = pMsg->pt.x - horizonCoordinate;
			LONG RELATIVE_VERTICAL_DISTANCE = pMsg->pt.y - verticalCoordinate;

            /// Create windows
            std::string image_window("XXXXXX");
            cv::namedWindow(image_window, CV_WINDOW_AUTOSIZE);
            //namedWindow(result_window, CV_WINDOW_AUTOSIZE);
            /// Create Trackbar
            int match_method(0);
            char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
            cv::createTrackbar(trackbar_label, image_window, &match_method, 5, nullptr);

            //cv::Point capturedPosition = captureTemplate("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t2-cpp11\\testdata\\cancel.png");
            cv::Point capturedPosition(0, 0);
            //display result
			sprintf(coordinates, "%d, %d, %d, %d, %s, %d, %d", RELATIVE_LEFT,RELATIVE_TOP,RELATIVE_RIGHT,RELATIVE_BOTTOM,
				captureText(RELATIVE_LEFT, RELATIVE_TOP, RELATIVE_RIGHT, RELATIVE_BOTTOM).c_str(), capturedPosition.x, capturedPosition.y);
			editorMy.SetWindowTextA(coordinates);
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
	RECT rect;
	GetWindowRect(&rect);
	
#if defined _WIN32 || defined _WIN64
	
	//HWND SomeWindowHandle = GetDesktopWindow();
	HWND SomeWindowHandle = GetDesktopWindow()->m_hWnd;
	//HDC DC = GetDC(SomeWindowHandle);
	HDC DC = GetDC()->m_hDC;
    
    
    // @todo, current dialog's hDC (image area)
    //GetScreenShotDialog(DC);
    //return std::string("test");


    // @todo, fix offset caused by dialog title and frame area (width=8,height=30)
	Image Img = Image(DC, relativeLeft - DIALOG_FRAME_LEFT_WIDTH, relativeTop - DIALOG_FRAME_TOP_HEIGHT, 
                      relativeRight - relativeLeft, relativeBottom - relativeTop); //screenshot of 0, 0, 200, 200..
	
	//ReleaseDC(SomeWindowHandle, DC);
	ReleaseDC(GetDC());
#else
	Image Img = Image(some_pixel_pointer, 200, 200); //pointer to pixels..
#endif
	

	
    // For testing purpose
	//GetScreenShot(relativeRight - relativeLeft, relativeBottom - relativeTop, rect.left + relativeLeft, rect.top + relativeTop);

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
    cv::Mat dialogShot = hwnd2mat(WindowFromDC(GetDC()->m_hDC));
    //@todo, make sure file is removed beforehand. Or in memory.
    //                                             solve the Mat.type() dismatch
    cv::imwrite("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t2-cpp11\\testdata\\Capture_tmp.jpg", dialogShot);
    cv::Mat img = cv::imread("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t2-cpp11\\testdata\\Capture_tmp.jpg", 1);
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
    return  cv::Point(matchLoc.x + templ.cols/2, matchLoc.y + templ.rows/2);
}