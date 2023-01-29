#ifndef GRAPHICS_H
#define GRAPHICS_H

#include <windows.h>
#include "positioning.h"

#define THIN_PIECE_CHAR L'▒' //Character to use when a single height piece is shown
#define THICK_PIECE_CHAR L'█' //Character to use when a double height piece is shown

#define RED_PLAYER_CABEZA_COLOR FOREGROUND_RED
#define BLUE_PLAYER_CABEZA_COLOR FOREGROUND_BLUE
#define RED_PLAYER_PIECE_COLOR FOREGROUND_RED
#define BLUE_PLAYER_PIECE_COLOR FOREGROUND_BLUE

#define BOARD_COLOR FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN

namespace graphics{

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

    const float kEvaluationGraphLimit = 25.0;


    //Graphics calculated constants

    const int kDrawingWidth = kBoardDrawingWidth + kEvaluationMeterBarGraphWidth + kEvaluationMeterValueCharNum + 6;   //Calculate total size of window
    const int kDrawingHeight = kBoardDrawingHeight + 5;

    const int kGraphicArraySize = kDrawingHeight*kDrawingWidth; //Size of the array needed to represent board image

    /*
    Struct used to pass secondary details onto the board drawing
    */
    struct board_graphic_details{
        bool shade[10][10];
    };

    extern board_graphic_details no_detail_struct; //Default no aditional details structure. Defined in graphics.cpp

    int init_graphics_console();
    

    /*
    Generates CHAR_INFO array for use in WINAPI WriteConsoleOutptputW
    Accetps a game_state and optionally a board_graphic_details to produce the array.
    */
    int generate_unicode_board(positioning::game_state state, CHAR_INFO drawing[kGraphicArraySize], board_graphic_details details = no_detail_struct);


    /*
    Calls generate_unicode_board and draws it in console at specific coordinates
    */
    int draw_on_console(positioning::game_state state, board_graphic_details details=no_detail_struct);

}


#endif