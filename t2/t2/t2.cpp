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
}