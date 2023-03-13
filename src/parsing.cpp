#include <algorithm>
#include <string.h>

#include "positioning.h"
#include "parsing.h"


namespace parsing{

    std::map<std::string, positioning::move_bitarray_t> movement_str_dict = {{"N",MOVEMENT_N}, {"NN",MOVEMENT_NN}, {"NE",MOVEMENT_NE}, {"EN",MOVEMENT_EN},
                                                        {"NNE",MOVEMENT_NNE}, {"NNEE",MOVEMENT_NNEE}, {"NEE",MOVEMENT_NEE}, {"E",MOVEMENT_E},
                                                        {"EE",MOVEMENT_EE}, {"ES",MOVEMENT_ES}, {"SE",MOVEMENT_SE}, {"SEE",MOVEMENT_SEE},
                                                        {"SSEE",MOVEMENT_SSEE}, {"SSE",MOVEMENT_SSE}, {"S",MOVEMENT_S}, {"SS",MOVEMENT_SS},
                                                        {"SW",MOVEMENT_SW}, {"WS",MOVEMENT_WS}, {"SSW",MOVEMENT_SSW}, {"SSWW",MOVEMENT_SSWW},
                                                        {"SWW",MOVEMENT_SWW}, {"W",MOVEMENT_W}, {"WW",MOVEMENT_WW}, {"WN",MOVEMENT_WN},
                                                        {"NW",MOVEMENT_NW}, {"NWW",MOVEMENT_NWW}, {"NNWW",MOVEMENT_NNWW}, {"NNW",MOVEMENT_NNW}};




                                                            
    positioning::move parse_movement_str(std::string str){
        
        using namespace std;

        positioning::move move;

        if(str.length() < 2 || str.length() > 5){
            positioning::move ret = {};
            return ret;
        }

        if(str.at(0) == 'C' || str.at(0) == 'M' || str.at(0) == 'F' || str.at(0) == 'H' || str.at(0) == 'O'){
            move.player = positioning::red;
        }   
        else if(str.at(0) == 'c' || str.at(0) == 'm' || str.at(0) == 'f' || str.at(0) == 'h' || str.at(0) == 'o'){
            move.player = positioning::blue;
        }
        else{
            return {};
        }

        transform(str.begin(), str.end(), str.begin(), ::toupper); //Convert to upper case
 
        
        char piece_char = str.at(0);

        switch (piece_char)
        {
        case 'C':
            move.piece = positioning::cabeza_idx;
            break;
        
        case 'M':
            move.piece = positioning::mini_idx;
            break;

        case 'F':
            move.piece = positioning::flaco_idx;
            break;

        case 'H':
            move.piece = positioning::chato_idx;
            break;

        case 'O':
            move.piece = positioning::gordo_idx;
            break;

        default:
            return move; //Return null move
        }

        str.erase(0,1);

        string ordered_move_str;

        //Count how many N/S/E/W characters

        int num_north = count(str.begin(), str.end(), 'N');
        int num_south = count(str.begin(), str.end(), 'S');
        int num_east = count(str.begin(), str.end(), 'E');
        int num_west = count(str.begin(), str.end(), 'W');

        //Total movement
        int total_north = num_north - num_south; 
        int total_east = num_east - num_west; 

        if(total_north > 0){
            ordered_move_str.append(total_north, 'N');
        }
        if(total_north < 0){
            ordered_move_str.append(-total_north, 'S');
        }
        if(total_east > 0){
            ordered_move_str.append(total_east, 'E');
        }
        if(total_east < 0){
            ordered_move_str.append(-total_east, 'W');
        }

        move.move = movement_str_dict[ordered_move_str];

        return move;
        
    }


    std::string get_move_str(positioning::move move_to_translate){

        std::string ret = "";



        switch(move_to_translate.piece){
            case positioning::cabeza_idx:
                ret += move_to_translate.player == positioning::red ? 'C' : 'c';
                break;
            
            case positioning::mini_idx:
                ret += move_to_translate.player == positioning::red ? 'M' : 'm';
                break;

            case positioning::flaco_idx:
                ret += move_to_translate.player == positioning::red ? 'F' : 'f';
                break;
            
            case positioning::chato_idx:
                ret += move_to_translate.player == positioning::red ? 'H' : 'h';
                break;    

            case positioning::gordo_idx:
                ret += move_to_translate.player == positioning::red ? 'G' : 'g';
                break;
        }

        for(int i = 0; i < 28; i++){
            if(move_to_translate.move & (1<<i)){
                ret += positioning::kMovementBitName[i];
            }
        }

        return ret;
    }


    /*
    Apply PGN string
    */
    positioning::game_state apply_pgn(positioning::game_state state, std::string pgn_str, bool * success){

        char * move_token = strtok(pgn_str.data(), ",");

        bool ok = true;

        positioning::game_state new_state = state;

        while(move_token != NULL){

            positioning::move move = parse_movement_str(move_token);

            new_state = apply_move_safe(new_state, move, &ok);

            if(!ok){
                *success = false;
                return state;
            }

            move_token = strtok(NULL, ",");
            
        }
        *success = true;
        return new_state;

    }

}