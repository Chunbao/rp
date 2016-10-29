
// t1Dlg.h : header file
//

#pragma once

#include "CWebBrowser2.h"
#include "afxwin.h"

#include <string>


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
    std::string captureText(int x1, int y1, int x2, int y2);


public:

    afx_msg void OnBnClickedButtonRefresh();
	BOOL Ct1Dlg::PreTranslateMessage(MSG* pMsg);


private:
	CWebBrowser2 m_pBrowserMy;
    // 1
    CEdit editorMy;
	// display information pannel
	CEdit infoPanelEditor;
};
