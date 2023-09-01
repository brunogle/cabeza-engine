#include "game.h"
#include "positioning.h"
#include "parsing.h"
#include <sstream>
#include <string.h>
#include <iostream>
#include "color.hpp"


Game::Game(Search * search_engine){
    this->search_engine = search_engine;
    this->reset();
}

Game::Game(){
    this->reset();
}

void Game::set_search_engine(Search * search_engine){
    this->search_engine = search_engine;
}

void Game::reset(){
    parsing::parse_fen("3hcmf3/3hoof3/4oo4/10/10/10/10/4OO4/3FOOH3/3FMCH3,r", this->current_game_state);

    this->game_state_history.clear();
    //this->game_state_history.push_back(this->current_game_state);

    this->move_history.clear();

}


bool Game::set_fen(std::string fen){
    return parsing::parse_fen(fen, this->current_game_state);
}

std::string Game::get_fen(){
    return parsing::generate_fen(this->current_game_state);
}

positioning::move Game::search(){
    return search_engine->search(this->current_game_state);
}

bool Game::check_win(){
    bool win;
    if(positioning::check_for_win(this->current_game_state)){
        return true;
    }
    else{
        switch_turn();
        win = positioning::check_for_win(this->current_game_state);
        switch_turn();
    }
    return win;
}


bool Game::apply_move(positioning::move move){

    positioning::game_state new_game_state;

    if(positioning::apply_move_safe(move, this->current_game_state, new_game_state)){

        this->move_history.push_back(move);
        this->game_state_history.push_back(this->current_game_state);

        this->current_game_state = new_game_state;

        return true;
    }
    else{
        return false;
    }

}


bool Game::undo_moves(int semimoves_to_undo){

    if(this->game_state_history.size() >= (uint64_t)semimoves_to_undo){

        for(int i = 0; i < semimoves_to_undo - 1; i++){
            this->move_history.pop_back();
            this->game_state_history.pop_back();
        }

        this->current_game_state = this->game_state_history.back();
        this->game_state_history.pop_back();

        return true;

    }

    else{
        return false;
    }


}

void Game::switch_turn(){
    this->current_game_state.turn = (this->current_game_state.turn == positioning::red) ? positioning::blue : positioning::red;
}

void Game::set_turn(positioning::Player player){
    this->current_game_state.turn = player;
}


std::string Game::get_game_drawing(bool color){

    using namespace positioning;

    std::string board_drawing = "";

    std::string red_color_str = color ? COLOR_RED : "";
    std::string blue_color_str = color ? COLOR_BLUE : "";
    std::string reset_color_str = color ? COLOR_RESET : "";

    for(int i = 9; i >= 0; i--){//row
        for(int j = 0; j < 10; j++){//col

            bitboard_t current_square_bitboard = (bitboard_t)1 << (j + i*10);

            if(this->current_game_state.pieces[cabeza_idx][red].bitboard & current_square_bitboard){
                board_drawing += red_color_str + "CC " + reset_color_str;
            }
            else if(this->current_game_state.pieces[mini_idx][red].bitboard & current_square_bitboard){
                board_drawing += red_color_str + "MM " + reset_color_str;
            }
            else if(this->current_game_state.pieces[flaco_idx][red].bitboard & current_square_bitboard){
                board_drawing += red_color_str + "FF " + reset_color_str;
            }
            else if(this->current_game_state.pieces[chato_idx][red].bitboard & current_square_bitboard){
                board_drawing += red_color_str + "HH " + reset_color_str;
            }
            else if(this->current_game_state.pieces[gordo_idx][red].bitboard & current_square_bitboard){
                board_drawing += red_color_str + "OO " + reset_color_str;
            }
            else if(this->current_game_state.pieces[cabeza_idx][blue].bitboard & current_square_bitboard){
                board_drawing += blue_color_str + "cc " + reset_color_str;
            }
            else if(this->current_game_state.pieces[mini_idx][blue].bitboard & current_square_bitboard){
                board_drawing += blue_color_str + "mm " + reset_color_str;
            }
            else if(this->current_game_state.pieces[flaco_idx][blue].bitboard & current_square_bitboard){
                board_drawing += blue_color_str + "ff " + reset_color_str;
            }
            else if(this->current_game_state.pieces[chato_idx][blue].bitboard & current_square_bitboard){
                board_drawing += blue_color_str + "hh " + reset_color_str;
            }
            else if(this->current_game_state.pieces[gordo_idx][blue].bitboard & current_square_bitboard){
                board_drawing += blue_color_str + "oo " + reset_color_str;
            }
            else{
                board_drawing += ".. ";
            }
        }
        board_drawing += "\n";
    }
    board_drawing += "          Turn:" + ((this->current_game_state.turn == red) ? (red_color_str + "red") : (blue_color_str + "blue")) + reset_color_str;

    return board_drawing;
}

positioning::Player Game::get_turn(){
    return this->current_game_state.turn;
}


