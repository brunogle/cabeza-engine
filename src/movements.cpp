#include <iostream>
#include "movements.h"

/*
Returns starting game state
*/
game_state initial_game_state(){

	game_state state;
	state.red.cabeza = {5,0};
    state.red.mini = {4,0};
    state.red.flaco = {3,0,orient_vertical};
    state.red.chato = {6,0,orient_vertical};
    state.red.gordo = {4,1};

	state.blue.cabeza = {4,9};
    state.blue.mini = {5,9};
    state.blue.flaco = {6,8,orient_vertical};
    state.blue.chato = {3,8,orient_vertical};
    state.blue.gordo = {4,7};


	return state;
}