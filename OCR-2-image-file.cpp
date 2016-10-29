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

int main(void){

    tesseract::TessBaseAPI api;
    //api.Init("", "eng", tesseract::OEM_TESSERACT_CUBE_COMBINED);
    //api.SetPageSegMode(static_cast<tesseract::PageSegMode>(7));

	//api.Init("C:\Users\andrew\Desktop\rp\trunk\t1\t1\configs", "eng", tesseract::OEM_CUBE_ONLY);
	api.Init("", "eng", tesseract::OEM_DEFAULT);
	
	api.SetVariable("classify_bln_numeric_mode", "1");
	//api.SetVariable("tessedit_char_whitelist", "0123456789");
	api.SetVariable("tessedit_char_whitelist", "0123456789");
	api.SetVariable("tessedit_char_blacklist", "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ");
	api.SetPageSegMode(tesseract::PSM_SINGLE_LINE);

    api.SetOutputName("out");

    cout<<"File name:";
    char image[256];
    cin>>image;

    PIX   *pixs = pixRead(image);

    STRING text_out;
    api.ProcessPages(image, NULL, 0, &text_out);
    cout<<text_out.string();

    system("pause");
}