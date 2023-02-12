#include "search.h"
#include <functional>
#include <limits>
#include <iostream>
#include "color.hpp"

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

    int nodes_searched = 0;

    const int max_search_depth = 10;

    const int max_seconds = 20;

    int iter_depth;

    int search_start_time = 0;

    bool time_over = false;

    move pv_table[max_search_depth+1][max_search_depth+1];

    move killer_moves[2][max_search_depth];

    int (*const* eval_func)(game_state);

    void update_pv_table(move new_best_move, int ply, bool is_leaf_node){

        pv_table[ply][ply] = new_best_move;
        if(is_leaf_node){//If node analyzed is a terminal node
            pv_table[ply][ply + 1].move = -1; //If move bitarray is set to -1, this is a terminal node used ony for killer moves.     
        }
        else{
            for(int j = ply + 1; j < iter_depth; j++){
                pv_table[ply][j] = pv_table[ply+1][j];
            }
        }
    }

    void reorder_moves(move possible_moves[MAX_POSSIBLE_MOVEMENTS], int num_moves, int ply){

        for(int i = 0; i < num_moves; i++){
            move pv_move = pv_table[0][ply];

            if(possible_moves[i].move == pv_move.move && possible_moves[i].piece == pv_move.piece){
                move temp = possible_moves[0];
                possible_moves[0] = possible_moves[i];
                possible_moves[i] = temp;
            }

        }

        for(int i = 0; i < num_moves; i++){
            move killer_move = killer_moves[0][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[1];
                possible_moves[1] = possible_moves[i];
                possible_moves[i] = temp;
            }

        }
        
        for(int i = 0; i < num_moves; i++){
            move killer_move = killer_moves[1][ply];

            if(possible_moves[i].move == killer_move.move && possible_moves[i].piece == killer_move.piece){
                move temp = possible_moves[2];
                possible_moves[2] = possible_moves[i];
                possible_moves[i] = temp;
            }

        }
        
    }

    void set_killer_move(move m, int ply) {


        /* make sure killer moves will be different
        before saving secondary killer move */
        if (m.move != killer_moves[0][ply].move || m.piece != killer_moves[0][ply].piece)
            killer_moves[1][ply] = killer_moves[0][ply];

        /* save primary killer move */
        killer_moves[0][ply] = m;

    }

    int pv_search(game_state prev_node_state, int alpha, int beta, int depth) {
        nodes_searched++;

        //Leaf node conditions

        if(depth == 0){
            return DistanceEval::eval(prev_node_state);
        }
        if(check_for_win(prev_node_state)){
            return -MAX_INT;
        }
        if((nodes_searched & 0x7fff) == 0){
            
            if(time(NULL) > search_start_time + max_seconds){
                time_over = true;
                
                return 0;
            }
        }

        int ply = iter_depth - depth;

        int win_score = -MAX_INT + ply; //Further into the tree, the lower the win_score is

        //If window is an infinite window, limit the values to win score
        if (alpha < win_score) alpha = win_score;
        if (beta > -win_score - 1) beta = -win_score - 1;
        if (alpha >= beta){
            return alpha;
        } 

        //Generate moves
        move possible_moves[MAX_POSSIBLE_MOVEMENTS];
        int num_moves = get_moves_as_list(prev_node_state, possible_moves);

        reorder_moves(possible_moves, num_moves, ply);

        bool search_pv = true;
        for(int i = 0; i < num_moves; i++){
            game_state node_state = apply_move(prev_node_state, possible_moves[i]);

            int score;
            if(search_pv)
                score = -pv_search(node_state, -beta, -alpha, depth - 1);
            else{
                score = -pv_search(node_state, -alpha - 1, - alpha, depth - 1);
                if(score > alpha)
                    score = -pv_search(node_state, -beta, -alpha, depth - 1);
            }

            if(time_over)
                return 0;

            //Update alpha (better child node has been found)
            if(score > alpha){
                alpha = score;
                search_pv = false;
                update_pv_table(possible_moves[i], ply, score == MAX_INT);
                
                if(score >= beta){
                    //If condition is met, this node is garanteed to have a score smaller than
                    //another previously searched sibling node

                    set_killer_move(possible_moves[i], ply);

                    return beta;
                }
            }
        }
        return alpha;
    
    }

    int root_search(game_state prev_node_state, int depth) {

        //Leaf node conditions

        int ply = 0;
        int alpha = -MAX_INT;
        int beta = MAX_INT;

        int win_score = MAX_INT - ply; //Further into the tree, the lower the win_score is


        //Generate moves
        move possible_moves[MAX_POSSIBLE_MOVEMENTS];
        int num_moves = get_moves_as_list(prev_node_state, possible_moves);

        reorder_moves(possible_moves, num_moves, ply);
        bool search_pv = true;
        for(int i = 0; i < num_moves; i++){
            game_state node_state = apply_move(prev_node_state, possible_moves[i]);

            int score;

            if(search_pv)
                score = -pv_search(node_state, -beta, -alpha, depth - 1);
            else{
                score = -pv_search(node_state, -alpha - 1, - alpha, depth - 1);
                if(score > alpha)
                    score = -pv_search(node_state, -beta, -alpha, depth - 1);
            }            
            if(time_over)
                break;

            //Update alpha (better child node has been found)
            if(score > alpha){
                alpha = score;
                search_pv = false;
                update_pv_table(possible_moves[i], ply, score == MAX_INT);

                if(score >= beta){
                    //If condition is met, this node is garanteed to have a score smaller than
                    //another previously searched sibling node

                    return beta;
                }
            }
        }
        return alpha;
    
    }

    move search(game_state state, eval_func_t eval_func_){

        eval_func = eval_func_.target<int(*)(game_state)>();

        nodes_searched = 0;
        move possible_moves[MAX_POSSIBLE_MOVEMENTS];

        int num_moves = get_moves_as_list(state, possible_moves);

        int scores[MAX_POSSIBLE_MOVEMENTS];

        int score;
        
        search_start_time = time(NULL);

        time_over = false;

        for(int i = 0; i < max_search_depth; i++){
            killer_moves[0][i].move = - 1;
            killer_moves[1][i].move = - 1;
            for(int j = 0; j < max_search_depth; j++){
                pv_table[i][j].move = -1;
            }
        }

        for(int depth = 1; depth <= max_search_depth; depth++){
            std::cout << "depth " << depth << " ";
            iter_depth = depth;
            score =  root_search(state, depth);
            std::cout << "pv: ";
            Team pv_print_turn = state.turn;

            for(int i = 0; i < max_search_depth; i++){
                if(pv_table[0][i].move == -1){
                    break;
                }
                else{
                    if(pv_print_turn == Team::red){
                        std::cout << hue::light_red;
                        pv_print_turn = Team::blue;
                    }
                    else{
                        std::cout << hue::light_blue;
                        pv_print_turn = Team::red;
                    }
                    
                    std::cout << get_move_str(pv_table[0][i]) << " ";

                    std::cout << hue::reset;
                }
            }


            std::cout << "(" << nodes_searched << " nodes, ";

            if(abs(score) > MAX_INT - 100 && !time_over){
                if(pv_print_turn == Team::blue)
                    std::cout << hue::light_red << "red can force win" << hue::reset << ", ";
                else 
                    std::cout << hue::light_blue << "blue can force win" << hue::reset << ", ";
            }
            

            if(time_over){
                std::cout << "timeout)" << std::endl;
                break;
            }

            std::cout << "\b\b) " << std::endl;
            if(abs(score) > MAX_INT - 100){
                break;
            }
        }
        
        std::cout << std::endl;

        move best_move = pv_table[0][0];

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
