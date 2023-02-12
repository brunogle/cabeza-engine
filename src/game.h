#ifndef GAME_H
#define GAME_H

#include <functional>
#include <vector>


#include "eval.h"
#include "search.h"
#include "positioning.h"
#include "graphics.h"

enum Player{
    red = 1,
    blue = -1,
    none = 0
};

class Game{

    public:

    Game(eval_func_t evaluator_function, search_func_t search_function);

    int create_graphics_window();     //Creates a graphics window to display game
    int terminate_graphics_window();  //Terminates graphics window associated to the game

    int undo_moves(int semimoves_to_undo);

    int reset_game_state();//Sets game state to initial configuration

    int apply_pgn(std::string move_string); //Applies the movement string to the current board

    int apply_move(positioning::move move);


    Player get_winner();

    Player get_turn();

    positioning::move search();
    
    std::string get_game_pgn();

    private:

    void update_win_flag();
 
    int update_graphics(); //Updates graphics window with current game state

    void check_if_win();

    eval_func_t evaluator_function;
    search_func_t search_function;

    positioning::game_state current_game_state; //Current game state (piece possitions and turn)

    bool graphics_enabled = false; //Graphics window enabled flag

    GraphicsWindow graphics_window; //Graphics window class

    Player winner = Player::none;

    std::vector<positioning::move> move_history;
    std::vector<positioning::game_state> game_state_history;

};

#endif