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

struct ColoredMove{
    Player player;
    positioning::move move;
};

class Game{

    public:

    Game(eval_func_t evaluator_function, Search * search_engine);

    int create_graphics_window();     //Creates a graphics window to display game
    int terminate_graphics_window();  //Terminates graphics window associated to the game

    int undo_moves(int semimoves_to_undo);

    int reset_game_state();//Sets game state to initial configuration

    int apply_pgn(std::string move_string); //Applies the movement string to the current board

    bool apply_move(positioning::move move);

    bool apply_move_str(std::string move);

    void reset();

    bool set_fen(std::string fen);

    Player get_winner();

    Player get_turn();

    positioning::move search();
    
    std::string get_game_pgn();

    positioning::game_state current_game_state; //Current game state (piece possitions and turn)
    int update_graphics(); //Updates graphics window with current game state


    private:

    void update_win_flag();
 

    void check_if_win();

    eval_func_t evaluator_function;
    Search * search_engine;


    bool graphics_enabled = false; //Graphics window enabled flag

    GraphicsWindow graphics_window; //Graphics window class

    Player winner = Player::none;

    std::vector<ColoredMove> move_history;
    std::vector<positioning::game_state> game_state_history;

    bool valid_game;

};

#endif