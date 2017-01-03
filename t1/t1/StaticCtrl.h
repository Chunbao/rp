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

namespace tim
{
    class TimeManager
    {
    public:
        TimeManager()
            : m_lastExecuted(0)
            , m_serverDiff(0)
        {}

        SYSTEMTIME getServerTime()
        {
            SYSTEMTIME      stLocal;
            FILETIME        ftLocal;
            ULARGE_INTEGER  uli;

            GetLocalTime(&stLocal);
            SystemTimeToFileTime(&stLocal, &ftLocal);

            //ftLocal.dwLowDateTime += (DWORD)m_serverDiff;

            uli.LowPart = ftLocal.dwLowDateTime;
            uli.HighPart = ftLocal.dwHighDateTime;

            uli.QuadPart -= m_serverDiff;
            ftLocal.dwLowDateTime = uli.LowPart;
            ftLocal.dwHighDateTime = uli.HighPart;

            FileTimeToSystemTime(&ftLocal, &stLocal);

            return stLocal;
        }

        SYSTEMTIME getLocalTime()
        {
            SYSTEMTIME stLocal;
            GetLocalTime(&stLocal);

            return stLocal;
        }

        ULONGLONG getGivenTimePoint(int hour, int minute, int second, int milli)
        {
            SYSTEMTIME      stLocal;
            FILETIME        ftLocal;
            ULARGE_INTEGER  uli;

            GetLocalTime(&stLocal);
            stLocal.wHour = hour;
            stLocal.wMinute = minute;
            stLocal.wSecond = second;
            stLocal.wMilliseconds = milli;

            SystemTimeToFileTime(&stLocal, &ftLocal);
            uli.LowPart = ftLocal.dwLowDateTime;
            uli.HighPart = ftLocal.dwHighDateTime;

            return uli.QuadPart;
        }

        // relative to local time
        void setServerDiff(LONGLONG timeDiff)
        {
            m_serverDiff = timeDiff;
        }

        LONGLONG getServerDiff()
        {
            return m_serverDiff;
        }

        ULONGLONG getFreeMilliSeconds()
        {
            return (currentTime() - m_lastExecuted)/10000;
        }

        void setLastExecutedTimePoint()
        {
            m_lastExecuted = currentTime();
        }

        ULONGLONG currentTime()
        {
            SYSTEMTIME      stLocal;
            FILETIME        ftLocal;
            ULARGE_INTEGER  uli;

            GetLocalTime(&stLocal);
            SystemTimeToFileTime(&stLocal, &ftLocal);
            uli.LowPart = ftLocal.dwLowDateTime;
            uli.HighPart = ftLocal.dwHighDateTime;

            return uli.QuadPart;
        }

        ULONGLONG timeLeftInMilliseconds()
        {
            return (getGivenTimePoint(11, 30, 0, 0) - currentTime())/10000;
        }

    private:

        ULONGLONG m_lastExecuted; // m_priceTimer

        LONGLONG m_serverDiff; // m_timeDiff
    };
}