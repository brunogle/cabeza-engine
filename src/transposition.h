#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "positioning.h"

enum node_type{
    pv_node,
    cut_node,
    all_node
};

struct tt_entry{
    uint64_t hash;
    node_type type;
    int val;
    uint8_t depth;
    positioning::move best_move;
};

class TranspositionTable{


    public:
        TranspositionTable(int size);

        uint64_t zobrist_piece_pos[10][100]; //100 position for 10 pieces
        uint64_t zobrist_blue_turn;

        uint64_t get_rand_64();

        uint64_t get_hash(positioning::game_state state);

        tt_entry * transposition_table;

        int transposition_table_size;

        uint64_t hash_key_mask;

};


#endif