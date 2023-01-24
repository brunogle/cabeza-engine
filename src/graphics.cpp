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
		CHAR_INFO board_drawing[kBoardDrawingHeight][kBoardDrawingWidth] = {0};


		CHAR_INFO final_drawing[kDrawingHeight][kDrawingWidth] = {0};

		for(int i = 0; i < kDrawingHeight; i++){
			for(int j = 0; j < kDrawingWidth; j++){
				final_drawing[i][j].Attributes = BOARD_COLOR;
			}
		}


		//Draw board grid
		for(int i = 0; i < kBoardDrawingHeight; i++){
			for(int j = 0; j < kBoardDrawingWidth; j++){
				board_drawing[i][j].Attributes = BOARD_COLOR;
				if(i == 0){
					if(j%(IN_SQUARE_WIDTH + 1) == 0){
						if(j == 0){
							board_drawing[i][j].Char.UnicodeChar = L'└';
						}
						else if(j == kBoardDrawingWidth-1){
							board_drawing[i][j].Char.UnicodeChar = L'┘';
						}
						else{
							board_drawing[i][j].Char.UnicodeChar = L'┴';
						}
					}
					else{
						board_drawing[i][j].Char.UnicodeChar = L'─';
					}
				}
				else if(i == kBoardDrawingHeight - 1){
					if(j%(IN_SQUARE_WIDTH + 1) == 0){
						if(j == 0){
							board_drawing[i][j].Char.UnicodeChar = L'┌';
						}
						else if(j == kBoardDrawingWidth-1){
							board_drawing[i][j].Char.UnicodeChar = L'┐';
						}
						else{
							board_drawing[i][j].Char.UnicodeChar = L'┬';
						}
					}
					else{
						board_drawing[i][j].Char.UnicodeChar = L'─';
					}				

				}
				else if(i%(IN_SQUARE_HEIGHT + 1) == 0){
					if(j == 0){
						board_drawing[i][j].Char.UnicodeChar = L'├';
					}
					else if(j == kBoardDrawingWidth-1){
						board_drawing[i][j].Char.UnicodeChar = L'┤';
					}
					else if(j%(IN_SQUARE_WIDTH + 1) == 0){
						board_drawing[i][j].Char.UnicodeChar = L'┼';
					}
					else{
						board_drawing[i][j].Char.UnicodeChar = L'─';
					}

				}
				else{
					if(j%(IN_SQUARE_WIDTH + 1) == 0){
						board_drawing[i][j].Char.UnicodeChar = L'│';
					}
					else{
						int square_x = (j - 1)/(IN_SQUARE_WIDTH + 1);
						int square_y = (i - 1)/(IN_SQUARE_HEIGHT + 1);
						if(details.shade != NULL){
							if(details.shade[square_x][square_y]){
								board_drawing[i][j].Attributes = BACKGROUND_RED | BACKGROUND_GREEN;
							}
						}
						board_drawing[i][j].Char.UnicodeChar = L' ';
					}
				}
			}
		}

		//Draw pieces for each player

		for (int player_num = 0; player_num <= 1; player_num++){ //0 = red, 1 = blue


			int color = player_num == (int)positioning::Team::red ? RED_PLAYER_CABEZA_COLOR : BLUE_PLAYER_CABEZA_COLOR; 
			

			int x_offset, y_offset; //Represents lower left character of the piece to draw
			int piece_width, piece_height; //Represents piece rectange draw size in characters
		
			int pos, x, y;

			//Draw CABEZA piece

			pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5].bitboard);
			x = pos%10;
			y = pos/10;

			x_offset = x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = y*(IN_SQUARE_HEIGHT + 1) + 1;

			//Cabeza doesn't use piece_width, piece_height

			board_drawing[y_offset][x_offset].Char.UnicodeChar = L'╰';
			board_drawing[y_offset + IN_SQUARE_HEIGHT - 1][x_offset].Char.UnicodeChar = L'╭';
			board_drawing[y_offset][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'╯';
			board_drawing[y_offset + IN_SQUARE_HEIGHT - 1][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'╮';

			for(int i = 0; i < IN_SQUARE_HEIGHT; i++){
				for(int j = 0; j < IN_SQUARE_WIDTH; j++){
					board_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}

			for(int i = 1; i < IN_SQUARE_WIDTH - 1; i++){
				board_drawing[y_offset][x_offset + i].Char.UnicodeChar = L'─';
				board_drawing[y_offset + IN_SQUARE_HEIGHT - 1][x_offset + i].Char.UnicodeChar = L'─';
			}

			for(int i = 1; i < IN_SQUARE_HEIGHT- 1; i++){
				board_drawing[y_offset + i][x_offset].Char.UnicodeChar = L'│';
				board_drawing[y_offset + i][x_offset + IN_SQUARE_WIDTH - 1].Char.UnicodeChar = L'│';
			}
		
			color = player_num == (int)positioning::Team::red ? RED_PLAYER_PIECE_COLOR : BLUE_PLAYER_PIECE_COLOR; 

			//Draw MINI piece

			pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5 + 1].bitboard);
			x = pos%10;
			y = pos/10;

			x_offset = x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = y*(IN_SQUARE_HEIGHT + 1) + 1;

			//Mini doesn't use piece_width, piece_height

			for(int i = 0; i < IN_SQUARE_HEIGHT; i++){
				for(int j = 0; j < IN_SQUARE_WIDTH; j++){
					board_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THIN_PIECE_CHAR;
					board_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}

			//Draw FLACO piece

			pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5 + 2].bitboard);
			x = pos%10;
			y = pos/10;

			x_offset = x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = y*(IN_SQUARE_HEIGHT + 1) + 1;

			piece_width = state.pieces[player_num*5 + 2].o == positioning::Orientation::horizontal ? (IN_SQUARE_WIDTH*2+1) : IN_SQUARE_WIDTH;
			piece_height = state.pieces[player_num*5 + 2].o == positioning::Orientation::vertical ? (IN_SQUARE_HEIGHT*2+1) : IN_SQUARE_HEIGHT;

			for(int i = 0; i < piece_height; i++){
				for(int j = 0; j < piece_width; j++){
					board_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = (state.pieces[player_num*5 + 2].o == positioning::Orientation::flat) ? THICK_PIECE_CHAR : THIN_PIECE_CHAR;
					board_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}	

			//Draw CHATO piece

			pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5 + 3].bitboard);
			x = pos%10;
			y = pos/10;

			x_offset = x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = y*(IN_SQUARE_HEIGHT + 1) + 1;

			piece_width = state.pieces[player_num*5 + 3].o == positioning::Orientation::vertical ? IN_SQUARE_WIDTH : (IN_SQUARE_WIDTH*2+1);
			piece_height = state.pieces[player_num*5 + 3].o == positioning::Orientation::horizontal ? IN_SQUARE_HEIGHT: (IN_SQUARE_HEIGHT*2+1);

			for(int i = 0; i < piece_height; i++){
				for(int j = 0; j < piece_width; j++){
					board_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = (state.pieces[player_num*5 + 3].o == positioning::Orientation::flat) ? THIN_PIECE_CHAR : THICK_PIECE_CHAR;
					board_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}	

			//Draw GORDO piece

			pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5 + 4].bitboard);
			x = pos%10;
			y = pos/10;

			x_offset = x*(IN_SQUARE_WIDTH + 1) + 1;
			y_offset = y*(IN_SQUARE_HEIGHT + 1) + 1;

			piece_width = IN_SQUARE_WIDTH*2 + 1;
			piece_height = IN_SQUARE_HEIGHT*2 + 1;

			for(int i = 0; i < piece_height; i++){
				for(int j = 0; j < piece_width; j++){
					board_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THICK_PIECE_CHAR;
					board_drawing[y_offset + i][x_offset + j].Attributes = color;
				}
			}	

		}

		for(int i = 0; i < 10; i++){ //Draw labels
			final_drawing[kBoardMarginLower + i*(IN_SQUARE_HEIGHT + 1) + IN_SQUARE_HEIGHT/2 + 1][kBoardDrawingWidth].Char.UnicodeChar = L"abcdefghij"[i];
			final_drawing[kBoardMarginLower - 1][i*(IN_SQUARE_WIDTH + 1) + IN_SQUARE_WIDTH/2 + 1].Char.UnicodeChar = L"0123456789"[i];
		}


		if(state.turn == positioning::Team::red){
			for(int i = 0; i < 9; i++){
				final_drawing[kBoardMarginLower/2][kBoardDrawingWidth/2 + i].Char.UnicodeChar = L"Turn: Red"[i];
				if(i > 5)
					final_drawing[kBoardMarginLower/2][kBoardDrawingWidth/2 + i].Attributes = BACKGROUND_RED;
				else
					final_drawing[kBoardMarginLower/2][kBoardDrawingWidth/2 + i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			}
		}
		else{
			for(int i = 0; i < 10; i++){
				final_drawing[kBoardMarginLower/2][kBoardDrawingWidth/2 + i].Char.UnicodeChar = L"Turn: Blue"[i];
				if(i > 5)
					final_drawing[kBoardMarginLower/2][kBoardDrawingWidth/2 + i].Attributes = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				else
					final_drawing[kBoardMarginLower/2][kBoardDrawingWidth/2 + i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			}
		}
		

		for(int i = 0; i < kBoardDrawingHeight; i++){
			for(int j = 0; j < kBoardDrawingWidth; j++){
				final_drawing[i  + (kBoardMarginLower)][j] = board_drawing[i][j];
			}
		}

		for(int i = 0; i < kDrawingHeight; i++){
			for(int j = 0; j < kDrawingWidth; j++){
				drawing[i*kDrawingWidth + j] = final_drawing[kDrawingHeight - i - 1][j];
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