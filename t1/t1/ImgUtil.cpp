#include "stdafx.h"
#include "ImgUtil.h"


namespace img
{
    //http://www.newxing.com/Tech/Program/Cpp/695.html
    HBITMAP GetSrcBit(HDC hDC, int BitLeft, int BitTop, int BitWidth, int BitHeight)
    {
        HDC hBufDC;
        HBITMAP hBitmap, hBitTemp;
        //创建设备上下文(HDC)
        hBufDC = CreateCompatibleDC(hDC);
        //创建HBITMAP
        hBitmap = CreateCompatibleBitmap(hDC, BitWidth, BitHeight);
        hBitTemp = (HBITMAP)SelectObject(hBufDC, hBitmap);
        //得到位图缓冲区
        //StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight, hDC, 0, 0, BitWidth, BitHeight, SRCCOPY);
        StretchBlt(hBufDC, 0, 0, BitWidth, BitHeight, hDC, BitLeft, BitTop, BitWidth, BitHeight, SRCCOPY);
        
        //得到最终的位图信息
        hBitmap = (HBITMAP)SelectObject(hBufDC, hBitTemp);
        //释放内存
        DeleteObject(hBitTemp);
        ::DeleteDC(hBufDC);
        return hBitmap;
    }

    //http://wenku.baidu.com/view/c587176e7e21af45b307a8c3.html
    IplImage* hBitmap2Ipl(HBITMAP hBmp)

    {
        BITMAP bmp;
        ::GetObject(hBmp, sizeof(BITMAP), &bmp);//hBmp-->bmp
        int nChannels = bmp.bmBitsPixel == 1 ? 1 : bmp.bmBitsPixel / 8;
        int depth = bmp.bmBitsPixel == 1 ? IPL_DEPTH_1U : IPL_DEPTH_8U;
        IplImage* img = cvCreateImage(cvSize(bmp.bmWidth, bmp.bmHeight), depth, nChannels); //cvCreateImageHeader

                                                                                            //pBuffer = (char*)malloc(bmp.bmHeight*bmp.bmWidth*nChannels*sizeof(char));
        memcpy(img->imageData, (char*)(bmp.bmBits), bmp.bmHeight*bmp.bmWidth*nChannels);
        IplImage *dst = cvCreateImage(cvGetSize(img), img->depth, 3);
        cvCvtColor(img, dst, CV_BGRA2BGR);
        cvReleaseImage(&img);
        return dst;
    }

    HBITMAP IplImage2hBitmap(IplImage* pImg)
    {
        BYTE tmp[sizeof(BITMAPINFO) + 1024];
        BITMAPINFO *bmi = (BITMAPINFO*)tmp;
        HBITMAP hBmp;

        int i;
        memset(bmi, 0, sizeof(BITMAPINFO));
        bmi->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
        bmi->bmiHeader.biWidth = pImg->width;
        bmi->bmiHeader.biHeight = pImg->height;
        bmi->bmiHeader.biPlanes = 1;
        bmi->bmiHeader.biBitCount = pImg->nChannels * pImg->depth;
        bmi->bmiHeader.biCompression = BI_RGB;
        bmi->bmiHeader.biSizeImage = 0;//if biCompression is BI_RGB,this can be 0
        bmi->bmiHeader.biClrImportant = 0;
        switch (pImg->nChannels * pImg->depth)
        {
        case 8:
            for (i = 0; i < 256; i++) {
                bmi->bmiColors[i].rgbBlue = i;
                bmi->bmiColors[i].rgbGreen = i;
                bmi->bmiColors[i].rgbRed = i;
            }
            break;
        case 32:
        case 24:
            ((DWORD*)bmi->bmiColors)[0] = 0x00FF0000;
            ((DWORD*)bmi->bmiColors)[1] = 0x0000FF00;
            ((DWORD*)bmi->bmiColors)[2] = 0x000000FF;
            break;
        }
        hBmp = ::CreateDIBSection(NULL, bmi, DIB_RGB_COLORS, NULL, 0, 0);
        SetDIBits(NULL, hBmp, 0, pImg->height, pImg->imageData, bmi, DIB_RGB_COLORS);
        return hBmp;
    }


    // http://stackoverflow.com/questions/14148758/how-to-capture-the-desktop-in-opencv-ie-turn-a-bitmap-into-a-mat
    cv::Mat hwnd2mat(HWND hwnd, int dialogFrameLeftWidth, int dialogFrameTopHeight) {
        HDC hwindowDC, hwindowCompatibleDC;

        int height, width, srcheight, srcwidth;
        HBITMAP hbwindow;
        cv::Mat src;
        BITMAPINFOHEADER  bi;

        hwindowDC = GetDC(hwnd);
        hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
        SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

        RECT windowsize;    // get the height and width of the screen
        GetWindowRect(hwnd, &windowsize);

        srcheight = windowsize.bottom;
        srcwidth = windowsize.right;

        //height = windowsize.bottom / 2;  //change this to whatever size you want to resize to
        //width = windowsize.right / 2;
        height = windowsize.bottom - windowsize.top;  //change this to whatever size you want to resize to
        width = windowsize.right - windowsize.left;

        //src.create(height, width, CV_8UC4); // OK code
        src.create(height, width, CV_8UC4); //CV_32FC1??? wrong

        // create a bitmap
        hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
        bi.biSize = sizeof(BITMAPINFOHEADER);    //http://msdn.microsoft.com/en-us/library/windows/window/dd183402%28v=vs.85%29.aspx
        bi.biWidth = width;
        bi.biHeight = -height;  //this is the line that makes it draw upside down or not
        bi.biPlanes = 1;
        bi.biBitCount = 32;

        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        // use the previously created device context with the bitmap
        SelectObject(hwindowCompatibleDC, hbwindow);

        // copy from the window device context to the bitmap device context
        //StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
        StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, -dialogFrameLeftWidth, -dialogFrameTopHeight, width, height, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
        GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow
                                                                             // avoid memory leak
        DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);
        return src;
    }


    void Image::Flip(void* In, void* Out, int width, int height, unsigned int Bpp)
    {
        unsigned long Chunk = (Bpp > 24 ? width * 4 : width * 3 + width % 4);
        unsigned char* Destination = static_cast<unsigned char*>(Out);
        unsigned char* Source = static_cast<unsigned char*>(In) + Chunk * (height - 1);

        while (Source != In)
        {
            std::memcpy(Destination, Source, Chunk);
            Destination += Chunk;
            Source -= Chunk;
        }
    }

#if defined _WIN32 || defined _WIN64
    Image::Image(HDC DC, int X, int Y, int Width, int Height) : Pixels(), width(Width), height(Height), BitsPerPixel(32)
    {
        BITMAP Bmp = { 0 };
        HBITMAP hBmp = reinterpret_cast<HBITMAP>(GetCurrentObject(DC, OBJ_BITMAP));

        if (GetObject(hBmp, sizeof(BITMAP), &Bmp) == 0)
            throw std::runtime_error("BITMAP DC NOT FOUND.");

        RECT area = { X, Y, X + Width, Y + Height };
        HWND Window = WindowFromDC(DC);
        GetClientRect(Window, &area);

        HDC MemDC = GetDC(nullptr);
        HDC SDC = CreateCompatibleDC(MemDC);
        HBITMAP hSBmp = CreateCompatibleBitmap(MemDC, width, height);
        DeleteObject(SelectObject(SDC, hSBmp));

        BitBlt(SDC, 0, 0, width, height, DC, X, Y, SRCCOPY);
        unsigned int data_size = ((width * BitsPerPixel + 31) / 32) * 4 * height;
        std::vector<std::uint8_t> Data(data_size);
        this->Pixels.resize(data_size);

        BITMAPINFO Info = { sizeof(BITMAPINFOHEADER), static_cast<long>(width), static_cast<long>(height), 1, BitsPerPixel, BI_RGB, data_size, 0, 0, 0, 0 };
        GetDIBits(SDC, hSBmp, 0, height, &Data[0], &Info, DIB_RGB_COLORS);
        this->Flip(&Data[0], &Pixels[0], width, height, BitsPerPixel);

        DeleteDC(SDC);
        DeleteObject(hSBmp);
        ReleaseDC(nullptr, MemDC);
    }
#endif

}



namespace ipt { // Virtual input of mouse of keyboard
                // Mouse click event
                // http://stackoverflow.com/questions/5789843/how-i-can-simulate-a-double-mouse-click-on-window-i-khow-handle-on-x-y-coord
                // http://www.cplusplus.com/forum/lounge/17053/
                //sentInput
                //sendmessage
                //postmessage
    void mouseMove(int x, int y)
    {
        double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
        double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
        double fx = x*(65535.0f / fScreenWidth);
        double fy = y*(65535.0f / fScreenHeight);
        INPUT  Input = { 0 };
        Input.type = INPUT_MOUSE;
        Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
        Input.mi.dx = fx;
        Input.mi.dy = fy;
        ::SendInput(1, &Input, sizeof(INPUT));
    }

    void leftButtonClick(int x, int y)
    {
        double fScreenWidth = ::GetSystemMetrics(SM_CXSCREEN) - 1;
        double fScreenHeight = ::GetSystemMetrics(SM_CYSCREEN) - 1;
        double fx = x*(65535.0f / fScreenWidth);
        double fy = y*(65535.0f / fScreenHeight);
        INPUT  Input = { 0 };
        Input.type = INPUT_MOUSE;
        Input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_LEFTDOWN | MOUSEEVENTF_LEFTUP;
        Input.mi.dx = fx;
        Input.mi.dy = fy;
        ::SendInput(1, &Input, sizeof(INPUT));
    }

    void keyboardSendBackspaceKey()
    {
        INPUT input_down = { 0 };
        input_down.type = INPUT_KEYBOARD;
        input_down.ki.dwFlags = 0;
        input_down.ki.wScan = 0;
        input_down.ki.wVk = VK_BACK;
        SendInput(1, &input_down, sizeof(input_down));//keydown     
        INPUT input_up = { 0 };
        input_up.type = INPUT_KEYBOARD;
        input_up.ki.wScan = 0;
        input_up.ki.wVk = VK_BACK;
        input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP);
        SendInput(1, &input_up, sizeof(input_up));//keyup 
    }

    void keyboardSendDeleteKey()
    {
        INPUT input_down = { 0 };
        input_down.type = INPUT_KEYBOARD;
        input_down.ki.dwFlags = 0;
        input_down.ki.wScan = 0;
        input_down.ki.wVk = VK_DELETE;
        SendInput(1, &input_down, sizeof(input_down));//keydown     
        INPUT input_up = { 0 };
        input_up.type = INPUT_KEYBOARD;
        input_up.ki.wScan = 0;
        input_up.ki.wVk = VK_DELETE;
        input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP);
        SendInput(1, &input_up, sizeof(input_up));//keyup 
    }

    void keyboardSendUnicodeInput(std::string message)
    {
        for (int i = 0; i < message.size() - 2; i++)
        {
            INPUT input_down = { 0 };
            input_down.type = INPUT_KEYBOARD;
            input_down.ki.dwFlags = KEYEVENTF_UNICODE;
            input_down.ki.wScan = (short)message.at(i);
            input_down.ki.wVk = 0;
            SendInput(1, &input_down, sizeof(input_down));//keydown     
            INPUT input_up = { 0 };
            input_up.type = INPUT_KEYBOARD;
            input_up.ki.wScan = (short)message.at(i);
            input_up.ki.wVk = 0;
            input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP | KEYEVENTF_UNICODE);
            SendInput(1, &input_up, sizeof(input_up));//keyup      
        }
    }

}
