#include "transposition.h"
#include "positioning.h"
#include <random>
#include <string.h>

TranspositionTable::TranspositionTable(int size){
    
    for(int piece_num = 0; piece_num < 5; piece_num++){
        for(int pos = 0; pos < 100; pos++){
            for(int orientation = 0; orientation < 3; orientation++){
                this->zobrist_piece_pos[piece_num][positioning::red][pos][orientation] = this->get_rand_64();
            }
        }
    }
    for(int piece_num = 0; piece_num < 5; piece_num++){
        for(int pos = 0; pos < 100; pos++){
            for(int orientation = 0; orientation < 3; orientation++){
                this->zobrist_piece_pos[piece_num][positioning::blue][pos][orientation] = this->get_rand_64();
            }
        }
    }
    this->zobrist_blue_turn = this->get_rand_64();

    

    this->transposition_table_size = 1;
    for(; size != 1; size >>= 1)
        this->transposition_table_size <<= 1;

    
    this->hash_key_mask = this->transposition_table_size - 1;

    this->transposition_table = new tt_entry[transposition_table_size];

    memset(this->transposition_table, 0, sizeof(tt_entry)*this->transposition_table_size);

}

TranspositionTable::~TranspositionTable(){
    delete[] this->transposition_table;
}

uint64_t TranspositionTable::get_rand_64(){
    static std::mt19937_64 rng(5489ULL);

    return rng();

}

uint64_t TranspositionTable::get_hash(positioning::game_state state){

    using enum positioning::Player;

    uint64_t hash = 0ULL;

    for(int piece_num = 0; piece_num < 5; piece_num++){
        hash ^= this->zobrist_piece_pos[piece_num][red][positioning::get_pos_from_bitboard(state.pieces[piece_num][red].bitboard)][state.pieces[piece_num][red].o];
        hash ^= this->zobrist_piece_pos[piece_num][blue][positioning::get_pos_from_bitboard(state.pieces[piece_num][blue].bitboard)][state.pieces[piece_num][blue].o];
    }

    if(state.turn == positioning::Player::blue)
        hash ^= this->zobrist_blue_turn;

    return hash;
}

void TranspositionTable::save(uint64_t hash, int val, node_type type, uint8_t depth, positioning::move best_move){

    tt_entry * tt_loc = &this->transposition_table[hash & this->hash_key_mask];

    

    if((tt_loc->hash == hash) && (tt_loc->depth > depth)){
        return;
    }

    tt_loc->hash = hash;
    tt_loc->val = val;
    tt_loc->type = type;
    tt_loc->depth = depth;
    tt_loc->best_move = best_move;
    
}

bool TranspositionTable::probe(uint64_t hash, uint8_t depth, int alpha, int beta, positioning::move * best_move_out, int * val_out){

    int pos = hash & this->hash_key_mask;

    tt_entry * tt_loc = &this->transposition_table[pos];


    if (tt_loc->hash == hash) {

        *best_move_out = tt_loc->best_move; //Used for sorting purposes

        if (tt_loc->depth >= depth) {

            switch (tt_loc->type){
                case exact_node:
                    *val_out =  tt_loc->val;
                    return true;
                    break;
                
                case alpha_node:
                    if(tt_loc->val <= alpha){
                        *val_out = alpha;
                        return true;
                    }
                    break;
                
                case beta_node:
                    if(tt_loc->val >= beta){
                        *val_out = beta;
                        return true;
                    }
                    break;

                default:
                    break;
            }

        }

    }

    return false;

}

void TranspositionTable::clear(){
    memset(this->transposition_table, 0, sizeof(tt_entry)*this->transposition_table_size);
    
}