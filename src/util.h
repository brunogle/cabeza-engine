#ifndef UTIL_H
#define UTIL_H

#include <time.h>

long get_ms(){
    timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return ts.tv_sec*1000 + ts.tv_nsec/1000000UL;
}

#endif