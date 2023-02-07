#ifndef EVAL_H
#define EVAL_H

#include "positioning.h"




namespace DistanceEval{

    int eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Team cabeza_team, positioning::Team pieces_team);
    int eval_cabeza_distance_to_win(positioning::game_state state, positioning::Team team);

    int eval(positioning::game_state state);

}


#endif