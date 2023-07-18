#ifndef UTIL_H
#define UTIL_H

#define REL_ASSERT(x, m) {if(!(x)){std::cout<<m<<std::flush;std::abort();}}




/*
//Works on linux, not windows

#include <time.h>


inline long get_ms(){
    timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec*1000 + ts.tv_nsec/1000000UL;
}
*/

#include <chrono>

inline long get_ms(){
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}


#endif