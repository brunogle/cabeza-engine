#ifndef SEARCH_H
#define SEARCH_H

#include "positioning.h"
#include "eval.h"


typedef std::function<positioning::move(positioning::game_state, eval_func_t)> search_func_t;

namespace RandomMoveSearch{

    using namespace positioning;

    move search(game_state state, eval_func_t eval_func);

}

namespace SimpleMoveSearch{

    using namespace positioning;

    move search(game_state state, eval_func_t eval_func);

}

#endif