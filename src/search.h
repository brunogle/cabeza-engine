#ifndef SEARCH_H
#define SEARCH_H

//#include "positioning.h"
#include "eval.h"
#include "transposition.h"


/////////////////////////////////////////
//                                     //
//    PARENT CLASS DEFINITION          //
//                                     //
/////////////////////////////////////////

class Search{
    
    private:

        eval_func_t eval_func;

    public:
    
        Search(eval_func_t eval_func_);
        Search();

        virtual positioning::move search(positioning::game_state state);
    
};


/////////////////////////////////////////
//                                     //
//       PV SEARCH with PV table,      //
// Killer moves, transposition table   //
//                                     //
/////////////////////////////////////////

class PVSearch : public Search{

    private:
        
        eval_func_t eval_func; //Evaluation function pointer

        //Data tables used for searching
        positioning::move ** pv_table;
        positioning::move (* killer_moves)[2];
        TranspositionTable transposition_table;

        //Statistics variables
        int nodes_searched;    //Times seach() function was called
        int search_start_time; //Search start time, to peform timeouts
        int tt_cutoffs;

        //Configuration variables
        int max_search_depth; //Maximum search depth. Set before calling search()
        int max_seconds;  //Maximum time to search for. Set before calling serch()

        //Variables used for a search shared between iterations
        int iter_search_depth; //Maximum search depth for a specific iteration of the iterative deepening framework.
        bool timeout; //Timeout flag


        //Functions for appending data tables.
        void update_pv_table(positioning::move new_best_move, int ply, bool is_leaf_node);
        void set_killer_move(positioning::move m, int ply);

        //Used for move ordering
        void score_moves(int move_scores[MAX_POSSIBLE_MOVEMENTS], positioning::move possible_moves[MAX_POSSIBLE_MOVEMENTS], int num_moves, int ply, positioning::move best_move_tt);

        //First depth search function
        int root_search(positioning::game_state node, int depth);

        //Main search fucntion
        int pv_search(positioning::game_state node, int alpha, int beta, int depth, bool is_pv_node);

    public:

        PVSearch(eval_func_t eval_func, int max_seconds, int max_depth);
        ~PVSearch();

        //Perform search, returns best move
        positioning::move search(positioning::game_state state);

        //Used for configuring seach
        void set_timeout(int timeout);
        void set_depth(int depth);

        int get_timeout();
        int get_depth();


};



#endif