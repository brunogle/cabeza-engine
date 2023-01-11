#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include  <cstdint>


/*
Movements are represented with 8 bit words. Odd bits for first step, even bits for second step

2 step movements can be represented with bitwise OR operator, ej: UP_1 | RIGHT_2

Use (x >> 2) | (x << 6) to perform 90 degree clockwise symetries for one step rotation.

*/

#define UP_1     0b10000000
#define RIGHT_1  0b00100000
#define DOWN_1   0b00001000
#define LEFT_1   0b00000010
#define UP_2     0b01000000
#define RIGHT_2  0b00010000
#define DOWN_2   0b00000100
#define LEFT_2   0b00000001

enum orientation {orient_flat, orient_vertical, orient_horizontal};

/*
Represents cabeza position
*/
struct cabeza{
    int x;
    int y;
};

/*
Represents mini position
*/
struct mini{
    int x;
    int y;
};

/*
Represents flaco position as lower left coordinate if horizontal or vertical, or only position if flat
*/
struct flaco{
    int x;
    int y;
    orientation o;
};

/*
Represents chato position as lower left coordinate if horizontal or vertical, or only position if flat
*/
struct chato{
    int x;
    int y;
    orientation o;
};

/*
Represents gordo position
*/
struct gordo{
    int x;
    int y;
};

struct player_state{
    cabeza cabeza;
    mini mini;
    flaco flaco;
    chato chato;
    gordo gordo;
};

struct game_state{
    player_state red;
    player_state blue;
};

game_state initial_game_state();

#endif