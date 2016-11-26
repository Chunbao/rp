#pragma once
#include <chrono>
#include <Windows.h>

class StaticImageCtrl
{
public:
    StaticImageCtrl(CWnd* cWnd, HBITMAP hBitMap);
    StaticImageCtrl();
    ~StaticImageCtrl();

    bool isWorking() const;
    bool setVisible(int seconds, CStatic* cWnd);
    bool setInvisibleIfTimeIsup();

private:
    std::chrono::high_resolution_clock::time_point m_startVisible;
    int m_visibleSeconds;

    CStatic* m_pCStatic_A;
};

