#include <stdio.h>
#include <fcntl.h>
#include <iostream>
#include <bitset>
#include <chrono>
#include <time.h>
#include <sstream>
#include <string>
#include <readline/readline.h>
#include <readline/history.h>


#include "parsing.h"
#include "positioning.h"
#include "eval.h"
#include "search.h"
#include "game.h"
#include "cui.h"
/*
int main(){
	positioning::init();
	AllSearch search_engine(DistanceEval::eval, 10000, 10);
	Game game(DistanceEval::eval, &search_engine);
	game.search();
	std::cin.get();
	return 0;
}
*/



int main(){
	positioning::init();//Initialize positioning functions (fixed lookup tables)

	Cui cui;

	cui.start();

	return 0;

}

