#ifndef EVAL_H
#define EVAL_H

#define MAX_INT (std::numeric_limits<int>::max() >> 2)
#define MIN_INT (-std::numeric_limits<int>::max() >> 2)


#include "positioning.h"
#include <functional>

typedef int (* eval_func_t)(positioning::game_state);

namespace DistanceEval{

    int eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Team cabeza_team, positioning::Team pieces_team);
    int eval_cabeza_distance_to_win(positioning::game_state state, positioning::Team team);

    int eval(positioning::game_state state);
    //eval_func_t eval;
}


#endif