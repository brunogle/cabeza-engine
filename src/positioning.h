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
#define POSSIBLE_MOVEMENT_C_N    0x000001
#define POSSIBLE_MOVEMENT_C_NN   0x000002
#define POSSIBLE_MOVEMENT_C_NE   0x000004
#define POSSIBLE_MOVEMENT_C_NNE  0x000008
#define POSSIBLE_MOVEMENT_C_NNEE 0x000010
#define POSSIBLE_MOVEMENT_C_NEE  0x000020
#define POSSIBLE_MOVEMENT_C_E    0x000040
#define POSSIBLE_MOVEMENT_C_EE   0x000080
#define POSSIBLE_MOVEMENT_C_SE   0x000100
#define POSSIBLE_MOVEMENT_C_SEE  0x000200
#define POSSIBLE_MOVEMENT_C_SSEE 0x000400
#define POSSIBLE_MOVEMENT_C_ESS  0x000800
#define POSSIBLE_MOVEMENT_C_S    0x001000
#define POSSIBLE_MOVEMENT_C_SS   0x002000
#define POSSIBLE_MOVEMENT_C_SW   0x004000
#define POSSIBLE_MOVEMENT_C_SSW  0x008000
#define POSSIBLE_MOVEMENT_C_SSWW 0x010000
#define POSSIBLE_MOVEMENT_C_SWW  0x020000
#define POSSIBLE_MOVEMENT_C_W    0x040000
#define POSSIBLE_MOVEMENT_C_WW   0x080000
#define POSSIBLE_MOVEMENT_C_NW   0x100000
#define POSSIBLE_MOVEMENT_C_NWW  0x200000
#define POSSIBLE_MOVEMENT_C_NNWW 0x400000
#define POSSIBLE_MOVEMENT_C_NNW  0x800000

//Movement bits for mini
#define POSSIBLE_MOVEMENT_M_N  0x000001
#define POSSIBLE_MOVEMENT_M_NN 0x000002
#define POSSIBLE_MOVEMENT_M_NE 0x000004
#define POSSIBLE_MOVEMENT_M_E  0x000008
#define POSSIBLE_MOVEMENT_M_EE 0x000010
#define POSSIBLE_MOVEMENT_M_SE 0x000020
#define POSSIBLE_MOVEMENT_M_S  0x000040
#define POSSIBLE_MOVEMENT_M_SS 0x000080
#define POSSIBLE_MOVEMENT_M_SW 0x000100
#define POSSIBLE_MOVEMENT_M_W  0x000200
#define POSSIBLE_MOVEMENT_M_WW 0x000400
#define POSSIBLE_MOVEMENT_M_NW 0x000800

//Movement bts for flaco and chato
#define POSSIBLE_MOVEMENT_FH_N  0x000001
#define POSSIBLE_MOVEMENT_FH_NN 0x000002
#define POSSIBLE_MOVEMENT_FH_NW 0x000004
#define POSSIBLE_MOVEMENT_FH_NE 0x000008
#define POSSIBLE_MOVEMENT_FH_E  0x000010
#define POSSIBLE_MOVEMENT_FH_EE 0x000020
#define POSSIBLE_MOVEMENT_FH_EN 0x000040
#define POSSIBLE_MOVEMENT_FH_ES 0x000080
#define POSSIBLE_MOVEMENT_FH_S  0x000100
#define POSSIBLE_MOVEMENT_FH_SS 0x000200
#define POSSIBLE_MOVEMENT_FH_SE 0x000400
#define POSSIBLE_MOVEMENT_FH_SW 0x000800
#define POSSIBLE_MOVEMENT_FH_W  0x001000
#define POSSIBLE_MOVEMENT_FH_WW 0x002000
#define POSSIBLE_MOVEMENT_FH_WS 0x004000
#define POSSIBLE_MOVEMENT_FH_WN 0x008000

//Movement bits for gordo
#define POSSIBLE_MOVEMENT_G_N 0x000001
#define POSSIBLE_MOVEMENT_G_E 0x000002
#define POSSIBLE_MOVEMENT_G_S 0x000004
#define POSSIBLE_MOVEMENT_G_W 0x000008


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
    MSB (WNN, WWNN, WWN, WN, WW, W, SWW, SSWW, SSW, SW, SS, S, ESS, EESS, EES, ES, EE, E, NEE, NNEE, NNE, NE, NN, N) LSB
    These are all possible movements a CABEZA can make. A cabeza that is alone can move in all 24 directions.
    */

    typedef uint32_t possible_movements_c; //Used for Cabeza
    const std::string kCabezaMovementBitName[] = {"N", "NN", "NE", "NNE", "NNEE", "NEE", "E", "EE", "ES", "EES", "EESS", "ESS", "S", "SS", "SW", "SSW", "SSWW", "SWW", "W", "WW", "WN", "WWN", "WWNN", "WNN"};


    /*
    12 bit Variable type that can encode all possible movements for a MINI. (3 bit rotational symmetry)
    Encoded as:
    MSB (WN, WW, W, SW, SS, S, ES, EE, E, NE, NN, N) LSB
    These are all possible movements a MINI can make.
    */
    typedef uint32_t possible_movements_m; //Used for Mini
    const std::string kMiniMovementBitName[] = {"N", "NN", "NE", "E", "EE", "ES", "S", "SS", "SW", "W", "WW", "WN"};

    /*
    16 bit Variable type that can encode all possible movements for a FLACO. (4 bit rotational symmetry)
    Encoded as:
    MSB (WN, WS, WW, W, SW, SE, SS, S, ES, EN, EE, E, NE, NW, NN, N) LSB
    These are all possible movements a FLACO or CHATO can make. Distiction is made between NE, EN moves, etc.
    */
    typedef uint32_t possible_movements_fh; //Used for Flaco
    const std::string kFlacoChatoMovementBitName[] = {"N", "NN", "NW", "NE", "E", "EE", "EN", "ES", "S", "SS", "SE", "SW", "W", "WW", "WS", "WN"};

    /*
    4 bit Variable type that can encode all possible movements for a FLACO. (4 bit rotational symmetry)
    Encoded as:
    MSB (W, S, E, N) LSB
    These are all possible movements a FLACO or CHATO can make. Distiction is made between NE, EN moves, etc.
    */
    typedef uint32_t possible_movements_g; //Used for Flaco
    const std::string kGordoMovementBitName[] = {"N", "E", "S", "W"};

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

    struct possible_movements{
        possible_movements_c cabeza;
        possible_movements_m mini;
        possible_movements_fh flaco;
        possible_movements_fh chato;
        possible_movements_g gordo;
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

    possible_movements get_movements(game_state state);
    
    possible_movements_c get_cabeza_moves(game_state state);
    possible_movements_m get_mini_moves(game_state state);
    possible_movements_fh get_flaco_moves(game_state state);
    possible_movements_fh get_chato_moves(game_state state);
    possible_movements_g get_gordo_moves(game_state state);

    

}
#endif