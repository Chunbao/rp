#include "stdafx.h"
#include "ImgUtil.h"

#include "leptonica\allheaders.h"



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



    void writePriceToFile(HDC hdcWindow, int x, int y, int width, int height, std::string ui)
    {
        //int width, height = 0;

        HDC hdcMemDC = NULL;
        HBITMAP hbmScreen = NULL;
        BITMAP bmpScreen;

        // Create a compatible DC which is used in a BitBlt from the window DC
        hdcMemDC = CreateCompatibleDC(hdcWindow);

        // Create a compatible bitmap from the Window DC
        hbmScreen = CreateCompatibleBitmap(hdcWindow, width, height);

        // Select the compatible bitmap into the compatible memory DC.
        SelectObject(hdcMemDC, hbmScreen);

        // Bit block transfer into our compatible memory DC.
        BitBlt(hdcMemDC, 0, 0, width, height, hdcWindow, x, y, SRCCOPY);

        // Get the BITMAP from the HBITMAP
        GetObject(hbmScreen, sizeof(BITMAP), &bmpScreen);

        BITMAPFILEHEADER   bmfHeader;
        BITMAPINFOHEADER   bi;

        bi.biSize = sizeof(BITMAPINFOHEADER);
        bi.biWidth = bmpScreen.bmWidth;
        bi.biHeight = bmpScreen.bmHeight;
        bi.biPlanes = 1;
        bi.biBitCount = 32;
        bi.biCompression = BI_RGB;
        bi.biSizeImage = 0;
        bi.biXPelsPerMeter = 0;
        bi.biYPelsPerMeter = 0;
        bi.biClrUsed = 0;
        bi.biClrImportant = 0;

        DWORD dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

        // Starting with 32-bit Windows, GlobalAlloc and LocalAlloc are implemented as wrapper functions that 
        // call HeapAlloc using a handle to the process's default heap. Therefore, GlobalAlloc and LocalAlloc 
        // have greater overhead than HeapAlloc.
        HANDLE hDIB = GlobalAlloc(GHND, dwBmpSize);
        char *lpbitmap = (char *)GlobalLock(hDIB);

        // Gets the "bits" from the bitmap and copies them into a buffer 
        // which is pointed to by lpbitmap.
        GetDIBits(hdcWindow, hbmScreen, 0,
            (UINT)bmpScreen.bmHeight,
            lpbitmap,
            (BITMAPINFO *)&bi, DIB_RGB_COLORS);

        // A file is created, this is where we will save the screen capture.
        HANDLE hFile = CreateFile(CString(ui.c_str()),
            GENERIC_WRITE,
            0,
            NULL,
            CREATE_ALWAYS,
            FILE_ATTRIBUTE_NORMAL, NULL);

        // Add the size of the headers to the size of the bitmap to get the total file size
        DWORD dwSizeofDIB = dwBmpSize + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

        //Offset to where the actual bitmap bits start.
        bmfHeader.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER);

        //Size of the file
        bmfHeader.bfSize = dwSizeofDIB;

        //bfType must always be BM for Bitmaps
        bmfHeader.bfType = 0x4D42; //BM   

        DWORD dwBytesWritten = 0;
        WriteFile(hFile, (LPSTR)&bmfHeader, sizeof(BITMAPFILEHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, (LPSTR)&bi, sizeof(BITMAPINFOHEADER), &dwBytesWritten, NULL);
        WriteFile(hFile, (LPSTR)lpbitmap, dwBmpSize, &dwBytesWritten, NULL);

        //Unlock and Free the DIB from the heap
        GlobalUnlock(hDIB);
        GlobalFree(hDIB);

        //Close the handle for the file that was created
        CloseHandle(hFile);

        DeleteObject(hbmScreen);
        DeleteObject(hdcMemDC);
        //ReleaseDC(hWnd, hdcWindow);
    }







#define NO_VALUE    (-1)
#define LEPT_TRUE   1
#define LEPT_FALSE  0
#define LEPT_OK     0
#define LEPT_ERROR  1

#define MAX_FILE_LEN  512

    /* Minimum number of foreground pixels that a line must contain for it to be part of a span.
    Needed because sometimes furigana does not have a perfect gap between the text and itself. */
#define FURIGANA_MIN_FG_PIX_PER_LINE  2

    /* Minimum width of a span (in pixels) for it to be included in the span list. */
#define FURIGANA_MIN_WIDTH  5

    /* Maximum number of spans used during furigana removal */
#define FURIGANA_MAX_SPANS  50


    typedef enum
    {
        NEGATE_NO,   /* Do not negate image */
        NEGATE_YES,  /* Force negate */
        NEGATE_AUTO, /* Automatically negate if border pixels are dark */

    } Negate_enum;


    typedef enum
    {
        REMOVE_FURIGANA_NO,         /* Do not remove furigana */
        REMOVE_FURIGANA_VERTICAL,   /* Remove furigana from vertically formatted text */
        REMOVE_FURIGANA_HORIZONTAL, /* Remove furigana from horizontally formatted text */

    } Remove_furigana_enum;


    /* Span of lines that contain foreground text. Used during furigana removal. */
    typedef struct
    {
        int start;
        int end;

    } Span;


    static int erase_furigana_vertical(PIX *pixs, float scale_factor);
    static int erase_furigana_horizontal(PIX *pixs, float scale_factor);
    static l_int32 erase_area_left_to_right(PIX *pixs, l_int32 x, l_int32 width);
    static l_int32 erase_area_top_to_bottom(PIX *pixs, l_int32 y, l_int32 height);

    /* Erase the furigana from the provided binary PIX. Works by finding spans of foreground
    text and removing the spans that are too narrow and are likely furigana.
    Use this version for vertical text.
    Returns LEPT_OK on success. */
    static int erase_furigana_vertical(PIX *pixs, float scale_factor)
    {
        int min_fg_pix_per_line = (int)(FURIGANA_MIN_FG_PIX_PER_LINE * scale_factor);
        int min_span_width = (int)(FURIGANA_MIN_WIDTH * scale_factor);
        l_uint32 x = 0;
        l_uint32 y = 0;
        int num_fg_pixels_on_line = 0;
        int good_line = LEPT_FALSE;
        int num_good_lines_in_cur_span = 0;
        int total_good_lines = 0;
        l_uint32 pixel_value = 0;
        Span span = { NO_VALUE, NO_VALUE };
        Span span_list[FURIGANA_MAX_SPANS];
        int total_spans = 0;
        int ave_span_width = 0;
        int span_idx = 0;
        Span *cur_span = NULL;
        l_int32 status = LEPT_ERROR;

        /* Get list of spans that contain fg pixels */
        for (x = 0; x < pixs->w; x++)
        {
            num_fg_pixels_on_line = 0;
            good_line = LEPT_FALSE;

            for (y = 0; y < pixs->h; y++)
            {
                status = pixGetPixel(pixs, x, y, &pixel_value);

                if (status != LEPT_OK)
                {
                    return status;
                }

                /* If this is a foreground pixel */
                if (pixel_value == 1)
                {
                    num_fg_pixels_on_line++;

                    /* If this line has already meet the minimum number of fg pixels, stop scanning it */
                    if (num_fg_pixels_on_line >= min_fg_pix_per_line)
                    {
                        good_line = LEPT_TRUE;
                        break;
                    }
                }
            }

            /* If last line is good, set it bad in order to close the span */
            if (good_line && (x == pixs->w - 1))
            {
                good_line = LEPT_FALSE;
                num_good_lines_in_cur_span++;
            }

            /* If this line has the minimum number of fg pixels */
            if (good_line)
            {
                /* Start a new span */
                if (span.start == NO_VALUE)
                {
                    span.start = x;
                }

                num_good_lines_in_cur_span++;
            }
            else /* Line doesn't have enough fg pixels to consider as part of a span */
            {
                /* If a span has already been started, then end it */
                if (span.start != NO_VALUE)
                {
                    /* If this span isn't too small (needed so that the average isn't skewed) */
                    if (num_good_lines_in_cur_span >= min_span_width)
                    {
                        span.end = x;

                        total_good_lines += num_good_lines_in_cur_span;

                        /* Add span to the list */
                        span_list[total_spans] = span;
                        total_spans++;

                        /* Prevent span list overflow */
                        if (total_spans >= FURIGANA_MAX_SPANS)
                        {
                            break;
                        }
                    }
                }

                /* Reset span */
                span.start = NO_VALUE;
                span.end = NO_VALUE;
                num_good_lines_in_cur_span = 0;
            }
        }

        if (total_spans == 0)
        {
            return LEPT_OK;
        }

        /* Get average width of the spans */
        ave_span_width = total_good_lines / total_spans;

        x = 0;

        /* Erase areas of the PIX where either no span exists or where a span is too narrow */
        for (span_idx = 0; span_idx < total_spans; span_idx++)
        {
            cur_span = &span_list[span_idx];

            /* If span is at least of average width, erase area between the previous span and this span */
            if ((cur_span->end - cur_span->start + 1) >= (int)(ave_span_width * 0.9))
            {
                status = erase_area_left_to_right(pixs, x, cur_span->start - x);

                if (status != LEPT_OK)
                {
                    return status;
                }

                x = cur_span->end + 1;
            }
        }

        /* Clear area between the end of the right-most span and the right edge of the PIX */
        if ((x != 0) && (x < (pixs->w - 1)))
        {
            status = erase_area_left_to_right(pixs, x, pixs->w - x);

            if (status != LEPT_OK)
            {
                return status;
            }
        }

        return LEPT_OK;

    } /* erase_furigana_vertical */


      /* Erase the furigana from the provided binary PIX. Works by finding spans of foreground
      text and removing the spans that are too narrow and are likely furigana.
      Use this version for horizontal text.
      Returns LEPT_OK on success. */
    static int erase_furigana_horizontal(PIX *pixs, float scale_factor)
    {
        int min_fg_pix_per_line = (int)(FURIGANA_MIN_FG_PIX_PER_LINE * scale_factor);
        int min_span_width = (int)(FURIGANA_MIN_WIDTH * scale_factor);
        l_uint32 x = 0;
        l_uint32 y = 0;
        int num_fg_pixels_on_line = 0;
        int good_line = LEPT_FALSE;
        int num_good_lines_in_cur_span = 0;
        int total_good_lines = 0;
        l_uint32 pixel_value = 0;
        Span span = { NO_VALUE, NO_VALUE };
        Span span_list[FURIGANA_MAX_SPANS];
        int total_spans = 0;
        int ave_span_width = 0;
        int span_idx = 0;
        Span *cur_span = NULL;
        l_int32 status = LEPT_ERROR;

        /* Get list of spans that contain fg pixels */
        for (y = 0; y < pixs->h; y++)
        {
            num_fg_pixels_on_line = 0;
            good_line = LEPT_FALSE;

            for (x = 0; x < pixs->w; x++)
            {
                status = pixGetPixel(pixs, x, y, &pixel_value);

                if (status != LEPT_OK)
                {
                    return status;
                }

                /* If this is a foreground pixel */
                if (pixel_value == 1)
                {
                    num_fg_pixels_on_line++;

                    /* If this line has already meet the minimum number of fg pixels, stop scanning it */
                    if (num_fg_pixels_on_line >= min_fg_pix_per_line)
                    {
                        good_line = LEPT_TRUE;
                        break;
                    }
                }
            }

            /* If last line is good, set it bad in order to close the span */
            if (good_line && (y == pixs->h - 1))
            {
                good_line = LEPT_FALSE;
                num_good_lines_in_cur_span++;
            }

            /* If this line has the minimum number of fg pixels */
            if (good_line)
            {
                /* Start a new span */
                if (span.start == NO_VALUE)
                {
                    span.start = y;
                }

                num_good_lines_in_cur_span++;
            }
            else /* Line doesn't have enough fg pixels to consider as part of a span */
            {
                /* If a span has already been started, then end it */
                if (span.start != NO_VALUE)
                {
                    /* If this span isn't too small (needed so that the average isn't skewed) */
                    if (num_good_lines_in_cur_span >= min_span_width)
                    {
                        span.end = y;

                        total_good_lines += num_good_lines_in_cur_span;

                        /* Add span to the list */
                        span_list[total_spans] = span;
                        total_spans++;

                        /* Prevent span list overflow */
                        if (total_spans >= FURIGANA_MAX_SPANS)
                        {
                            break;
                        }
                    }
                }

                /* Reset span */
                span.start = NO_VALUE;
                span.end = NO_VALUE;
                num_good_lines_in_cur_span = 0;
            }
        }

        if (total_spans == 0)
        {
            return LEPT_OK;
        }

        /* Get average width of the spans */
        ave_span_width = total_good_lines / total_spans;

        y = 0;

        /* Erase areas of the PIX where either no span exists or where a span is too narrow */
        for (span_idx = 0; span_idx < total_spans; span_idx++)
        {
            cur_span = &span_list[span_idx];

            /* If span is at least of average width, erase area between the previous span and this span */
            if ((cur_span->end - cur_span->start + 1) >= (int)(ave_span_width * 0.9))
            {
                status = erase_area_top_to_bottom(pixs, y, cur_span->start - y);

                if (status != LEPT_OK)
                {
                    return status;
                }

                y = cur_span->end + 1;
            }
        }

        /* Clear area between the end of the right-most span and the right edge of the PIX */
        if ((y != 0) && (y < (pixs->h - 1)))
        {
            status = erase_area_top_to_bottom(pixs, y, pixs->h - y);

            if (status != LEPT_OK)
            {
                return status;
            }
        }

        return LEPT_OK;

    } /* erase_furigana_horizontal */


      /* Clear/erase a left-to-right section of the provided binary PIX.
      Returns 0 on success. */
    static l_int32 erase_area_left_to_right(PIX *pixs, l_int32 x, l_int32 width)
    {
        l_int32 status = LEPT_ERROR;
        BOX box;

        box.x = x;
        box.y = 0;
        box.w = width;
        box.h = pixs->h;

        status = pixClearInRect(pixs, &box);

        return status;

    } /* erase_area_left_to_right */


      /* Clear/erase a top-to-bottom section of the provided binary PIX.
      Returns 0 on success. */
    static l_int32 erase_area_top_to_bottom(PIX *pixs, l_int32 y, l_int32 height)
    {
        l_int32 status = LEPT_ERROR;
        BOX box;

        box.x = 0;
        box.y = y;
        box.w = pixs->w;
        box.h = height;

        status = pixClearInRect(pixs, &box);

        return status;

    } /* erase_area_top_to_bottom */



    int enhanceImage(std::string inFile, std::string outFile)
    {
        //char source_file[MAX_FILE_LEN] = inFile.c_str();
        //char dest_file[MAX_FILE_LEN] = outFile.c_str();
        const char* source_file = inFile.c_str();
        const char* dest_file = outFile.c_str();


        Negate_enum perform_negate = NEGATE_AUTO;
        l_float32 dark_bg_threshold = 0.5f; /* From 0.0 to 1.0, with 0 being all white and 1 being all black */

        int perform_scale = LEPT_TRUE;
        l_float32 scale_factor = 3.5f;

        int perform_unsharp_mask = LEPT_TRUE;
        l_int32 usm_halfwidth = 5;
        l_float32 usm_fract = 2.5f;

        int perform_otsu_binarize = LEPT_TRUE;
        l_int32 otsu_sx = 2000;
        l_int32 otsu_sy = 2000;
        l_int32 otsu_smoothx = 0;
        l_int32 otsu_smoothy = 0;
        l_float32 otsu_scorefract = 0.0f;

        Remove_furigana_enum remove_furigana = REMOVE_FURIGANA_NO;

        l_int32 status = LEPT_ERROR;
        l_float32 border_avg = 0.0f;
        PIX *pixs = NULL;
        char *ext = NULL;

        pixs = pixRead(source_file);

        if (pixs == NULL)
        {
            return 1;
        }

        /* Convert to grayscale */
        pixs = pixConvertRGBToGray(pixs, 0.0f, 0.0f, 0.0f);

        if (pixs == NULL)
        {
            return 2;
        }

        if (perform_negate == NEGATE_YES)
        {
            /* Negate image */
            pixInvert(pixs, pixs);

            if (pixs == NULL)
            {
                return 3;
            }
        }
        else if (perform_negate == NEGATE_AUTO)
        {
            PIX *otsu_pixs = NULL;

            status = pixOtsuAdaptiveThreshold(pixs, otsu_sx, otsu_sy, otsu_smoothx, otsu_smoothy, otsu_scorefract, NULL, &otsu_pixs);

            if (status != LEPT_OK)
            {
                return 4;
            }

            /* Get the average intensity of the border pixels,
            with average of 0.0 being completely white and 1.0 being completely black. */
            border_avg = pixAverageOnLine(otsu_pixs, 0, 0, otsu_pixs->w - 1, 0, 1);                               /* Top */
            border_avg += pixAverageOnLine(otsu_pixs, 0, otsu_pixs->h - 1, otsu_pixs->w - 1, otsu_pixs->h - 1, 1); /* Bottom */
            border_avg += pixAverageOnLine(otsu_pixs, 0, 0, 0, otsu_pixs->h - 1, 1);                               /* Left */
            border_avg += pixAverageOnLine(otsu_pixs, otsu_pixs->w - 1, 0, otsu_pixs->w - 1, otsu_pixs->h - 1, 1); /* Right */
            border_avg /= 4.0f;

            pixDestroy(&otsu_pixs);

            /* If background is dark */
            if (border_avg > dark_bg_threshold)
            {
                /* Negate image */
                pixInvert(pixs, pixs);

                if (pixs == NULL)
                {
                    return 5;
                }
            }
        }

        if (perform_scale)
        {
            /* Scale the image (linear interpolation) */
            pixs = pixScaleGrayLI(pixs, scale_factor, scale_factor);

            if (pixs == NULL)
            {
                return 6;
            }
        }

        if (perform_unsharp_mask)
        {
            /* Apply unsharp mask */
            pixs = pixUnsharpMaskingGray(pixs, usm_halfwidth, usm_fract);

            if (pixs == NULL)
            {
                return 7;
            }
        }

        if (perform_otsu_binarize)
        {
            /* Binarize */
            status = pixOtsuAdaptiveThreshold(pixs, otsu_sx, otsu_sy, otsu_smoothx, otsu_smoothy, otsu_scorefract, NULL, &pixs);

            if (status != LEPT_OK)
            {
                return 8;
            }

            /* Remove furigana? */
            if (remove_furigana == REMOVE_FURIGANA_VERTICAL)
            {
                status = erase_furigana_vertical(pixs, scale_factor);

                if (status != LEPT_OK)
                {
                    return 9;
                }
            }
            else if (remove_furigana == REMOVE_FURIGANA_HORIZONTAL)
            {
                status = erase_furigana_horizontal(pixs, scale_factor);

                if (status != LEPT_OK)
                {
                    return 10;
                }
            }
        }

        /* Get extension of output image */
        status = splitPathAtExtension(dest_file, NULL, &ext);

        if (status != LEPT_OK)
        {
            return 11;
        }

        /* pixWriteImpliedFormat() doesn't recognize PBM/PGM/PPM extensions */
        if ((strcmp(ext, ".pbm") == 0) || (strcmp(ext, ".pgm") == 0) || (strcmp(ext, ".ppm") == 0))
        {
            /* Write the image to file as a PNM */
            status = pixWrite(dest_file, pixs, IFF_PNM);
        }
        else
        {
            /* Write the image to file */
            status = pixWriteImpliedFormat(dest_file, pixs, 0, 0);
        }

        if (status != LEPT_OK)
        {
            return 12;
        }

        /* Free image data */
        pixDestroy(&pixs);

        return 0;
    } /* main */

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
        Input.mi.dx = (long)fx;
        Input.mi.dy = (long)fy;
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
        Input.mi.dx = (long)fx;
        Input.mi.dy = (long)fy;
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

    void keyboardSendF6Key()
    {
        INPUT input_down = { 0 };
        input_down.type = INPUT_KEYBOARD;
        input_down.ki.dwFlags = 0;
        input_down.ki.wScan = 0;
        input_down.ki.wVk = VK_F6;
        SendInput(1, &input_down, sizeof(input_down));//keydown     
        INPUT input_up = { 0 };
        input_up.type = INPUT_KEYBOARD;
        input_up.ki.wScan = 0;
        input_up.ki.wVk = VK_F6;
        input_up.ki.dwFlags = (int)(KEYEVENTF_KEYUP);
        SendInput(1, &input_up, sizeof(input_up));//keyup 
    }

    void keyboardSendBackspaceKey(unsigned int times)
    {
        for (unsigned i = 0; i < times; ++i)
        {
            keyboardSendBackspaceKey();
        }
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
        assert(message.size() == 5);
        for (unsigned i = 0; i < message.size(); i++)
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


namespace utl
{
    bool ifInRange(const cv::Point& target, const cv::Point& validLeft, const cv::Point& validRight)
    {
        return (target.x > validLeft.x && target.x < validRight.x)
            && (target.y > validLeft.y && target.y < validRight.y);
    }

    int getBorderAreaWidth(HDC h1)
    {
        RECT rectWindow;
        GetWindowRect(WindowFromDC(h1), &rectWindow);
        RECT rectClient;
        GetClientRect(WindowFromDC(h1), &rectClient);

        return ((rectWindow.right - rectWindow.left) - (rectClient.right-rectClient.left))/2;
    }

    int getCaptionAreaHeight(HDC h1)
    {
        RECT rectWindow;
        GetWindowRect(WindowFromDC(h1), &rectWindow);
        RECT rectClient;
        GetClientRect(WindowFromDC(h1), &rectClient);

        return (rectWindow.bottom - rectWindow.top) - (rectClient.bottom - rectClient.top) - getBorderAreaWidth(h1);
    }


    // PriceFilter
    PriceFilter::PriceFilter() : MAX_CACHE(100)
    {}

    void PriceFilter::reset()
    {
        m_data.clear();
    }

    int PriceFilter::process(std::string price)
    {
        const std::string originalPrice = price;

        price.erase(price.find_last_not_of(" \n") + 1);

        int ret(0);
        if (m_data.size() >= MAX_CACHE)
        {
            m_data.erase(m_data.begin());
        }

        if (price.size() == 5) // The number of digits,e.g. 86500
        {
            if (price.at(0) == '8' &&
                price.at(3) == '0' &&
                price.at(4) == '0')
            {
                if (std::stoi(price) / 10000 == 0)
                {
                    if (' ' == price.at(1))
                    {
                        price.replace(1, 1, "7");
                    }
                    if (' ' == price.at(2))
                    {
                        price.replace(2, 1, "7");
                    }
                }
                if (std::stoi(price) / 10000 != 0)
                {
                    ret = std::stoi(price);
                }
            }
            //return previous
        }
        else if (price.size() == 6)
        {
            if (price.at(0) == '8' &&
                price.at(4) == '0' &&
                price.at(5) == '0')
            {
                if (' ' == price.at(1))
                {
                    price.replace(1, 2, "7");
                    if (' ' == price.at(2))
                    {
                        price.replace(2, 1, "7");
                    }
                }
                else if (' ' == price.at(2))
                {
                    price.replace(2, 2, "7");
                }

                if (std::stoi(price) / 10000 != 0)
                {
                    ret = std::stoi(price);
                }
            }
        }


        if (ret == 0)
        {
            if (m_data.size() > 0)
            {
                //if the same
                if (originalPrice == m_data.at(m_data.size() - 1).first)
                {
                    ret = m_data.at(m_data.size() - 1).second;
                }
                else
                {
                    ret = m_data.at(m_data.size() - 1).second + 100;
                }
            }
        }
        else if (ret < 82000)
        {
            ret = 82000; //
        }
        else if (ret >= 82000)
        {
            if (m_data.size() > 0 && ret > 99000 )
            {
                if (originalPrice == m_data.at(m_data.size() - 1).first)
                {
                    ret = m_data.at(m_data.size() - 1).second;
                }
                else
                {
                    ret = m_data.at(m_data.size() - 1).second + 100;
                }
            }
        }

        m_data.push_back(std::pair<std::string, int>(originalPrice, ret));

        return ret;
    }


    TimeFilter::TimeFilter()
        : MAX_CACHE(10)
        , m_timeReady(false)
    {
    }

    bool TimeFilter::process(std::string time)
    {
        const std::string originalTime = time;
        time.erase(time.find_last_not_of(" \n") + 1);
        if (time.size() != 8)
            return false;
        
        // add checks ...

        // right? 
        // check if the first correct one?
        //   no add

        if (m_data.size() >= MAX_CACHE)
        {
            m_data.erase(m_data.begin());
        }
        m_data.push_back(time);

        if (m_data.size() >= 3 && 
            m_data.at(m_data.size() - 1) != m_data.at(m_data.size() - 2) &&
            m_data.at(m_data.size() - 2) == m_data.at(m_data.size() - 3))
        {
            m_timeReady = true;
            return true;
        }
        return false;
        //   yes return true and set the final time as last one in vector
        //      set true
    }

    bool TimeFilter::ready()
    {
        return m_timeReady;
    }

    void TimeFilter::reset()
    {
        m_data.clear();
        m_timeReady = false;
    }

    std::string TimeFilter::getTime()
    {
        if (!m_timeReady)
            throw std::exception("Time not ready");
        return m_data.at(m_data.size() - 1);
    }
    
    int TimeFilter::getHour()
    {
        if (!m_timeReady)
            throw std::exception("Time not ready");
        return std::stoi(getTime().substr(0, 2));
    }

    int TimeFilter::getMinute()
    {
        if (!m_timeReady)
            throw std::exception("Time not ready");
        return std::stoi(getTime().substr(3, 2));
    }
    
    int TimeFilter::getSecond()
    {
        if (!m_timeReady)
            throw std::exception("Time not ready");
        return std::stoi(getTime().substr(6, 2));
    }


    std::tm getServerTime(long long timeDiff)
    {
        std::chrono::system_clock::time_point local = std::chrono::system_clock::now();
        std::time_t serverTime = std::chrono::system_clock::to_time_t(local - std::chrono::milliseconds(timeDiff));
        std::tm server = *std::localtime(&serverTime);
        return server;
    }
    /*
    http://blog.csdn.net/aheroofeast/article/details/7860936
    */

} // end of util


namespace prc
{
    int getIntelligencePrice(long long timeDiff)
    {
        int ret(300);
        std::tm server = utl::getServerTime(timeDiff);
        const int hour = server.tm_hour;
        const int min = server.tm_min;
        const int sec = server.tm_sec;
        if (hour == 11 && min == 29)
        {
            if (sec <= 35)
                ret = 1100;
            else if (sec >= 36 && sec <= 41)
                ret = 1000;
            else if (sec >= 42 && sec <= 45)
                ret = 900;
            else if (sec >= 46 && sec <= 48)
                ret = 800;
            else if (sec >= 49 && sec <= 50)
                return 700;
            else if (sec >= 51 && sec <= 52)
                ret = 600;
            else if (sec >= 53 && sec <= 55)
                ret = 500;
            else if (sec <= 56 && sec <= 59)
                ret = 300;
        }
        return ret;
    }
}