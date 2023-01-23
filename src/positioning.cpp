#include <iostream>
#include <cmath>


#include "positioning.h"



namespace positioning{



    /*
    Returns starting game state
    */
    game_state initial_game_state(){
 
        game_state state;
        state.pieces[PieceIndex::red_cabeza_idx] = {5,0};
        state.pieces[PieceIndex::red_mini_idx] = {4,0};
        state.pieces[PieceIndex::red_flaco_idx] = {3,0,Orientation::vertical};
        state.pieces[PieceIndex::red_chato_idx] = {6,0,Orientation::vertical};
        state.pieces[PieceIndex::red_gordo_idx] = {4,1};

        state.pieces[PieceIndex::blue_cabeza_idx] = {4,9};
        state.pieces[PieceIndex::blue_mini_idx]  = {5,9};
        state.pieces[PieceIndex::blue_flaco_idx] = {6,8,Orientation::vertical};
        state.pieces[PieceIndex::blue_chato_idx] = {3,8,Orientation::vertical};
        state.pieces[PieceIndex::blue_gordo_idx] = {4,7};

        generate_bitboards(&state);

        return state;
    }

    /*
    Returns a randomly arranged game. It's a slow function
    */
    game_state random_game_state(){
        game_state state;

        state.turn = Team(rand()%2); //Generate random team



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

            PieceType piece_type = kPieceType[piece_num];

            if(piece_type & (PIECE_CABEZA_MASK | PIECE_MINI_MASK)){//If piece is cabeza or mini
                test_x = rand() % 10;
                test_y = rand() % 10;
            }

            else if(piece_type & PIECE_FLACO_MASK){//If piece is flaco

                test_o = Orientation(rand()%3);

                test_x = rand() % (test_o == Orientation::horizontal ? 9 : 10);
                test_y = rand() % (test_o == Orientation::vertical ? 9 : 10);

            }
            else if(piece_type & PIECE_CHATO_MASK){//If piece is chato 

                test_o = Orientation(rand()%3);

                test_x = rand() % (test_o == Orientation::vertical ? 10 : 9);
                test_y = rand() % (test_o == Orientation::horizontal ? 10 : 9);
            }

            else{ //If piece is gordo
                test_x = rand() % 9;
                test_y = rand() % 9;
            }
            
            test_bitboard = generate_bitboard_for_piece(piece_type, test_o, test_x, test_y);

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
    bitboard_t generate_bitboard_for_piece(PieceType piece_type, Orientation o, int x, int y){

        bitboard_t bitboard;

        
        bitboard_t flaco_template[] = {0x1, 0x3, 0x401};   //Flat, horizontal, vertical
        bitboard_t chato_template[] = {0xC03, 0x3, 0x401}; //Flat, horizontal, vertical
        
        //If piece type is a cabeza or a mini
        if(piece_type & (PIECE_CABEZA_MASK | PIECE_MINI_MASK)){
            bitboard = (bitboard_t)1 << (y*10 + x);
        }

        //If piece type is a flaco
        else if(piece_type & PIECE_FLACO_MASK){
            bitboard = flaco_template[o] << (y*10 + x);
        }

        //If piece type is a flaco
        else if(piece_type & PIECE_CHATO_MASK){
            bitboard = chato_template[o] << (y*10 + x);
        }

        //If piece type is a chato
        else if(piece_type & PIECE_GORDO_MASK){
            bitboard = (bitboard_t)0xC03 << (y*10 + x);
        }

        return bitboard;
    }

    /*
    Writes bitboard variables on each piece for a given game state
    */
    int generate_bitboards(game_state * state){
        
        for(int i = 0; i < 10; i++){
            PieceType piece_type = kPieceType[i];
            piece piece_ = state->pieces[i];
            
            state->pieces[i].bitboard = generate_bitboard_for_piece(piece_type, piece_.o, piece_.x, piece_.y);
        }

        return 0;
    }


    /*
    Calculates occupancy bitboard for the board of the given game state
    If opponent_cabeza_free is true, the function will consider the cabeza of the non moving player as free,
    thus it permits a capture
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
    Calculates leading zeroes in a bitboard.
    Used to determine the lower left coordinate of a piece from the bitboard.
    */
    inline int ctz_u128 (__uint128_t u) {
        uint64_t hi = u>>64;
        uint64_t lo = u;
        int retval[3]={ 
            __builtin_ctzll(lo),
            __builtin_ctzll(hi)+64,
            128
        };
        int idx = !lo + ((!lo)&(!hi));
        return retval[idx];
    }

    /*
    Bitboards used for masking out the specific "zones" needed to analyze a piece.
    First index encodes the possible possitions, as obtained by ctz_u128()
    Second index encodes the four possible symmetries used in each case.
    Third index encodes the 1,5,6,7 or 8 zones used in analyzing a piece.
    */
    bitboard_t zone_mask_cabeza[100][4][8];
    bitboard_t zone_mask_mini[100][4][4];
    bitboard_t zone_mask_flaco_flat[100][4][4];

    int generate_zone_masks(){

        for(int pos = 0; pos < 100; pos++){
            for(int rot = 0; rot < 4; rot++){
                for(int zone = 0; zone < 8; zone++)
                    zone_mask_cabeza[pos][rot][zone] = generate_zone_mask_from_string(zone_mask_drawing_cabeza[rot], pos, rot, zone);

                for(int zone = 0; zone < 4; zone++)
                    zone_mask_mini[pos][rot][zone] = generate_zone_mask_from_string(zone_mask_drawing_mini[rot], pos, rot, zone);

                for(int zone = 0; zone < 4; zone++)
                    zone_mask_flaco_flat[pos][rot][zone] = generate_zone_mask_from_string(zone_mask_drawing_flaco_flat[rot], pos, rot, zone);


            }
        }

        return 0;
    }

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

    possible_movements_cabeza movement_lookup_cabeza[256]; 
    possible_movements_mini movement_lookup_mini[16];
    possible_movements_flaco movement_lookup_flaco[16];

    int generate_movement_lookups(){


        //Generate cabeza movement lookup

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

            //This code is NOT scalable to larger movement numbers
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

            possible_movements_cabeza possible_movements_ = 0;

            possible_movements_ |= (occupancy[0][1] == 'a') ? POSSIBLE_MOVEMENT_C_U : 0;
            possible_movements_ |= (occupancy[0][2] == 'a') ? POSSIBLE_MOVEMENT_C_UU : 0;
            possible_movements_ |= (occupancy[1][0] == 'a') ? POSSIBLE_MOVEMENT_C_R : 0;
            possible_movements_ |= (occupancy[1][1] == 'a') ? POSSIBLE_MOVEMENT_C_UR : 0;
            possible_movements_ |= (occupancy[1][2] == 'a') ? POSSIBLE_MOVEMENT_C_UUR : 0;
            possible_movements_ |= (occupancy[2][0] == 'a') ? POSSIBLE_MOVEMENT_C_RR : 0;
            possible_movements_ |= (occupancy[2][1] == 'a') ? POSSIBLE_MOVEMENT_C_URR : 0;
            possible_movements_ |= (occupancy[2][2] == 'a') ? POSSIBLE_MOVEMENT_C_UURR : 0;

            movement_lookup_cabeza[addr] = possible_movements_;

        }

        //Generate mini movement lookups

        movement_lookup_mini[0b0000] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_UU | POSSIBLE_MOVEMENT_M_UR | POSSIBLE_MOVEMENT_M_LU;
        movement_lookup_mini[0b0001] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_UR | POSSIBLE_MOVEMENT_M_LU;
        movement_lookup_mini[0b0010] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_UU | POSSIBLE_MOVEMENT_M_UR;
        movement_lookup_mini[0b0011] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_UR;
        movement_lookup_mini[0b0100] = (possible_movements_mini) 0;
        movement_lookup_mini[0b0101] = (possible_movements_mini) 0;
        movement_lookup_mini[0b0110] = (possible_movements_mini) 0;
        movement_lookup_mini[0b0111] = (possible_movements_mini) 0;
        movement_lookup_mini[0b1000] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_UU | POSSIBLE_MOVEMENT_M_LU;
        movement_lookup_mini[0b1001] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_LU;
        movement_lookup_mini[0b1010] = POSSIBLE_MOVEMENT_M_U | POSSIBLE_MOVEMENT_M_UU;
        movement_lookup_mini[0b1011] = POSSIBLE_MOVEMENT_M_U;
        movement_lookup_mini[0b1100] = (possible_movements_mini) 0;
        movement_lookup_mini[0b1101] = (possible_movements_mini) 0;
        movement_lookup_mini[0b1110] = (possible_movements_mini) 0;
        movement_lookup_mini[0b1111] = (possible_movements_mini) 0;

        //Generate falco movement lookups

        movement_lookup_flaco[0b0000] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UU | POSSIBLE_MOVEMENT_FH_UR | POSSIBLE_MOVEMENT_FH_UL;
        movement_lookup_flaco[0b0001] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UR | POSSIBLE_MOVEMENT_FH_UL;
        movement_lookup_flaco[0b0010] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UU | POSSIBLE_MOVEMENT_FH_UR;
        movement_lookup_flaco[0b0011] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UR;
        movement_lookup_flaco[0b0100] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b0101] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b0110] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b0111] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b1000] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UU | POSSIBLE_MOVEMENT_FH_UL;
        movement_lookup_flaco[0b1001] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UL;
        movement_lookup_flaco[0b1010] = POSSIBLE_MOVEMENT_FH_U | POSSIBLE_MOVEMENT_FH_UU;
        movement_lookup_flaco[0b1011] = POSSIBLE_MOVEMENT_FH_U;
        movement_lookup_flaco[0b1100] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b1101] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b1110] = (possible_movements_mini) 0;
        movement_lookup_flaco[0b1111] = (possible_movements_mini) 0;
        
        return 0;
    }

    possible_movements_cabeza get_cabeza_moves(game_state state, PieceIndex piece_idx){

        int pos = ctz_u128(state.pieces[piece_idx].bitboard);

        bitboard_t occupancy = get_occupancy(state, false);

        possible_movements_cabeza moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 8; zone++){
                zone_occupancy |= ((zone_mask_cabeza[pos][rot][zone] & occupancy) != 0) << zone;
            }
            moves |= movement_lookup_cabeza[zone_occupancy] << rot*6;;
        }
        moves |= moves >> 24; //Performs or operation to joing last rotation with first
        return moves;
    }

    possible_movements_mini get_mini_moves(game_state state, PieceIndex piece_idx){

        int pos = ctz_u128(state.pieces[piece_idx].bitboard);

        bitboard_t occupancy = get_occupancy(state, true);

        possible_movements_mini moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_mini[pos][rot][zone] & occupancy) != 0) << zone;
            }
            moves |= movement_lookup_mini[zone_occupancy] << rot*3;;
        }
        moves |= moves >> 12; //Performs or operation to joing last rotation with first
        return moves;
    }

    possible_movements_flaco get_flaco_flat_moves(game_state state, PieceIndex piece_idx){

        int pos = ctz_u128(state.pieces[piece_idx].bitboard);

        bitboard_t occupancy = get_occupancy(state, true);

        possible_movements_mini moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_flaco_flat[pos][rot][zone] & occupancy) != 0) << zone;
            }
            moves |= movement_lookup_flaco[zone_occupancy] << rot*4;;
        }
        return moves;
    }

}