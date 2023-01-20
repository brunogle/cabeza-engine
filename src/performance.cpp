#include <iostream>
#include <chrono>
#include "positioning.h"
#include "graphics.h"
#include "performance.h"

using namespace std;

/*
int test_get_occupancy(){

    game_state state1 = initial_game_state();
    game_state state2;
    state2.red.cabeza = {4,0};
    state2.red.mini = {3,0};
    state2.red.flaco = {0,0,orient_flat};
    state2.red.chato = {7,1,orient_horizontal};
    state2.red.gordo = {4,2};

	state2.blue.cabeza = {9,9};
    state2.blue.mini = {0,9};
    state2.blue.flaco = {6,7,orient_vertical};
    state2.blue.chato = {3,7,orient_vertical};
    state2.blue.gordo = {4,8};
    game_state state3;
    state3.red.cabeza = {0,0};
    state3.red.mini = {3,0};
    state3.red.flaco = {0,0,orient_flat};
    state3.red.chato = {7,1,orient_horizontal};
    state3.red.gordo = {4,2};

	state3.blue.cabeza = {0,1};
    state3.blue.mini = {0,5};
    state3.blue.flaco = {6,7,orient_flat};
    state3.blue.chato = {3,7,orient_flat};
    state3.blue.gordo = {4,4};

    using std::chrono::high_resolution_clock;
    using std::chrono::duration_cast;
    using std::chrono::duration;
    using std::chrono::milliseconds;

    auto t1 = high_resolution_clock::now();
    for(int i = 0; i < 10000000; i++){
        get_occupancy(state1);
        get_occupancy(state2);
        get_occupancy(state3);
        get_occupancy(state1);
        get_occupancy(state2);
        get_occupancy(state3);
        get_occupancy(state1);
        get_occupancy(state2);
        get_occupancy(state3);
        get_occupancy(state1);      
    }
    auto t2 = high_resolution_clock::now();
    std::chrono::duration<double> diff = t2 - t1;
    cout << 1000*diff.count() << "ms" <<  endl;

    return 0;
}
*/