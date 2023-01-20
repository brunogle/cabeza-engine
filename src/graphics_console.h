#ifndef GRAPHICS_CONSOLE_H
#define GRAPHICS_CONSOLE_H

#include <windows.h>


class GraphicsConsole{

	public:

		GraphicsConsole();
	
		~GraphicsConsole();

		/*
		This function expects a width*height CHAR_INFO array which is coppied into shared memory.
		*/
		bool drawOnConsole(CHAR_INFO * graphicsArray);
		
		int initializeWindow(int width, int height);

		void closeWindow();

		int getWidth();

		int getHeigth();

		int getGraphicArraySize();

	private:

	HANDLE hProcessGraphics; //Graphics process window

    STARTUPINFOW startupInfo; //StartupInfo for process

	WCHAR charInfoFileName[16]; //Shared memory file name

	CHAR_INFO * sharedGraphicsArray;

	HANDLE g_hChildStd_IN_Rd = NULL;
	HANDLE g_hChildStd_IN_Wr = NULL;

	bool updateConsole(); //Sends update screen buffer signal on pipe

	int width, height, graphicArraySize;

};

#endif