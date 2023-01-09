#include <iostream>
#include "movements.h"

/*
Returns starting game state
*/
game_state initial_game_state(){

	game_state ret;
	ret.red.cabeza.x = 5;
	ret.red.cabeza.y = 8;

	ret.red.mini.x = 4;
	ret.red.mini.y = 8;

	ret.red.flaco.x1 = 3;
	ret.red.flaco.y1 = 7;
	ret.red.flaco.x2 = 3;
	ret.red.flaco.y2 = 8;

	ret.red.chato.x1 = 6;
	ret.red.chato.y1 = 8;
	ret.red.chato.x2 = 6;
	ret.red.chato.y2 = 7;

	ret.red.gordo.x1 = 4;
	ret.red.gordo.y1 = 7;
	ret.red.gordo.x2 = 5;
	ret.red.gordo.y2 = 6;

	ret.blue.cabeza.x = 5;
	ret.blue.cabeza.y = 1;

	ret.blue.mini.x = 4;
	ret.blue.mini.y = 1;

	ret.blue.flaco.x1 = 3;
	ret.blue.flaco.y1 = 1;
	ret.blue.flaco.x2 = 3;
	ret.blue.flaco.y2 = 2;

	ret.blue.chato.x1 = 6;
	ret.blue.chato.y1 = 1;
	ret.blue.chato.x2 = 6;
	ret.blue.chato.y2 = 2;

	ret.blue.gordo.x1 = 4;
	ret.blue.gordo.y1 = 2;
	ret.blue.gordo.x2 = 5;
	ret.blue.gordo.y2 = 3;
	return ret;
}