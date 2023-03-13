#ifndef UTIL_H
#define UTIL_H

#define REL_ASSERT(x, m) {if(!(x)){std::cout<<m<<std::flush;std::abort();}}


#include <time.h>

inline long get_ms(){
    timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec*1000 + ts.tv_nsec/1000000UL;
}

#endif