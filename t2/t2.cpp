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
    api.Init("", "eng", tesseract::OEM_TESSERACT_CUBE_COMBINED);
    api.SetPageSegMode(static_cast<tesseract::PageSegMode>(7));
    api.SetOutputName("out");

    cout<<"File name:";
    char image[256];
    cin>>image;
    PIX   *pixs = pixRead(image);

    STRING text_out;
    api.ProcessPages(image, NULL, 0, &text_out);

    cout<<text_out.string();




	//std::vector<DWORD> SetOfPID;
	//GetProcessID("Rundll32",SetOfPID); 
     // get all process id's of Rundll32

	if (SetOfPID.empty())   // Process is not running
	{
		printf("Process is not running\n");
	}
	else    // Process is running
	{
		for (int i=0;i < SetOfPID.size(); i++)
		{
			printf("Process ID is %d\n", SetOfPID[i]);
			HANDLE hProcess = OpenProcess(
				 PROCESS_ALL_ACCESS,FALSE,SetOfPID[i]);
			// some code...
			CloseHandle(hProcess);
		}
	}


    system("pause");
}