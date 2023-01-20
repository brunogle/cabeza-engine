#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include <cstdint>
#include <string>

#define OCCUPANCY_LOOKUP_O(x, y, o) (x << 6) | (y << 2) | o
#define OCCUPANCY_LOOKUP(x, y) (x << 6) | (y << 2)

#define PIECE_CABEZA_MASK 0b00000001
#define PIECE_MINI_MASK   0b00000010
#define PIECE_FLACO_MASK  0b00000100
#define PIECE_CHATO_MASK  0b00001000
#define PIECE_GORDO_MASK  0b00010000
#define PIECE_PLAYER_MASK 0b10000000

#define POSSIBLE_MOVEMENT_U    0x000001
#define POSSIBLE_MOVEMENT_UU   0x000002
#define POSSIBLE_MOVEMENT_UR   0x000004
#define POSSIBLE_MOVEMENT_UUR  0x000008
#define POSSIBLE_MOVEMENT_UURR 0x000010
#define POSSIBLE_MOVEMENT_URR  0x000020
#define POSSIBLE_MOVEMENT_R    0x000040
#define POSSIBLE_MOVEMENT_RR   0x000080

namespace positioning{

    /*
    Variable type for storing a bitboard. Only first 100 bits are used. LSB is (0,0), next bit is (1,0)
    ... bit 100 is (9,9)
    */
    typedef __uint128_t bitboard_t;

    /*
    Variable type that can encode all possible movements a piece can perform.
    Encoded as:
         1      0    0    0   1  1   1   1      0    0   0000000000000000
    MSB (LUU, LLUU, LLU, LU, LL, L, DLL, DDLL, DDL, DL, DD, D, RDD, RRDD, RRD, RD, RR, R, URR, UURR, UUR, UR, UU, U) LSB
    This are all possible movements a piece can make. A cabeza that is alone can move in all 24 directions.
    */
    typedef uint32_t possible_movements;

    namespace{
        enum PieceType
        {
            red_cabeza  = 0b00000001,
            red_mini    = 0b00000010,
            red_flaco   = 0b00000100,
            red_chato   = 0b00001000,
            red_gordo   = 0b00010000,
            blue_cabeza = 0b10000001,
            blue_mini   = 0b10000010,
            blue_flaco  = 0b10000100,
            blue_chato  = 0b10001000,
            blue_gordo  = 0b10010000
        };
    }

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
        red,
        blue
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


    const PieceType kPieceType[] = {PieceType::red_cabeza, PieceType::red_mini, PieceType::red_flaco, PieceType::red_chato, PieceType::red_gordo, 
                                PieceType::blue_cabeza, PieceType::blue_mini, PieceType::blue_flaco, PieceType::blue_chato, PieceType::blue_gordo};


    const std::string kMovementBitName[] = {"U", "UU", "UR", "UUR", "UURR", "URR", "R", "RR", "RD", "RRD", "RRDD", "RDD", "D", "DD", "DL", "DDL", "DDLL", "DLL", "L", "LL", "LU", "LLU", "LLUU", "LUU"};


    /*
    Masks used to analyze each "zone" of each piece.
    First index represents position as given by the lower left occupied square by the piece.
    Second index reprensents the 4 symetties of the piece. Starting from (up-right), (right-down), (down-left) (left-up)
    Third index represents the zone # being analyzed
    */
    extern bitboard_t zone_mask_cabeza[100][4][8];

    //extern bitboard_t zone_mask_mini[100][4][5];

    //extern bitboard_t zone_mask_flaco_f[100][4][7];
    //extern bitboard_t zone_mask_flaco_v[100][4][6];
    //extern bitboard_t zone_mask_flaco_h[100][4][6];

    //extern bitboard_t zone_mask_chato_f[100][4][7];
    //extern bitboard_t zone_mask_chato_v[100][4][6];
    //extern bitboard_t zone_mask_chato_h[100][4][6];

    //extern bitboard_t zone_mask_gordo[100][4][1];
    
    extern possible_movements movement_lookup_cabeza[256]; 

    /*
    Calculates and writes zone masks arrays. Must be called before using get_movments()
    */
    int generate_zone_masks();

    /*
    Calculates and writes movement lookup arrays. Must be called before using get_movements()
    */
    int generate_movement_lookup();

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
    bitboard_t generate_bitboard_for_piece(PieceType piece_type, Orientation o, int x, int y);


    /*
    Fills a game state with bitboards using coordinate and orientation data
    */
    int generate_bitboards(game_state *state);

    /*
    Get occupancy bitboard of current game
    */
    bitboard_t get_occupancy(game_state state, bool opponent_cabeza_free = false);

    uint32_t get_movements(game_state state);
    
    possible_movements get_cabeza_moves(game_state state, PieceIndex piece_idx);


}
#endif