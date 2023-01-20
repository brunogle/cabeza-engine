#include <windows.h>


#include "graphics.h"
#include "graphics_console.h"

namespace graphics{

	board_graphic_details no_detail_struct = {NULL}; //Default no aditional details structure. Defined in graphics.cpp

	GraphicsConsole	board_graphics_console;

	int init_graphics_console(){
		board_graphics_console.initializeWindow(kDrawingWidth, kDrawingHeight);
		return 1;
	}

    /*
    Generates CHAR_INFO array for use in WINAPI WriteConsoleOutptputW
    Accetps a game_state and optionally a board_graphic_details to produce the array.
    */
	int generate_unicode_board(positioning::game_state state, CHAR_INFO drawing[kGraphicArraySize], board_graphic_details details){


		//Tempoprary drawing. Pixels are drawn with corrdinates oriented as player's board. Not as console coordinates
		CHAR_INFO temp_drawing[kDrawingHeight][kDrawingWidth];

		//Draw board grid
		for(int i = 0; i < kDrawingHeight; i++){
			for(int j = 0; j < kDrawingWidth; j++){
				temp_drawing[i][j].Attributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE;
				if(i == 0){
					if(j%(IN_SQUARE_WIDTH + 1) == 0){
						if(j == 0){
							temp_drawing[i][j].Char.UnicodeChar = L'└';
						}
						else if(j == kDrawingWidth-1){
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
				else if(i == kDrawingHeight - 1){
					if(j%(IN_SQUARE_WIDTH + 1) == 0){
						if(j == 0){
							temp_drawing[i][j].Char.UnicodeChar = L'┌';
						}
						else if(j == kDrawingWidth-1){
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
					else if(j == kDrawingWidth-1){
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
						int square_x = (j - 1)/(IN_SQUARE_WIDTH + 1);
						int square_y = (i - 1)/(IN_SQUARE_HEIGHT + 1);
						if(details.shade != NULL){
							if(details.shade[square_x][square_y]){
								temp_drawing[i][j].Attributes = BACKGROUND_RED | BACKGROUND_GREEN;
							}
						}
						temp_drawing[i][j].Char.UnicodeChar = L' ';
					}
				}
			}
		}

		//Draw pieces for each player

		for (int player_num = 0; player_num <= 1; player_num++){ //0 = red, 1 = blue


			int color = player_num == 0 ? FOREGROUND_RED : FOREGROUND_BLUE; 
			

			int x_offset, y_offset; //Represents lower left character of the piece to draw
			int piece_width, piece_height; //Represents piece rectange draw size in characters


			//Draw CABEZA piece
			x_offset = state.pieces[player_num*5].x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = state.pieces[player_num*5].y*(IN_SQUARE_HEIGHT + 1) + 1;

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
				temp_drawing[y_offset + i][x_offset].Char.UnicodeChar = L'│';
				temp_drawing[y_offset + i][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'│';
			}
		

			//Draw MINI piece
			x_offset = state.pieces[player_num*5 + 1].x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = state.pieces[player_num*5 + 1].y*(IN_SQUARE_HEIGHT + 1) + 1;

			//Mini doesn't use piece_width, piece_height

			for(int i = 0; i < IN_SQUARE_HEIGHT; i++){
				for(int j = 0; j < IN_SQUARE_WIDTH; j++){
					temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THIN_PIECE_CHAR;
					temp_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}

			//Draw FLACO piece
			x_offset = state.pieces[player_num*5 + 2].x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = state.pieces[player_num*5 + 2].y*(IN_SQUARE_HEIGHT + 1) + 1;

			piece_width = state.pieces[player_num*5 + 2].o == positioning::Orientation::horizontal ? (IN_SQUARE_WIDTH*2+1) : IN_SQUARE_WIDTH;
			piece_height = state.pieces[player_num*5 + 2].o == positioning::Orientation::vertical ? (IN_SQUARE_HEIGHT*2+1) : IN_SQUARE_HEIGHT;

			for(int i = 0; i < piece_height; i++){
				for(int j = 0; j < piece_width; j++){
					temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = (state.pieces[player_num*5 + 2].o == positioning::Orientation::flat) ? THICK_PIECE_CHAR : THIN_PIECE_CHAR;
					temp_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}	

			//Draw CHATO piece
			x_offset = state.pieces[player_num*5 + 3].x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = state.pieces[player_num*5 + 3].y*(IN_SQUARE_HEIGHT + 1) + 1;

			piece_width = state.pieces[player_num*5 + 3].o == positioning::Orientation::vertical ? IN_SQUARE_WIDTH : (IN_SQUARE_WIDTH*2+1);
			piece_height = state.pieces[player_num*5 + 3].o == positioning::Orientation::horizontal ? IN_SQUARE_HEIGHT: (IN_SQUARE_HEIGHT*2+1);

			for(int i = 0; i < piece_height; i++){
				for(int j = 0; j < piece_width; j++){
					temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = (state.pieces[player_num*5 + 3].o == positioning::Orientation::flat) ? THIN_PIECE_CHAR : THICK_PIECE_CHAR;
					temp_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}	

			//Draw GORDO piece
			x_offset = state.pieces[player_num*5 + 4].x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = state.pieces[player_num*5 + 4].y*(IN_SQUARE_HEIGHT + 1) + 1;

			piece_width = IN_SQUARE_WIDTH*2 + 1;
			piece_height = IN_SQUARE_HEIGHT*2 + 1;

			for(int i = 0; i < piece_height; i++){
				for(int j = 0; j < piece_width; j++){
					temp_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THICK_PIECE_CHAR;
					temp_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}	

		}

		for(int i = 0; i < kDrawingHeight; i++){
			for(int j = 0; j < kDrawingWidth; j++){
				drawing[i*kDrawingWidth + j] = temp_drawing[kDrawingHeight - i - 1][j];
			}
		}

		return 0;
	}

    /*
    Calls generate_unicode_board and draws it in console at specific coordinates
    */
	int draw_on_console(positioning::game_state state, board_graphic_details details){
		CHAR_INFO drawing[kGraphicArraySize];

		generate_unicode_board(state, drawing, details);

		board_graphics_console.drawOnConsole(drawing);
		
		return 1;
	}
}