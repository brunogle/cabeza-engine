#include "search.h"
#include <functional>
#include <limits>
#include <iostream>

namespace RandomMoveSearch{

    positioning::move search(positioning::game_state state, eval_func_t eval_func){

        positioning::move movements[MAX_POSSIBLE_MOVEMENTS];

        int number_moves = get_moves_as_list(state, movements);

        return movements[rand()%number_moves];

    }

}

namespace SimpleMoveSearch{

    positioning::move search(positioning::game_state state, eval_func_t eval_func){

        positioning::move movements[MAX_POSSIBLE_MOVEMENTS];

        int number_moves = get_moves_as_list(state, movements);

        positioning::move best_move;

        int best_move_score;

        if(state.turn == positioning::Team::red)
            best_move_score = -100000;
        else
            best_move_score = 100000;

        for(int i = 0; i < number_moves; i++){
            positioning::game_state test_game_state = positioning::apply_move(state, movements[i]);

            int score = eval_func(test_game_state);
            
            if(state.turn == positioning::Team::red){
                if(score > best_move_score){
                    best_move_score = score;
                    best_move = movements[i];
                }
            }
            else{
                if(score < best_move_score){
                    best_move_score = score;
                    best_move = movements[i];
                }                
            }
        }

        return best_move;

    }

}

namespace AlphaBetaSearch{

    using namespace positioning;

    int x = 0;

    const int search_depth = 6;

    move killer_moves[search_depth][search_depth];


    int alpha_beta(game_state state, int alpha, int beta, int depth, bool is_pv, eval_func_t eval_func){
        x++;
        int ply = search_depth - depth;

        int score = eval_func(state);

        if(depth == 0){
            return score;
        }

        if(check_for_win(state)){
            return -MAX_INT;
        }

        int win_score = MAX_INT - ply;
        if (alpha < -win_score) alpha = -win_score;
        if (beta > win_score - 1) beta = win_score - 1;
        //if (alpha >= beta) return alpha;

        int val = MIN_INT;

        move possible_moves[MAX_POSSIBLE_MOVEMENTS];
        
        int num_moves = get_moves_as_list(state, possible_moves);

        for(int i = 0; i < num_moves; i++){
            move killer_move = killer_moves[0][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[0];
                possible_moves[0] = killer_move;
                possible_moves[i] = temp;
            }
        }
        for(int i = 0; i < num_moves; i++){
            move killer_move = killer_moves[1][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[1];
                possible_moves[1] = killer_move;
                possible_moves[i] = temp;
            }
        }

        bool search_pv = true;
        for(int i = 0; i < num_moves; i++){
            game_state new_state = apply_move(state, possible_moves[i]);

            if(search_pv){
                val = -alpha_beta(new_state, -beta, -alpha, depth-1, is_pv, eval_func);
            }
            else{
                val = -alpha_beta(new_state, -alpha-1, -alpha, depth-1, false, eval_func);  
                if(val > alpha){
                    val = -alpha_beta(new_state, -beta, -alpha, depth-1, true, eval_func);
                }
            }
            if(val >= beta){
                return beta;
            }
            if(val > alpha){
                alpha = val;
                search_pv = false;
                if(is_pv){
                    killer_moves[ply][ply] = possible_moves[i];
                    if(abs(val) == MAX_INT - 1){//If node analyzed is a terminal node
                        killer_moves[ply][ply + 1].move = -1; //If move bitarray is set to -1, this is a terminal node used ony for killer moves.     
                    }
                    else{
                        for(int j = ply + 1; j < search_depth; j++){
                            killer_moves[ply][j] = killer_moves[ply+1][j];
                        }
                    }
                }

            }


        }
        return alpha;

        /*
        for(int i = 0; i < num_moves; i++){
                game_state new_state = apply_move(state, possible_moves[i]);

            val = -alpha_beta(new_state, -beta, -alpha, depth-1, eval_func);
            if(val > alpha){
                alpha = val;

                killer_moves[ply][ply] = possible_moves[i];
                if(abs(val) == MAX_INT){//If node analyzed is a terminal node
                    killer_moves[ply][ply + 1].move = -1; //If move bitarray is set to -1, this is a terminal node used ony for killer moves.     
                }
                else{
                    for(int j = ply + 1; j < search_depth; j++){
                        killer_moves[ply][j] = killer_moves[ply+1][j];
                    }
                }

            }
            if(val >= beta){
                return beta;
            }

        }
        return alpha;
        */
    }

    std::pair<move, int> root_search(game_state state, int depth, eval_func_t eval_func){

        int alpha = MIN_INT;
        int beta = MAX_INT;

        for(int i = 0; i < search_depth; i++)
            for(int j = 0; j < search_depth; j++){
                killer_moves[i][j].move = 0;
            }


        move possible_moves[MAX_POSSIBLE_MOVEMENTS];
        int num_moves = get_moves_as_list(state, possible_moves);

        int ply = search_depth - depth;

        move best_move = possible_moves[0];
        int best_move_val;

        int val;
        for(int i = 0; i < num_moves; i++){
            move killer_move = killer_moves[0][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[0];
                possible_moves[0] = killer_move;
                possible_moves[i] = temp;
            }
        }
        for(int i = 0; i < num_moves; i++){
            move killer_move = killer_moves[1][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[1];
                possible_moves[1] = killer_move;
                possible_moves[i] = temp;
            }
        }

        for(int i = 0; i < num_moves; i++){

            move killer_move = killer_moves[0][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[0];
                possible_moves[0] = killer_move;
                possible_moves[i] = temp;
            }
        }
        for(int i = 0; i < num_moves; i++){

            move killer_move = killer_moves[0][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[0];
                possible_moves[0] = killer_move;
                possible_moves[i] = temp;
            }
        }

        bool search_pv = true;
        for(int i = 0; i < num_moves; i++){
            game_state new_state = apply_move(state, possible_moves[i]);

            if(search_pv){
                val = -alpha_beta(new_state, -beta, -alpha, depth-1, true, eval_func);
            }
            else{
                val = -alpha_beta(new_state, -alpha-1, -alpha, depth-1, false, eval_func);  
                if(val > alpha){
                    val = -alpha_beta(new_state, -beta, -alpha, depth-1, true, eval_func);
                }
            }


            if(val > alpha){
                alpha = val;
                search_pv = false;
                best_move = possible_moves[i];
                killer_moves[ply][ply] = possible_moves[i];
                if(abs(val) == MAX_INT){//If node analyzed is a terminal node
                    killer_moves[ply][ply + 1].move = -1; //If move bitarray is set to -1, this is a terminal node used ony for killer moves.     
                }
                else{
                    for(int j = ply + 1; j < search_depth; j++){
                        killer_moves[ply][j] = killer_moves[ply+1][j];
                    }
                }

            }
            if(val >= beta){
                return std::pair<move, int>(best_move, beta);
            }

        }

        return std::pair<move, int>(best_move, alpha);

    }

    move search(game_state state, eval_func_t eval_func){
        x = 0;
        move possible_moves[MAX_POSSIBLE_MOVEMENTS];

        int num_moves = get_moves_as_list(state, possible_moves);

        int scores[MAX_POSSIBLE_MOVEMENTS];

        std::pair<move, int> result = root_search(state, search_depth, eval_func);

        move best_move = result.first;
        int node_val = result.second;

        std::cout << "pv: ";
        for(int i = 0; i < search_depth; i++){
            if(killer_moves[0][i].move == -1){
                if( (i%2 == 0) == (state.turn == Team::red))
                    std::cout << " END: Blue win";
                else
                    std::cout << " END: Red wins";
                break;
            }
            else{
                std::cout << get_move_str(killer_moves[0][i]) << " ";
            }
        }
        std::cout << std::endl;

        /*
        if(node_val > MAX_INT - 100){
            if(state.turn == Team::red)
                std::cout << "Red wins in " << MAX_INT - node_val << std::endl;
            else
                std::cout << "Blue wins in " << MAX_INT - node_val << std::endl;

        }
        */


        return best_move;

    }

}
