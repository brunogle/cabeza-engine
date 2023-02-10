#include "game.h"
#include "graphics.h"
#include "positioning.h"



Game::Game(eval_func_t evaluator_function, search_func_t search_function){
    this->current_game_state = positioning::initial_game_state();
    this->evaluator_function = evaluator_function;
    this->graphics_window.set_evaluator_function(evaluator_function);
    this->search_function = search_function;
    this->game_state_history.push_back(this->current_game_state);
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
    bool success;
    this->current_game_state = positioning::apply_pgn(this->current_game_state, move_string, &success);
    this->update_win_flag();
    this->update_graphics();
    return success ;
}

int Game::apply_move(positioning::move move){
    if(this->winner == Player::none){

        bool success;

        this->current_game_state = positioning::apply_move_safe(this->current_game_state, move, &success);

        if(success){
            this->move_history.push_back(move);
            this->game_state_history.push_back(current_game_state);
        }


        this->update_win_flag();
        
        this->update_graphics();

        this->move_history.push_back(move);

        return success;
    }

    return 0;
}

int Game::undo_moves(int semimoves_to_undo){

    if(this->game_state_history.size() > semimoves_to_undo){

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

positioning::move Game::search(){
    return search_function(this->current_game_state, this->evaluator_function);
}