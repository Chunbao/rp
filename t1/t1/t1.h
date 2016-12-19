
// t1.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols
#include <memory>

class Ct1Dlg;


// Ct1App:
// See t1.cpp for the implementation of this class
//

class Ct1App : public CWinAppEx
{
public:
	Ct1App();

// Overrides
	public:
	virtual BOOL InitInstance();
	//BOOL PreTranslateMessage(MSG* pMsg);

// Implementation

	DECLARE_MESSAGE_MAP()

private:
	std::shared_ptr<Ct1Dlg> m_mainDialog;
};

extern Ct1App theApp;