#include "cui.h"
#include "parsing.h"
#include "positioning.h"
#include "search.h"
#include "eval.h"


#include <iomanip>
#include <sstream>
#include <iostream>
#include <readline/readline.h>
#include <readline/history.h>

Cui::Cui(){

}

bool Cui::seperate_command(std::string command, std::string & operation, std::vector<std::string> & args){
    std::stringstream command_ss(command);

    operation = "";
    args.clear();

    command_ss >> operation;

    std::string argument;
    while (command_ss >> argument) {
        args.push_back(argument);
    }

    return true;
}




bool Cui::start(){

    bool running = true;

    PVSearch search_engine(DistanceEval::eval, 30000, 12);

    this->game.set_search_engine(&search_engine);

    std::cout << "Welcome to Cabeza Engine. Default configuration set to:\ntime: "
    << std::setprecision(2) << search_engine.get_timeout() << "ms, depth: "
    << search_engine.get_depth() << std::endl << std::endl;

    while(running){

        std::string command_str = readline("> ");

        std::string operation;
        std::vector<std::string> args;

        this->seperate_command(command_str, operation, args);

        int args_count = args.size();


        if(operation == "fen" && args_count == 1){
            if(this->game.set_fen(args.at(0))){
                std::cout << "FEN ok" << std::endl;
            }
            else{
                std::cout << "FEN error" << std::endl;
            }
        }
        else if(operation == "draw" || operation == "d"){
            std::cout << this->game.get_game_drawing(true) << std::endl;
        }
        else if((operation == "move" || operation == "m") && args_count == 1){
            if(this->game.apply_move(parsing::parse_movement_str(args.at(0)))){
                std::cout << "move ok" << std::endl;
            }
            else{
                std::cout << "move error" << std::endl;
            }
        }
        else if((operation == "turn" || operation == "t") && args_count == 1){
            if(args.at(0) == "red" || args.at(0) == "r"){
                this->game.set_turn(positioning::red);
            }
            else if(args.at(0) == "blue" || args.at(0) == "b"){
                this->game.set_turn(positioning::blue);
            }
            else{
                std::cout << "player error" << std::endl;
            }
        }
        else if(operation == "undo" || operation == "i"){
            this->game.undo_moves(1);
        }
        else if(operation == "search" || operation == "s"){
            this->game.search();
        }
        else if(operation == "play" || operation == "p"){
            positioning::move best_move = this->game.search();

            this->game.apply_move(best_move);
        }
        else if(operation == "quit" || operation == "q"){
            return true;
        }
        else{
            std::cout << "Invalid command" << std::endl;
        }

        add_history(command_str.c_str());

    }

    return true;

}
