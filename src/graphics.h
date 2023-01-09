#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "movements.h"
#include<windows.h>

#define DRAWING_WIDTH  41
#define DRAWING_HEIGHT 25


int generate_unicode_board(game_state state, CHAR_INFO drawing[DRAWING_HEIGHT*DRAWING_WIDTH]);
int draw_on_console(game_state state, HANDLE hConsole, int coord_x, int coord_y);

#endif