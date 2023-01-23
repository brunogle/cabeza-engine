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


//Movement bits for cabeza
#define POSSIBLE_MOVEMENT_C_U    0x000001
#define POSSIBLE_MOVEMENT_C_UU   0x000002
#define POSSIBLE_MOVEMENT_C_UR   0x000004
#define POSSIBLE_MOVEMENT_C_UUR  0x000008
#define POSSIBLE_MOVEMENT_C_UURR 0x000010
#define POSSIBLE_MOVEMENT_C_URR  0x000020
#define POSSIBLE_MOVEMENT_C_R    0x000040
#define POSSIBLE_MOVEMENT_C_RR   0x000080
#define POSSIBLE_MOVEMENT_C_RD   0x000100
#define POSSIBLE_MOVEMENT_C_RRD  0x000200
#define POSSIBLE_MOVEMENT_C_RRDD 0x000400
#define POSSIBLE_MOVEMENT_C_RDD  0x000800
#define POSSIBLE_MOVEMENT_C_D    0x001000
#define POSSIBLE_MOVEMENT_C_DD   0x002000
#define POSSIBLE_MOVEMENT_C_DL   0x004000
#define POSSIBLE_MOVEMENT_C_DDL  0x008000
#define POSSIBLE_MOVEMENT_C_DDLL 0x010000
#define POSSIBLE_MOVEMENT_C_DLL  0x020000
#define POSSIBLE_MOVEMENT_C_L    0x040000
#define POSSIBLE_MOVEMENT_C_LL   0x080000
#define POSSIBLE_MOVEMENT_C_LU   0x100000
#define POSSIBLE_MOVEMENT_C_LLU  0x200000
#define POSSIBLE_MOVEMENT_C_LLUU 0x400000
#define POSSIBLE_MOVEMENT_C_LUU  0x800000

//Movement bits for mini
#define POSSIBLE_MOVEMENT_M_U  0x000001
#define POSSIBLE_MOVEMENT_M_UU 0x000002
#define POSSIBLE_MOVEMENT_M_UR 0x000004
#define POSSIBLE_MOVEMENT_M_R  0x000008
#define POSSIBLE_MOVEMENT_M_RR 0x000010
#define POSSIBLE_MOVEMENT_M_RD 0x000020
#define POSSIBLE_MOVEMENT_M_D  0x000040
#define POSSIBLE_MOVEMENT_M_DD 0x000080
#define POSSIBLE_MOVEMENT_M_DL 0x000100
#define POSSIBLE_MOVEMENT_M_L  0x000200
#define POSSIBLE_MOVEMENT_M_LL 0x000400
#define POSSIBLE_MOVEMENT_M_LU 0x000800

//Movement bts for flaco and chato
#define POSSIBLE_MOVEMENT_FH_U  0x000001
#define POSSIBLE_MOVEMENT_FH_UU 0x000002
#define POSSIBLE_MOVEMENT_FH_UL 0x000004
#define POSSIBLE_MOVEMENT_FH_UR 0x000008
#define POSSIBLE_MOVEMENT_FH_R  0x000010
#define POSSIBLE_MOVEMENT_FH_RR 0x000020
#define POSSIBLE_MOVEMENT_FH_RU 0x000040
#define POSSIBLE_MOVEMENT_FH_RD 0x000080
#define POSSIBLE_MOVEMENT_FH_D  0x000100
#define POSSIBLE_MOVEMENT_FH_DD 0x000200
#define POSSIBLE_MOVEMENT_FH_DR 0x000400
#define POSSIBLE_MOVEMENT_FH_DL 0x000800
#define POSSIBLE_MOVEMENT_FH_L  0x001000
#define POSSIBLE_MOVEMENT_FH_LL 0x002000
#define POSSIBLE_MOVEMENT_FH_LD 0x004000
#define POSSIBLE_MOVEMENT_FH_LU 0x008000

namespace positioning{

    /*
    Variable type for storing a bitboard. Only first 100 bits are used. LSB is (0,0), next bit is (1,0)
    ... bit 100 is (9,9)
    */
    typedef __uint128_t bitboard_t;

    //TODO: TRY TO UNIFY MOVEMENT TPYES


    /*
    24 bit Variable type that can encode all possible movements for a CABEZA. (6 bit rotational symmetry)
    Encoded as:
    MSB (LUU, LLUU, LLU, LU, LL, L, DLL, DDLL, DDL, DL, DD, D, RDD, RRDD, RRD, RD, RR, R, URR, UURR, UUR, UR, UU, U) LSB
    These are all possible movements a CABEZA can make. A cabeza that is alone can move in all 24 directions.
    */

    typedef uint32_t possible_movements_cabeza; //Used for Cabeza
    const std::string kCabezaMovementBitName[] = {"U", "UU", "UR", "UUR", "UURR", "URR", "R", "RR", "RD", "RRD", "RRDD", "RDD", "D", "DD", "DL", "DDL", "DDLL", "DLL", "L", "LL", "LU", "LLU", "LLUU", "LUU"};


    /*
    12 bit Variable type that can encode all possible movements for a MINI. (3 bit rotational symmetry)
    Encoded as:
    MSB (LU, LL, L, DL, DD, D, RD, RR, R, UR, UU, U) LSB
    These are all possible movements a MINI can make.
    */
    typedef uint32_t possible_movements_mini; //Used for Mini
    const std::string kMiniMovementBitName[] = {"U", "UU", "UR", "R", "RR", "RD", "D", "DD", "DL", "L", "LL", "LU"};

    /*
    16 bit Variable type that can encode all possible movements for a FLACO. (4 bit rotational symmetry)
    Encoded as:
    MSB (LU, LD, LL, L, DL, DR, DD, D, RD, RU, RR, R, UR, UL, UU, U) LSB
    These are all possible movements a FLACO or CHATO can make. Distiction is made between UR, RU moves, etc.
    */
    typedef uint32_t possible_movements_flaco; //Used for Flaco
    const std::string kFlacoMovementBitName[] = {"U", "UU", "UL", "UR", "R", "RR", "RU", "RD", "D", "DD", "DR", "DL", "L", "LL", "LD", "LU"};

    


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


    /*
    Drawings of zone masks arrays. 'P' Represents piece position. Non negative integers represent mask associated with that possition. '.' represents no mask.
    '\n' for defining new row. Strings must be a square of characters.
    Index represents rotation number.
    */

    const std::string zone_mask_drawing_cabeza[] = {"025136P47", "P10432765", "74P631520", "56723401P"};

    const std::string zone_mask_drawing_mini[] = {".0.123.P.", ".1.P20.3.", ".P.321.0.", ".3.02P.1."};

    const std::string zone_mask_drawing_flaco_flat[] = {".0..123.123..P..", ".11.P220.33.....", ".P..321.321..0..", ".33.022P.11....."};

    /*
    Generates bitboard zone mask from string drawing for a given position, rotation and zone number
    */
    bitboard_t generate_zone_mask_from_string(std::string mask_drawing, int pos, int rot, int zone);

    /*
    Calculates and writes zone masks arrays. Must be called before using get_movments()
    */
    int generate_zone_masks();

    /*
    Calculates and writes movement lookup arrays. Must be called before using get_movements()
    */
    int generate_movement_lookups();

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
    
    possible_movements_cabeza get_cabeza_moves(game_state state, PieceIndex piece_idx);
    possible_movements_mini get_mini_moves(game_state state, PieceIndex piece_idx);
    possible_movements_flaco get_flaco_flat_moves(game_state state, PieceIndex piece_idx);
}
#endif