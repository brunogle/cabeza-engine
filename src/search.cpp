#include "search.h"
#include <functional>
#include <limits>
#include <iostream>
#include <iomanip>
#include <assert.h>     /* assert */

#include "color.hpp"
#include "transposition.h"
#include "util.h"

using namespace positioning;


Search::Search(eval_func_t eval_func_){
    this->eval_func = eval_func_;
}

Search::Search(){
    this->eval_func = nullptr;
}

move Search::search(game_state state){
    (void)state;
    move ret = {};
    return ret;
}

AllSearch::AllSearch(eval_func_t eval_func, int max_seconds, int max_search_depth) : Search(eval_func), transposition_table(32*1024*1024){
    this->max_search_depth = max_search_depth;
    this->max_seconds = max_seconds;

    //Initialize caches
    
    this->pv_table = new move* [max_search_depth + 1];
    for(int i = 0; i < max_search_depth + 1; i++)
        this->pv_table[i] = new move[max_search_depth + 1];
        
    this->killer_moves = new move[max_search_depth][2];

    for(int i = 0; i < max_search_depth; i++){
        this->killer_moves[i][0].move = - 1;
        this->killer_moves[i][1].move = - 1;
    }
    
    for(int i = 0; i < max_search_depth + 1; i++){
        for(int j = 0; j < max_search_depth + 1; j++){
            pv_table[i][j].move = -1;
        }
    }


}


AllSearch::~AllSearch(){
    
    for(int i = 0; i < max_search_depth; i++)
        delete[] this->pv_table[i];
    delete[] this->pv_table;
    
    delete[] this->killer_moves;

}


void AllSearch::update_pv_table(move new_best_move, int ply, bool is_leaf_node){

    this->pv_table[ply][ply] = new_best_move;
    if(ply + 1 == this->iter_search_depth){
        return;
    }
    if(is_leaf_node){//If node analyzed is a terminal node
        //assert(ply + 1 < iter_search_depth);
        this->pv_table[ply][ply + 1].move = ~0u; //If move bitarray is set to -1, this is a terminal node used ony for killer moves.     
    }
    else{
        //assert(ply + 1 < iter_search_depth);
        for(int j = ply + 1; j < this->iter_search_depth; j++){
            this->pv_table[ply][j] = this->pv_table[ply+1][j];
        }
    }
}

void AllSearch::score_moves(int move_scores[MAX_POSSIBLE_MOVEMENTS], move possible_moves[MAX_POSSIBLE_MOVEMENTS], int num_moves, int ply, move best_move_tt){

    memset(move_scores, 0, MAX_POSSIBLE_MOVEMENTS * sizeof(int));

    for(int i = 0; i < num_moves; i++){
        move pv_move = this->pv_table[ply][ply];
        move killer_move1 = this->killer_moves[ply][0];
        move killer_move2 = this->killer_moves[ply][1];


        if(possible_moves[i].move == pv_move.move && possible_moves[i].piece == pv_move.piece){
            move_scores[i] += 100;
        }
        if(possible_moves[i].move == killer_move1.move && possible_moves[i].piece == killer_move1.piece){
            move_scores[i] += 80;
        }
        if(possible_moves[i].move == killer_move2.move && possible_moves[i].piece == killer_move2.piece){
            move_scores[i] += 80 - 1;
        }
        if(possible_moves[i].move == best_move_tt.move && possible_moves[i].piece == best_move_tt.piece){
            move_scores[i] += 90;
        }
    }
}

void AllSearch::set_killer_move(move m, int ply) {


    /* make sure killer moves will be different
    before saving secondary killer move */
    if (m.move != this->killer_moves[ply][0].move || m.piece != this->killer_moves[ply][0].piece)
        this->killer_moves[ply][1] = this->killer_moves[ply][0];

    /* save primary killer move */
    this->killer_moves[ply][0] = m;

}



int AllSearch::pv_search(game_state node, int alpha, int beta, int depth, bool is_pv_node) {

    this->nodes_searched++;
    uint64_t node_hash = this->transposition_table.get_hash(node);

    move best_move_tt;
    int val_tt;

    node_type tt_node = alpha_node;

    //_mm_prefetch((char *)&this->transposition_table.transposition_table[node_hash & this->transposition_table.hash_key_mask], _MM_HINT_NTA);

    //Leaf node conditions

    if(depth == 0){
        return DistanceEval::eval(node);
    }
    if(check_for_win(node)){
        return -MAX_INT;
    }

    //get_ms() is slow, so check timeout condition every ~33k nodes analyzed.
    if((this->nodes_searched & 0x7fff) == 0){
        
        if(get_ms() > this->search_start_time + this->max_seconds){
            this->timeout = true;
            
            return 0;
        }
    }



    int ply = this->iter_search_depth - depth; //Calculate ply (distance from root node)




    int win_score = -MAX_INT + ply; //Further into the tree, the lower the win_score should be

    //If window is unbounded, limit the values to win_score to ensure engine tries shortest possible win. 
    if (alpha < win_score) alpha = win_score;
    if (beta > -win_score - 1) beta = -win_score - 1;

    //If alpha>=beta, node scoring is impossible for it to be greater than beta
    if (alpha >= beta){
        return alpha;
    } 

    if(this->transposition_table.probe(node_hash, depth, alpha, beta, &best_move_tt, &val_tt)){
        //if(!is_pv_node){
            return val_tt;
        //}
    }

    //Generate moves
    move possible_moves[MAX_POSSIBLE_MOVEMENTS];
    int num_moves = get_moves_as_list(node, possible_moves);
    
    //Score moves
    int move_scores[MAX_POSSIBLE_MOVEMENTS];
    this->score_moves(move_scores, possible_moves, num_moves, ply, best_move_tt);

    bool evaluated_moves[MAX_POSSIBLE_MOVEMENTS];
    memset(evaluated_moves, 0, MAX_POSSIBLE_MOVEMENTS * sizeof(bool));

    bool search_pv = true; //Used for PVS
    for(int i = 0; i < num_moves; i++){

        //Get a node to analyze

        int best_move_score = 0;
        move testing_move = possible_moves[0];
        int move_idx = 0;
        for(int j = 0; j < num_moves; j++){
            if(move_scores[j] >= best_move_score && !evaluated_moves[j]){
                testing_move = possible_moves[j];
                best_move_score = move_scores[j];
                move_idx = j;
            }
        }
        evaluated_moves[move_idx] = true;

        game_state next_node = apply_move(node, testing_move); //Apply movement


        //Principal-variation search.

        int score;

        if(search_pv)
            score = -this->pv_search(next_node, -beta, -alpha, depth - 1, is_pv_node);
        else{
            score = -this->pv_search(next_node, -alpha - 1, - alpha, depth - 1, false);
            if(score > alpha)
                score = -this->pv_search(next_node, -beta, -alpha, depth - 1, true);
        }

        //If timeout, return 0
        if(this->timeout)
            return 0;


        //If better child node has been found
        if(score > alpha){
            

            search_pv = false;

            //Because a better move as been found at this ply, update pv table.
            this->update_pv_table(testing_move, ply, score == MAX_INT);

            //If condition is met, this node is garanteed to have a score smaller than
            //another previously searched sibling node  
            if(score >= beta){

                this->set_killer_move(testing_move, ply); //A cut-node has been found. Add to killer move array.

                tt_node = beta_node;
                alpha = beta;
                break;
            }

            alpha = score; //Update alpha
            tt_node = exact_node;

        }
        
    }

    this->transposition_table.save(node_hash, alpha, tt_node, depth, this->pv_table[ply][ply]);

    return alpha; //If all nodes have been search, this is an all node.

}

int AllSearch::root_search(game_state node, int depth) {

    int ply = 0; //Root node has ply=0

    int alpha = -MAX_INT; //Initial alpha beta parameters
    int beta = MAX_INT;

    uint64_t node_hash = this->transposition_table.get_hash(node);

    move best_move_tt;
    int val_tt;

    if(this->transposition_table.probe(node_hash, depth, alpha, beta, &best_move_tt, &val_tt)){
        if((val_tt > alpha && val_tt < beta)){
            this->tt_cutoffs++;
            return val_tt;
        }
    }



    //Generate moves
    move possible_moves[MAX_POSSIBLE_MOVEMENTS];
    int num_moves = get_moves_as_list(node, possible_moves);

    //Score moves
    int move_scores[MAX_POSSIBLE_MOVEMENTS];
    this->score_moves(move_scores, possible_moves, num_moves, ply, best_move_tt);

    bool evaluated_moves[MAX_POSSIBLE_MOVEMENTS];
    memset(evaluated_moves, 0, MAX_POSSIBLE_MOVEMENTS * sizeof(bool));
    


    bool search_pv = true; //Used for PVS

        
    for(int i = 0; i < num_moves; i++){
        //Get a node to analyze

        int best_move_score = 0;
        move testing_move = possible_moves[0];
        int move_idx = 0;
        for(int j = 0; j < num_moves; j++){
            if(move_scores[j] >= best_move_score && !evaluated_moves[j]){
                testing_move = possible_moves[j];
                best_move_score = move_scores[j];
                move_idx = j;
            }
        }
        evaluated_moves[move_idx] = true;

        game_state next_node = apply_move(node, testing_move); //Apply movement

        //Principal-variation search.
        
        int score;

        if(search_pv)
            score = -this->pv_search(next_node, -beta, -alpha, depth - 1, true);
        else{
            score = -this->pv_search(next_node, -alpha - 1, - alpha, depth - 1, false);
            if(score > alpha)
                score = -this->pv_search(next_node, -beta, -alpha, depth - 1, true);
        }

        //If timeout, return 0
        if(this->timeout)
            break;

        //If better child node has been found
        if(score > alpha){
            alpha = score;//Update alpha

            search_pv = false;

            //Because a better move as been found at this ply, update pv table.
            this->update_pv_table(testing_move, ply, score == MAX_INT);

            //If condition is met, this node is garanteed to have a score smaller than
            //another previously searched sibling node
            if(score >= beta){
                //If condition is met, this node is garanteed to have a score smaller than
                //another previously searched sibling node

                this->transposition_table.save(node_hash, beta, beta_node, depth, this->pv_table[ply][ply]);

                return beta;
            }

            this->transposition_table.save(node_hash, score, alpha_node, depth, this->pv_table[ply][ply]);
        }
    }

    this->transposition_table.save(node_hash, alpha, exact_node, depth, this->pv_table[ply][ply]);

    return alpha;

}

move AllSearch::search(game_state state){

    std::cout << "Searching hash " << std::setfill('0') << std::setw(16) << std::right << std::hex << this->transposition_table.get_hash(state) << std::endl;
    std::cout << std::dec;

    //Reset pv table and killer moves between searches.
    this->search_start_time = get_ms();
    this->nodes_searched = 0;
    this->timeout = false;
    this->tt_cutoffs = 0;
    this->transposition_table.clear();
    
    for(int i = 0; i < max_search_depth; i++){
        this->killer_moves[i][0].move = - 1;
        this->killer_moves[i][1].move = - 1;
    }
    
    for(int i = 0; i < max_search_depth + 1; i++){
        for(int j = 0; j < max_search_depth + 1; j++){
            pv_table[i][j].move = -1;
        }
    }


    //Iterative deepening loop
    for(int depth = 1; depth <= max_search_depth; depth++){
        std::cout << "depth " << depth << " ";

        this->iter_search_depth = depth;

        int score = root_search(state, depth); //Search root


        //Print principal variation and additional data.

        std::cout << "pv: ";

        Team pv_print_turn = state.turn;

        for(int i = 0; i < max_search_depth; i++){
            if(pv_table[0][i].move == -1u){
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

        if(abs(score) > MAX_INT - 100 && !this->timeout){
            if(pv_print_turn == Team::blue)
                std::cout << hue::light_red << "red can force win" << hue::reset << ", ";
            else 
                std::cout << hue::light_blue << "blue can force win" << hue::reset << ", ";
        }
        

        if(this->timeout){
            std::cout << "timeout, ";
        }

        std::cout << "\b\b) " << std::endl;


        if(this->timeout){
            break; //Stop search if timeout
        }

        //If a forced win has been found, stop search
        if(abs(score) > MAX_INT - 100){
            break;
        }
    }
    

    std::cout << "time: " << get_ms() - this->search_start_time << std::endl;

    move best_move = pv_table[0][0];

    return best_move;

}

