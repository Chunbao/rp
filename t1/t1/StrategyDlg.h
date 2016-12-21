#pragma once
#include "afxwin.h"


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
	//https://www.codeproject.com/articles/1651/tutorial-modeless-dialogs-with-mfc

// Dialog Data
//#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG_Strategy };
//#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CStatic m_recognizedPrice;
	CStatic m_intelligentPrice;
	CStatic m_ocrTime;
	CStatic m_localTime;
	CComboBox m_strategySelection;
	CEdit m_secondMinute;
	CEdit m_secondSecond;
	CEdit m_secondAdd;
	CComboBox m_secondSend;
	CEdit m_secondSendTime;

	CEdit m_thirdMinute;
	CEdit m_thirdSecond;
	CEdit m_thirdAdd;
	CComboBox m_thirdSend;
	CEdit m_thirdSendTime;
};
