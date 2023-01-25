#include <iostream>
#include <cmath>
#include <algorithm>

#include "positioning.h"


namespace positioning{
    /*
    Returns starting game state
    */
    game_state initial_game_state(){
 
        game_state state;

        using enum PieceIndex;
        using enum Orientation;

        state.pieces[red_cabeza_idx].bitboard = generate_bitboard_for_piece(red_cabeza_idx, flat, 5, 0);
        state.pieces[red_mini_idx].bitboard  =  generate_bitboard_for_piece(red_mini_idx, flat, 4, 0);
        state.pieces[red_flaco_idx].bitboard =  generate_bitboard_for_piece(red_flaco_idx, vertical, 3, 0);
        state.pieces[red_chato_idx].bitboard =  generate_bitboard_for_piece(red_chato_idx, vertical, 6, 0);
        state.pieces[red_gordo_idx].bitboard =  generate_bitboard_for_piece(red_gordo_idx, flat, 4, 1);

        state.pieces[blue_cabeza_idx].bitboard = generate_bitboard_for_piece(blue_cabeza_idx, flat, 4, 9);
        state.pieces[blue_mini_idx].bitboard  =  generate_bitboard_for_piece(blue_mini_idx, flat, 5, 9);
        state.pieces[blue_flaco_idx].bitboard =  generate_bitboard_for_piece(blue_flaco_idx, vertical, 6, 8);
        state.pieces[blue_chato_idx].bitboard =  generate_bitboard_for_piece(blue_chato_idx, vertical, 3, 8);
        state.pieces[blue_gordo_idx].bitboard =  generate_bitboard_for_piece(blue_gordo_idx, flat, 4, 7);

        state.pieces[red_cabeza_idx].o = flat;
        state.pieces[red_mini_idx].o = flat;
        state.pieces[red_flaco_idx].o = vertical;
        state.pieces[red_chato_idx].o = vertical;
        state.pieces[red_gordo_idx].o = flat;
        state.pieces[blue_cabeza_idx].o = flat;
        state.pieces[blue_mini_idx].o = flat;
        state.pieces[blue_flaco_idx].o = vertical;
        state.pieces[blue_chato_idx].o = vertical;
        state.pieces[blue_gordo_idx].o = flat;
        
        state.turn = Team::red;

        return state;
    }

    /*
    Returns a randomly arranged game. It's a slow function
    */
    game_state random_game_state(){
        game_state state;

        state.turn = Team(5*(rand()%2)); //Generate random team

        //Initialize to 0
        for(int i = 0; i < 10; i++){
            state.pieces[i] = {0,0,Orientation::flat,0};
        }

        int piece_num = 0; //Piece number couter
        
        while(piece_num < 10){

            bitboard_t test_bitboard; //To store a randomly generated piece.
            int test_x;
            int test_y;
            Orientation test_o;

            if(piece_num == red_cabeza_idx || piece_num == red_mini_idx || piece_num == blue_cabeza_idx || piece_num == blue_mini_idx){//If piece is cabeza or mini
                test_x = rand() % 10;
                test_y = rand() % 10;
            }

            else if(piece_num == red_flaco_idx || piece_num == blue_flaco_idx){//If piece is flaco

                test_o = Orientation(rand()%3);

                test_x = rand() % (test_o == Orientation::horizontal ? 9 : 10);
                test_y = rand() % (test_o == Orientation::vertical ? 9 : 10);

            }
            else if(piece_num == red_chato_idx || piece_num == blue_chato_idx){//If piece is chato 

                test_o = Orientation(rand()%3);

                test_x = rand() % (test_o == Orientation::vertical ? 10 : 9);
                test_y = rand() % (test_o == Orientation::horizontal ? 10 : 9);
            }

            else{ //If piece is gordo
                test_x = rand() % 9;
                test_y = rand() % 9;
            }
            
            test_bitboard = generate_bitboard_for_piece(piece_num, test_o, test_x, test_y);

            bitboard_t occupancy = get_occupancy(state, false);

            if(!(occupancy & test_bitboard)){
                state.pieces[piece_num].bitboard = test_bitboard;
                state.pieces[piece_num].x = test_x;
                state.pieces[piece_num].y = test_y;
                state.pieces[piece_num].o = test_o;
                
                piece_num ++;
            }
        }

        return state;
    }

    /*
    Generates occupancy bitboard for a specific piece
    */
    bitboard_t generate_bitboard_for_piece(int piece_num, Orientation o, int x, int y){

        bitboard_t bitboard;

        
        bitboard_t flaco_template[] = {0x1, 0x3, 0x401};   //Flat, horizontal, vertical
        bitboard_t chato_template[] = {0xC03, 0x3, 0x401}; //Flat, horizontal, vertical
        
        //If piece type is a cabeza or a mini
        if(piece_num == red_cabeza_idx || piece_num == red_mini_idx || piece_num == blue_cabeza_idx || piece_num == blue_mini_idx){
            bitboard = (bitboard_t)1 << (y*10 + x);
        }

        //If piece type is a flaco
        else if(piece_num == red_flaco_idx || piece_num == blue_flaco_idx){
            bitboard = flaco_template[o] << (y*10 + x);
        }

        //If piece type is a flaco
        else if(piece_num == red_chato_idx || piece_num == blue_chato_idx){
            bitboard = chato_template[o] << (y*10 + x);
        }

        //If piece type is a chato
        else{
            bitboard = (bitboard_t)0xC03 << (y*10 + x);
        }

        return bitboard;
    }

    /*
    Calculates occupancy bitboard for the board of the given game state
    If opponent_cabeza_free is true, the function will consider the cabeza of the non moving player as free,
    thus it permits a capture if true
    */
    bitboard_t get_occupancy(game_state state, bool opponent_cabeza_free){
        bitboard_t occupancy = (bitboard_t)0;

        for(int i = 0; i < 10; i++){
            occupancy |= state.pieces[i].bitboard;
        }

        if(opponent_cabeza_free){
            if(state.turn == Team::red){
                occupancy &= ~state.pieces[PieceIndex::blue_cabeza_idx].bitboard;
            }
            else{
                occupancy &= ~state.pieces[PieceIndex::red_cabeza_idx].bitboard;
            }
        }

        return occupancy;
    }

    /*
    Bitboards used for masking out the specific "zones" needed to analyze a piece.
    First index encodes the possible possitions, as obtained by get_pos_from_bitboard()
    Second index encodes the four possible symmetries used in each case.
    Third index encodes the 1,5,6,7 or 8 zones used in analyzing a piece.
    Fourth index represents orientation if applicable.
    */
    bitboard_t zone_mask_cabeza[100][4][8];
    bitboard_t zone_mask_mini[100][4][4];
    bitboard_t zone_mask_flaco[100][4][4][3];
    bitboard_t zone_mask_chato[100][4][4][3];
    bitboard_t zone_mask_gordo[100][4];

    /*
    Each bit of the index corresponds to the occupancy zone.
    This is used after zone mask arrays are used to obtain the occupancy at each zone.
    Each element only represents the possible movement_t for the first of the four symetries.
    */
    possible_movements_t movement_lookup_c[256]; 
    possible_movements_t movement_lookup_m[16];
    possible_movements_t movement_lookup_fh[16];

    /*
    Converts one of the zone_mask_drawings to a bitboard mask for a specific position, rotation and zone
    */
    bitboard_t generate_zone_mask_from_string(std::string mask_drawing, int pos, int rot, int zone){

        int str_length = sqrt(mask_drawing.length());

        int piece_position_in_string = mask_drawing.find('P');

        int piece_x_in_drawing = piece_position_in_string%str_length;
        int piece_y_in_drawing = piece_position_in_string/str_length;

            
        bitboard_t mask = (bitboard_t)0;

        int zone_num_found_pos = mask_drawing.find(zone + '0');

        while(zone_num_found_pos != std::string::npos){
            
            int piece_x = pos%10;
            int piece_y = pos/10;

            int zone_x_rel_to_piece = zone_num_found_pos%str_length - piece_x_in_drawing;
            int zone_y_rel_to_piece = - zone_num_found_pos/str_length + piece_y_in_drawing;

            int zone_x = piece_x + zone_x_rel_to_piece;
            int zone_y = piece_y + zone_y_rel_to_piece;

            if(zone_x < 0 || zone_y < 0 || zone_x > 9 || zone_y > 9){
                mask = (bitboard_t)-1;
            }
            else{
                mask |= (bitboard_t)1 << zone_x + zone_y*10;
            }

            zone_num_found_pos = mask_drawing.find(zone + '0', zone_num_found_pos + 1);

        }

        return mask;

    }


    int init_zone_masks(){

        for(int pos = 0; pos < 100; pos++){
            for(int rot = 0; rot < 4; rot++){
                for(int zone = 0; zone < 8; zone++)
                    zone_mask_cabeza[pos][rot][zone] = generate_zone_mask_from_string(zone_mask_drawing_cabeza[rot], pos, rot, zone);

                for(int zone = 0; zone < 4; zone++){
                    
                    zone_mask_mini[pos][rot][zone] = generate_zone_mask_from_string(zone_mask_drawing_mini[rot], pos, rot, zone);
                    zone_mask_flaco[pos][rot][zone][0] = generate_zone_mask_from_string(zone_mask_drawing_flaco_flat[rot], pos, rot, zone);
                    zone_mask_flaco[pos][rot][zone][1] = generate_zone_mask_from_string(zone_mask_drawing_flaco_horizontal[rot], pos, rot, zone);
                    zone_mask_flaco[pos][rot][zone][2] = generate_zone_mask_from_string(zone_mask_drawing_flaco_vertical[rot], pos, rot, zone);
                    zone_mask_chato[pos][rot][zone][0] = generate_zone_mask_from_string(zone_mask_drawing_chato_flat[rot], pos, rot, zone);
                    zone_mask_chato[pos][rot][zone][1] = generate_zone_mask_from_string(zone_mask_drawing_chato_horizontal[rot], pos, rot, zone);
                    zone_mask_chato[pos][rot][zone][2] = generate_zone_mask_from_string(zone_mask_drawing_chato_vertical[rot], pos, rot, zone);

                }

                zone_mask_gordo[pos][rot] =  generate_zone_mask_from_string(zone_mask_drawing_gordo[rot], pos, rot, 0);
            }
        }

        return 0;
    }

    int init_movement_lookups(){


        //Generate cabeza movement_t lookup

        char occupancy[3][3]; //3x3 sorounding of cabeza (C) piece.
        //'x'=not accessbile, ' '=not analyzed, '0'=piece, 'a'=accessible
        
        /*
        bool[x][y]
        0 2 5    02 12 22
        1 3 6  = 01 11 21
        C 4 7    XX 10 20
        */

        for(int addr = 0; addr <= 0xFF; addr++){
            occupancy[0][2] = addr&0x01 ? 'x' : ' '; //This associates each bit of the array adress with a specific zone
            occupancy[0][1] = addr&0x02 ? 'x' : ' ';
            occupancy[1][2] = addr&0x04 ? 'x' : ' ';
            occupancy[1][1] = addr&0x08 ? 'x' : ' ';
            occupancy[1][0] = addr&0x10 ? 'x' : ' ';
            occupancy[2][2] = addr&0x20 ? 'x' : ' ';
            occupancy[2][1] = addr&0x40 ? 'x' : ' ';
            occupancy[2][0] = addr&0x80 ? 'x' : ' ';
            occupancy[0][0] = '0';

            //This code is NOT scalable to larger movement_t numbers
            for(int move = 1; move <= 2; move++){

                
                //Only pass through inner zones
                for(int x = 1; x >= 0; x--){
                    for(int y = 1; y >= 0; y--){

                        if(occupancy[x][y] != 'x' && occupancy[x][y] != ' '){ //If piece can be accessed by a move (including move #0) i.e. starting square
                            if(occupancy[x][y+1] == ' '){
                                occupancy[x][y+1] = 'a';
                            }
                            if(occupancy[x+1][y] == ' '){
                                occupancy[x+1][y] = 'a';
                            }
                            if(occupancy[x+1][y+1] == ' '){
                                if(occupancy[x][y+1] != 'x' || occupancy[x+1][y] != 'x'){
                                    occupancy[x+1][y+1] = 'a';
                                }
                                else{
                                    occupancy[x+1][y+1] = 'x';
                                }
                            }
                            if(x > 0){
                                if(occupancy[x-1][y+1] == ' '){
                                    if(occupancy[x][y+1] != 'x' || occupancy[x-1][y] != 'x'){
                                        occupancy[x-1][y+1] = 'a';
                                    }
                                    else{
                                        occupancy[x-1][y+1] = 'x';
                                    }
                                }
                            }
                            if(y > 0){
                                if(occupancy[x+1][y-1] == ' '){
                                    if(occupancy[x][y-1] != 'x' || occupancy[x+1][y] != 'x'){
                                        occupancy[x+1][y-1] = 'a';
                                    }
                                    else{
                                        occupancy[x+1][y-1] = 'x';
                                    }
                                }
                            }
                        }

                    }                    
                }
            }

            possible_movements_t possible_movements_ = 0;

            possible_movements_ |= (occupancy[0][1] == 'a') ? MOVEMENT_N : 0;
            possible_movements_ |= (occupancy[0][2] == 'a') ? MOVEMENT_NN : 0;
            possible_movements_ |= (occupancy[1][0] == 'a') ? MOVEMENT_E : 0;
            possible_movements_ |= (occupancy[1][1] == 'a') ? MOVEMENT_NE : 0;
            possible_movements_ |= (occupancy[1][2] == 'a') ? MOVEMENT_NNE : 0;
            possible_movements_ |= (occupancy[2][0] == 'a') ? MOVEMENT_EE : 0;
            possible_movements_ |= (occupancy[2][1] == 'a') ? MOVEMENT_NEE : 0;
            possible_movements_ |= (occupancy[2][2] == 'a') ? MOVEMENT_NNEE : 0;

            movement_lookup_c[addr] = possible_movements_;

        }

        //Generate mini movement_t lookups

        movement_lookup_m[0b0000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE | MOVEMENT_WN;
        movement_lookup_m[0b0001] = MOVEMENT_N | MOVEMENT_NE | MOVEMENT_WN;
        movement_lookup_m[0b0010] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE;
        movement_lookup_m[0b0011] = MOVEMENT_N | MOVEMENT_NE;
        movement_lookup_m[0b0100] = (possible_movements_t) 0;
        movement_lookup_m[0b0101] = (possible_movements_t) 0;
        movement_lookup_m[0b0110] = (possible_movements_t) 0;
        movement_lookup_m[0b0111] = (possible_movements_t) 0;
        movement_lookup_m[0b1000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_WN;
        movement_lookup_m[0b1001] = MOVEMENT_N | MOVEMENT_WN;
        movement_lookup_m[0b1010] = MOVEMENT_N | MOVEMENT_NN;
        movement_lookup_m[0b1011] = MOVEMENT_N;
        movement_lookup_m[0b1100] = (possible_movements_t) 0;
        movement_lookup_m[0b1101] = (possible_movements_t) 0;
        movement_lookup_m[0b1110] = (possible_movements_t) 0;
        movement_lookup_m[0b1111] = (possible_movements_t) 0;

        //Generate falco movement_t lookups

        movement_lookup_fh[0b0000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE | MOVEMENT_NW;
        movement_lookup_fh[0b0001] = MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW;
        movement_lookup_fh[0b0010] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE;
        movement_lookup_fh[0b0011] = MOVEMENT_N | MOVEMENT_NE;
        movement_lookup_fh[0b0100] = (possible_movements_t) 0;
        movement_lookup_fh[0b0101] = (possible_movements_t) 0;
        movement_lookup_fh[0b0110] = (possible_movements_t) 0;
        movement_lookup_fh[0b0111] = (possible_movements_t) 0;
        movement_lookup_fh[0b1000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NW;
        movement_lookup_fh[0b1001] = MOVEMENT_N | MOVEMENT_NW;
        movement_lookup_fh[0b1010] = MOVEMENT_N | MOVEMENT_NN;
        movement_lookup_fh[0b1011] = MOVEMENT_N;
        movement_lookup_fh[0b1100] = (possible_movements_t) 0;
        movement_lookup_fh[0b1101] = (possible_movements_t) 0;
        movement_lookup_fh[0b1110] = (possible_movements_t) 0;
        movement_lookup_fh[0b1111] = (possible_movements_t) 0;
        
        return 0;
    }
    
    /*
    Initializes lookup arrays
    */
    int init(){
        init_zone_masks();
        init_movement_lookups();
        return 0;
    }



    possible_movements_t get_cabeza_moves(bitboard_t occupancy, int pos){

        possible_movements_t moves = 0;

        uint8_t zone_occupancy = 0;

        possible_movements_t unshifted_moves = movement_lookup_c[zone_occupancy];

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 8; zone++){
                zone_occupancy |= ((zone_mask_cabeza[pos][rot][zone] & occupancy) != 0) << zone;
            }
            unshifted_moves = movement_lookup_c[zone_occupancy];
            moves |= (unshifted_moves << rot*7)|(unshifted_moves >> (28 - rot*7));
        }
        return moves;
    }

    possible_movements_t get_mini_moves(bitboard_t occupancy, int pos){

        possible_movements_t moves = 0;

        uint8_t zone_occupancy = 0;

        possible_movements_t unshifted_moves;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_mini[pos][rot][zone] & occupancy) != 0) << zone;
            }
            unshifted_moves = movement_lookup_m[zone_occupancy];

            moves |= (unshifted_moves << rot*7)|(unshifted_moves >> (28 - rot*7));
        }
        return moves;
    }

    possible_movements_t get_flaco_moves(bitboard_t occupancy, int pos, Orientation o){

        possible_movements_t moves = 0;

        uint8_t zone_occupancy = 0;

        possible_movements_t unshifted_moves = movement_lookup_fh[zone_occupancy];

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_flaco[pos][rot][zone][o] & occupancy) != 0) << zone;
            }
            moves |= (unshifted_moves << rot*7)|(unshifted_moves >> (28 - rot*7));;
        }
        return moves;
    }

    possible_movements_t get_chato_moves(bitboard_t occupancy, int pos, Orientation o){

        possible_movements_t moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_chato[pos][rot][zone][o] & occupancy) != 0) << zone;
            }
            moves |= (movement_lookup_fh[zone_occupancy] << rot*7)|(movement_lookup_fh[zone_occupancy] >> (28 - rot*7));
        }
        return moves;
    }

    possible_movements_t get_gordo_moves(bitboard_t occupancy, int pos){

        possible_movements_t moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            zone_occupancy |= ((zone_mask_gordo[pos][rot] & occupancy) == 0);
    
            moves |= zone_occupancy << rot*7;
        }
        return moves;
    }

    /*
    Finds all movements for a given game state
    */
    all_possible_movements get_movements(game_state state){

        using enum PieceIndex;

        all_possible_movements all_possible_movements_; //Struct to return movements

        bitboard_t occupancy_normal = (bitboard_t)0;     //Ocupancy as seen for block pieces
        bitboard_t occupancy_for_cabeza = (bitboard_t)0; //Occupancy as seen for cabeza piece (cabeza cant eat another cabeza)

        int piece_index_offset = 0; //Used to select team (0=red, 5=blue)

        occupancy_for_cabeza |= state.pieces[1].bitboard;
        occupancy_for_cabeza |= state.pieces[2].bitboard;
        occupancy_for_cabeza |= state.pieces[3].bitboard;
        occupancy_for_cabeza |= state.pieces[4].bitboard;
        occupancy_for_cabeza |= state.pieces[6].bitboard;
        occupancy_for_cabeza |= state.pieces[7].bitboard;
        occupancy_for_cabeza |= state.pieces[8].bitboard;
        occupancy_for_cabeza |= state.pieces[9].bitboard;


        if(state.turn == Team::red){
            occupancy_normal = occupancy_for_cabeza | state.pieces[0].bitboard;
        }
        else{
            occupancy_normal = occupancy_for_cabeza | state.pieces[5].bitboard;
            piece_index_offset = 5;
        }


        all_possible_movements_.cabeza = get_cabeza_moves(occupancy_for_cabeza, get_pos_from_bitboard(state.pieces[piece_index_offset].bitboard));
        all_possible_movements_.mini = get_mini_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[piece_index_offset + 1].bitboard));
        all_possible_movements_.flaco = get_flaco_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[piece_index_offset + 2].bitboard), state.pieces[piece_index_offset + 2].o);
        all_possible_movements_.chato = get_chato_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[piece_index_offset + 3].bitboard), state.pieces[piece_index_offset + 3].o);
        all_possible_movements_.gordo = get_gordo_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[piece_index_offset + 4].bitboard));

        return all_possible_movements_;
    }

    std::map<std::string, movement_t> movement_str_dict = {{"N",MOVEMENT_N},
                                                            {"NN",MOVEMENT_NN},
                                                            {"NE",MOVEMENT_NE},
                                                            {"EN",MOVEMENT_EN},
                                                            {"NNE",MOVEMENT_NNE},
                                                            {"NNEE",MOVEMENT_NNEE},
                                                            {"NEE",MOVEMENT_NEE},
                                                            {"E",MOVEMENT_E},
                                                            {"EE",MOVEMENT_EE},
                                                            {"ES",MOVEMENT_ES},
                                                            {"SE",MOVEMENT_SE},
                                                            {"SEE",MOVEMENT_SEE},
                                                            {"SSEE",MOVEMENT_SSEE},
                                                            {"SSE",MOVEMENT_SSE},
                                                            {"S",MOVEMENT_S},
                                                            {"SS",MOVEMENT_SS},
                                                            {"SW",MOVEMENT_SW},
                                                            {"WS",MOVEMENT_WS},
                                                            {"SSW",MOVEMENT_SSW},
                                                            {"SSWW",MOVEMENT_SSWW},
                                                            {"SWW",MOVEMENT_SWW},
                                                            {"W",MOVEMENT_W},
                                                            {"WW",MOVEMENT_WW},
                                                            {"WN",MOVEMENT_WN},
                                                            {"NW",MOVEMENT_NW},
                                                            {"NWW",MOVEMENT_NWW},
                                                            {"NNWW",MOVEMENT_NNWW},
                                                            {"NNW",MOVEMENT_NNW}};
                                                            
    movement_t parse_movement_str(std::string str){
        
        using namespace std;


        transform(str.begin(), str.end(), str.begin(), ::toupper); //Convert to upper case
 
        movement_t movement;

        //If movement command is longer that 2 char, it is a cabeza move
        if(str.length() > 2){

            string final_str;

            //Count how many N/S/E/W characters

            int num_north = count(str.begin(), str.end(), 'N');
            int num_south = count(str.begin(), str.end(), 'S');
            int num_east = count(str.begin(), str.end(), 'E');
            int num_west = count(str.begin(), str.end(), 'W');

            //Total movement
            int total_north = num_north - num_south; 
            int total_east = num_east - num_west; 

            if(total_north > 0){
                final_str.append(total_north, 'N');
            }
            if(total_north < 0){
                final_str.append(-total_north, 'S');
            }
            if(total_east > 0){
                final_str.append(total_east, 'E');
            }
            if(total_east < 0){
                final_str.append(-total_east, 'W');
            }

            movement = movement_str_dict[final_str];
        }

        //If its a simple cabeza move or another piece move
        else{
            movement = movement_str_dict[str];
        }
        
        return movement;
        
    }


    /*
    Applies a movement_t to a cabeza piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_cabeza(piece p, movement_t movement){

        bitboard_t moved_bitboard = p.bitboard;

        if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_EN | MOVEMENT_NW | MOVEMENT_WN | MOVEMENT_NEE | MOVEMENT_NWW)){
            moved_bitboard <<= 10;
        }
        else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_ES | MOVEMENT_SW | MOVEMENT_WS | MOVEMENT_SEE | MOVEMENT_SWW)){
            moved_bitboard >>= 10;
        }
        if(movement & (MOVEMENT_E | MOVEMENT_NE | MOVEMENT_EN | MOVEMENT_SE | MOVEMENT_ES | MOVEMENT_NNE | MOVEMENT_SSE)){
            moved_bitboard <<= 1; 
        }
        else if(movement & (MOVEMENT_W | MOVEMENT_NW | MOVEMENT_WN | MOVEMENT_SW | MOVEMENT_WS | MOVEMENT_NNW | MOVEMENT_SSW)){
            moved_bitboard >>= 1;
        }

        if(movement & (MOVEMENT_NNE | MOVEMENT_NNW | MOVEMENT_NNWW | MOVEMENT_NNEE | MOVEMENT_NN)){
            moved_bitboard <<= 20;
        }
        else if(movement & (MOVEMENT_SSE | MOVEMENT_SSW | MOVEMENT_SSWW | MOVEMENT_SSEE | MOVEMENT_SS)){
            moved_bitboard >>= 20;
        }
        if(movement & (MOVEMENT_NEE | MOVEMENT_SEE | MOVEMENT_NNEE | MOVEMENT_SSEE | MOVEMENT_EE)){
            moved_bitboard <<= 2;
        }
        else if(movement & (MOVEMENT_NWW | MOVEMENT_SWW | MOVEMENT_NNWW | MOVEMENT_SSWW | MOVEMENT_WW)){
            moved_bitboard >>= 2;
        }

        piece ret;
        ret.bitboard = moved_bitboard;

        return ret;

    }

    /*
    Applies a movement_t to a mini piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_mini(piece p, movement_t movement){

        bitboard_t moved_bitboard = p.bitboard;

        if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_EN | MOVEMENT_NW | MOVEMENT_WN)){
            moved_bitboard <<= 10;
        }
        else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_ES | MOVEMENT_SW | MOVEMENT_WS)){
            moved_bitboard >>= 10;
        }
        if(movement & (MOVEMENT_E | MOVEMENT_NE | MOVEMENT_EN | MOVEMENT_SE | MOVEMENT_ES)){
            moved_bitboard <<= 1; 
        }
        else if(movement & (MOVEMENT_W | MOVEMENT_NW | MOVEMENT_WN | MOVEMENT_SW | MOVEMENT_WS)){
            moved_bitboard >>= 1;
        }
        if(movement & MOVEMENT_NN){
            moved_bitboard <<= 20;
        }
        else if(movement & MOVEMENT_SS){
            moved_bitboard >>= 20;
        }
        if(movement & MOVEMENT_EE){
            moved_bitboard <<= 2;
        }
        else if(movement & MOVEMENT_WW){
            moved_bitboard >>= 2;
        }

        piece ret;
        ret.bitboard = moved_bitboard;

        return ret;
    }

    /*
    Applies a movement_t to a flaco piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_flaco(piece p, movement_t movement){

        piece new_piece = p;

        int pos = get_pos_from_bitboard(new_piece.bitboard);

        
        switch (new_piece.o){
        case Orientation::flat:
            if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW | MOVEMENT_NN)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 10);
            }
            else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_SW | MOVEMENT_SS)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 20);
            }
            else if(movement & (MOVEMENT_E | MOVEMENT_EN | MOVEMENT_ES | MOVEMENT_EE)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 1);
            }
            else if(movement & (MOVEMENT_W | MOVEMENT_WN | MOVEMENT_WS | MOVEMENT_WW)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 2);  
            }            
            break;
        
        case Orientation::horizontal:
            if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW | MOVEMENT_NN)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 10);
            }
            else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_SW | MOVEMENT_SS)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 10);
            }
            else if(movement & (MOVEMENT_E | MOVEMENT_EN | MOVEMENT_ES | MOVEMENT_EE)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos + 2);
            }
            else if(movement & (MOVEMENT_W | MOVEMENT_WN | MOVEMENT_WS | MOVEMENT_WW)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos - 1);  
            }            
            break;
        
        case Orientation::vertical:
            if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW | MOVEMENT_NN)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos + 20);
            }
            else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_SW | MOVEMENT_SS)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos - 10);
            }
            else if(movement & (MOVEMENT_E | MOVEMENT_EN | MOVEMENT_ES | MOVEMENT_EE)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 1);
            }
            else if(movement & (MOVEMENT_W | MOVEMENT_WN | MOVEMENT_WS | MOVEMENT_WW)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 1);  
            }            
            break;
        }

        pos = get_pos_from_bitboard(new_piece.bitboard);
        
        switch (new_piece.o){
        case Orientation::flat:
            if(movement & (MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NN)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 10);
            }
            else if(movement & (MOVEMENT_ES | MOVEMENT_WS | MOVEMENT_SS)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 20);
            }
            else if(movement & (MOVEMENT_NE | MOVEMENT_SE | MOVEMENT_EE)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 1);
            }
            else if(movement & (MOVEMENT_NW | MOVEMENT_SW | MOVEMENT_WW)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 2);  
            }            
            break;
        
        case Orientation::horizontal:
            if(movement & (MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NN)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 10);
            }
            else if(movement & (MOVEMENT_ES | MOVEMENT_WS | MOVEMENT_SS)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 10);
            }
            else if(movement & (MOVEMENT_NE | MOVEMENT_SE | MOVEMENT_EE)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos + 2);
            }
            else if(movement & (MOVEMENT_NW | MOVEMENT_SW | MOVEMENT_WW)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos - 1);  
            }            
            break;
        
        case Orientation::vertical:
            if(movement & (MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NN)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos + 20);
            }
            else if(movement & (MOVEMENT_ES | MOVEMENT_WS | MOVEMENT_SS)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0x1 << (pos - 10);
            }
            else if(movement & (MOVEMENT_NE | MOVEMENT_SE | MOVEMENT_EE)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 1);
            }
            else if(movement & (MOVEMENT_NW | MOVEMENT_SW | MOVEMENT_WW)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 1);  
            }            
            break;
        }

        return new_piece;

    }

    /*
    Applies a movement_t to a chato piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_chato(piece p, movement_t movement){

        piece new_piece = p;

        int pos = get_pos_from_bitboard(new_piece.bitboard);

        
        switch (new_piece.o){
        case Orientation::flat:
            if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW | MOVEMENT_NN)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 20);
            }
            else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_SW | MOVEMENT_SS)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 10);
            }
            else if(movement & (MOVEMENT_E | MOVEMENT_EN | MOVEMENT_ES | MOVEMENT_EE)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 2);
            }
            else if(movement & (MOVEMENT_W | MOVEMENT_WN | MOVEMENT_WS | MOVEMENT_WW)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 1);  
            }            
            break;
        
        case Orientation::horizontal:
            if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW | MOVEMENT_NN)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos + 10);
            }
            else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_SW | MOVEMENT_SS)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos - 20);
            }
            else if(movement & (MOVEMENT_E | MOVEMENT_EN | MOVEMENT_ES | MOVEMENT_EE)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 2);
            }
            else if(movement & (MOVEMENT_W | MOVEMENT_WN | MOVEMENT_WS | MOVEMENT_WW)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 2);  
            }            
            break;
        
        case Orientation::vertical:
            if(movement & (MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW | MOVEMENT_NN)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 20);
            }
            else if(movement & (MOVEMENT_S | MOVEMENT_SE | MOVEMENT_SW | MOVEMENT_SS)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 20);
            }
            else if(movement & (MOVEMENT_E | MOVEMENT_EN | MOVEMENT_ES | MOVEMENT_EE)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos + 1);
            }
            else if(movement & (MOVEMENT_W | MOVEMENT_WN | MOVEMENT_WS | MOVEMENT_WW)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos - 2);  
            }            
            break;
        }

        pos = get_pos_from_bitboard(new_piece.bitboard);
        
        switch (new_piece.o){
        case Orientation::flat:
            if(movement & (MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NN)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 20);
            }
            else if(movement & (MOVEMENT_ES | MOVEMENT_WS | MOVEMENT_SS)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 10);
            }
            else if(movement & (MOVEMENT_NE | MOVEMENT_SE | MOVEMENT_EE)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 2);
            }
            else if(movement & (MOVEMENT_NW | MOVEMENT_SW | MOVEMENT_WW)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 1);  
            }            
            break;
        
        case Orientation::horizontal:
            if(movement & (MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NN)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos + 10);
            }
            else if(movement & (MOVEMENT_ES | MOVEMENT_WS | MOVEMENT_SS)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos - 20);
            }
            else if(movement & (MOVEMENT_NE | MOVEMENT_SE | MOVEMENT_EE)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos + 2);
            }
            else if(movement & (MOVEMENT_NW | MOVEMENT_SW | MOVEMENT_WW)){
                new_piece.o = Orientation::horizontal;
                new_piece.bitboard =  (bitboard_t)0x3 << (pos - 2);  
            }            
            break;
        
        case Orientation::vertical:
            if(movement & (MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NN)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos + 20);
            }
            else if(movement & (MOVEMENT_ES | MOVEMENT_WS | MOVEMENT_SS)){
                new_piece.o = Orientation::vertical;
                new_piece.bitboard =  (bitboard_t)0x401 << (pos - 20);
            }
            else if(movement & (MOVEMENT_NE | MOVEMENT_SE | MOVEMENT_EE)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos + 1);
            }
            else if(movement & (MOVEMENT_NW | MOVEMENT_SW | MOVEMENT_WW)){
                new_piece.o = Orientation::flat;
                new_piece.bitboard =  (bitboard_t)0xC03 << (pos - 2);  
            }            
            break;
        }

        return new_piece;

    }

    /*
    Applies a movement_t to a gordo piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */ 
    piece apply_move_gordo(piece p, movement_t movement){

        bitboard_t moved_bitboard = p.bitboard;

        if(movement & (MOVEMENT_N)){
            moved_bitboard <<= 20;
        }
        else if(movement & (MOVEMENT_S)){
            moved_bitboard >>= 20;
        }
        if(movement & (MOVEMENT_E)){
            moved_bitboard <<= 2; 
        }
        else if(movement & (MOVEMENT_W)){
            moved_bitboard >>= 2;
        }

        piece ret;
        ret.bitboard = moved_bitboard;

        return ret;
    }


}