#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>
#include "positioning.h"
#include "graphics_console.h"
#include "eval.h"

#include <functional>
#include <vector>

#define THIN_PIECE_CHAR L'▒' //Character to use when a single height piece is shown
#define THICK_PIECE_CHAR L'█' //Character to use when a double height piece is shown

#define RED_PLAYER_CABEZA_COLOR FOREGROUND_RED
#define BLUE_PLAYER_CABEZA_COLOR FOREGROUND_BLUE
#define RED_PLAYER_PIECE_COLOR FOREGROUND_RED
#define BLUE_PLAYER_PIECE_COLOR FOREGROUND_BLUE

#define BOARD_COLOR FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN

namespace consts{

    //Board drawing user-defined constants

    const int kInSquareWidth = 6;  //Inner width of a board square
    const int kInSquareHeight = 3; //Inner height of a board square


    //Board drawing calculated constants

    const int kBoardDrawingWidth = kInSquareWidth*10 + 11;   //Calculate total size of the board image
    const int kBoardDrawingHeight = kInSquareHeight*10 + 11;


    //Evaluation meter user-defined constants

    const int kEvaluationMeterX = kBoardDrawingWidth + 4; //X coordinate of left side of evaluation meter
    const int kEvaluationMeterY = 0; //Y coordinate of top side of evaluation meter

    const int kEvaluationMeterHeight = kBoardDrawingHeight;
    const int kEvaluationMeterBarGraphWidth = 3;

    const int kEvaluationMeterValueCharNum = 4;

    const int kEvaluationGraphLimit = 40;


    //Graphics calculated constants

    const int kDrawingWidth = kBoardDrawingWidth + kEvaluationMeterBarGraphWidth + kEvaluationMeterValueCharNum + 6;   //Calculate total size of window
    const int kDrawingHeight = kBoardDrawingHeight + 5;

    const int kGraphicArraySize = kDrawingHeight*kDrawingWidth; //Size of the array needed to represent board image

}

class GraphicsWindow{

    private:

    GraphicsWindowHandler board_graphics_console;

    int draw_board_graphic(positioning::game_state state, std::vector<std::vector<CHAR_INFO>> & image_drawing);
    int draw_eval_meter(positioning::game_state state, std::vector<std::vector<CHAR_INFO>> & image_drawing);
    int draw_board_turn_label(positioning::game_state state, std::vector<std::vector<CHAR_INFO>> & image_drawing);
    int draw_board_labels(std::vector<std::vector<CHAR_INFO>> & image_drawing);

    /*
    Generates CHAR_INFO array for use in WINAPI WriteConsoleOutptputW
    Accetps a game_state and optionally a board_graphic_details to produce the array.
    */
    int generate_unicode_board(positioning::game_state state, CHAR_INFO drawing[consts::kGraphicArraySize]);

    eval_func_t evaluator_function;

    public:

    GraphicsWindow();

    int init_graphics_console();

    /*
    Calls generate_unicode_board and draws it in console at specific coordinates
    */
    int draw_on_console(positioning::game_state state);

    int stop_graphics_console();

    int set_evaluator_function(eval_func_t evaluator_function);

};


#endif