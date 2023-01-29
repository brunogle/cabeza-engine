#ifndef EVAL_H
#define EVAL_H

#include "positioning.h"


namespace eval{

    /*
    Checks if a player won in last semimove.
    Only returns true if the player that has the win condition hast played last semimove (i.e. the player whoes turn is not)
    */
    bool check_for_win(positioning::game_state state);

    /*
    Main evaulation heuristic.
    */
    float eval_board(positioning::game_state state);

    float eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Team cabeza_team, positioning::Team pieces_team);

}
#endif