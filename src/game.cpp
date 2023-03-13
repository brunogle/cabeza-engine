#include "game.h"
#include "positioning.h"
#include "parsing.h"
#include <sstream>
#include <string.h>
#include <iostream>
#include "color.hpp"


Game::Game(eval_func_t evaluator_function, Search * search_engine){
    this->current_game_state = positioning::initial_game_state();
    this->evaluator_function = evaluator_function;
    this->search_engine = search_engine;
    this->game_state_history.push_back(this->current_game_state);
    this->valid_game = true;
}

int Game::create_graphics_window(){
    return 0;
}

int Game::terminate_graphics_window(){
    return 0;
}

int Game::update_graphics(){
    return 0;
}

int Game::apply_pgn(std::string move_string){

    char * move_token = strtok(move_string.data(), ",");

    bool ok = true;


    while(move_token != NULL){
        this->apply_move_str(move_token);

        if(!ok){
            return 0;
        }

        move_token = strtok(NULL, ",");
        
    }

    return true;
}

bool Game::apply_move(positioning::move move){

    bool success;

    this->current_game_state = positioning::apply_move_safe(this->current_game_state, move, &success);

    if(success){

        this->move_history.push_back(move);
        this->game_state_history.push_back(current_game_state);
    }

    
    this->update_graphics();

    return success;

    return 0;
}

bool Game::apply_move_str(std::string move_str){

    bool success;

    positioning::Player move_color = positioning::Player::red;

    if(move_str[0] == 'c' || move_str[0] == 'm' || move_str[0] == 'f' || move_str[0] == 'h' || move_str[0] == 'o'){
        move_color = positioning::Player::blue;
    }
    else if(move_str[0] == 'C' || move_str[0] == 'M' || move_str[0] == 'F' || move_str[0] == 'H' || move_str[0] == 'O'){
        move_color = positioning::Player::red;
    }

    positioning::move move = parsing::parse_movement_str(move_str);

    positioning::Player inital_turn = this->current_game_state.turn;

    this->current_game_state.turn = move_color;

    this->current_game_state = positioning::apply_move_safe(this->current_game_state, move, &success);

    if(move_color != inital_turn)
        this->current_game_state.turn = inital_turn; 

    if(success){
        
        this->move_history.push_back(move);
        this->game_state_history.push_back(current_game_state);
    }

    
    this->update_graphics();

    return success;
    

    return 0;
}

// 3hcmf3/3hoof3/4oo4/10/10/10/10/4OO4/3FOOH3/3FMCH3,r,0
bool Game::set_fen(std::string fen){

    using namespace std;
    using namespace positioning;

    stringstream fen_ss(fen);
    vector<string> fen_split;

    string tmp;

    while(getline(fen_ss, tmp, ',')){
        fen_split.push_back(tmp);
    }

    stringstream board_segment_ss(fen_split[0]);
    vector<string> board_segment_split;

    while(getline(board_segment_ss, tmp, '/')){
        board_segment_split.push_back(tmp);
    }    

    positioning::game_state new_state;

    for(int i = 0; i < 5; i++){
        new_state.pieces[i][red].bitboard = (positioning::bitboard_t)0;
        new_state.pieces[i][blue].bitboard = (positioning::bitboard_t)0;
        new_state.pieces[i][red].o = positioning::Orientation::flat;
        new_state.pieces[i][blue].o = positioning::Orientation::flat;
    }

    struct coord{
        int row;
        int col;
    };

    coord red_flaco_first_square = {-1,-1};
    coord red_chato_first_square = {-1,-1};

    coord blue_flaco_first_square = {-1,-1};
    coord blue_chato_first_square = {-1,-1};

    int character_count[5][2] = {{0,0},{0,0},{0,0},{0,0},{0,0}};


    for(int row = 9; row >= 0; row--){
        if(board_segment_split[9-row] == "10"){
            continue;
        }
        int col_char_pos = 0;
        for(int col = 0; col < 10; col++){
            char fen_board_char = board_segment_split.at(9-row).at(col_char_pos);

            if('1' <= fen_board_char && fen_board_char <= '9'){
                col += fen_board_char - '0' - 1;
            }
            else if(fen_board_char == 'C'){
                new_state.pieces[cabeza_idx][red].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[cabeza_idx][red]++;
            }
            else if(fen_board_char == 'M'){
                new_state.pieces[mini_idx][red].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[mini_idx][red]++;
            }
            else if(fen_board_char == 'F'){
                new_state.pieces[flaco_idx][red].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(red_flaco_first_square.row == -1){
                    red_flaco_first_square = {row,col};
                }
                else{
                    if(red_flaco_first_square.row == row){
                        new_state.pieces[flaco_idx][red].o = positioning::Orientation::horizontal;
                    }
                    if(red_flaco_first_square.col == col){
                        new_state.pieces[flaco_idx][red].o = positioning::Orientation::vertical;
                    }
                }
                character_count[flaco_idx][red]++;
            }
            else if(fen_board_char == 'H'){
                new_state.pieces[chato_idx][red].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(red_chato_first_square.row == -1){
                    red_chato_first_square = {row,col};
                }
                else{
                    if(red_chato_first_square.row == row){
                        new_state.pieces[chato_idx][red].o = positioning::Orientation::horizontal;
                    }
                    else if(red_chato_first_square.col == col && new_state.pieces[chato_idx][red].o == positioning::Orientation::flat){
                        new_state.pieces[chato_idx][red].o = positioning::Orientation::vertical;
                    }
                    else if(red_chato_first_square.col == col && new_state.pieces[chato_idx][red].o == positioning::Orientation::horizontal){
                        new_state.pieces[chato_idx][red].o = positioning::Orientation::flat;
                    }
                }
                character_count[chato_idx][red]++;
            }
            else if(fen_board_char == 'O'){
                new_state.pieces[gordo_idx][red].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                character_count[gordo_idx][red]++;
            }
            else if(fen_board_char == 'c'){
                new_state.pieces[cabeza_idx][blue].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[cabeza_idx][blue]++;
            }
            else if(fen_board_char == 'm'){
                new_state.pieces[mini_idx][blue].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[mini_idx][blue]++;
            }
            else if(fen_board_char == 'f'){
                new_state.pieces[flaco_idx][blue].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(blue_flaco_first_square.row == -1){
                    blue_flaco_first_square = {row,col};
                }
                else{
                    if(blue_flaco_first_square.row == row){
                        new_state.pieces[flaco_idx][blue].o = positioning::Orientation::horizontal;
                    }
                    if(blue_flaco_first_square.col == col){
                        new_state.pieces[flaco_idx][blue].o = positioning::Orientation::vertical;
                    }
                }
                character_count[flaco_idx][blue]++;
            }
            else if(fen_board_char == 'h'){
                new_state.pieces[chato_idx][blue].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(blue_chato_first_square.row == -1){
                    blue_chato_first_square = {row,col};
                }
                else{
                    if(blue_chato_first_square.row == row){
                        new_state.pieces[chato_idx][blue].o = positioning::Orientation::horizontal;
                    }
                    else if(blue_chato_first_square.col == col && new_state.pieces[chato_idx][blue].o == positioning::Orientation::flat){
                        new_state.pieces[chato_idx][blue].o = positioning::Orientation::vertical;
                    }
                    else if(blue_chato_first_square.col == col && new_state.pieces[chato_idx][blue].o == positioning::Orientation::horizontal){
                        new_state.pieces[chato_idx][blue].o = positioning::Orientation::flat;
                    }
                }
                character_count[chato_idx][blue]++;
            }
            else if(fen_board_char == 'o'){
                new_state.pieces[gordo_idx][blue].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                character_count[gordo_idx][blue]++;
            }
            
            col_char_pos++;

        }
    }

    if(fen_split[1] == "r"){
        new_state.turn = positioning::Player::red;
    }
    else if(fen_split[1] == "b"){
        new_state.turn = positioning::Player::blue;
    }

    if( character_count[cabeza_idx][red] == 1 &&
        character_count[mini_idx][red] == 1 &&
        (character_count[flaco_idx][red] == 1 || character_count[flaco_idx][red] == 2)&&
        (character_count[chato_idx][red] == 2 || character_count[chato_idx][red] == 4) &&
        character_count[gordo_idx][red] == 4 &&
        character_count[cabeza_idx][blue] == 1 &&
        character_count[mini_idx][blue] == 1 &&
        (character_count[flaco_idx][blue] == 1 || character_count[flaco_idx][blue] == 2)&&
        (character_count[chato_idx][blue] == 2 || character_count[chato_idx][blue] == 4) &&
        character_count[gordo_idx][blue] == 4){

        this->current_game_state = new_state;
        this->update_graphics();
        return true;
    
    }

    return false;

}

int Game::undo_moves(int semimoves_to_undo){

    if(this->game_state_history.size() > (uint64_t)semimoves_to_undo){

        for(int i = 0; i < semimoves_to_undo; i++){
            this->move_history.pop_back();
            this->game_state_history.pop_back();
        }

        this->current_game_state = this->game_state_history.back();

        this->update_graphics();

        return 1;

    }

    else{
        return 0;
    }


}

int Game::reset_game_state(){
    this->current_game_state = positioning::initial_game_state();
    this->update_graphics();
    return 1;
}



positioning::Player Game::get_turn(){
    return this->current_game_state.turn;
}

std::string Game::get_game_pgn(){

    std::string ret = "";

    for(positioning::move m : this->move_history){

        std::string uncolored_move_str = parsing::get_move_str(m);

        std::string colored_move;
        if(m.player == positioning::Player::red)
            colored_move += std::toupper(uncolored_move_str[0]);
        else
            colored_move += uncolored_move_str[0];
        colored_move += uncolored_move_str.substr(1);

        ret += colored_move + ",";


    }

    ret.pop_back();

    return ret;
}

std::string Game::get_game_drawing(){

    using namespace positioning;

    for(int i = 9; i >= 0; i--){//row
        for(int j = 0; j < 10; j++){//col

            bitboard_t current_square_bitboard = (bitboard_t)1 << (j + i*10);

            if(this->current_game_state.pieces[cabeza_idx][red].bitboard & current_square_bitboard){
                std::cout << COLOR_RED << "C" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[mini_idx][red].bitboard & current_square_bitboard){
                std::cout << COLOR_RED << "M" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[flaco_idx][red].bitboard & current_square_bitboard){
                std::cout << COLOR_RED << "F" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[chato_idx][red].bitboard & current_square_bitboard){
                std::cout << COLOR_RED << "H" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[gordo_idx][red].bitboard & current_square_bitboard){
                std::cout << COLOR_RED << "O" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[cabeza_idx][blue].bitboard & current_square_bitboard){
                std::cout << COLOR_BLUE << "c" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[mini_idx][blue].bitboard & current_square_bitboard){
                std::cout << COLOR_BLUE << "m" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[flaco_idx][blue].bitboard & current_square_bitboard){
                std::cout << COLOR_BLUE << "f" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[chato_idx][blue].bitboard & current_square_bitboard){
                std::cout << COLOR_BLUE << "h" << COLOR_RESET;
            }
            else if(this->current_game_state.pieces[gordo_idx][blue].bitboard & current_square_bitboard){
                std::cout << COLOR_BLUE << "o" << COLOR_RESET;
            }
            else{
                std::cout << ".";
            }
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    return "";
}

void Game::reset(){
    this->current_game_state = positioning::initial_game_state();
    this->game_state_history.clear();
    this->game_state_history.push_back(this->current_game_state);
    this->move_history.clear();
    this->update_graphics();
}

positioning::move Game::search(){
    return search_engine->search(this->current_game_state);
}