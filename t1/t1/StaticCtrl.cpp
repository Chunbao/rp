#include "stdafx.h"
#include "StaticCtrl.h"
#include <iostream>
#include <fstream>

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
    , m_timerWorking(false)
    , m_captchaWorking(false)
{
}

StaticImageCtrl::~StaticImageCtrl()
{
    //delete m_pCStatic_A;
}

bool StaticImageCtrl::isTimerWorking() const
{
    return m_timerWorking;
}

bool StaticImageCtrl::isCaptchaWorking() const
{
    return m_captchaWorking;
}

bool StaticImageCtrl::setVisible(int seconds, CStatic* cWnd)
{
    m_visibleSeconds = seconds;
    m_startVisible = std::chrono::high_resolution_clock::now();
    m_timerWorking = true;

    if (cWnd)
    {
        m_pCStatic_A = cWnd;
        m_captchaWorking = true;
        m_pCStatic_A->ShowWindow(SW_SHOW);
        logger::log(CString("验证码放大倒计时开始 ..."));
    }
    return true;
}

bool StaticImageCtrl::setInvisibleIfTimeIsup()
{
    std::chrono::high_resolution_clock::time_point now(std::chrono::high_resolution_clock::now());
    std::chrono::seconds elapsed =
        std::chrono::duration_cast<std::chrono::seconds>(now - m_startVisible);
    if (elapsed.count() >= m_visibleSeconds)
    {
        if (m_pCStatic_A)
        {
            m_pCStatic_A->ShowWindow(SW_HIDE);
            m_pCStatic_A = nullptr;
            logger::log(CString("验证码放大功能结束 ..."));
        }
        
        m_timerWorking = false;
        m_captchaWorking = false;
        return true;
    }
    return false;
}


namespace logger
{
    void log(CString msg)
    {
        try
        {
            CString msLine;
            CTime tt = CTime::GetCurrentTime();

            msLine = tt.Format("[%H:%M:%S] ") + msg;
            //msLine += "/n";

            std::wofstream myfile;
            myfile.open("log.txt", std::ios::out | std::ios::app);
            myfile << (LPCWSTR)msLine << "\n";
            myfile.close();
        }
        catch (CFileException *fx)
        {
            fx->Delete();
        }
    }
}

namespace net
{
    HttpServer::HttpServer()
        : serverHost("guopai.duapp.com")
        , pathKey("/tqz0T0g5i03Vudm97UDhqx3a187U02t7084Wm310zX20BB2Fy2F1dKDv7F79YJ394545II9T4p7dp5D4QjmSa4tG775C3AXG5B623g8l6Jl1191sm3sn5x9lAi348dXp")
        , pathAnswer("69866i1a956r1K0Su7169Z98464z60683157w67nsg2L201y9og4c991Rj4B0HRw")
    {
    }

    bool HttpServer::connectServer()
    {
        try {
            getAnswer(pathKey);
        }
        catch (const std::exception& e)
        {
            return false;
        }
        return true;
    }

    bool HttpServer::matchKey()
    {
        std::string answer;
        try {
            answer == getAnswer(pathKey);
        }
        catch (const std::exception& e)
        {
            return false;
        }
        return pathAnswer == getAnswer(pathKey);
    }

    // Removes "key===
    // and Removes newline characters "\t" and "\n"
    std::string HttpServer::getAnswer(std::string path)
    {
        std::string page = loadPage(path);

        std::string keyWord = path.erase(0, 1) + "===";
        std::string::size_type n = page.find(keyWord);
        if (n != std::string::npos)
        {
            // Removes "key===
            std::string ret = page.substr(n + keyWord.length());

            // Removes newline characters "\t" and "\n"
            std::string::size_type pos = 0;
            while (pos < ret.length()) {
                pos = ret.find('\n');
                if (pos == std::string::npos) {
                    break;
                }
                ret.erase(pos);
            }
            pos = 0;
            while (pos < ret.length()) {
                pos = ret.find('\r');
                if (pos == std::string::npos) {
                    break;
                }
                ret.erase(pos);
            }

            return ret;
        }
        return std::string();
    }

    // Support only page size smaller than 10000, otherwise process the last one
    std::string HttpServer::loadPage(std::string uriPath)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            //cout << "WSAStartup failed.\n";
            //system("pause");
            return std::string();
        }
        SOCKET Socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        struct hostent *host;
        host = gethostbyname(serverHost.c_str());
        if (!host)
        {
            throw std::exception("e");
        }

        SOCKADDR_IN SockAddr;
        SockAddr.sin_port = htons(80);
        SockAddr.sin_family = AF_INET;
        SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
        //cout << "Connecting...\n";
        if (connect(Socket, (SOCKADDR*)(&SockAddr), sizeof(SockAddr)) != 0) {
            //cout << "Could not connect";
            //system("pause");
            return std::string();
        }
        //cout << "Connected.\n";
        //const std::string uriPath("/test ");
        const std::string httpRequest = "GET " + uriPath
            + " HTTP/1.1\r\nHost: "
            + serverHost
            + "\r\nConnection: close\r\n\r\n";
        send(Socket, httpRequest.c_str(), httpRequest.length(), 0);
        char buffer[10000];
        int nDataLength;
        while ((nDataLength = recv(Socket, buffer, 10000, 0)) > 0) {
            /*int i = 0;
            while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            cout << buffer[i];
            i += 1;
            }*/
        }
        closesocket(Socket);
        WSACleanup();
        return std::string(buffer);
    }

}