#ifndef DA_H
#define DA_H

#include <stdio.h>
#include <stdlib.h>

#define DA_INITIAL_SIZE  20000

#define DA_FULL(da) ((da)->count >= (da)->size)

#define DA_RESIZE(da) \
    do { \
        if ((da)->size == 0) { (da)->size = DA_INITIAL_SIZE / 2; } \
        (da)->size *= 2; \
        (da)->items = realloc((da)->items, (da)->size * sizeof((da)->items[0])); \
        if (!((da)->items)) { fprintf(stderr, "could not reallocate"); exit(EXIT_FAILURE); } \
    } while(0)

#define DA_APPEND(da, v) \
    do {\
        if (DA_FULL(da)) { DA_RESIZE(da); } \
        (da)->items[(da)->count++] = v; \
    } while(0)

#endif