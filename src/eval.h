#ifndef EVAL_H
#define EVAL_H

#define MAX_INT std::numeric_limits<int>::max()
#define MIN_INT -std::numeric_limits<int>::max()


#include "positioning.h"
#include <functional>

typedef std::function<int(positioning::game_state)> eval_func_t;

namespace DistanceEval{

    int eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Team cabeza_team, positioning::Team pieces_team);
    int eval_cabeza_distance_to_win(positioning::game_state state, positioning::Team team);

    int eval(positioning::game_state state);
    //eval_func_t eval;
}


namespace AlphaBetaSearch{

    using namespace positioning;

    int alpha_beta(game_state state, int alpha, int beta, int depth, eval_func_t eval_func);

    move search(game_state state, eval_func_t eval_func);

}

#endif