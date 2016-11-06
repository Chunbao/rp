#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>
// http://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat-type
// @@@!!! online specification

namespace img
{
    //@make first 2 available
    HBITMAP GetSrcBit(HDC hDC, int BitLeft, int BitTop,  int BitWidth, int BitHeight);

    IplImage* hBitmap2Ipl(HBITMAP hBmp);

    HBITMAP IplImage2hBitmap(IplImage* pImg);

    cv::Mat hwnd2mat(HWND hwnd, int, int);

    class Image
    {
    private:
        std::vector<std::uint8_t> Pixels;
        std::uint32_t width, height;
        std::uint16_t BitsPerPixel;

        void Flip(void* In, void* Out, int width, int height, unsigned int Bpp);

    public:
#if defined _WIN32 || defined _WIN64
        explicit Image(HDC DC, int X, int Y, int Width, int Height);
#endif

        inline std::uint16_t GetBitsPerPixel() { return this->BitsPerPixel; }
        inline std::uint16_t GetBytesPerPixel() { return this->BitsPerPixel / 8; }
        inline std::uint16_t GetBytesPerScanLine() { return (this->BitsPerPixel / 8) * this->width; }
        inline int GetWidth() const { return this->width; }
        inline int GetHeight() const { return this->height; }
        inline const std::uint8_t* GetPixels() { return this->Pixels.data(); }
    };
}


namespace ipt {

    void mouseMove(int x, int y);

    void leftButtonClick(int x, int y);

    void keyboardSendBackspaceKey();

    void keyboardSendBackspaceKey(unsigned int times);

    void keyboardSendDeleteKey();

    void keyboardSendUnicodeInput(std::string message);
}


namespace utl
{
    inline bool ifInRange(const cv::Point& target, const cv::Point& validLeft, const cv::Point& validRight)
    {
        return (target.x > validLeft.x && target.x < validRight.x)
            && (target.y > validLeft.y && target.y < validRight.y);
    }
}