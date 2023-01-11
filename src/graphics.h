#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "movements.h"
#include<windows.h>

#define IN_SQUARE_WIDTH  5
#define IN_SQUARE_HEIGHT 2

const int drawing_width = IN_SQUARE_WIDTH*10 + 11;
const int drawing_height = IN_SQUARE_HEIGHT*10 + 11;

const int graphic_array_size = drawing_height*drawing_width;


#define THIN_PIECE_CHAR L'▒'
#define THICK_PIECE_CHAR L'█'

int generate_unicode_board(game_state state, CHAR_INFO drawing[graphic_array_size]);
int draw_on_console(game_state state, HANDLE hConsole, int coord_x, int coord_y);

#endif