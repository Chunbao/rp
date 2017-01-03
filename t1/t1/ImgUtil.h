#pragma once

#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc_c.h>

#include <ctime>
#include <chrono>
// http://docs.opencv.org/2.4/modules/core/doc/basic_structures.html#mat-type
// @@@!!! online specification

extern const cv::Point TEMPLATE_AREA_LEFT;
extern const cv::Point TEMPLATE_AREA_RIGHT;

namespace img
{
    //@make first 2 available
    HBITMAP GetSrcBit(HDC hDC, int BitLeft, int BitTop,  int BitWidth, int BitHeight);

    IplImage* hBitmap2Ipl(HBITMAP hBmp);

    HBITMAP IplImage2hBitmap(IplImage* pImg);

    cv::Mat hwnd2mat(HWND hwnd, int, int);

    cv::Mat hSmallwnd2mat(HWND hwnd, int, int);

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


    void writePriceToFile(HDC hdcWindow, int x, int y, int width, int height, std::string ui);

    int enhanceImage(std::string inFile, std::string outFile);
}


namespace ipt {

    void mouseMove(int x, int y);

    void leftButtonClick(int x, int y);

    void keyboardSendBackspaceKey();

    void keyboardSendKey(int key);

    void keyboardSendBackspaceKey(unsigned int times);

    void keyboardSendDeleteKey();

    void keyboardSendUnicodeInput(std::string message);
}


namespace utl
{
    bool ifInRange(const cv::Point& target, const cv::Point& validLeft, const cv::Point& validRight);

    bool ifOkNormalInRange(const cv::Point& target);

    bool ifOkMiddleInRange(const cv::Point& target);

    bool ifCancelInRange(const cv::Point& target);

    bool ifRefreshInRange(const cv::Point& target);

    int getBorderAreaWidth(HDC h1);

    int getCaptionAreaHeight(HDC h1);

    // Ugly code, needs to be improved
    inline CString getWorkingPath()
    {
        wchar_t ownPth[MAX_PATH];

        // Will contain exe path
        HMODULE hModule = GetModuleHandle(NULL);
        if (hModule != NULL)
        {
            // When passing NULL to GetModuleHandle, it returns handle of exe itself
            GetModuleFileNameW(hModule, ownPth, (sizeof(ownPth)));

            // Use above module handle to get the path using GetModuleFileName()
            //return path;
        }
        CString path = CString(&ownPth[0]);
        path.Replace(_T("t1.exe"), _T(""));
        return path;
    }


    /*
      Using the output of OCR, this class manipulate the price, and make it reasonable
      price according to the actual market price. Since  the output of OCR could be wrong.
      Currently, it can recoginized all digits except 7*/
    class PriceFilter
    {
    public:
        PriceFilter();
        void reset();
        int process(std::string price);

    private:
        std::vector<std::pair<std::string, int>> m_data;
        const unsigned MAX_CACHE;
    };
    /*
      The mission of this class is to get the accurate server time.
      Take a series of time snapshots, and filter out the most accurate one.
          (the first different from previous shoule be the best)
    */
    class TimeFilter
    {
    public:
        TimeFilter();
        bool process(std::string time);
        bool ready();
        void reset();
        std::string getTime();
        int getHour();
        int getMinute();
        int getSecond();

    private:
        std::vector<std::string> m_data;
        bool m_timeReady;
        const unsigned MAX_CACHE;
    };

    // Server time = local time - time difference
    std::tm getServerTime(long long timeDiff);

    // The end time is 11:30:00, return time left(milliseconds), minus means past the deadline
    long long timeLeftInMilliseconds(long long timeDiff);
}

namespace prc
{
    //price
    int getIntelligencePrice(SYSTEMTIME serverTime);

    int getIntelligencePriceBwRelease(SYSTEMTIME serverTime);
}