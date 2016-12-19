// StrategyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "t1.h"
#include "StrategyDlg.h"
#include "afxdialogex.h"


// CStrategyDlg dialog

IMPLEMENT_DYNAMIC(CStrategyDlg, CDialogEx)

CStrategyDlg::CStrategyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG_Strategy, pParent)
{

}

CStrategyDlg::~CStrategyDlg()
{
}

void CStrategyDlg::OnCancel()
{
	// CDialog::OnCancel();  // MUST remove this line
	DestroyWindow();
}

void CStrategyDlg::OnOK()
{
	// CDialog::OnOK(); // MUST remove this line
	DestroyWindow();
}

void CStrategyDlg::PostNcDestroy()
{
	delete this;
}

void CStrategyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CStrategyDlg, CDialogEx)
END_MESSAGE_MAP()


// CStrategyDlg message handlers
