#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include <cstdint>
#include <string>
#include <map>


#define OCCUPANCY_LOOKUP_O(x, y, o) (x << 6) | (y << 2) | o
#define OCCUPANCY_LOOKUP(x, y) (x << 6) | (y << 2)

#define MAX_POSSIBLE_MOVEMENTS 72

//Movement bits
#define MOVEMENT_N      0x0000001
#define MOVEMENT_NN     0x0000002
#define MOVEMENT_NE     0x0000004
#define MOVEMENT_EN     0x0000008
#define MOVEMENT_NNE    0x0000010
#define MOVEMENT_NNEE   0x0000020
#define MOVEMENT_NEE    0x0000040
#define MOVEMENT_E      0x0000080
#define MOVEMENT_EE     0x0000100
#define MOVEMENT_ES     0x0000200
#define MOVEMENT_SE     0x0000400
#define MOVEMENT_SEE    0x0000800
#define MOVEMENT_SSEE   0x0001000
#define MOVEMENT_SSE    0x0002000
#define MOVEMENT_S      0x0004000
#define MOVEMENT_SS     0x0008000
#define MOVEMENT_SW     0x0010000
#define MOVEMENT_WS     0x0020000
#define MOVEMENT_SSW    0x0040000
#define MOVEMENT_SSWW   0x0080000
#define MOVEMENT_SWW    0x0100000
#define MOVEMENT_W      0x0200000
#define MOVEMENT_WW     0x0400000
#define MOVEMENT_WN     0x0800000
#define MOVEMENT_NW     0x1000000
#define MOVEMENT_NWW    0x2000000
#define MOVEMENT_NNWW   0x4000000
#define MOVEMENT_NNW    0x8000000


namespace positioning{

    /*
    Variable type for storing a bitboard. Only first 100 bits are used. LSB is (0,0), next bit is (1,0)
    ... bit 100 is (9,9)
    */
    typedef __uint128_t bitboard_t;

    /*
    28 bit Variable type that can encode all possible movements any piece can make  (7 bit rotational symmetry)
    */
    typedef uint32_t possible_moves_t; //Used for Cabeza
    const std::string kMovementBitName[] = {"n","nn","ne","en","nne","nnee","nee","e","ee","es","se","see","ssee","sse","s","ss","sw","ws","ssw","ssww","sww","w","ww","wn","nw","nww","nnww","nnw"};

    /*
    Specifies a specific movement. Should only take one value of MOVEMENT_XXX (i.e. only ONE bit should be set)
    */
    typedef uint32_t move_bitarray_t;


    enum Orientation
    {
        flat = 0,
        horizontal = 1,
        vertical = 2
    };

    enum PieceType
    {
        cabeza_idx,
        mini_idx,
        flaco_idx,
        chato_idx,
        gordo_idx,
    };


    enum Player
    {
        red = 0,
        blue = 1
    };
    

    struct piece
    {
        Orientation o;
        bitboard_t bitboard;
    };

    struct game_state
    {
        Player turn = Player::red;
        piece pieces[5][2];
    };

    struct all_possible_moves_t{
        possible_moves_t cabeza;
        possible_moves_t mini;
        possible_moves_t flaco;
        possible_moves_t chato;
        possible_moves_t gordo;
        Player player;
    };

    struct move{
        move_bitarray_t move;
        PieceType piece;
        Player player;
    };

    /*
    Drawings of zone masks arrays. 'P' Represents piece position. Non negative integers represent mask associated with that possition. '.' represents no mask.
    Strings must be a square of characters.
    Index represents rotation number.
    */


    const std::string zone_mask_drawing_cabeza[] = {"025136P47", "P10432765", "74P631520", "56723401P"};

    const std::string zone_mask_drawing_mini[] = {".0.123.P.", ".1.P20.3.", ".P.321.0.", ".3.02P.1."};

    const std::string zone_mask_drawing_flaco_flat[] = {".0..123.123..P..", ".11.P220.33.....", ".P..321.321..0..", ".33.022P.11....."};

    const std::string zone_mask_drawing_flaco_horizontal[] = {".00.1223.P......", "..1....1..P.200..3....3..", ".....P..3221.00.", "..3....3..002P...1....1.."};

    const std::string zone_mask_drawing_flaco_vertical[] = {"..0....0..11233.......P..", ".1...20.P20..3..", ".......P..33211..0....0..", ".3..02..02P..1.."};

    const std::string zone_mask_drawing_chato_flat[] = {"..00....00..112233........P.........", "..1.....1.....200.P.200...3.....3...", "........P...332211..00....00........", "..3.....3...002...002P....1.....1..."};

    const std::string zone_mask_drawing_chato_horizontal[] = {".00.12231223.P..", "..11....11..P.2200..33....33........", ".P..32213221.00.", "..33....33..0022P...11....11........"};

    const std::string zone_mask_drawing_chato_vertical[] = {"..0.....0...11233.11233.........P...", ".11..220P220.33.", "........P...33211.33211...0.....0...", ".33.022.022P.11."};

    const std::string zone_mask_drawing_gordo[] = {"00..00......P...", "..........00P.00", "....P...00..00..", "........00..00P."};



    /////////////////////////////////////////
    //                                     //
    //       PREDEFINED GAME STATES        //
    //                                     //
    /////////////////////////////////////////

    /*
    Returns starting game state
    */
    game_state initial_game_state();


    /*
    Generates bitboard zone mask from string drawing for a given position, rotation and zone number
    */
    bitboard_t generate_bitboard_for_piece(PieceType, Orientation, int, int);


    /////////////////////////////////////////
    //                                     //
    //     MOVEGEN LOOKUP ARRAYS           //
    //                                     //
    /////////////////////////////////////////

    /*
    Initializes lookup arrays
    */
    int init();

    /*
    Converts one of the zone_mask_drawings to a bitboard mask for a specific position, rotation and zone.
    Zone mask drawings are specified for squares of sides n, as n^2 sized strings, starting from upper left to bottom right.
    A P in a string represents the bottom left square of the corresponding piece. Numbers represent the mask number. Periods
    represent no mask assigned for that square.
    */
    bitboard_t generate_zone_mask_from_string(std::string, int, int);

    /*
    Initialize zone masks used for movement generation.
    Uses generate_zone_mask_from_string() to generate masks from zone_mask_drawaing_xxx strings.
    */
    int init_zone_masks();

    /*
    Initialize movement_lookup arrays used for move generation
    */
    int init_movement_lookups();


    /////////////////////////////////////////
    //                                     //
    //     MOVE GENERATION FUNCTIONS       //
    //                                     //
    /////////////////////////////////////////


    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_cabeza_moves(bitboard_t, int);

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_mini_moves(bitboard_t, int);
    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_flaco_moves(bitboard_t, int, Orientation);
    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_chato_moves(bitboard_t, int, Orientation);

    /*
    Receives occupancy and piece position.
    Returns possible_moves_t, bit-coded possible moves the piece can move.
    */
    possible_moves_t get_gordo_moves(bitboard_t, int);

    /*
    Finds all movements for a given game state
    */
    all_possible_moves_t get_moves(game_state);

    /*
    List all possible movements as an array of moves for use in seach. Returns number of possible moves.
    */
    int get_moves_as_list(game_state, move[MAX_POSSIBLE_MOVEMENTS]);


    /////////////////////////////////////////
    //                                     //
    //     MOVE APPLICATION FUNCTIONS      //
    //                                     //
    /////////////////////////////////////////

    /*
    Applies a movement to a cabeza piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_cabeza(piece, move_bitarray_t);
    /*
    Applies a movement to a mini piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_mini(piece, move_bitarray_t);
    /*
    Applies a movement to a flaco piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_flaco(piece, move_bitarray_t);
    /*
    Applies a movement to a chato piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */
    piece apply_move_chato(piece, move_bitarray_t);
    /*
    Applies a movement to a gordo piece.
    This function does NOT check if movement is valid, or if only one bit is set.
    */ 
    piece apply_move_gordo(piece, move_bitarray_t);
    /*
    Applies a movement to a board. Does not check if piece is valid
    */
    game_state apply_move(game_state, move);
    /*
    Checks if movement is valid
    */
    bool check_move_valid(game_state, move);
    /*
    Applies a movement to a board. Checks if move succeeds
    */
    bool apply_move_safe(move move, game_state state_in, game_state & state_out);


    /////////////////////////////////////////
    //                                     //
    //          GAME FUNCTIONS             //
    //                                     //
    /////////////////////////////////////////


    /*
    Checks if a player won in last semimove.
    Only returns true if the player that has the win condition hast played last semimove (i.e. the player whoes turn is not)
    */
    bool check_for_win(positioning::game_state state);

    void switch_team(game_state &);

    /*
    Calculates leading zeroes in a bitboard.
    Used to determine the lower left coordinate of a piece from the bitboard.
    */
    inline int get_pos_from_bitboard (bitboard_t u) {
        uint64_t hi = u>>64;
        uint64_t lo = u;

        if(hi && !lo){
            return __builtin_ctzll(hi)+64;
        }
        else{
            return __builtin_ctzll(lo);
        }
    }


}

#endif