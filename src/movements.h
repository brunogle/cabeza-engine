#ifndef MOVEMENTS_H
#define MOVEMENTS_H

#include  <cstdint>

struct simple_coord{
	uint8_t x;
	uint8_t y;
};

struct double_coord{
	uint8_t x1;
	uint8_t y1;
	uint8_t x2;
	uint8_t y2;
};

struct player_state{
	simple_coord cabeza;
	simple_coord mini;
	double_coord flaco;
	double_coord chato;
	double_coord gordo;
};

struct game_state{
	player_state red;
	player_state blue;
};


game_state initial_game_state();

#endif