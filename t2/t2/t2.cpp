// t2.cpp : Defines the entry point for the console application.
//
/*
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

*/
// t2.cpp : Defines the entry point for the console application.
//
/*
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

*/
/*
#include <tesseract\baseapi.h>
#include <leptonica\allheaders.h>
#include <iostream>


#include <vector>
#include <windef.h>
#include <WinBase.h>

using namespace std;


void GetScreenShot(HDC h1)
{
    int x1, y1, x2, y2, w, h;

    // get screen dimensions
    x1  = GetSystemMetrics(SM_XVIRTUALSCREEN);
    y1  = GetSystemMetrics(SM_YVIRTUALSCREEN);
    x2  = GetSystemMetrics(SM_CXVIRTUALSCREEN);
    y2  = GetSystemMetrics(SM_CYVIRTUALSCREEN);
    w   = x2 - x1;
    h   = y2 - y1;

    // copy screen to bitmap
    //HDC     hScreen = GetDC(NULL);
    HDC     hScreen = h1;
    HDC     hDC     = CreateCompatibleDC(hScreen);
    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BOOL    bRet    = BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    // save bitmap to clipboard
    OpenClipboard(NULL);
    EmptyClipboard();
    SetClipboardData(CF_BITMAP, hBitmap);
    CloseClipboard();   

    // clean up
    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}

int main(void){

    GetScreenShot(GetDC(NULL));

    system("pause");
}*/


//http://stackoverflow.com/questions/389069/programmatically-reading-a-web-page
//http://www.cplusplus.com/forum/windows/91470/
#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <string>

#include <string>
#include <sstream>
#include <vector>

#pragma comment(lib,"ws2_32.lib")
using namespace std;

class HttpServer
{
public:
    HttpServer()
    : serverHost("guopai.duapp.com")
    {
    
    }
    
    std::string getAnswer(std::string path)
    {
        std::string page = loadPage(path);
        //filter page
        // sucess return answer
        // fail return std::string();
        std::vector<std::string> lines = split(page, "\r\n");
        std::string keyWord = page.erase(0, 1) + "===";
        for (UINT i = 0; i < lines.size(); ++i)
        {
            
        }
        //return loadPage(path);
    }

private:


    void split(const std::string &s, char delim, std::vector<std::string> &elems) {
        std::stringstream ss;
        ss.str(s);
        std::string item;
        while (std::getline(ss, item, delim)) {
            elems.push_back(item);
        }
    }
    
    std::vector<std::string> split(const std::string &s, char delim) {
        std::vector<std::string> elems;
        split(s, delim, elems);
        return elems;
    }

    std::string loadPage(std::string uriPath)
    {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
            cout << "WSAStartup failed.\n";
            system("pause");
            return std::string();
        }
        SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
        struct hostent *host;
        host = gethostbyname(serverHost.c_str());
        SOCKADDR_IN SockAddr;
        SockAddr.sin_port=htons(80);
        SockAddr.sin_family=AF_INET;
        SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
        cout << "Connecting...\n";
        if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
            cout << "Could not connect";
            system("pause");
            return std::string();
        }
        cout << "Connected.\n";
        //const std::string uriPath("/test ");
        const std::string httpRequest = "GET " + uriPath 
                                        + " HTTP/1.1\r\nHost: " 
                                        + serverHost 
                                        + "\r\nConnection: close\r\n\r\n";
        send(Socket, httpRequest.c_str(), httpRequest.length(),0);
        char buffer[10000];
        int nDataLength;
        while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){        
            int i = 0;
            while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
                cout << buffer[i];
                i += 1;
            }
        }
        closesocket(Socket);
        WSACleanup();
        return std::string(buffer);
    }
    
    std::string serverHost;    
};

int main ()
{
    HttpServer server;
    ;
    std::cout << "Raw data" << std::endl;
    std::cout << server.getAnswer("/test ") << std::endl;
    system("pause");
    return 0;
}

#if 0
int main (){
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2,2), &wsaData) != 0) {
        cout << "WSAStartup failed.\n";
        system("pause");
        return 1;
    }
    SOCKET Socket=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);
    struct hostent *host;
    host = gethostbyname("guopai.duapp.com");
    SOCKADDR_IN SockAddr;
    SockAddr.sin_port=htons(80);
    SockAddr.sin_family=AF_INET;
    SockAddr.sin_addr.s_addr = *((unsigned long*)host->h_addr);
    cout << "Connecting...\n";
    if(connect(Socket,(SOCKADDR*)(&SockAddr),sizeof(SockAddr)) != 0){
        cout << "Could not connect";
        system("pause");
        return 1;
    }
    cout << "Connected.\n";
    const std::string uriPath("/test ");
    const std::string httpRequest = "GET " + uriPath + "HTTP/1.1\r\nHost: guopai.duapp.com\r\nConnection: close\r\n\r\n";
    send(Socket, httpRequest.c_str(), httpRequest.length(),0);
    char buffer[10000];
    int nDataLength;
    while ((nDataLength = recv(Socket,buffer,10000,0)) > 0){        
        int i = 0;
        while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
            cout << buffer[i];
            i += 1;
        }
    }
    closesocket(Socket);
    WSACleanup();
    
    std::cout << "Raw data" << std::endl;
    std::cout << buffer << std::endl;
    system("pause");
    return 0;
}
#endif