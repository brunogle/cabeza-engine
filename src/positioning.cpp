#include <iostream>
#include <cmath>
#include <algorithm>
#include <string.h>

#include "parsing.h"
#include "util.h"
#include "positioning.h"


namespace positioning{


    /////////////////////////////////////////
    //                                     //
    //       PREDEFINED GAME STATES        //
    //                                     //
    /////////////////////////////////////////


    /*
    Generates occupancy bitboard for a specific piece
    */
    bitboard_t generate_bitboard_for_piece(PieceType piece_type, Orientation o, int x, int y){

        bitboard_t bitboard;

        
        bitboard_t flaco_template[] = {0x1, 0x3, 0x401};   //Flat, horizontal, vertical
        bitboard_t chato_template[] = {0xC03, 0x3, 0x401}; //Flat, horizontal, vertical
        
        //If piece type is a cabeza or a mini
        if(piece_type == cabeza_idx || piece_type == mini_idx){
            bitboard = (bitboard_t)1 << (y*10 + x);
        }

        //If piece type is a flaco
        else if(piece_type == flaco_idx){
            bitboard = flaco_template[o] << (y*10 + x);
        }

        //If piece type is a chato
        else if(piece_type == chato_idx){
            bitboard = chato_template[o] << (y*10 + x);
        }

        //If piece type is a gordo
        else{
            bitboard = (bitboard_t)0xC03 << (y*10 + x);
        }

        return bitboard;
    }


    /////////////////////////////////////////
    //                                     //
    //     MOVEGEN LOOKUP ARRAYS           //
    //                                     //
    /////////////////////////////////////////


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
    Each element only represents the possible move for the first of the four symetries.
    */
    possible_moves_t movement_lookup_c[256]; 
    possible_moves_t movement_lookup_m[16];
    possible_moves_t movement_lookup_fh[16];




    /////////////////////////////////////////
    //                                     //
    //     LOOKUP ARRAY INITIALIZATION     //
    //                                     //
    /////////////////////////////////////////

    /*
    Initializes lookup arrays
    */
    int init(){
        init_zone_masks();
        init_movement_lookups();
        return 0;
    }

    /*
    Converts one of the zone_mask_drawings to a bitboard mask for a specific position, rotation and zone.
    Zone mask drawings are specified for squares of sides n, as n^2 sized strings, starting from upper left to bottom right.
    A P in a string represents the bottom left square of the corresponding piece. Numbers represent the mask number. Periods
    represent no mask assigned for that square.
    */
    bitboard_t generate_zone_mask_from_string(std::string mask_drawing, int piece_pos, int zone_num){


        int zone_side_size = sqrt(mask_drawing.length());

        //mask_drawing length must be a square number
        REL_ASSERT(zone_side_size*zone_side_size == (int)mask_drawing.length(), "Non square zone mask drawing");

        int piece_position_in_string = mask_drawing.find('P');

        int piece_x_in_drawing = piece_position_in_string%zone_side_size;
        int piece_y_in_drawing = piece_position_in_string/zone_side_size;

        bitboard_t mask = (bitboard_t)0;


        //Scan string for number characters matching "zone"

        int zone_num_found_pos = mask_drawing.find(zone_num + '0'); //Convert char to int

        while((unsigned long long)zone_num_found_pos != std::string::npos){
            
            int piece_pos_x = piece_pos%10; //Lower left piece pos
            int piece_pos_y = piece_pos/10;

            int zone_x_rel_to_piece = zone_num_found_pos%zone_side_size - piece_x_in_drawing;
            int zone_y_rel_to_piece = - zone_num_found_pos/zone_side_size + piece_y_in_drawing;

            int zone_square_x = piece_pos_x + zone_x_rel_to_piece;
            int zone_square_y = piece_pos_y + zone_y_rel_to_piece;

            //If zone square is outside board area, set a fully set bitmask.
            //Any OR operation with a "-1" bitmask will always return non-null with the occupancy grid.
            //This is necesary because if a zone lies outside the board, that zone is unaccessible by the piece.
            if(zone_square_x < 0 || zone_square_y < 0 || zone_square_x > 9 || zone_square_y > 9){
                mask = (bitboard_t)-1;
            }

            //If zone square lies inside board, set corresponding bit
            else{
                mask |= (bitboard_t)1 << (zone_square_x + zone_square_y*10);
            }

            zone_num_found_pos = mask_drawing.find(zone_num + '0', zone_num_found_pos + 1); //Find next zone

        }

        return mask;

    }

    /*
    Initialize zone masks used for movement generation.
    Uses generate_zone_mask_from_string() to generate masks from zone_mask_drawaing_xxx strings.
    */
    int init_zone_masks(){

        for(int piece_pos = 0; piece_pos < 100; piece_pos++){ //Loop though all 100 board positions
            for(int piece_rot = 0; piece_rot < 4; piece_rot++){ //Loop through all 4 piece rotation states

                for(int zone_num = 0; zone_num < 8; zone_num++) //Cabeza has 8 different zone masks for each rotation
                    zone_mask_cabeza[piece_pos][piece_rot][zone_num] = generate_zone_mask_from_string(zone_mask_drawing_cabeza[piece_rot], piece_pos, zone_num);

                for(int zone_num = 0; zone_num < 4; zone_num++){ // Mini, flaco and chato have four
                    
                    zone_mask_mini[piece_pos][piece_rot][zone_num] = generate_zone_mask_from_string(zone_mask_drawing_mini[piece_rot], piece_pos, zone_num);

                    zone_mask_flaco[piece_pos][piece_rot][zone_num][0] = generate_zone_mask_from_string(zone_mask_drawing_flaco_flat[piece_rot], piece_pos, zone_num);
                    zone_mask_flaco[piece_pos][piece_rot][zone_num][1] = generate_zone_mask_from_string(zone_mask_drawing_flaco_horizontal[piece_rot], piece_pos, zone_num);
                    zone_mask_flaco[piece_pos][piece_rot][zone_num][2] = generate_zone_mask_from_string(zone_mask_drawing_flaco_vertical[piece_rot], piece_pos, zone_num);

                    zone_mask_chato[piece_pos][piece_rot][zone_num][0] = generate_zone_mask_from_string(zone_mask_drawing_chato_flat[piece_rot], piece_pos, zone_num);
                    zone_mask_chato[piece_pos][piece_rot][zone_num][1] = generate_zone_mask_from_string(zone_mask_drawing_chato_horizontal[piece_rot], piece_pos, zone_num);
                    zone_mask_chato[piece_pos][piece_rot][zone_num][2] = generate_zone_mask_from_string(zone_mask_drawing_chato_vertical[piece_rot], piece_pos, zone_num);

                }

                zone_mask_gordo[piece_pos][piece_rot] =  generate_zone_mask_from_string(zone_mask_drawing_gordo[piece_rot], piece_pos, 0); //Gordo piece has 1 zone mask per rotation
            }
        }

        return 0;
    }


    /*
    Initialize movement_lookup arrays used for move generation
    */
    int init_movement_lookups(){


        //Generate cabeza move lookup


        //Temporary array
        //3x3 sorounding of cabeza (C) piece.
        //'x'=not accessbile, ' '=not analyzed, '0'=piece, 'a'=accessible
        
        /*
        bool[x][y]
        0 2 5    02 12 22
        1 3 6  = 01 11 21
        C 4 7    XX 10 20
        */
        char occupancy[3][3];




        //Loop through all 256 possible zone occupancies.
        //TODO: Consider other initialization forms
        for(int addr = 0; addr <= 0xFF; addr++){


            //Manually initialize occupancy[][]
            occupancy[0][2] = addr&0x01 ? 'x' : ' '; //This associates each bit of the array adress with a specific zone
            occupancy[0][1] = addr&0x02 ? 'x' : ' ';
            occupancy[1][2] = addr&0x04 ? 'x' : ' ';
            occupancy[1][1] = addr&0x08 ? 'x' : ' ';
            occupancy[1][0] = addr&0x10 ? 'x' : ' ';
            occupancy[2][2] = addr&0x20 ? 'x' : ' ';
            occupancy[2][1] = addr&0x40 ? 'x' : ' ';
            occupancy[2][0] = addr&0x80 ? 'x' : ' ';
            occupancy[0][0] = '0';

            //This code is NOT scalable to larger move numbers
            //move=1 for single move cabeza movement, move=2 for 2 move movement
            //Each pass the function checks what pieces the cabeza piece can reach
            for(int move = 1; move <= 2; move++){

                //Only pass through inner zones
                for(int x = 1; x >= 0; x--){
                    for(int y = 1; y >= 0; y--){
                        
                        //If piece can be accessed by a move (including move #0) i.e. starting square
                        if(occupancy[x][y] == 'a' || occupancy[x][y] == '0'){
                            
                            //Test soroundings and identify with 'a' accessible squares
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

            //Find possible movements based on completed occupancy[][] array

            possible_moves_t possible_movements_ = 0;

            possible_movements_ |= (occupancy[0][1] == 'a') ? MOVEMENT_N : 0;
            possible_movements_ |= (occupancy[0][2] == 'a') ? MOVEMENT_NN : 0;
            possible_movements_ |= (occupancy[1][0] == 'a') ? MOVEMENT_E : 0;
            possible_movements_ |= (occupancy[1][1] == 'a') ? MOVEMENT_NE | MOVEMENT_EN : 0;
            possible_movements_ |= (occupancy[1][2] == 'a') ? MOVEMENT_NNE : 0;
            possible_movements_ |= (occupancy[2][0] == 'a') ? MOVEMENT_EE : 0;
            possible_movements_ |= (occupancy[2][1] == 'a') ? MOVEMENT_NEE : 0;
            possible_movements_ |= (occupancy[2][2] == 'a') ? MOVEMENT_NNEE : 0;

            movement_lookup_c[addr] = possible_movements_;

        }

        //Manually fill mini move lookups

        movement_lookup_m[0b0000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE | MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NW;
        movement_lookup_m[0b0001] = MOVEMENT_N | MOVEMENT_NE | MOVEMENT_EN | MOVEMENT_WN | MOVEMENT_NW;
        movement_lookup_m[0b0010] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE | MOVEMENT_EN;
        movement_lookup_m[0b0011] = MOVEMENT_N | MOVEMENT_NE | MOVEMENT_EN;
        movement_lookup_m[0b0100] = (possible_moves_t) 0;
        movement_lookup_m[0b0101] = (possible_moves_t) 0;
        movement_lookup_m[0b0110] = (possible_moves_t) 0;
        movement_lookup_m[0b0111] = (possible_moves_t) 0;
        movement_lookup_m[0b1000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_WN | MOVEMENT_NW;
        movement_lookup_m[0b1001] = MOVEMENT_N | MOVEMENT_WN | MOVEMENT_NW;
        movement_lookup_m[0b1010] = MOVEMENT_N | MOVEMENT_NN;
        movement_lookup_m[0b1011] = MOVEMENT_N;
        movement_lookup_m[0b1100] = (possible_moves_t) 0;
        movement_lookup_m[0b1101] = (possible_moves_t) 0;
        movement_lookup_m[0b1110] = (possible_moves_t) 0;
        movement_lookup_m[0b1111] = (possible_moves_t) 0;

        //Manually fill falco and chato move lookups

        movement_lookup_fh[0b0000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE | MOVEMENT_NW;
        movement_lookup_fh[0b0001] = MOVEMENT_N | MOVEMENT_NE | MOVEMENT_NW;
        movement_lookup_fh[0b0010] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NE;
        movement_lookup_fh[0b0011] = MOVEMENT_N | MOVEMENT_NE;
        movement_lookup_fh[0b0100] = (possible_moves_t) 0;
        movement_lookup_fh[0b0101] = (possible_moves_t) 0;
        movement_lookup_fh[0b0110] = (possible_moves_t) 0;
        movement_lookup_fh[0b0111] = (possible_moves_t) 0;
        movement_lookup_fh[0b1000] = MOVEMENT_N | MOVEMENT_NN | MOVEMENT_NW;
        movement_lookup_fh[0b1001] = MOVEMENT_N | MOVEMENT_NW;
        movement_lookup_fh[0b1010] = MOVEMENT_N | MOVEMENT_NN;
        movement_lookup_fh[0b1011] = MOVEMENT_N;
        movement_lookup_fh[0b1100] = (possible_moves_t) 0;
        movement_lookup_fh[0b1101] = (possible_moves_t) 0;
        movement_lookup_fh[0b1110] = (possible_moves_t) 0;
        movement_lookup_fh[0b1111] = (possible_moves_t) 0;
        
        return 0;
    }
    



    /////////////////////////////////////////
    //                                     //
    //     MOVE GENERATION FUNCTIONS       //
    //                                     //
    /////////////////////////////////////////

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_cabeza_moves(bitboard_t occupancy, int piece_pos){

        possible_moves_t moves = 0;

        uint8_t zone_occupancy = 0;

        possible_moves_t unshifted_moves = movement_lookup_c[zone_occupancy];

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 8; zone++){
                zone_occupancy |= ((zone_mask_cabeza[piece_pos][rot][zone] & occupancy) != 0) << zone;
            }
            unshifted_moves = movement_lookup_c[zone_occupancy];
            moves |= (unshifted_moves << rot*7)|(unshifted_moves >> (28 - rot*7));
        }
        return moves;
    }

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_mini_moves(bitboard_t occupancy, int piece_pos){

        possible_moves_t moves = 0;

        uint8_t zone_occupancy = 0;

        possible_moves_t unshifted_moves;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_mini[piece_pos][rot][zone] & occupancy) != 0) << zone;
            }
            unshifted_moves = movement_lookup_m[zone_occupancy];

            moves |= (unshifted_moves << rot*7)|(unshifted_moves >> (28 - rot*7));
        }
        return moves;
    }

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_flaco_moves(bitboard_t occupancy, int piece_pos, Orientation o){

        possible_moves_t moves = 0;

        uint8_t zone_occupancy = 0;

        possible_moves_t unshifted_moves;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_flaco[piece_pos][rot][zone][o] & occupancy) != 0) << zone;
            }
            unshifted_moves = movement_lookup_fh[zone_occupancy];
            moves |= (unshifted_moves << rot*7)|(unshifted_moves >> (28 - rot*7));;
        }
        return moves;
    }

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_chato_moves(bitboard_t occupancy, int piece_pos, Orientation o){

        possible_moves_t moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            for(int zone = 0; zone < 4; zone++){
                zone_occupancy |= ((zone_mask_chato[piece_pos][rot][zone][o] & occupancy) != 0) << zone;
            }
            moves |= (movement_lookup_fh[zone_occupancy] << rot*7)|(movement_lookup_fh[zone_occupancy] >> (28 - rot*7));
        }
        return moves;
    }

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_gordo_moves(bitboard_t occupancy, int piece_pos){

        possible_moves_t moves = 0;

        uint8_t zone_occupancy = 0;

        for(int rot = 0; rot < 4; rot++){
            
            zone_occupancy = 0;

            zone_occupancy |= ((zone_mask_gordo[piece_pos][rot] & occupancy) == 0);
    
            moves |= zone_occupancy << rot*7;
        }
        return moves;
    }

    /*
    Finds all movements for a given game state
    */
    all_possible_moves_t get_moves(game_state state){

        using enum Player;
        using enum PieceType;

        all_possible_moves_t all_possible_movements_; //Struct to return movements

        bitboard_t occupancy_normal = (bitboard_t)0;     //Ocupancy as seen for block pieces
        bitboard_t occupancy_for_cabeza = (bitboard_t)0; //Occupancy as seen for cabeza piece (cabeza cant eat another cabeza)


        occupancy_normal |= state.pieces[mini_idx][red].bitboard;
        occupancy_normal |= state.pieces[flaco_idx][red].bitboard;
        occupancy_normal |= state.pieces[chato_idx][red].bitboard;
        occupancy_normal |= state.pieces[gordo_idx][red].bitboard;
        occupancy_normal |= state.pieces[mini_idx][blue].bitboard;
        occupancy_normal |= state.pieces[flaco_idx][blue].bitboard;
        occupancy_normal |= state.pieces[chato_idx][blue].bitboard;
        occupancy_normal |= state.pieces[gordo_idx][blue].bitboard;

        if(state.turn == Player::red){
            occupancy_normal |= state.pieces[cabeza_idx][red].bitboard;
            occupancy_for_cabeza = occupancy_normal | state.pieces[cabeza_idx][blue].bitboard;
        }
        else{
            occupancy_normal |= state.pieces[cabeza_idx][blue].bitboard;
            occupancy_for_cabeza = occupancy_normal | state.pieces[cabeza_idx][red].bitboard;
        }


        all_possible_movements_.cabeza = get_cabeza_moves(occupancy_for_cabeza, get_pos_from_bitboard(state.pieces[cabeza_idx][state.turn].bitboard));
        all_possible_movements_.mini = get_mini_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[mini_idx][state.turn].bitboard));
        all_possible_movements_.flaco = get_flaco_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[flaco_idx][state.turn].bitboard), state.pieces[flaco_idx][state.turn].o);
        all_possible_movements_.chato = get_chato_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[chato_idx][state.turn].bitboard), state.pieces[chato_idx][state.turn].o);
        all_possible_movements_.gordo = get_gordo_moves(occupancy_normal, get_pos_from_bitboard(state.pieces[gordo_idx][state.turn].bitboard));
        
        all_possible_movements_.player = state.turn;

        return all_possible_movements_;
    }

    /*
    List all possible movements as an array of moves for use in seach. Returns number of possible moves.
    */
    int get_moves_as_list(game_state state, move movements[MAX_POSSIBLE_MOVEMENTS]){
        all_possible_moves_t moves = get_moves(state);

        int move_count = 0;

        moves.cabeza |= (moves.cabeza & (MOVEMENT_NE | MOVEMENT_ES | MOVEMENT_SW | MOVEMENT_WN)) << 1;
        moves.cabeza &= ~(MOVEMENT_NE | MOVEMENT_ES | MOVEMENT_SW | MOVEMENT_WN);
        moves.mini |= (moves.mini & (MOVEMENT_NE | MOVEMENT_ES | MOVEMENT_SW | MOVEMENT_WN)) << 1;
        moves.mini &= ~(MOVEMENT_NE | MOVEMENT_ES | MOVEMENT_SW | MOVEMENT_WN);

        for(int i = 0; i < 28; i++){

            int move_bit = 1<<i;

            if(moves.cabeza & move_bit){
                movements[move_count].move = move_bit;
                movements[move_count].piece = cabeza_idx;
                move_count++;
            }
            if(moves.mini & move_bit){
                movements[move_count].move = move_bit;
                movements[move_count].piece = mini_idx;
                move_count++;
            }
            if(moves.flaco & move_bit){
                movements[move_count].move = move_bit;
                movements[move_count].piece = flaco_idx;
                move_count++;
            }
            if(moves.chato & move_bit){
                movements[move_count].move = move_bit;
                movements[move_count].piece = chato_idx;
                move_count++;
            }
            if(moves.gordo & move_bit){
                movements[move_count].move = move_bit;
                movements[move_count].piece = gordo_idx;
                move_count++;
            }

            movements[move_count].player = state.turn;
        }


        return move_count;

    }


    /////////////////////////////////////////
    //                                     //
    //     MOVE APPLICATION FUNCTIONS      //
    //                                     //
    /////////////////////////////////////////

    /*
    Applies a movement to a cabeza piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_cabeza(piece p, move_bitarray_t movement){

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
        ret.o = p.o;
        return ret;

    }

    /*
    Applies a movement to a mini piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_mini(piece p, move_bitarray_t movement){

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
        ret.o = p.o;
        return ret;
    }

    /*
    Applies a movement to a flaco piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_flaco(piece p, move_bitarray_t movement){

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
    Applies a movement to a chato piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_chato(piece p, move_bitarray_t movement){

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
    Applies a movement to a gordo piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */ 
    piece apply_move_gordo(piece p, move_bitarray_t movement){

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
        ret.o = p.o;
        return ret;
    }

    /*
    Applies a movement to a board. Does not check if piece is valid
    */
    game_state apply_move(game_state state, move move){

        switch (move.piece)
        {
        case PieceType::cabeza_idx:
            state.pieces[move.piece][state.turn] = apply_move_cabeza(state.pieces[move.piece][state.turn], move.move);
            break;
        
        case PieceType::mini_idx:
            state.pieces[move.piece][state.turn] = apply_move_mini(state.pieces[move.piece][state.turn], move.move);
            break;

        case PieceType::flaco_idx:
            state.pieces[move.piece][state.turn] = apply_move_flaco(state.pieces[move.piece][state.turn], move.move);
            break;

        case PieceType::chato_idx:
            state.pieces[move.piece][state.turn] = apply_move_chato(state.pieces[move.piece][state.turn], move.move);
            break;

        case PieceType::gordo_idx:
            state.pieces[move.piece][state.turn] = apply_move_gordo(state.pieces[move.piece][state.turn], move.move);
            break;
        
        default:
            return state;
        }

        return state;
    
    }

    /*
    Checks if movement is valid
    */
    bool check_move_valid(game_state state, move move){

        all_possible_moves_t possible_moves = get_moves(state);
        bool valid;

        switch (move.piece)
        {
        case PieceType::cabeza_idx:
            valid = (possible_moves.cabeza & move.move) != 0;
            break;
        
        case PieceType::mini_idx:
            valid = (possible_moves.mini & move.move) != 0;
            break;

        case PieceType::flaco_idx:
            valid = (possible_moves.flaco & move.move) != 0;
            break;

        case PieceType::chato_idx:
            valid = (possible_moves.chato & move.move) != 0;
            break;

        case PieceType::gordo_idx:
            valid = possible_moves.gordo & move.move;
            break;
        
        default:
            valid = false;
        }

        return valid;

    
    }

    /*
    Applies a movement to a board. Checks if move succeeds
    */
    bool apply_move_safe(move move, game_state state_in, game_state & state_out){

        if(check_move_valid(state_in, move)){
            state_out = apply_move(state_in, move);
            return true;
        }
        else{
            return false;
        }

    }


    /////////////////////////////////////////
    //                                     //
    //          GAME FUNCTIONS             //
    //                                     //
    /////////////////////////////////////////


    /*
    Checks if a player won in last semimove.
    Only returns true if the player that has the win condition hast played last semimove (i.e. the player whoes turn is not)
    */
    bool check_for_win(positioning::game_state state){

        using namespace positioning;

        using enum Player;
        using enum PieceType;

        bool win = false;

        bitboard_t idle_player_non_cabeza_occupancy;

        switch (state.turn)
        {
        case red:
            if(state.pieces[cabeza_idx][blue].bitboard & ((bitboard_t) 0x3ff)){
                win = true;
                break;
            }
            idle_player_non_cabeza_occupancy = state.pieces[cabeza_idx][blue].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[mini_idx][blue].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[flaco_idx][blue].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[chato_idx][blue].bitboard;   

            win = state.pieces[cabeza_idx][red].bitboard & idle_player_non_cabeza_occupancy;

            break;

        case positioning::Player::blue:
            if(state.pieces[cabeza_idx][red].bitboard & ((bitboard_t) 0x3ff << 90)){
                win = true;
                break;
            }
            idle_player_non_cabeza_occupancy = state.pieces[cabeza_idx][red].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[mini_idx][red].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[flaco_idx][red].bitboard;
            idle_player_non_cabeza_occupancy |= state.pieces[chato_idx][red].bitboard;  

            win = state.pieces[cabeza_idx][blue].bitboard & idle_player_non_cabeza_occupancy;
            break;

        default:
            win = false;

        }
        
        return win;


    }

    void switch_team(game_state & state){
        state.turn = (state.turn == Player::red) ? Player::blue : Player::red;
    }


}