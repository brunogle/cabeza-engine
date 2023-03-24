#ifndef CUI_H
#define CUI_H

#include <vector>
#include <string>
#include "game.h"

class Cui{

    private:

    bool seperate_command(std::string command, std::string & operation, std::vector<std::string> & args);

    Game game;

    public:

    Cui();

    bool start();

};


#endif