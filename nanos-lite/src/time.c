#include "klib-macros.h"
#include <time.h>

int gettimeofday(struct timeval *restrict tv, void *restrict tz) {
    if (!tv) {
        return -1;
    }
    
    tv->tv_usec = io_read(AM_TIMER_UPTIME).us;
    tv->tv_sec = tv->tv_usec / 1000000;
    return 0;
}