#ifndef EVAL_H
#define EVAL_H

#define MAX_INT (std::numeric_limits<int>::max() >> 2)
#define MIN_INT (-std::numeric_limits<int>::max() >> 2)


#include "positioning.h"
#include <functional>

typedef int (* eval_func_t)(positioning::game_state);

namespace DistanceEval{

    int eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Player cabeza_team, positioning::Player pieces_team);
    int eval_cabeza_distance_to_win(positioning::game_state state, positioning::Player team);

    int eval(positioning::game_state state);
    //eval_func_t eval;
}


#endif