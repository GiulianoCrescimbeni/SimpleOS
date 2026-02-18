#ifndef DEBUG_H
#define DEBUG_H

#include <kernel/utils.h>

#define VERBOSE_BOOT 0

#if VERBOSE_BOOT == 1
    #define DEBUG_LOG(...) printf(__VA_ARGS__)
#else
    #define DEBUG_LOG(...) ((void)0)
#endif

#endif