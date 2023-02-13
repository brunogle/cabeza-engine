#include "transposition.h"
#include <random>

TranspositionTable::TranspositionTable(int size){
    
    for(int piece_num = 0; piece_num < 10; piece_num++){
        for(int pos = 0; pos < 10; pos++){
            this->zobrist_piece_pos[piece_num][pos] = this->get_rand_64();
        }
    }

    this->zobrist_blue_turn = this->get_rand_64();

    

    this->transposition_table_size = 1;
    for(; size != 1; size >>= 1)
        this->transposition_table_size <<= 1;

    
    this->hash_key_mask = this->transposition_table_size - 1;

    this->transposition_table = new tt_entry[hash_key_mask];

}

TranspositionTable::~TranspositionTable(){
    delete[] this->transposition_table;
}

uint64_t TranspositionTable::get_rand_64(){
    static std::mt19937_64 rng(5489ULL);

    return rng();

}

uint64_t TranspositionTable::get_hash(positioning::game_state state){

    uint64_t hash = 0ULL;

    for(int piece_num = 0; piece_num < 10; piece_num++){
        hash ^= zobrist_piece_pos[piece_num][positioning::get_pos_from_bitboard(state.pieces[piece_num].bitboard)];
    }

    if(state.turn == positioning::Team::blue)
        hash ^= zobrist_blue_turn;

    return hash;
}

