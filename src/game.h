#ifndef GAME_H
#define GAME_H

#include <functional>
#include <vector>


#include "eval.h"
#include "positioning.h"
#include "search.h"
//#include "positioning.h"



class Game{

    public:

    Game(Search * search_engine);

    Game();

    void set_search_engine(Search * search_engine);

    void reset();

    bool set_fen(std::string fen);

    std::string get_fen();

    positioning::move search();
    
    bool apply_move(positioning::move move);

    bool undo_moves(int semimoves_to_undo);

    void switch_turn();

    void set_turn(positioning::Player player);

    bool check_win();
    
    std::string get_game_drawing(bool color);

    positioning::Player get_turn();


    private:

    positioning::game_state current_game_state;

    eval_func_t evaluator_function;
    Search * search_engine;

    std::vector<positioning::move> move_history;
    std::vector<positioning::game_state> game_state_history;

    bool valid_game;

};

#endif