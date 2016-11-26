#include "stdafx.h"
#include "StaticCtrl.h"


StaticImageCtrl::StaticImageCtrl(CWnd* cWnd, HBITMAP hBitMap)
{
    m_pCStatic_A = new CStatic;
    m_pCStatic_A->Create(_T("A bitmap static control (A)"),
        WS_CHILD | WS_BORDER | WS_VISIBLE | SS_BITMAP | SS_CENTERIMAGE, CRect(0, 0, 224, 98),
        cWnd);
    //CBmp.LoadOEMBitmap(OBM_CLOSE);  // Loads one of the default Windows bitmaps
    m_pCStatic_A->SetBitmap(/*HBITMAP(CBmp)*/hBitMap);

}

StaticImageCtrl::StaticImageCtrl()
    : m_pCStatic_A(nullptr)
{
}

StaticImageCtrl::~StaticImageCtrl()
{
    //delete m_pCStatic_A;
}

bool StaticImageCtrl::isWorking() const
{
    return m_pCStatic_A;
}

bool StaticImageCtrl::setVisible(int seconds, CStatic* cWnd)
{
    m_visibleSeconds = seconds;
    m_startVisible = std::chrono::high_resolution_clock::now();
    m_pCStatic_A = cWnd;
    m_pCStatic_A->ShowWindow(SW_SHOW);
    return true;
}

bool StaticImageCtrl::setInvisibleIfTimeIsup()
{
    std::chrono::high_resolution_clock::time_point now(std::chrono::high_resolution_clock::now());
    std::chrono::seconds elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_startVisible);
    if (m_pCStatic_A && elapsed.count() >= m_visibleSeconds)
    {
        m_pCStatic_A->ShowWindow(SW_HIDE);
        m_pCStatic_A = nullptr;
        return true;
    }
    return false;
}