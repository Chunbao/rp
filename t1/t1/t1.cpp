
// t1.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "t1.h"
#include "t1Dlg.h"
#include "CWebBrowser2.h"
#include "StaticCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif



// Ct1App

BEGIN_MESSAGE_MAP(Ct1App, CWinAppEx)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// Ct1App construction

Ct1App::Ct1App()
{
	EnableHtmlHelp();

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only Ct1App object

Ct1App theApp;


// Ct1App initialization

BOOL Ct1App::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

    //net::HttpServer server;
    //if (!server.connectServer())
    //{
    //    MessageBox(
    //        NULL,
    //        (LPCWSTR)L"链接服务器异常\n程序即将退出",
    //        (LPCWSTR)L"授权信息！",
    //        MB_ICONERROR | MB_OK
    //        );
    //    return false;
    //}
/*
    if (!server.matchKey())
    {
        MessageBox(
            NULL,
            (LPCWSTR)L"服务器验证未通过\n程序即将退出",
            (LPCWSTR)L"授权信息！",
            MB_ICONERROR | MB_OK
            );
        return false;
    }
    else {
        MessageBox(
            NULL,
            (LPCWSTR)L"授权验证通过\n程序即将启动...",
            (LPCWSTR)L"授权信息！",
            MB_ICONINFORMATION | MB_OK
            );
    }
*/
	Ct1Dlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

/*
BOOL Ct1App::PreTranslateMessage(MSG* pMsg)
{

	return CWinApp::PreTranslateMessage(pMsg);
}*/
