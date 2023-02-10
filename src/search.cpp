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


    int alpha_beta(game_state state, int alpha, int beta, int depth, eval_func_t eval_func){
        x++;
        if(depth == 0){
            return eval_func(state);
            
        }
        int score = eval_func(state);
        if(score == MAX_INT || score == MIN_INT){
            return score;
        }
        if(state.turn == Team::red){
            int val = MIN_INT;

            move possible_moves[MAX_POSSIBLE_MOVEMENTS];
            int num_moves = get_moves_as_list(state, possible_moves);

            for(int i = 0; i < num_moves; i++){
                game_state new_state = apply_move(state, possible_moves[i]);
                val = std::max(val, alpha_beta(new_state, alpha, beta, depth-1, eval_func));
                if(val > beta){
                    break;
                }
                alpha = std::max(val, alpha);
            }
            if(val > MAX_INT - 100)
                return val - 1;
            return val;
        }
        else{
            int val = MAX_INT;

            move possible_moves[MAX_POSSIBLE_MOVEMENTS];
            int num_moves = get_moves_as_list(state, possible_moves);

            for(int i = 0; i < num_moves; i++){
                game_state new_state = apply_move(state, possible_moves[i]);
                val = std::min(val, alpha_beta(new_state, alpha, beta, depth-1, eval_func));
                if(val < alpha){
                    break;
                }
                beta = std::min(val, beta);
            }
            if(val < MIN_INT + 100)
                return val + 1;
            return val;
        }

    }

    std::pair<move, int> root_search(game_state state, int depth, eval_func_t eval_func){

        int alpha = MIN_INT;
        int beta = MAX_INT;

        move possible_moves[MAX_POSSIBLE_MOVEMENTS];
        int num_moves = get_moves_as_list(state, possible_moves);


        move best_move = possible_moves[0];
        int best_move_val;


        if(state.turn == Team::red){
            int val = MIN_INT;
            best_move_val = MIN_INT;

            for(int i = 0; i < num_moves; i++){
                game_state new_state = apply_move(state, possible_moves[i]);
                int temp_val = alpha_beta(new_state, alpha, beta, depth-1, eval_func); 
                if(temp_val >= best_move_val){
                    val = temp_val;
                    best_move = possible_moves[i];
                }
                if(val > beta){
                    break;
                }
                alpha = std::max(val, alpha);
            }

            return std::pair<move, int>(best_move, val);
        }
        else{
            int val = MAX_INT;
            best_move_val = MAX_INT;

            for(int i = 0; i < num_moves; i++){
                game_state new_state = apply_move(state, possible_moves[i]);
                int temp_val = alpha_beta(new_state, alpha, beta, depth-1, eval_func);
                if(temp_val <= val){
                    val = temp_val;
                    best_move = possible_moves[i];
                }
                if(val < alpha){
                    break;
                }
                beta = std::min(val, beta);
            }
            return std::pair<move, int>(best_move, val);
        }
    }

    move search(game_state state, eval_func_t eval_func){
        x = 0;
        move possible_moves[MAX_POSSIBLE_MOVEMENTS];

        int num_moves = get_moves_as_list(state, possible_moves);

        int scores[MAX_POSSIBLE_MOVEMENTS];

        std::pair<move, int> result = root_search(state, 6, eval_func);

        move best_move = result.first;
        int node_val = result.second;

        if(node_val > MAX_INT - 100){
            std::cout << "Red wins in " << MAX_INT - node_val << std::endl;
        }
        else if(node_val < MIN_INT + 100){
            std::cout << "Blue wins in " << node_val - MIN_INT << std::endl;
        }

        std::cout << "Analyzed " << x << std::endl;

        return best_move;

    }

}
