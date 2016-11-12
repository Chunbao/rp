
// t1Dlg.h : header file
//

#pragma once

#include "CWebBrowser2.h"
#include "afxwin.h"

#include <opencv2\core\core.hpp>
#include <string>
#include <chrono>

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
//https://msdn.microsoft.com/en-us/library/aa752046(v=vs.85).aspx

// tesseract
// http://stackoverflow.com/questions/18180824/how-to-implement-tesseract-to-run-with-project-in-visual-studio-2010
// http://stackoverflow.com/questions/22924209/how-to-make-tesseract-ocr-read-from-coordinates-on-a-screen

//compiled lib
// https://sourceforge.net/projects/tesseract-ocr-alt/files/

//developer guide
//t1/3rd/tesseract-3.02.02/vs2008/doc/programming.html

/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */


// Ct1Dlg dialog
class Ct1Dlg : public CDHtmlDialog
{
// Construction
public:
	Ct1Dlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_T1_DIALOG, IDH = IDR_HTML_T1_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

	HRESULT OnButtonOK(IHTMLElement *pElement);
	HRESULT OnButtonCancel(IHTMLElement *pElement);
	HRESULT OnButtonHelp(IHTMLElement *pElement);

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	
private:
    bool manageUserEvent(MSG* pMsg);
    void automateWorkFlow();

    std::string captureText(int relativeLeft, int relativeTop, int relativeRight, int relativeBottom);
    std::string captureEnhancedText(std::string enhancedFile);

    cv::Point captureTemplate(const std::string& templ);

    void performPriceRecognition();

public:

    afx_msg void OnBnClickedButtonRefresh();
	BOOL Ct1Dlg::PreTranslateMessage(MSG* pMsg);


private:
	CWebBrowser2 m_pBrowserMy;
    // 1
    CEdit editorMy;
	// display information pannel
	CEdit infoPanelEditor;

    time_t m_workFlowTimer;

    cv::Point m_okPositionWhenSending;
    int m_bidUserFinalPrice; // user set
    int m_bidPrice; // realtime
    std::chrono::high_resolution_clock::time_point m_priceTimer;

    // flag to remember input start, use enum 3 states if necessary
    //bool inputFinish;

    //flag to stop captureTemplate from executing
    bool m_isInUserInputStage;

    // in seconds, +1 if server is 1 second faster than local
    int m_timeDiff;

    enum STATE_MACHINE
    {
        STATE_NONE,
        STATE_PRICE_INPUT,
        STATE_PRICE_CONFIRM,
        STATE_PRICE_SEND,
        STATE_PRICE_RESULT
    };
    STATE_MACHINE m_stateMachine;



    // // in which seconds the price is fixed
    CComboBox m_confirmPriceSeconds;
    // How much extra money to add based on you time
    CComboBox m_confirmPriceAdd;
};
