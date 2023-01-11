#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>

#include "movements.h"
#include "graphics.h"



int main() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	HWND hWndConsole = GetConsoleWindow();
	SetWindowPos(hWndConsole, NULL, 0, 0, 900, 600, SWP_NOMOVE);


	game_state state = initial_game_state();

	draw_on_console(state, hConsole, 0, 0);

	getchar();
	return 0;
}