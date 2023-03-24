#ifndef PARSING_H
#define PARSING_H

#include "positioning.h"

namespace parsing{

    positioning::move parse_movement_str(std::string);


    std::string get_move_str(positioning::move);
    /*
    Apply PGN string
    */
    positioning::game_state apply_pgn(positioning::game_state, std::string, bool *);

    bool parse_fen(std::string fen_str, positioning::game_state & game_state_out);

}

#endif