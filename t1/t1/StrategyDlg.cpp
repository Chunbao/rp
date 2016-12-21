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
	DDX_Control(pDX, IDC_STATIC_RECOGNIZED_PRICE, m_recognizedPrice);
	DDX_Control(pDX, IDC_STATIC_INTELLIGENT_PRICE, m_intelligentPrice);
	DDX_Control(pDX, IDC_COMBO_STRATEGY, m_strategySelection);
	DDX_Control(pDX, IDC_EDIT_SECOND_MINUTE, m_secondMinute);
	DDX_Control(pDX, IDC_EDIT_SECOND_SECOND, m_secondSecond);
	DDX_Control(pDX, IDC_EDIT_SECOND_ADD, m_secondAdd);
	DDX_Control(pDX, IDC_COMBO_SECOND_SEND, m_secondSend);
	DDX_Control(pDX, IDC_EDIT_SECOND_SEND_TIME, m_secondSendTime);
	DDX_Control(pDX, IDC_EDIT_THIRD_MINUTE, m_thirdMinute);
	DDX_Control(pDX, IDC_EDIT_THIRD_SECOND, m_thirdSecond);
	DDX_Control(pDX, IDC_EDIT_THIRD_ADDED, m_thirdAdd);
	DDX_Control(pDX, IDC_COMBO_THIRD_SEND, m_thirdSend);
	DDX_Control(pDX, IDC_EDIT_THIRD_SEND_TIME, m_thirdSendTime);
}

BOOL CStrategyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_strategySelection.SetCurSel(0);
	m_secondMinute.SetWindowTextW(_T("29"));
	m_secondSecond.SetWindowTextW(_T("40"));
	m_secondAdd.SetWindowTextW(_T("1100"));
	m_secondSend.SetCurSel(1);
	m_secondSendTime.SetWindowTextW(_T("0.0"));

	m_thirdMinute.SetWindowTextW(_T("29"));
	m_thirdSecond.SetWindowTextW(_T("48"));
	m_thirdAdd.SetWindowTextW(_T("700"));
	m_thirdSend.SetCurSel(1);
	m_thirdSendTime.SetWindowTextW(_T("0.0"));

	return TRUE;
}


BEGIN_MESSAGE_MAP(CStrategyDlg, CDialogEx)
END_MESSAGE_MAP()


// CStrategyDlg message handlers
