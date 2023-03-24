#include "eval.h"
#include "positioning.h"
#include <iostream>
#include <limits>


namespace DistanceEval{

    int piece_pos[5][2];
    positioning::game_state global_game_state;

    int eval_cabeza_distance_to_pieces(positioning::Player cabeza_team, positioning::Player pieces_team){

        using namespace positioning;

        // int piece_pos[5][2];
        
        int distance_sum = 0;



        int x_diff, y_diff;

        int cabeza_x = piece_pos[0][cabeza_team]%10;
        int cabeza_y = piece_pos[0][cabeza_team]/10;

        //Distance to mini
        x_diff = (cabeza_x - piece_pos[1][pieces_team]%10);
        y_diff = (cabeza_y - piece_pos[1][pieces_team]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff));

        //Distance to flaco
        x_diff = (cabeza_x - piece_pos[2][pieces_team]%10);
        y_diff = (cabeza_y - piece_pos[2][pieces_team]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff));

        switch (global_game_state.pieces[3][pieces_team].o){
        case Orientation::horizontal:
            distance_sum += (x_diff > 0) ? -1 : 1;
            break;
        case Orientation::vertical:
            distance_sum += (y_diff > 0) ? -1 : 1;
            break;
        default:
            break;
        
        }

        //Distance to chato
        x_diff = (cabeza_x - piece_pos[3][pieces_team]%10);
        y_diff = (cabeza_y - piece_pos[3][pieces_team]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff));

        switch (global_game_state.pieces[3][pieces_team ].o){
        case Orientation::horizontal:
            distance_sum += (x_diff > 0) ? -1 : 1;
            break;
        case Orientation::vertical:
            distance_sum += (y_diff > 0) ? -1 : 1;
            break;
        default:
            distance_sum += ((x_diff > 0) ? -1 : 1) + ((y_diff > 0) ? -1 : 1);
            break;
        }

        //Distance to gordo
        x_diff = (cabeza_x - piece_pos[4][pieces_team]%10);
        y_diff = (cabeza_y - piece_pos[4][pieces_team]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff)) + ((x_diff > 0) ? -1 : 1) + ((y_diff > 0) ? -1 : 1);

        return distance_sum;
        
    }

    int eval_cabeza_distance_to_win(positioning::Player team){
        using namespace positioning;

        switch (team){
        case Player::red:
            return piece_pos[cabeza_idx][red]/10;
            break;
        
        case Player::blue:
            return 9 - piece_pos[cabeza_idx][blue]/10;
            break;

        default:
            break;
        }

        return 0;
    }

    /*
    Main evaulation heuristic.
    */
    int eval(positioning::game_state state){
        using namespace positioning;

        int eval = 0;

        if(check_for_win(state)){
            return -MAX_INT;
        }


        for(int i = 0; i < 5; i++){
            piece_pos[i][red] = get_pos_from_bitboard(state.pieces[i][red].bitboard);
            piece_pos[i][blue] = get_pos_from_bitboard(state.pieces[i][blue].bitboard);
        }

        global_game_state = state;

        //Distance from cabezas to end
        eval += 16.0*(eval_cabeza_distance_to_win(Player::red) - eval_cabeza_distance_to_win(Player::blue));

        //Distance from cabezas to enemies

        eval += 1*(eval_cabeza_distance_to_pieces(Player::red, Player::blue) - eval_cabeza_distance_to_pieces(Player::blue, Player::red));

        //Distance from cabezas to friendly pieces

        eval += 1*(eval_cabeza_distance_to_pieces(Player::blue, Player::blue) - eval_cabeza_distance_to_pieces(Player::red, Player::red));

        if(state.turn == Player::blue)
            return -eval;
        else
            return eval;
    }

}