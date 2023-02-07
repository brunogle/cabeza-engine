#include <windows.h>
#include <vector>
#include <string>
#include <cmath>


#include "graphics.h"
#include "graphics_console.h"
#include "eval.h"

#include <limits>
using std::vector;
using namespace consts;


int null_evaluator_function(positioning::game_state state){
    return 0;
}

GraphicsWindow::GraphicsWindow(){
	this->evaluator_function = null_evaluator_function;
}

int GraphicsWindow::init_graphics_console(){
	board_graphics_console.initializeWindow(kDrawingWidth, kDrawingHeight);
	return 1;
}

int GraphicsWindow::stop_graphics_console(){
	board_graphics_console.closeWindow();
	return 1;
}

int GraphicsWindow::set_evaluator_function(std::function<int(positioning::game_state)> evaluator_function){
	this->evaluator_function = evaluator_function;
	return 1;
}

int GraphicsWindow::draw_board_graphic(positioning::game_state state, vector<vector<CHAR_INFO>> & image_drawing){

	vector< vector<CHAR_INFO> > board_drawing(kBoardDrawingHeight, vector<CHAR_INFO> (kBoardDrawingWidth));

	//Draw board grid
	for(int i = 0; i < kBoardDrawingHeight; i++){
		for(int j = 0; j < kBoardDrawingWidth; j++){
			board_drawing[i][j].Attributes = BOARD_COLOR;
			if(i == 0){
				if(j%(kInSquareWidth + 1) == 0){
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
				if(j%(kInSquareWidth + 1) == 0){
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
			else if(i%(kInSquareHeight + 1) == 0){
				if(j == 0){
					board_drawing[i][j].Char.UnicodeChar = L'├';
				}
				else if(j == kBoardDrawingWidth-1){
					board_drawing[i][j].Char.UnicodeChar = L'┤';
				}
				else if(j%(kInSquareWidth + 1) == 0){
					board_drawing[i][j].Char.UnicodeChar = L'┼';
				}
				else{
					board_drawing[i][j].Char.UnicodeChar = L'─';
				}

			}
			else{
				if(j%(kInSquareWidth + 1) == 0){
					board_drawing[i][j].Char.UnicodeChar = L'│';
				}
				else{
					int square_x = (j - 1)/(kInSquareWidth + 1);
					int square_y = (i - 1)/(kInSquareHeight + 1);
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

		x_offset = x*(kInSquareWidth + 1) + 1;
		y_offset = y*(kInSquareHeight + 1) + 1;

		//Cabeza doesn't use piece_width, piece_height

		board_drawing[y_offset][x_offset].Char.UnicodeChar = L'╰';
		board_drawing[y_offset + kInSquareHeight - 1][x_offset].Char.UnicodeChar = L'╭';
		board_drawing[y_offset][x_offset + kInSquareWidth - 1].Char.UnicodeChar = L'╯';
		board_drawing[y_offset + kInSquareHeight - 1][x_offset + kInSquareWidth - 1].Char.UnicodeChar = L'╮';

		for(int i = 0; i < kInSquareHeight; i++){
			for(int j = 0; j < kInSquareWidth; j++){
				board_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}

		for(int i = 1; i < kInSquareWidth - 1; i++){
			board_drawing[y_offset][x_offset + i].Char.UnicodeChar = L'─';
			board_drawing[y_offset + kInSquareHeight - 1][x_offset + i].Char.UnicodeChar = L'─';
		}

		for(int i = 1; i < kInSquareHeight- 1; i++){
			board_drawing[y_offset + i][x_offset].Char.UnicodeChar = L'│';
			board_drawing[y_offset + i][x_offset + kInSquareWidth - 1].Char.UnicodeChar = L'│';
		}
	
		color = player_num == (int)positioning::Team::red ? RED_PLAYER_PIECE_COLOR : BLUE_PLAYER_PIECE_COLOR; 

		//Draw MINI piece

		pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5 + 1].bitboard);
		x = pos%10;
		y = pos/10;

		x_offset = x*(kInSquareWidth + 1) + 1;
		y_offset = y*(kInSquareHeight + 1) + 1;

		//Mini doesn't use piece_width, piece_height

		for(int i = 0; i < kInSquareHeight; i++){
			for(int j = 0; j < kInSquareWidth; j++){
				board_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THIN_PIECE_CHAR;
				board_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		}

		//Draw FLACO piece

		pos = positioning::get_pos_from_bitboard(state.pieces[player_num*5 + 2].bitboard);
		x = pos%10;
		y = pos/10;

		x_offset = x*(kInSquareWidth + 1) + 1;
		y_offset = y*(kInSquareHeight + 1) + 1;

		piece_width = state.pieces[player_num*5 + 2].o == positioning::Orientation::horizontal ? (kInSquareWidth*2+1) : kInSquareWidth;
		piece_height = state.pieces[player_num*5 + 2].o == positioning::Orientation::vertical ? (kInSquareHeight*2+1) : kInSquareHeight;

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

		x_offset = x*(kInSquareWidth + 1) + 1;
		y_offset = y*(kInSquareHeight + 1) + 1;

		piece_width = state.pieces[player_num*5 + 3].o == positioning::Orientation::vertical ? kInSquareWidth : (kInSquareWidth*2+1);
		piece_height = state.pieces[player_num*5 + 3].o == positioning::Orientation::horizontal ? kInSquareHeight: (kInSquareHeight*2+1);

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

		x_offset = x*(kInSquareWidth + 1) + 1;
		y_offset = y*(kInSquareHeight + 1) + 1;

		piece_width = kInSquareWidth*2 + 1;
		piece_height = kInSquareHeight*2 + 1;

		for(int i = 0; i < piece_height; i++){
			for(int j = 0; j < piece_width; j++){
				board_drawing[y_offset + i][x_offset + j].Char.UnicodeChar = THICK_PIECE_CHAR;
				board_drawing[y_offset + i][x_offset + j].Attributes = color;
			}
		} 	

	}

	for(int i = 0; i < kBoardDrawingHeight; i++){
		for(int j = 0; j < kBoardDrawingWidth; j++){
			image_drawing[i][j] = board_drawing[kBoardDrawingHeight - i - 1][j];
		}
	}

	return 0;
}

int GraphicsWindow::draw_board_labels(vector<vector<CHAR_INFO>> & image_drawing){
	for(int i = 0; i < 10; i++){ //Draw labels
		for(int j = 0; j < 2; j++)
			image_drawing[i*(kInSquareHeight + 1) + kInSquareHeight/2 + 1][kBoardDrawingWidth + j].Char.UnicodeChar = L"109 8 7 6 5 4 3 2 1 "[i*2 + j];
		
		image_drawing[kBoardDrawingHeight][i*(kInSquareWidth + 1) + kInSquareWidth/2 + 1].Char.UnicodeChar = L"abcdefghij"[i];
	}

	return 0;

}

int GraphicsWindow::draw_board_turn_label(positioning::game_state state, vector<vector<CHAR_INFO>> & image_drawing){
	
	if(state.turn == positioning::Team::red){
		for(int i = 0; i < 9; i++){
			image_drawing[kBoardDrawingHeight + 3][kBoardDrawingWidth/2 + i].Char.UnicodeChar = L"Turn: Red"[i];
			if(i > 5)
				image_drawing[kBoardDrawingHeight + 3][kBoardDrawingWidth/2 + i].Attributes = BACKGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			else
				image_drawing[kBoardDrawingHeight + 3][kBoardDrawingWidth/2 + i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		}
	}
	else{
		for(int i = 0; i < 10; i++){
			image_drawing[kBoardDrawingHeight + 3][kBoardDrawingWidth/2 + i].Char.UnicodeChar = L"Turn: Blue"[i];
			if(i > 5)
				image_drawing[kBoardDrawingHeight + 3][kBoardDrawingWidth/2 + i].Attributes = BACKGROUND_BLUE | FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
			else
				image_drawing[kBoardDrawingHeight + 3][kBoardDrawingWidth/2 + i].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		}
	}

	return 0; 
}

int GraphicsWindow::draw_eval_meter(positioning::game_state state, vector<vector<CHAR_INFO>> & image_drawing){
	int score = this->evaluator_function(state);
	float bounded_score = score;

	if(bounded_score > kEvaluationGraphLimit)
		bounded_score = kEvaluationGraphLimit;
	if(bounded_score < -kEvaluationGraphLimit)
		bounded_score = -kEvaluationGraphLimit;


	//image_drawing[meter_upper_y][meter_left_x].Char.UnicodeChar = ''

	int bar_graph_top = kEvaluationMeterY + kEvaluationMeterHeight*(1.0 + bounded_score/kEvaluationGraphLimit)/2.0;

	if(bar_graph_top > kEvaluationMeterY + kEvaluationMeterHeight - 1){
		bar_graph_top = kEvaluationMeterY + kEvaluationMeterHeight - 1;
	}
	else if(bar_graph_top < kEvaluationMeterY + 1){
		bar_graph_top = kEvaluationMeterY + 1;
	}

	for(int j = kEvaluationMeterX; j < kEvaluationMeterX + kEvaluationMeterBarGraphWidth; j++){

		for(int i = 0; i < kEvaluationMeterHeight; i++){

			int y_coord = kEvaluationMeterY + i;

			float dist_from_center =  i - kBoardDrawingHeight/2;

			float score_at_pos = 2.0*kEvaluationGraphLimit*dist_from_center/kBoardDrawingHeight;

			if(bounded_score == 0){
				if(dist_from_center == 0){
					image_drawing[i][j].Char.UnicodeChar = L'█';
					image_drawing[i][j].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
				}
			}
			else if(bounded_score > 0){
				if(i <= bar_graph_top && dist_from_center >= 0){
					image_drawing[i][j].Char.UnicodeChar = L'█';
					image_drawing[i][j].Attributes = FOREGROUND_RED;						
				}
			}
			else if(bounded_score < 0){
				if(i >= bar_graph_top && dist_from_center <= 0){
					image_drawing[i][j].Char.UnicodeChar = L'█';
					image_drawing[i][j].Attributes = FOREGROUND_BLUE;						
				}
			}

		}
	}

	for(int j = 0; j < kEvaluationMeterValueCharNum; j++){

		int x_coord = kEvaluationMeterX + kEvaluationMeterBarGraphWidth + j + 1;

		std::wstring score_str;
		if(score == std::numeric_limits<int>::max()){
			score_str = L"+inf";
		}
		else if(score == -std::numeric_limits<int>::max()){
			score_str = L"-inf";
		}
		else{
			score_str = std::to_wstring(score);
		}

		
		if(j < score_str.length())
			image_drawing[bar_graph_top][x_coord].Char.UnicodeChar = score_str[j];
		
		if(score == 0){
			image_drawing[bar_graph_top][x_coord].Attributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED;
		}
		else if(score > 0){
			image_drawing[bar_graph_top][x_coord].Attributes = FOREGROUND_RED;
		}
		if(score < 0){
			image_drawing[bar_graph_top][x_coord].Attributes = FOREGROUND_BLUE;
		}
	}

	return 0;
}

/*
Generates CHAR_INFO array for use in WINAPI WriteConsoleOutptputW
Accetps a game_state and optionally a board_graphic_details to produce the array.
*/
int GraphicsWindow::generate_unicode_board(positioning::game_state state, CHAR_INFO drawing[kGraphicArraySize]){

	vector<std::vector<CHAR_INFO>> final_drawing(kDrawingHeight, vector<CHAR_INFO>(kDrawingWidth));

	for(int i = 0; i < kDrawingHeight; i++){
		for(int j = 0; j < kDrawingWidth; j++){
			final_drawing[i][j].Attributes = BOARD_COLOR;
		}
	}

	draw_board_graphic(state, final_drawing);

	draw_board_labels(final_drawing);

	draw_board_turn_label(state, final_drawing);
	
	draw_eval_meter(state, final_drawing);

	//Draw evaluation score meter

	//int 
	for(int i = 0; i < kDrawingHeight; i++){
		for(int j = 0; j < kDrawingWidth; j++){
			drawing[i*kDrawingWidth + j] = final_drawing[i][j];
		}
	}		



	return 0;
}

/*
Calls generate_unicode_board and draws it in console at specific coordinates
*/
int GraphicsWindow::draw_on_console(positioning::game_state state){
	CHAR_INFO drawing[kGraphicArraySize];

	generate_unicode_board(state, drawing);

	board_graphics_console.drawOnConsole(drawing);
	
	return 1;
}