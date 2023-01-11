#include "graphics.h"

#include <windows.h>
#include "movements.h"

/*
Generates CHAR_INFO array ready to be used with WriteConsoleOutputW for displaying board
*/
int generate_unicode_board(game_state state, CHAR_INFO drawing[graphic_array_size]){


	//Tempoprary drawing. Pixels are drawn with corrdinates oriented as player's board. Not as console coordinates
	CHAR_INFO temp_drawing[drawing_height][drawing_width];


	//Draw board grid
	for(int i = 0; i < drawing_height; i++){
		for(int j = 0; j < drawing_width; j++){
			temp_drawing[i][j].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
			if(i == 0){
				if(j%(IN_SQUARE_WIDTH + 1) == 0){
					if(j == 0){
						temp_drawing[i][j].Char.UnicodeChar = L'└';
					}
					else if(j == drawing_width-1){
						temp_drawing[i][j].Char.UnicodeChar = L'┘';
					}
					else{
						temp_drawing[i][j].Char.UnicodeChar = L'┴';
					}
				}
				else{
					temp_drawing[i][j].Char.UnicodeChar = L'─';
				}
			}
			else if(i == drawing_height - 1){
				if(j%(IN_SQUARE_WIDTH + 1) == 0){
					if(j == 0){
						temp_drawing[i][j].Char.UnicodeChar = L'┌';
					}
					else if(j == drawing_width-1){
						temp_drawing[i][j].Char.UnicodeChar = L'┐';
					}
					else{
						temp_drawing[i][j].Char.UnicodeChar = L'┬';
					}
				}
				else{
					temp_drawing[i][j].Char.UnicodeChar = L'─';
				}				
			}
			else if(i%(IN_SQUARE_HEIGHT + 1) == 0){
				if(j == 0){
					temp_drawing[i][j].Char.UnicodeChar = L'├';
				}
				else if(j == drawing_width-1){
					temp_drawing[i][j].Char.UnicodeChar = L'┤';
				}
				else if(j%(IN_SQUARE_WIDTH + 1) == 0){
					temp_drawing[i][j].Char.UnicodeChar = L'┼';
				}
				else{
					temp_drawing[i][j].Char.UnicodeChar = L'─';
				}
			}
			else{
				if(j%(IN_SQUARE_WIDTH + 1) == 0){
					temp_drawing[i][j].Char.UnicodeChar = L'│';
				}
				else{
					temp_drawing[i][j].Char.UnicodeChar = L' ';
				}
			}
		}
	}

	//Draw pieces for each player
	player_state player_states[] = {state.blue, state.red};
	for (int player_num = 0; player_num <= 1; player_num++){
		player_state player = player_states[player_num];

		int color = player_num == 0 ? FOREGROUND_BLUE | FOREGROUND_INTENSITY : FOREGROUND_RED | FOREGROUND_INTENSITY; 
		

		int x_offset, y_offset; //Represents lower left character of the piece to draw
		int piece_width, piece_height; //Represents piece rectange draw size in characters


		//Draw CABEZA piece
		x_offset = player.cabeza.x*(IN_SQUARE_WIDTH + 1) + 1;
		y_offset = player.cabeza.y*(IN_SQUARE_HEIGHT + 1) + 1;

		//Cabeza doesn't use piece_width, piece_height

		temp_drawing[y_offset][x_offset].Char.UnicodeChar = L'╰';
		temp_drawing[y_offset + IN_SQUARE_HEIGHT - 1][x_offset].Char.UnicodeChar = L'╭';
		temp_drawing[y_offset][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'╯';
		temp_drawing[y_offset + IN_SQUARE_HEIGHT - 1][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'╮';

		for(int i = 0; i < IN_SQUARE_HEIGHT; i++){
			for(int j = 0; j < IN_SQUARE_WIDTH; j++){
				temp_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}

		for(int i = 1; i < IN_SQUARE_WIDTH - 1; i++){
			temp_drawing[y_offset][x_offset + i].Char.UnicodeChar = L'─';
			temp_drawing[y_offset + IN_SQUARE_HEIGHT - 1][x_offset + i].Char.UnicodeChar = L'─';
		}

		for(int i = 1; i < IN_SQUARE_HEIGHT- 1; i++){
			temp_drawing[y_offset + i][x_offset].Char.UnicodeChar = L'─';
			temp_drawing[y_offset + i][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'─';
		}
	

		//Draw MINI piece
		x_offset = player.mini.x*(IN_SQUARE_WIDTH + 1) + 1;
		y_offset = player.mini.y*(IN_SQUARE_HEIGHT + 1) + 1;

		//Mini doesn't use piece_width, piece_height

		for(int i = 0; i < IN_SQUARE_HEIGHT; i++){
			for(int j = 0; j < IN_SQUARE_WIDTH; j++){
				temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THIN_PIECE_CHAR;
				temp_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}

		//Draw FLACO piece
		x_offset = player.flaco.x*(IN_SQUARE_WIDTH + 1) + 1;
		y_offset = player.flaco.y*(IN_SQUARE_HEIGHT + 1) + 1;

		piece_width = player.flaco.o == orient_horizontal ? (IN_SQUARE_WIDTH*2+1) : IN_SQUARE_WIDTH;
		piece_height = player.flaco.o == orient_vertical ? (IN_SQUARE_HEIGHT*2+1) : IN_SQUARE_HEIGHT;

		for(int i = 0; i < piece_height; i++){
			for(int j = 0; j < piece_width; j++){
				temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = (player.flaco.o == orient_flat) ? THICK_PIECE_CHAR : THIN_PIECE_CHAR;
				temp_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}	

		//Draw CHATO piece
		x_offset = player.chato.x*(IN_SQUARE_WIDTH + 1) + 1;
		y_offset = player.chato.y*(IN_SQUARE_HEIGHT + 1) + 1;

		piece_width = player.chato.o == orient_vertical ? IN_SQUARE_WIDTH : (IN_SQUARE_WIDTH*2+1);
		piece_height = player.chato.o == orient_horizontal ? IN_SQUARE_HEIGHT: (IN_SQUARE_HEIGHT*2+1);

		for(int i = 0; i < piece_height; i++){
			for(int j = 0; j < piece_width; j++){
				temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = (player.chato.o == orient_flat) ? THIN_PIECE_CHAR : THICK_PIECE_CHAR;
				temp_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}	

		//Draw GORDO piece
		x_offset = player.gordo.x*(IN_SQUARE_WIDTH + 1) + 1;
		y_offset = player.gordo.y*(IN_SQUARE_HEIGHT + 1) + 1;

		piece_width = IN_SQUARE_WIDTH*2 + 1;
		piece_height = IN_SQUARE_HEIGHT*2 + 1;

		for(int i = 0; i < piece_height; i++){
			for(int j = 0; j < piece_width; j++){
				temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THICK_PIECE_CHAR;
				temp_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}	

	}

	for(int i = 0; i < drawing_height; i++){
		for(int j = 0; j < drawing_width; j++){
			drawing[i*drawing_width + j] = temp_drawing[drawing_height - i - 1][j];
		}
	}

	return 0;
}

/*
Draws board on console with handle hConsole at coordinates coord_x, coord_y in console
*/
int draw_on_console(game_state state, HANDLE hConsole, int coord_x, int coord_y){
    CHAR_INFO drawing[graphic_array_size];
	generate_unicode_board(state, drawing);


	COORD coordBufSize ;
	coordBufSize.X = drawing_width;
	coordBufSize.Y = drawing_height;

	COORD coordBufCoord;
	coordBufCoord.X = 0;
	coordBufCoord.Y = 0;

    SMALL_RECT srctWriteRect;
	srctWriteRect.Bottom = drawing_height + coord_y;
	srctWriteRect.Left = 0 + coord_x;
	srctWriteRect.Top = 0 + coord_y;
	srctWriteRect.Right = drawing_width + coord_x;

	WriteConsoleOutputW(hConsole, drawing, coordBufSize, coordBufCoord, &srctWriteRect);
    return 1;
}

