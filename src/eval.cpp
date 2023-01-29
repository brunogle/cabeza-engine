#include "eval.h"
#include <iostream>
#include <limits>

//(a-b)/10 + (a-b)%10
namespace eval{

    /*
    Checks if a player won in last semimove.
    Only returns true if the player that has the win condition hast played last semimove (i.e. the player whoes turn is not)
    */
    bool check_for_win(positioning::game_state state){

        using namespace positioning;

        bool win = false;

        bitboard_t idle_player_non_cabeza_occupancy;

        switch (state.turn)
        {
        case Team::red:
            if(state.pieces[PieceIndex::blue_cabeza_idx].bitboard & ((bitboard_t) 0x3ff)){
                win = true;
                break;
            }
            idle_player_non_cabeza_occupancy = state.pieces[6].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[7].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[8].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[9].bitboard;   

            win = state.pieces[PieceIndex::red_cabeza_idx].bitboard & idle_player_non_cabeza_occupancy;

            break;

        case positioning::Team::blue:
            if(state.pieces[PieceIndex::red_cabeza_idx].bitboard & ((bitboard_t) 0x3ff << 90)){
                win = true;
                break;
            }
            idle_player_non_cabeza_occupancy = state.pieces[1].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[2].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[3].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[4].bitboard;   

            win = state.pieces[PieceIndex::blue_cabeza_idx].bitboard & idle_player_non_cabeza_occupancy;
            break;

        default:
            win = false;

        }
        
        return win;


    }

    float eval_cabeza_distance_to_pieces(positioning::game_state state, positioning::Team cabeza_team, positioning::Team pieces_team){

        using namespace positioning;

        int piece_pos[10];
        
        float distance_sum = 0;

        for(int i = 0; i < 10; i++)
            piece_pos[i] = get_pos_from_bitboard(state.pieces[i].bitboard);

        int x_diff, y_diff;

        int cabeza_x = piece_pos[(int)cabeza_team]%10;
        int cabeza_y = piece_pos[(int)cabeza_team]/10;

        //Distance to mini
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 1]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 1]/10);

        distance_sum += abs(x_diff) + abs(y_diff);

        //Distance to flaco
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 2]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 2]/10);

        distance_sum += abs(x_diff) + abs(y_diff);

        switch (state.pieces[(int)pieces_team + 2].o){
        case Orientation::horizontal:
            distance_sum += (x_diff > 0) ? -0.5 : 0.5;
            break;
        case Orientation::vertical:
            distance_sum += (y_diff > 0) ? -0.5 : 0.5;
            break;
        default:
            break;
        
        }

        //Distance to chato
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 3]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 3]/10);

        distance_sum += abs(x_diff) + abs(y_diff);

        switch (state.pieces[(int)pieces_team + 3].o){
        case Orientation::horizontal:
            distance_sum += (x_diff > 0) ? -0.5 : 0.5;
            break;
        case Orientation::vertical:
            distance_sum += (y_diff > 0) ? -0.5 : 0.5;
            break;
        default:
            distance_sum += ((x_diff > 0) ? -0.5 : 0.5) + ((y_diff > 0) ? -0.5 : 0.5);
            break;
        }

        //Distance to gordo
        x_diff = (cabeza_x - piece_pos[(int)pieces_team + 4]%10);
        y_diff = (cabeza_y - piece_pos[(int)pieces_team + 4]/10);

        distance_sum += abs(x_diff) + abs(y_diff) + ((x_diff > 0) ? -0.5 : 0.5) + ((y_diff > 0) ? -0.5 : 0.5);

        return distance_sum;
        
    }

    /*
    Evaulation heuristic.
    Evaluate distance from cabeza to opossite edge
    */
    float eval_cabeza_distance_to_win(positioning::game_state state, positioning::Team team){
        using namespace positioning;

        switch (team){
        case Team::red:
            return 9 - get_pos_from_bitboard(state.pieces[red_cabeza_idx].bitboard)/10;
            break;
        
        case Team::blue:
            return get_pos_from_bitboard(state.pieces[blue_cabeza_idx].bitboard)/10;
            break;

        default:
            break;
        }

        return 0;
    }

    /*
    Main evaulation heuristic.
    */
    float eval_board(positioning::game_state state){
        using namespace positioning;

        float eval = 0;

        if(check_for_win(state)){
            switch (state.turn){
            case Team::red:
                return -std::numeric_limits<float>::max();
                break;
            case Team::blue:
                return std::numeric_limits<float>::max();
                break;
            default:
                return 0;
                break;
            }
        }
        //Distance from cabezas to end
        eval += 2.0*(eval_cabeza_distance_to_win(state, Team::blue) - eval_cabeza_distance_to_win(state, Team::red));

        //Distance from cabezas to enemies

        eval += 0.5*(eval_cabeza_distance_to_pieces(state, Team::red, Team::blue) - eval_cabeza_distance_to_pieces(state, Team::blue, Team::red));

        //Distance from cabezas to friendly pieces

        eval += 0.5*(eval_cabeza_distance_to_pieces(state, Team::blue, Team::blue) - eval_cabeza_distance_to_pieces(state, Team::red, Team::red));


        return eval;
    }

}