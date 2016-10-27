
// t1Dlg.h : header file
//

#pragma once

#include "CWebBrowser2.h"


/* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
//https://msdn.microsoft.com/en-us/library/aa752046(v=vs.85).aspx
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
	
//private:
//    CWebBrowser2* m_pBrowser;	
public:
    CWebBrowser2 m_pBrowserMy;
    afx_msg void OnBnClickedButtonRefresh();
};
