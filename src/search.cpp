#include "search.h"


namespace RandomMoveSearch{

    positioning::move search(positioning::game_state state){

        positioning::move movements[MAX_POSSIBLE_MOVEMENTS];

        int number_moves = get_moves_as_list(state, movements);

        return movements[rand()%number_moves];

    }

}