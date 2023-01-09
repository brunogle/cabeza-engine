#include "graphics.h"

#include <windows.h>
#include "movements.h"

/*
Generates CHAR_INFO array ready to be used with WriteConsoleOutputW for displaying board
*/
int generate_unicode_board(game_state state, CHAR_INFO drawing[DRAWING_HEIGHT*DRAWING_WIDTH]){

	wchar_t temp_drawing[DRAWING_HEIGHT][DRAWING_WIDTH]; //Tempoprary drawing
    WORD temp_drawing_color[DRAWING_HEIGHT][DRAWING_WIDTH];
	//Draw board grid
	for(int i = 0; i < DRAWING_HEIGHT; i++){
		for(int j = 0; j < DRAWING_WIDTH; j++){
			temp_drawing_color[i][j] = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			if(i == 0){
				if(j%5 == 0){
					if(j == 0){
						temp_drawing[i][j] = L'┌';
					}
					else if(j == DRAWING_WIDTH-1){
						temp_drawing[i][j] = L'┐';
					}
					else{
						temp_drawing[i][j] = L'┬';
					}
				}
				else{
					temp_drawing[i][j] = L'─';
				}
			}
			else if(i == DRAWING_HEIGHT - 1){
				if(j%5 == 0){
					if(j == 0){
						temp_drawing[i][j] = L'└';
					}
					else if(j == DRAWING_WIDTH-1){
						temp_drawing[i][j] = L'┘';
					}
					else{
						temp_drawing[i][j] = L'┴';
					}
				}
				else{
					temp_drawing[i][j] = L'─';
				}				
			}
			else if(i%3 == 0){
				if(j == 0){
					temp_drawing[i][j] = L'├';
				}
				else if(j == DRAWING_WIDTH-1){
					temp_drawing[i][j] = L'┤';
				}
				else if(j%5 == 0){
					temp_drawing[i][j] = L'┼';
				}
				else{
					temp_drawing[i][j] = L'─';
				}
			}
			else{
				if(j%5 == 0){
					temp_drawing[i][j] = L'│';
				}
				else{
					temp_drawing[i][j] = L' ';
				}
			}
		}
	}

	player_state player_states[] = {state.blue, state.red};
	
	for(int player_num = 0; player_num <= 1; player_num++){
		player_state player = player_states[player_num];

		int color = player_num == 0 ? FOREGROUND_BLUE : FOREGROUND_RED;
		wchar_t thin_piece = L'▒';
		wchar_t thick_piece = L'█';


		//Draw cabeza
		temp_drawing[3 * (8 - player.cabeza.y) + 1][(player.cabeza.x - 1)*5 + 1] = L'╭';
		temp_drawing[3 * (8 - player.cabeza.y) + 1][(player.cabeza.x - 1)*5 + 2] = L'─';
		temp_drawing[3 * (8 - player.cabeza.y) + 1][(player.cabeza.x - 1)*5 + 3] = L'─';
		temp_drawing[3 * (8 - player.cabeza.y) + 1][(player.cabeza.x - 1)*5 + 4] = L'╮';
		temp_drawing[3 * (8 - player.cabeza.y) + 2][(player.cabeza.x - 1)*5 + 1] = L'╰';
		temp_drawing[3 * (8 - player.cabeza.y) + 2][(player.cabeza.x - 1)*5 + 2] = L'─';
		temp_drawing[3 * (8 - player.cabeza.y) + 2][(player.cabeza.x - 1)*5 + 3] = L'─';
		temp_drawing[3 * (8 - player.cabeza.y) + 2][(player.cabeza.x - 1)*5 + 4] = L'╯';
		for(int i = 3 * (8 - player.cabeza.y) + 1; i <= 3 * (8 - player.cabeza.y) + 2; i++){
			for(int j = (player.cabeza.x - 1)*5 + 1; j <= (player.cabeza.x - 1)*5 + 4; j++){
				temp_drawing_color[i][j] = color;
			}
		}

		//Draw mini
		for(int i = 1; i <= 2; i++){
			for(int j = 1; j <= 4; j++){
				temp_drawing[3 * (8 - player.mini.y) + i][(player.mini.x - 1)*5 + j] = thin_piece;
				temp_drawing_color[3 * (8 - player.mini.y) + i][(player.mini.x - 1)*5 + j] = color;
			}			
		}
		//Draw flaco
		uint8_t x_low = player.chato.x1;
		uint8_t y_low = 9 - player.chato.y1;
		uint8_t x_high = player.chato.x2;
		uint8_t y_high = 9 - player.chato.y2;
		if(x_low > x_high){
			uint8_t temp = x_low;
			x_low = x_high;
			x_high = temp;
		}
		if(y_low > y_high){
			uint8_t temp = y_low;
			y_low = y_high;
			y_high = temp;
		}
		wchar_t draw_char = (x_low == x_high) || (y_low == y_high) ? thick_piece : thin_piece;
		for(int i = (y_low - 1)*3+1; i <= (y_high - 1)*3+2; i++){
			for(int j = (x_low - 1)*5+1; j <= (x_high - 1)*5+4; j++){
				temp_drawing[i][j] = draw_char;
				temp_drawing_color[i][j] = color;
			}			
		}
		//Draw flaco
		x_low = player.flaco.x1;
		y_low = 9 - player.flaco.y1;
		x_high = player.flaco.x2;
		y_high = 9 - player.flaco.y2;
		if(x_low > x_high){
			uint8_t temp = x_low;
			x_low = x_high;
			x_high = temp;
		}
		if(y_low > y_high){
			uint8_t temp = y_low;
			y_low = y_high;
			y_high = temp;
		}
		draw_char = (x_low == x_high) && (y_low == y_high) ? thick_piece : thin_piece;
		for(int i = (y_low - 1)*3+1; i <= (y_high - 1)*3+2; i++){
			for(int j = (x_low - 1)*5+1; j <= (x_high - 1)*5+4; j++){
				temp_drawing[i][j] = draw_char;
				temp_drawing_color[i][j] = color;
			}			
		}
		//Draw gordo
		x_low = player.gordo.x1;
		y_low = 9 - player.gordo.y1;
		x_high = player.gordo.x2;
		y_high = 9 - player.gordo.y2;
		if(x_low > x_high){
			uint8_t temp = x_low;
			x_low = x_high;
			x_high = temp;
		}
		if(y_low > y_high){
			uint8_t temp = y_low;
			y_low = y_high;
			y_high = temp;
		}
		for(int i = (y_low - 1)*3+1; i <= (y_high - 1)*3+2; i++){
			for(int j = (x_low - 1)*5+1; j <= (x_high - 1)*5+4; j++){
				temp_drawing[i][j] = thick_piece;
				temp_drawing_color[i][j] = color;
			}			
		}
	}

	for(int i = 0; i < DRAWING_HEIGHT; i++){
		for(int j = 0; j < DRAWING_WIDTH; j++){
			drawing[i*DRAWING_WIDTH + j].Char.UnicodeChar = temp_drawing[i][j];
            drawing[i*DRAWING_WIDTH + j].Attributes = temp_drawing_color[i][j];
		}
	}

	return 0;
}

/*
Draws board on console with handle hConsole at coordinates coord_x, coord_y in console
*/
int draw_on_console(game_state state, HANDLE hConsole, int coord_x, int coord_y){
    CHAR_INFO drawing[25*41];
	generate_unicode_board(state, drawing);


	COORD coordBufSize ;
	coordBufSize.X = DRAWING_WIDTH;
	coordBufSize.Y = DRAWING_HEIGHT;

	COORD coordBufCoord;
	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

    SMALL_RECT srctWriteRect;
	srctWriteRect.Bottom = DRAWING_HEIGHT + coord_y;
	srctWriteRect.Left = 0 + coord_x;
	srctWriteRect.Top = 0 + coord_y;
	srctWriteRect.Right = DRAWING_WIDTH + coord_x;

	WriteConsoleOutputW(hConsole, drawing, coordBufSize, coordBufCoord, &srctWriteRect);
    return 1;
}

