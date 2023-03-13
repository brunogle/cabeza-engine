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


#include "performance.h"
#include "positioning.h"
#include "eval.h"
#include "search.h"
#include "game.h"


int main(){
	positioning::init();//Initialize positioning functions (fixed lookup tables)

	AllSearch search_engine(DistanceEval::eval, 30000, 20);//Create search instance
 
	Game game(DistanceEval::eval, &search_engine);

	std::string input_str;


	while(true){
		

		input_str = readline("> ");
		add_history(input_str.c_str());

		std::stringstream input_ss(input_str);


		std::string command_token;


		getline(input_ss, command_token, ' ');


		if(command_token == "undo" || command_token == "u"){
			game.undo_moves(1);	
		}
		else if(command_token == "search" || command_token == "s"){
			game.search();
		}
		else if(command_token == "play" || command_token == "p"){
			positioning::move best_move = game.search();
			game.apply_move(best_move);
		}
		else if(command_token == "pgn"){
			std::string pgn_token;

			getline(input_ss, pgn_token, ' ');
			
			if(pgn_token == ""){
				continue;
			}
			
			game.apply_pgn(pgn_token);

		}
		else if(command_token == "fen"){
			std::string fen_token;

			getline(input_ss, fen_token, ' ');
			
			if(fen_token == ""){
				continue;
			}
			
			game.set_fen(fen_token);

		}
		else if(command_token == "reset"){
			game.reset();
		}
		else if(command_token == "rand"){
			game.current_game_state = positioning::random_game_state();
			game.update_graphics();
		}
		else if(command_token == "time"){
			std::string time_token;
			getline(input_ss, time_token, ' ');

			int timeout = std::stoi(time_token, nullptr);

			search_engine.set_timeout(timeout);
		}
		else if(command_token == "draw"){
			game.get_game_drawing();
		}
		else if(command_token == "quit"){
			return 0;
		}
		else{
			game.apply_move_str(command_token);
		}
	}



	std::cout << "Game PGN string:" << std::endl;
	std::cout << game.get_game_pgn() << std::endl;

	std::cin.ignore();
	std::cin.get();

	return 0;

}