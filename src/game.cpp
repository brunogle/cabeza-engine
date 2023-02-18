#include "game.h"
#include "graphics.h"
#include "positioning.h"
#include <sstream>


Game::Game(eval_func_t evaluator_function, Search * search_engine){
    this->current_game_state = positioning::initial_game_state();
    this->evaluator_function = evaluator_function;
    this->graphics_window.set_evaluator_function(evaluator_function);
    this->search_engine = search_engine;
    this->game_state_history.push_back(this->current_game_state);
    this->valid_game = true;
}

int Game::create_graphics_window(){
    if(this->graphics_window.init_graphics_console()){
        this->graphics_enabled = true;
        this->update_graphics();
        return 1;
    }
    else{
        this->graphics_enabled = false;
        return false;
    }
}

int Game::terminate_graphics_window(){
    return this->graphics_window.stop_graphics_console();
}

int Game::update_graphics(){
    if(this->graphics_enabled){
        return this->graphics_window.draw_on_console(this->current_game_state);
    }
    else{
        return 0;
    }
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
    if(this->winner == Player::none){

        bool success;

        positioning::Team color_moved = this->current_game_state.turn;

        this->current_game_state = positioning::apply_move_safe(this->current_game_state, move, &success);

        if(success){

            ColoredMove performed_move = {(color_moved == positioning::Team::red) ? Player::red : Player::blue, move};

            this->move_history.push_back(performed_move);
            this->game_state_history.push_back(current_game_state);
        }


        this->update_win_flag();
        
        this->update_graphics();

        return success;
    }

    return 0;
}

bool Game::apply_move_str(std::string move_str){

    bool success;

    positioning::Team move_color = positioning::Team::red;

    if(move_str[0] == 'c' || move_str[0] == 'm' || move_str[0] == 'f' || move_str[0] == 'h' || move_str[0] == 'o'){
        move_color = positioning::Team::blue;
    }
    else if(move_str[0] == 'C' || move_str[0] == 'M' || move_str[0] == 'F' || move_str[0] == 'H' || move_str[0] == 'O'){
        move_color = positioning::Team::red;
    }

    positioning::move move = positioning::parse_movement_str(move_str);

    positioning::Team inital_turn = this->current_game_state.turn;

    this->current_game_state.turn = move_color;

    this->current_game_state = positioning::apply_move_safe(this->current_game_state, move, &success);
    
    if(move_color != inital_turn)
        this->current_game_state.turn = inital_turn; 

    if(success){
        
        ColoredMove performed_move = {(move_color == positioning::Team::red) ? Player::red : Player::blue, move};

        this->move_history.push_back(performed_move);
        this->game_state_history.push_back(current_game_state);
    }


    this->update_win_flag();
    
    this->update_graphics();

    return success;
    

    return 0;
}

// 3hcmf3/3hoof3/4oo4/10/10/10/10/4OO4/3FOOH3/3FMCH3,r,0
bool Game::set_fen(std::string fen){

    using namespace std;

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

    for(int i = 0; i < 10; i++){
        new_state.pieces[i].bitboard = (positioning::bitboard_t)0;
        new_state.pieces[i].o = positioning::Orientation::flat;
    }

    struct coord{
        int row;
        int col;
    };

    coord red_flaco_first_square = {-1,-1};
    coord red_chato_first_square = {-1,-1};

    coord blue_flaco_first_square = {-1,-1};
    coord blue_chato_first_square = {-1,-1};

    int character_count[10] = {0,0,0,0,0,0,0,0,0,0};


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
                new_state.pieces[0].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[0]++;
            }
            else if(fen_board_char == 'M'){
                new_state.pieces[1].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[1]++;
            }
            else if(fen_board_char == 'F'){
                new_state.pieces[2].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(red_flaco_first_square.row == -1){
                    red_flaco_first_square = {row,col};
                }
                else{
                    if(red_flaco_first_square.row == row){
                        new_state.pieces[2].o = positioning::Orientation::horizontal;
                    }
                    if(red_flaco_first_square.col == col){
                        new_state.pieces[2].o = positioning::Orientation::vertical;
                    }
                }
                character_count[2]++;
            }
            else if(fen_board_char == 'H'){
                new_state.pieces[3].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(red_chato_first_square.row == -1){
                    red_chato_first_square = {row,col};
                }
                else{
                    if(red_chato_first_square.row == row){
                        new_state.pieces[3].o = positioning::Orientation::horizontal;
                    }
                    else if(red_chato_first_square.col == col && new_state.pieces[3].o == positioning::Orientation::flat){
                        new_state.pieces[3].o = positioning::Orientation::vertical;
                    }
                    else if(red_chato_first_square.col == col && new_state.pieces[3].o == positioning::Orientation::horizontal){
                        new_state.pieces[3].o = positioning::Orientation::flat;
                    }
                }
                character_count[3]++;
            }
            else if(fen_board_char == 'O'){
                new_state.pieces[4].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                character_count[4]++;
            }
            else if(fen_board_char == 'c'){
                new_state.pieces[5].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[5]++;
            }
            else if(fen_board_char == 'm'){
                new_state.pieces[6].bitboard = (positioning::bitboard_t)1 << (row*10 + col);
                character_count[6]++;
            }
            else if(fen_board_char == 'f'){
                new_state.pieces[7].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(blue_flaco_first_square.row == -1){
                    blue_flaco_first_square = {row,col};
                }
                else{
                    if(blue_flaco_first_square.row == row){
                        new_state.pieces[7].o = positioning::Orientation::horizontal;
                    }
                    if(blue_flaco_first_square.col == col){
                        new_state.pieces[7].o = positioning::Orientation::vertical;
                    }
                }
                character_count[7]++;
            }
            else if(fen_board_char == 'h'){
                new_state.pieces[8].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                if(blue_chato_first_square.row == -1){
                    blue_chato_first_square = {row,col};
                }
                else{
                    if(blue_chato_first_square.row == row){
                        new_state.pieces[8].o = positioning::Orientation::horizontal;
                    }
                    else if(blue_chato_first_square.col == col && new_state.pieces[8].o == positioning::Orientation::flat){
                        new_state.pieces[8].o = positioning::Orientation::vertical;
                    }
                    else if(blue_chato_first_square.col == col && new_state.pieces[8].o == positioning::Orientation::horizontal){
                        new_state.pieces[8].o = positioning::Orientation::flat;
                    }
                }
                character_count[8]++;
            }
            else if(fen_board_char == 'o'){
                new_state.pieces[9].bitboard |= (positioning::bitboard_t)1 << (row*10 + col);
                character_count[9]++;
            }
            
            col_char_pos++;

        }
    }

    if(fen_split[1] == "r"){
        new_state.turn = positioning::Team::red;
    }
    else if(fen_split[1] == "b"){
        new_state.turn = positioning::Team::blue;
    }

    if( character_count[0] == 1 &&
        character_count[1] == 1 &&
        (character_count[2] == 1 || character_count[2] == 2)&&
        (character_count[3] == 2 || character_count[3] == 4) &&
        character_count[4] == 4 &&
        character_count[5] == 1 &&
        character_count[6] == 1 &&
        (character_count[7] == 1 || character_count[7] == 2)&&
        (character_count[8] == 2 || character_count[8] == 4) &&
        character_count[9] == 4){

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
        this->update_win_flag();

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

void Game::update_win_flag(){
    if(positioning::check_for_win(this->current_game_state)){
        if(this->current_game_state.turn == positioning::Team::red){
            this->winner = Player::blue;
        }
        else{
            this->winner = Player::red;
        }
    }
    else{
        this->winner = Player::none;
    }
}

Player Game::get_winner(){
    return this->winner;
}

Player Game::get_turn(){
    if(this->winner != Player::none){
        return Player::none;
    }

    if(this->current_game_state.turn == positioning::Team::red){
        return Player::red;
    }
    else{
        return Player::blue;
    }
}

std::string Game::get_game_pgn(){

    std::string ret = "";

    for(ColoredMove m : this->move_history){

        std::string uncolored_move_str = positioning::get_move_str(m.move);

        std::string colored_move;
        if(m.player == Player::red)
            colored_move += std::toupper(uncolored_move_str[0]);
        else
            colored_move += uncolored_move_str[0];
        colored_move += uncolored_move_str.substr(1);

        ret += colored_move + ",";


    }

    ret.pop_back();

    return ret;
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