#include <algorithm>
#include <string.h>
#include <sstream>
#include <string>
#include <sys/types.h>
#include <vector>

#include "positioning.h"
#include "parsing.h"


namespace parsing{


    //Map used for parsing move string to move
    std::map<std::string, positioning::move_bitarray_t> movement_str_dict = {{"N",MOVEMENT_N}, {"NN",MOVEMENT_NN}, {"NE",MOVEMENT_NE}, {"EN",MOVEMENT_EN},
                                                        {"NNE",MOVEMENT_NNE}, {"NNEE",MOVEMENT_NNEE}, {"NEE",MOVEMENT_NEE}, {"E",MOVEMENT_E},
                                                        {"EE",MOVEMENT_EE}, {"ES",MOVEMENT_ES}, {"SE",MOVEMENT_SE}, {"SEE",MOVEMENT_SEE},
                                                        {"SSEE",MOVEMENT_SSEE}, {"SSE",MOVEMENT_SSE}, {"S",MOVEMENT_S}, {"SS",MOVEMENT_SS},
                                                        {"SW",MOVEMENT_SW}, {"WS",MOVEMENT_WS}, {"SSW",MOVEMENT_SSW}, {"SSWW",MOVEMENT_SSWW},
                                                        {"SWW",MOVEMENT_SWW}, {"W",MOVEMENT_W}, {"WW",MOVEMENT_WW}, {"WN",MOVEMENT_WN},
                                                        {"NW",MOVEMENT_NW}, {"NWW",MOVEMENT_NWW}, {"NNWW",MOVEMENT_NNWW}, {"NNW",MOVEMENT_NNW}};




    //Parse a single move string to a move                                 
    positioning::move parse_movement_str(std::string str){
        
        using namespace std;

        positioning::move move;
        move.move = 0;

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
            return move; //Return null move
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
            return move; //Return move with no direction or piece
        }

        str.erase(0,1);

        if(movement_str_dict.count(str)){
            move.move = movement_str_dict[str];
        }

        return move;
        
    }

    //Convert move to move string
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


    bool parse_fen(std::string fen_str, positioning::game_state & game_state_out){

        using namespace std;
        using namespace positioning;

        stringstream fen_ss(fen_str);

        vector<string> fen_split;

        string tmp;

        //Split string by commas

        
        while(getline(fen_ss, tmp, ',')){
            fen_split.push_back(tmp);
        }

        //Check if string was seperated in two parts
        if(fen_split.size() != 2){
            return false;
        }

        //Split first part of fen (board piece positions seperated by /)

        stringstream board_rows_ss(fen_split[0]);
        vector<string> board_rows_split;

        while(getline(board_rows_ss, tmp, '/')){
            board_rows_split.push_back(tmp);
        }


        //Check if string was seperated in 10 parts (10 rows)
        if(board_rows_split.size() != 10){
            return false;
        }    

        

        game_state new_state;

        for (const auto player : { Player::red, Player::blue }){
            for (const auto piece : {0,1,2,3,4}){
                new_state.pieces[piece][player].bitboard = (bitboard_t)0;
                new_state.pieces[piece][player].o = flat;
            }
        }

        //Load piece bits in state bitboards
                        
        for(int row = 0; row < 10; row++){
            
            string row_fen_str = board_rows_split[9-row];
            
            //Empty row
            if(row_fen_str == "10"){
                continue;
            }

            int col = 0;
            int string_pos = 0;

            while(string_pos < (int)row_fen_str.length()){
                char row_string_char = row_fen_str[string_pos];

                if(row_string_char <= '9' && row_string_char >= '0'){
                    col += row_string_char - '0';
                }

                else{
                    switch (row_string_char) {
                        case 'C':
                            new_state.pieces[cabeza_idx][red].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;
                        
                        case 'M':
                            new_state.pieces[mini_idx][red].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;

                        case 'F':
                            new_state.pieces[flaco_idx][red].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;
                        
                        case 'H':
                            new_state.pieces[chato_idx][red].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;

                        case 'O':
                            new_state.pieces[gordo_idx][red].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;

                        case 'c':
                            new_state.pieces[cabeza_idx][blue].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;
                        
                        case 'm':
                            new_state.pieces[mini_idx][blue].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;

                        case 'f':
                            new_state.pieces[flaco_idx][blue].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;
                        
                        case 'h':
                            new_state.pieces[chato_idx][blue].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;

                        case 'o':
                            new_state.pieces[gordo_idx][blue].bitboard |= (bitboard_t)0x1 << (row*10 + col);
                            break;

                        default:
                            return false;
                    }
                    col++;
                }

                string_pos++;
            }
            
            if(col != 10){
                return false;
            }

        }            

        //Check piece validity and piece orientation

        //Check if pieces match a possible piece bitboards
   
        bool red_cabeza_ok = false;
        bool red_mini_ok = false;
        bool blue_cabeza_ok = false;
        bool blue_mini_ok = false;
        bool red_flaco_ok = false;
        bool blue_flaco_ok = false;
        bool red_chato_ok = false;
        bool blue_chato_ok = false;
        bool red_gordo_ok = false;
        bool blue_gordo_ok = false;


        for(int pos_test = 0; pos_test < 100; pos_test++){
            int row_test = pos_test%10;
            int col_test = pos_test/10;


            bitboard_t bitboard_test_1x1 = (bitboard_t)0x1 << (row_test*10 + col_test);

            if(new_state.pieces[cabeza_idx][red].bitboard == bitboard_test_1x1){
                red_cabeza_ok = true;
            }
            if(new_state.pieces[mini_idx][red].bitboard == bitboard_test_1x1){
                red_mini_ok = true;
            }
            if(new_state.pieces[cabeza_idx][blue].bitboard == bitboard_test_1x1){
                blue_cabeza_ok = true;
            }
            if(new_state.pieces[mini_idx][blue].bitboard == bitboard_test_1x1){
                blue_mini_ok = true;
            }
            if(new_state.pieces[flaco_idx][red].bitboard == bitboard_test_1x1){
                red_flaco_ok = true;
                new_state.pieces[flaco_idx][red].o = flat;
            }
            if(new_state.pieces[flaco_idx][blue].bitboard == bitboard_test_1x1){
                blue_flaco_ok = true;
                new_state.pieces[flaco_idx][blue].o = flat;
            }


            if(row_test < 9){
                bitboard_t bitboard_test_1x2 = (bitboard_t)0x3 << (row_test*10 + col_test);
                if(new_state.pieces[flaco_idx][red].bitboard == bitboard_test_1x2){
                    red_flaco_ok = true;
                    new_state.pieces[flaco_idx][red].o = horizontal;
                }
                if(new_state.pieces[flaco_idx][blue].bitboard == bitboard_test_1x2){
                    blue_flaco_ok = true;
                    new_state.pieces[flaco_idx][blue].o = horizontal;
                }
                if(new_state.pieces[chato_idx][red].bitboard == bitboard_test_1x2){
                    red_chato_ok = true;
                    new_state.pieces[chato_idx][red].o = horizontal;
                }
                if(new_state.pieces[chato_idx][blue].bitboard == bitboard_test_1x2){
                    blue_chato_ok = true;
                    new_state.pieces[chato_idx][blue].o = horizontal;
                }
            }

            if(col_test < 9){
                bitboard_t bitboard_test_2x1 = (bitboard_t)0x401 << (row_test*10 + col_test);
                if(new_state.pieces[flaco_idx][red].bitboard == bitboard_test_2x1){
                    red_flaco_ok = true;
                    new_state.pieces[flaco_idx][red].o = vertical;
                }
                if(new_state.pieces[flaco_idx][blue].bitboard == bitboard_test_2x1){
                    blue_flaco_ok = true;
                    new_state.pieces[flaco_idx][blue].o = vertical;
                }
                if(new_state.pieces[chato_idx][red].bitboard == bitboard_test_2x1){
                    red_chato_ok = true;
                    new_state.pieces[chato_idx][red].o = vertical;
                }
                if(new_state.pieces[chato_idx][blue].bitboard == bitboard_test_2x1){
                    blue_chato_ok = true;
                    new_state.pieces[chato_idx][blue].o = vertical;
                }
            }

            if(row_test < 9 && col_test < 9){
                bitboard_t bitboard_test_2x2 = (bitboard_t)0xC03 << (row_test*10 + col_test);
                if(new_state.pieces[chato_idx][red].bitboard == bitboard_test_2x2){
                    red_chato_ok = true;
                    new_state.pieces[chato_idx][red].o = flat;
                }
                if(new_state.pieces[chato_idx][blue].bitboard == bitboard_test_2x2){
                    blue_chato_ok = true;
                    new_state.pieces[chato_idx][blue].o = flat;
                }
                if(new_state.pieces[gordo_idx][red].bitboard == bitboard_test_2x2){
                    red_gordo_ok = true;
                }
                if(new_state.pieces[gordo_idx][blue].bitboard == bitboard_test_2x2){
                    blue_gordo_ok = true;
                }
            }
        }

        
        if(!(red_cabeza_ok && red_mini_ok && red_flaco_ok && red_chato_ok && red_gordo_ok && 
             blue_cabeza_ok && blue_mini_ok && blue_flaco_ok && blue_chato_ok && blue_gordo_ok)){

            return false;
        }

        game_state_out = new_state;

        return true;
    }


    std::string generate_fen(positioning::game_state game_state){


        using namespace positioning;

        std::string fen;

        for(int row = 9; row >= 0; row--){
            int blank_spaces = 0;
            for(int col = 0; col < 10; col++){

                bool piece_appended = true;

                if(game_state.pieces[cabeza_idx][red].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("C");
                }
                else if(game_state.pieces[cabeza_idx][blue].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("c");
                }
                else if(game_state.pieces[mini_idx][red].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("M");
                }
                else if(game_state.pieces[mini_idx][blue].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("m");
                }
                else if(game_state.pieces[flaco_idx][red].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("F");
                }
                else if(game_state.pieces[flaco_idx][blue].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("f");
                }     
                else if(game_state.pieces[chato_idx][red].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("H");
                }
                else if(game_state.pieces[chato_idx][blue].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("h");
                }
                else if(game_state.pieces[gordo_idx][red].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("O");
                }
                else if(game_state.pieces[gordo_idx][blue].bitboard & ((bitboard_t)0x1 << (row*10 + col))){
                    fen.append("o");
                }
                else{
                    piece_appended = false;
                    blank_spaces++;
                }

                if(piece_appended && blank_spaces > 0){
                    fen.insert(fen.length() - 1, std::to_string(blank_spaces), 0);
                    blank_spaces = 0;
                }
            }
            if(blank_spaces > 0){
                fen.append(std::to_string(blank_spaces));
            }
            fen.append("/");
        }

        fen.append(",");
        
        if(game_state.turn == red){
            fen.append("r");
        }
        else{
            fen.append("b");
        }
        
        return fen;
    }

    

}

