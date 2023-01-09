#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>

#include "movements.h"
#include "graphics.h"

#define DRAWING_WIDTH  41
#define DRAWING_HEIGHT 25



int main() {
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	game_state state = initial_game_state();

	draw_on_console(state, hConsole, 0, 0);

	getchar();
	return 0;
}