#ifndef TRANSPOSITION_H
#define TRANSPOSITION_H

#include "positioning.h"

enum node_type{
    exact_node,
    alpha_node,
    beta_node
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
        
        ~TranspositionTable();

        uint64_t zobrist_piece_pos[10][100][3]; //100 position for 10 pieces
        uint64_t zobrist_blue_turn;

        uint64_t get_rand_64();

        uint64_t get_hash(positioning::game_state state);

        void save(uint64_t hash, int val, node_type type, uint8_t depth, positioning::move best_move);

        bool probe(uint64_t hash, uint8_t depth, int alpha, int beta, positioning::move * best_move_out, int * val_out);

        void clear();

        tt_entry * transposition_table;

        int transposition_table_size;

        uint64_t hash_key_mask;

};


#endif