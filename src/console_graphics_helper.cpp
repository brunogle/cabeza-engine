/*
This file compiles to a seperate executable file, whcih the is called by the
GraphicsWindowHandler class.

A shared memory file is used to pass the CHAR_INFO array used by WriteConsoleOutput()
The stdin pipe is used to indicate a screen update.

*/

#include <windows.h>
#include <iostream>
#include <string>
#include <tchar.h>


int main(int argc,char **argv){


	if(argc != 4){
		std::cerr << "Wrong number of arguments given (" << argc << ")";
		return 1; 
	}

	LPCSTR charInfoFileName = argv[1];


	const short kWidth = atoi(argv[2]);
	const short kHeight = atoi(argv[3]);
 

	const int kCharInfoArraySize = kWidth*kHeight;

	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HWND  hWndConsole = GetConsoleWindow();
 
	//Set window title to shared memory string

	SetWindowText(hWndConsole, charInfoFileName);

	//Set window to max size

	SetWindowPos(hWndConsole, NULL, 0, 0, 1920, 1080, SWP_NOMOVE);

	//Prevent window from being resized
	
	LONG_PTR glwStyle = GetWindowLongPtr(hWndConsole, GWL_STYLE);
	glwStyle &= ~(WS_THICKFRAME | WS_VSCROLL | WS_HSCROLL); 
    SetWindowLongPtr(hWndConsole, GWL_STYLE, glwStyle);

	//Set screen size to screen buffer size

	SMALL_RECT conScreen = { 0, 0, kWidth, kHeight };

	SetConsoleWindowInfo(hStdOut, TRUE, &conScreen);
	SetConsoleActiveScreenBuffer(hStdOut);

	//Set screen buffer size

	//Remove console coursor
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hStdOut, &cursorInfo);
    cursorInfo.bVisible = 0;
    SetConsoleCursorInfo(hStdOut, &cursorInfo);

	//Prevent text selection and line input in console
	DWORD prev_mode;
	GetConsoleMode(hStdOut, &prev_mode); 
	SetConsoleMode(hStdOut, ENABLE_EXTENDED_FLAGS | 
		(prev_mode & ~ENABLE_QUICK_EDIT_MODE & ~ENABLE_LINE_INPUT));


	HANDLE hCharInfoFile = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, charInfoFileName);

	if (hCharInfoFile == NULL){
		std::cerr << "GraphicsWindowHandler Process: OpenFileMapping() Fail: " << GetLastError();
		return 1;
	}

   	CHAR_INFO * pBuf = (CHAR_INFO *) MapViewOfFile(hCharInfoFile, // handle to map object
		FILE_MAP_ALL_ACCESS,  // read/write permission
		0,
		0,
		kCharInfoArraySize*sizeof(CHAR_INFO));

	if (hCharInfoFile == NULL){
    	CloseHandle(hCharInfoFile);
    	return 1;
	}

	COORD size = {(SHORT)kWidth, (SHORT)kHeight};

	COORD zero = {0, 0};

	SMALL_RECT writeRegion;
	writeRegion.Bottom = kHeight;
	writeRegion.Right = kWidth;
	writeRegion.Left = 0;
	writeRegion.Top = 0;

	CHAR chBuf[1];

	chBuf[0] = '-';

	DWORD dwRead;
	BOOL bSuccess;

	while (1){

		WriteConsoleOutputW(hStdOut, pBuf, size, zero, &writeRegion); //Write CHAR_INFO array from memory onto screen

		bSuccess = ReadFile(hStdIn, chBuf, 1, &dwRead, NULL); //Wait for a character recieved on stdin

		if(!bSuccess){ //If ReadFile fails, parent process has terminated the pipe. Exit.
			break;
		}
	}
}