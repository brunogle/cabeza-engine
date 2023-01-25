#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include <cstdint>
#include <string>
#include <map>


#define OCCUPANCY_LOOKUP_O(x, y, o) (x << 6) | (y << 2) | o
#define OCCUPANCY_LOOKUP(x, y) (x << 6) | (y << 2)

#define PIECE_CABEZA_MASK 0b00000001
#define PIECE_MINI_MASK   0b00000010
#define PIECE_FLACO_MASK  0b00000100
#define PIECE_CHATO_MASK  0b00001000
#define PIECE_GORDO_MASK  0b00010000
#define PIECE_PLAYER_MASK 0b10000000

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
    typedef uint32_t possible_movements_t; //Used for Cabeza
    const std::string kMovementBitName[] = {"N","NN","NE","EN","NNE","NNEE","NEE","E","EE","ES","SE","SEE","SSEE","SSE","S","SS","SW","WS","SSW","SSWW","SWW","W","WW","WN","NW","NWW","NNWW","NNW"};


    /*
    Should only take one value of MOVEMENT_XXX (i.e. only ONE bit should be set)
    */
    typedef uint32_t movement_t;


    enum Orientation
    {
        flat = 0,
        horizontal = 1,
        vertical = 2
    };


    enum PieceIndex
    {
        red_cabeza_idx,
        red_mini_idx,
        red_flaco_idx,
        red_chato_idx,
        red_gordo_idx,
        blue_cabeza_idx,
        blue_mini_idx,
        blue_flaco_idx,
        blue_chato_idx,
        blue_gordo_idx
    };

    enum class Team
    {
        red = 0,
        blue = 5
    };
    

    struct piece
    {
        int x;
        int y;
        Orientation o;
        bitboard_t bitboard;
    };

    struct game_state
    {
        Team turn = Team::red;
        piece pieces[10];
    };

    struct all_possible_movements{
        possible_movements_t cabeza;
        possible_movements_t mini;
        possible_movements_t flaco;
        possible_movements_t chato;
        possible_movements_t gordo;
    };


    /*
    Drawings of zone masks arrays. 'P' Represents piece position. Non negative integers represent mask associated with that possition. '.' represents no mask.
    '\n' for defining new row. Strings must be a square of characters.
    Index represents rotation number.
    */

    const std::string zone_mask_drawing_cabeza[] = {"025136P47", "P10432765", "74P631520", "56723401P"};

    const std::string zone_mask_drawing_mini[] = {".0.123.P.", ".1.P20.3.", ".P.321.0.", ".3.02P.1."};

    const std::string zone_mask_drawing_flaco_flat[] = {".0..123.123..P..", ".11.P220.33.....", ".P..321.321..0..", ".33.022P.11....."};

    const std::string zone_mask_drawing_flaco_horizontal[] = {".00.1223.P......", "..1....1..P.200..3....3..", ".....P..3221.00.", ".3....3...02P...1....1..."};

    const std::string zone_mask_drawing_flaco_vertical[] = {"..0....0..11233.......P..", ".1...20.P20..3..", ".......P..33211..0....0..", ".3..02..02P..1.."};

    const std::string zone_mask_drawing_chato_flat[] = {"..00....00..112233........P.........", "..1.....1.....200..P200...3.....3...", ".......P....332211..00....00........", "..3.....3...002...002P....1.....1..."};

    const std::string zone_mask_drawing_chato_horizontal[] = {".00.12231223.P..", "..11....11..P.2200..33....33........", ".P..32213221.00.", "..33....33..0022P...11....11........"};

    const std::string zone_mask_drawing_chato_vertical[] = {"..0.....0...11233.11233.........P...", ".11..220P220.33.", "........P...33211.33211...0.....0...", ".33.022.022P.11."};

    const std::string zone_mask_drawing_gordo[] = {"00..00......P...", "..........00P.00", "....P...00..00..", "........00..00P."};

    /*
    Generates bitboard zone mask from string drawing for a given position, rotation and zone number
    */
    bitboard_t generate_zone_mask_from_string(std::string mask_drawing, int pos, int rot, int zone);

    /*
    Initializes lookup arrays
    */
    int init();


    /*
    Returns initial game state
    */
    game_state initial_game_state();

    /*
    Returns a randomly generated game state. Can be slow.
    */
    game_state random_game_state();

    /*
    Generates the corresponding bitboard for a specific piece
    */
    bitboard_t generate_bitboard_for_piece(int piece_type, Orientation o, int x, int y);

    /*
    Get occupancy bitboard of current game
    */
    bitboard_t get_occupancy(game_state state, bool opponent_cabeza_free = false);

    /*
    Calculates leading zeroes in a bitboard.
    Used to determine the lower left coordinate of a piece from the bitboard.
    */
    inline int get_pos_from_bitboard (bitboard_t u) {
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
    Finds all movements for a given game state
    */
    all_possible_movements get_movements(game_state state);

    
    movement_t parse_movement_str(std::string str);

    /*
    Applies a movement_t to a cabeza piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_cabeza(piece p, movement_t movement);

    /*
    Applies a movement_t to a mini piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_mini(piece p, movement_t movement);

    /*
    Applies a movement_t to a flaco piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_flaco(piece p, movement_t movement);
    
    /*
    Applies a movement_t to a flaco piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */
    piece apply_move_chato(piece p, movement_t movement);

    /*
    Applies a movement_t to a gordo piece.
    This function does NOT check if movement_t is valid, or if only one bit is set.
    */ 
    piece apply_move_gordo(piece p, movement_t movement);
}

#endif