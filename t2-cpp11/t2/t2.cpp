// t2.cpp : Defines the entry point for the console application.
//
/*
#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}

*/

#if 0
#include <iostream>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <memory>
#include <cstring>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

using namespace cv;
using namespace std;

int main(int argc, char* argv[])
{
    /// Load image and template
    string inputName = "sample1.png";
    string outputName = "sun2_detect.png";
    Mat img = imread(inputName, 1);
    Mat templ = imread("temp.png", 1);

    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    Mat result(result_cols, result_rows, CV_32FC1);

    /// Do the Matching and Normalize
    matchTemplate(img, templ, result, CV_TM_CCOEFF);
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    Point maxLoc;
    minMaxLoc(result, NULL, NULL, NULL, &maxLoc);

    rectangle(img, maxLoc, Point(maxLoc.x + templ.cols, maxLoc.y + templ.rows), Scalar(0, 255, 0), 2);
    rectangle(result, maxLoc, Point(maxLoc.x + templ.cols, maxLoc.y + templ.rows), Scalar(0, 255, 0), 2);

    imshow("img", img);
    imshow("result", result);

    imwrite(outputName, img);

    waitKey(0);

    return 0;
}
#endif






#if 1
//#include "opencv2/highgui/highgui.hpp"
//#include "opencv2/imgproc/imgproc.hpp"
//#include <iostream>
//#include <stdio.h>
//
//using namespace std;
//using namespace cv;

#include "stdafx.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <Windows.h>
#include <iostream>
#include <string>
using namespace std;
using namespace cv;



/// Global Variables
Mat img; Mat templ; Mat result;
char* image_window = "Source Image";
char* result_window = "Result window";

int match_method;
int max_Trackbar = 5;

/// Function Headers
void MatchingMethod(int, void*);
Mat hwnd2mat(HWND hwnd);

int main(int argc, char** argv)
{
    /// Load image and template
    //img = imread(argv[1], 1);
    //templ = imread(argv[2], 1);

    img = imread(std::string("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t2-cpp11\\testdata\\ss1.jpg"), 1);
    // @todo, image need to save to a local space
    //img = hwnd2mat(GetDesktopWindow());
    //imwrite("Capture_tmp.jpg", img);
    //img = imread("Capture_tmp.jpg", 1);
    templ = imread(std::string("C:\\Users\\andrew\\Desktop\\rp\\trunk\\t2-cpp11\\testdata\\cancel.png"), 1);
    
    /// Create windows
    namedWindow(image_window, CV_WINDOW_AUTOSIZE);
    namedWindow(result_window, CV_WINDOW_AUTOSIZE);

    /// Create Trackbar
    char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF NORMED \n 2: TM CCORR \n 3: TM CCORR NORMED \n 4: TM COEFF \n 5: TM COEFF NORMED";
    createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);

    MatchingMethod(0, 0);

    waitKey(0);
    return 0;
}

/**
* @function MatchingMethod
* @brief Trackbar callback
*/
void MatchingMethod(int, void*)
{
    /// Source image to display
    Mat img_display;
    img.copyTo(img_display);

    /// Create the result matrix
    int result_cols = img.cols - templ.cols + 1;
    int result_rows = img.rows - templ.rows + 1;

    result.create(result_rows, result_cols, CV_32FC1);

    /// Do the Matching and Normalize
    matchTemplate(img, templ, result, match_method);
    normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat());

    /// Localizing the best match with minMaxLoc
    double minVal; double maxVal; Point minLoc; Point maxLoc;
    Point matchLoc;

    minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());

    /// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
    if (match_method == CV_TM_SQDIFF || match_method == CV_TM_SQDIFF_NORMED)
    {
        matchLoc = minLoc;
    }
    else
    {
        matchLoc = maxLoc;
    }

    /// Show me what you got
    rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);
    rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);

    imshow(image_window, img_display);
    imshow(result_window, result);

    return;
}

Mat hwnd2mat(HWND hwnd) {

    HDC hwindowDC, hwindowCompatibleDC;

    int height, width, srcheight, srcwidth;
    HBITMAP hbwindow;
    Mat src;
    BITMAPINFOHEADER  bi;

    hwindowDC = GetDC(hwnd);
    hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
    SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

    RECT windowsize;    // get the height and width of the screen
    GetClientRect(hwnd, &windowsize);

    srcheight = windowsize.bottom;
    srcwidth = windowsize.right;

    //height = windowsize.bottom / 2;  //change this to whatever size you want to resize to
    //width = windowsize.right / 2;
    height = windowsize.bottom;  //change this to whatever size you want to resize to
    width = windowsize.right;

    //src.create(height, width, CV_8UC4);
    src.create(height, width, CV_32FC1);

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
    StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, 0, 0, srcwidth, srcheight, SRCCOPY); //change SRCCOPY to NOTSRCCOPY for wacky colors !
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, src.data, (BITMAPINFO *)&bi, DIB_RGB_COLORS);  //copy from hwindowCompatibleDC to hbwindow

                                                                                                       // avoid memory leak
    DeleteObject(hbwindow); DeleteDC(hwindowCompatibleDC); ReleaseDC(hwnd, hwindowDC);

    return src;
}
#endif