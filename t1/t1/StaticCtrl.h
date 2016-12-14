#pragma once
#include <chrono>
#include <Windows.h>
#include <string>

class StaticImageCtrl
{
public:
    StaticImageCtrl(CWnd* cWnd, HBITMAP hBitMap);
    StaticImageCtrl();
    ~StaticImageCtrl();

    bool isTimerWorking() const;
    bool isCaptchaWorking() const;
    bool setVisible(int seconds, CStatic* cWnd = nullptr);
    bool setInvisibleIfTimeIsup();

private:
    std::chrono::high_resolution_clock::time_point m_startVisible;
    int m_visibleSeconds;

    CStatic* m_pCStatic_A;
    bool m_timerWorking;
    bool m_captchaWorking;
};


namespace logger
{
    void log(CString msg);
}

namespace net
{
    class HttpServer
    {
    public:
        HttpServer();

        bool HttpServer::connectServer();
        bool matchKey();
        std::string getAnswer(std::string path);

    private:
        std::string loadPage(std::string uriPath);

        const std::string serverHost;
        const std::string pathKey;
        const std::string pathAnswer;
    };
}