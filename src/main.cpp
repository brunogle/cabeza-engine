#include <stdio.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <iostream>
#include <bitset>
#include <chrono>
#include <time.h>
#include <sstream>

#include "performance.h"
#include "positioning.h"
#include "eval.h"
#include "search.h"
#include "game.h"

//   6F3/10/8ff/2hhoo1mc1/2hhoo1HH1/4OO1HH1/4OO1M2/7C2/10/10,r,0


int main(){
	//srand(time(NULL));

	positioning::init();

	AllSearch search_engine(DistanceEval::eval, 60000, 10);

	Game game(DistanceEval::eval, &search_engine);
	
	game.create_graphics_window();

	//game.apply_pgn("hNN,hSS,oN,cSSW,fNW,oS,fNN,fES,fNN,cEE,fNE,cNEE,fEN,mES,fNE,cSSEE,hE,fE,cNNEE,hNN,fWS,mSS,fNE,hES,fS,mE,mNE,hWS,mNE,hS,mNE,cW,mEE");

	

	std::string input_str;

	Player winner = game.get_winner();

	while(winner == Player::none){
		
		Player turn = game.get_turn();

		std::cout << "> ";
		std::getline(std::cin, input_str);

		std::stringstream input_ss(input_str);


		std::string command_token;
		getline(input_ss, command_token, ' ');


		if(command_token == "undo" || command_token == "u"){
			game.undo_moves(1);	
		}
		else if(command_token == "search" || command_token == "s"){
			positioning::move best_move = game.search();
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
		else{
			game.apply_move_str(command_token);
		}

		winner = game.get_winner();
	}

	if(winner == Player::red){
		std::cout << "Winner: Red" << std::endl;
	}
	else if(winner == Player::blue){
		std::cout << "Winner: Blue" << std::endl;
	}

	std::cout << "Game PGN string:" << std::endl;
	std::cout << game.get_game_pgn() << std::endl;

	std::cin.ignore();
	std::cin.get();

	return 0;

}


// int main(){

// 	Game game;
// 	game.set_evaluator_function(DistanceEval::eval);
// 	game.set_search_function(RandomMoveSearch::search);


// 	getchar();

// 	positioning::init();

// 	positioning::game_state states[100];

// 	for(int i = 0; i < 100; i++)
// 		states[i] = positioning::random_game_state();
	

// 	positioning::game_state initstate = positioning::initial_game_state();

//     using std::chrono::high_resolution_clock;
//     using std::chrono::duration_cast;
//     using std::chrono::duration;
//     using std::chrono::milliseconds;

// 	auto t1 = high_resolution_clock::now();

// 	positioning::move arr[100];

// 	for(int i = 0; i < 10000000; i++)
// 		//RandomMoveSearch::search(initstate);
// 		game.search();
//     auto t2 = high_resolution_clock::now();
//     duration<double, std::milli> ms_double = t2 - t1;
//     std::cout << ms_double.count() << "ms\n";
// 	getchar();
// }


// int main()
// {

// 	positioning::init();
	
// 	graphics::init_graphics_console();


// 	while (1)
// 	{


// 		positioning::game_state state = positioning::random_game_state();

// 		graphics::draw_on_console(state);

// 		positioning::all_possible_moves_t moves = positioning::get_moves(state);

// 		if(state.turn == positioning::Team::red){
// 			std::cout << "Turn: Red" << std::endl;
// 		}
// 		else{
// 			std::cout << "Turn: Blue" << std::endl;
// 		}		
// 		std::cout << "Cabeza: ";
// 		for (int i = 0; i < 28; i++){
// 			if (moves.cabeza & (1 << i))
// 				std::cout << positioning::kMovementBitName[i] << " ";
// 		}
// 		std::cout << std::endl;

// 		std::cout << "Mini: ";
// 		for (int i = 0; i < 28; i++){
// 			if (moves.mini & (1 << i))
// 				std::cout << positioning::kMovementBitName[i] << " ";
// 		}
// 		std::cout << std::endl;


// 		std::cout << "Flaco: ";
// 		for (int i = 0; i < 28; i++){
// 			if (moves.flaco & (1 << i))
// 				std::cout << positioning::kMovementBitName[i] << " ";
// 		}
// 		std::cout << std::endl;

// 		std::cout << "Chato: ";
// 		for (int i = 0; i < 28; i++){
// 			if (moves.chato & (1 << i))
// 				std::cout << positioning::kMovementBitName[i] << " ";
// 		}
// 		std::cout << std::endl;

// 		std::cout << "Gordo: ";
// 		for (int i = 0; i < 28; i++){
// 			if (moves.gordo & (1 << i))
// 				std::cout << positioning::kMovementBitName[i] << " ";
// 		}
// 		std::cout << std::endl;
// 		std::cout << std::endl;



// 		getchar();
// 	}

// 	return 0;
// }



// int main(){

// 	getchar();

// 	positioning::init();

// 	positioning::game_state states[100];

// 	for(int i = 0; i < 100; i++)
// 		states[i] = positioning::random_game_state();

//     using std::chrono::high_resolution_clock;
//     using std::chrono::duration_cast;
//     using std::chrono::duration;
//     using std::chrono::milliseconds;

// 	auto t1 = high_resolution_clock::now();

// 	for(int i = 0; i < 10000000; i++)
// 		positioning::get_moves(states[i%100]);
			
//     auto t2 = high_resolution_clock::now();
//     duration<double, std::milli> ms_double = t2 - t1;
//     std::cout << ms_double.count() << "ms\n";
// 	getchar();
// }


/*
int main(){
	generate_zone_mask_cabeza();

	bitboard_t mask = zone_mask_cabeza[28][1][5];

	for(int y = 9; y >= 0; y--){
		for(int x = 0; x <= 9; x++){
			int bit = (mask >> (x + y*10))&1;
			if(bit){
				std::cout << 'x';
			}
			else{
				std::cout << '.';
			}
		}
		std::cout << std::endl;
	}

	getchar();

}

*/

/*
int main(){
	positioning::generate_movement_lookup_cabeza();

	char occupancy[3][3];

	for(int addr = 0x00; addr <= 0xFF; addr++){
		positioning::possible_moves_t moves = positioning::movement_lookup_c[addr];

		occupancy[0][2] = addr&0x01 ? 'x' : '.';
		occupancy[0][1] = addr&0x02 ? 'x' : '.';
		occupancy[1][2] = addr&0x04 ? 'x' : '.';
		occupancy[1][1] = addr&0x08 ? 'x' : '.';
		occupancy[1][0] = addr&0x10 ? 'x' : '.';
		occupancy[2][2] = addr&0x20 ? 'x' : '.';
		occupancy[2][1] = addr&0x40 ? 'x' : '.';
		occupancy[2][0] = addr&0x80 ? 'x' : '.';
		occupancy[0][0] = 'O';

		for(int y = 2; y >= 0; y--){
			for(int x = 0; x <= 2; x++){
				std::cout << occupancy[x][y];
			}
			std::cout << std::endl;
		}
		std::cout << "Moves: ";

		std::cout << ((moves & POSSIBLE_MOVEMENT_U) ? "- U " : "    ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UR) ? "- UR " : "     ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_R) ? "- R " : "    ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UU) ? "- UU " : "     ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UUR) ? "- UUR " : "      ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_UURR) ? "- UURR " : "       ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_URR) ? "- URR " : "      ");
		std::cout << ((moves & POSSIBLE_MOVEMENT_RR) ? "- RR " : "     ");


		std::cout << "------" << std::endl;
		getchar();

	}
}
*/

/*

	for(int i = 0; i < 10; i++){
	__int128_t occupancy = get_occupancy(state);

	for(int y = 0; y < 10; y++){
		for(int x = 0; x < 10; x++){
			if(occupancy&((__int128_t)1 << (y*10 + x))){
				std::cout << 'x';
			}
			else{
				std::cout << '.';
			}
		}
		std::cout << std::endl;
	}
	std::cout << "---------------------" << std::endl;
	getchar();
}
*/