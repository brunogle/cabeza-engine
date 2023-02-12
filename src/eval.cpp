#include "eval.h"
#include <iostream>
#include <limits>


namespace DistanceEval{

    int eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Team cabeza_team, positioning::Team pieces_team){

        using namespace positioning;

        int piece_pos[10];
        
        int distance_sum = 0;

        for(int i = 0; i < 10; i++)
            piece_pos[i] = get_pos_from_bitboard(state.pieces[i].bitboard);

        int x_diff, y_diff;

        int cabeza_x = piece_pos[(int)cabeza_team]%10;
        int cabeza_y = piece_pos[(int)cabeza_team]/10;

        //Distance to mini
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 1]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 1]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff));

        //Distance to flaco
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 2]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 2]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff));

        switch (state.pieces[(int)pieces_team + 2].o){
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
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 3]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 3]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff));

        switch (state.pieces[(int)pieces_team + 3].o){
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
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 4]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 4]/10);

        distance_sum += 2*(abs(x_diff) + abs(y_diff)) + ((x_diff > 0) ? -1 : 1) + ((y_diff > 0) ? -1 : 1);

        return distance_sum;
        
    }

    int eval_cabeza_distance_to_win(positioning::game_state state, positioning::Team team){
        using namespace positioning;

        switch (team){
        case Team::red:
            return get_pos_from_bitboard(state.pieces[red_cabeza_idx].bitboard)/10;
            break;
        
        case Team::blue:
            return 9 - get_pos_from_bitboard(state.pieces[blue_cabeza_idx].bitboard)/10;
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
            switch (state.turn){
            case Team::red:
                return -MAX_INT;
                break;
            case Team::blue:
                return -MAX_INT;
                break;
            default:
                return 0;
                break;
            }
        }
        //Distance from cabezas to end
        eval += 16.0*(eval_cabeza_distance_to_win(state, Team::red) - eval_cabeza_distance_to_win(state, Team::blue));

        //Distance from cabezas to enemies

        eval += 1*(eval_cabeza_distance_to_pieces(state, Team::red, Team::blue) - eval_cabeza_distance_to_pieces(state, Team::blue, Team::red));

        //Distance from cabezas to friendly pieces

        eval += 1*(eval_cabeza_distance_to_pieces(state, Team::blue, Team::blue) - eval_cabeza_distance_to_pieces(state, Team::red, Team::red));

        if(state.turn == Team::blue)
            return -eval;
        else
            return eval;
    }

}