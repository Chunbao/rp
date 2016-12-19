#pragma once


// CStrategyDlg dialog

class CStrategyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CStrategyDlg)

public:
	CStrategyDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CStrategyDlg();

	void OnCancel();
	void OnOK();
	void PostNcDestroy();

// Dialog Data
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Strategy };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
