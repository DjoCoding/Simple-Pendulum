#ifndef VEC2_H
#define VEC2_H

#include <assert.h>
#include "math.h"

typedef struct {
    float x, y;
} fV2;

typedef struct {
    int x, y;
} V2;

V2 v2(int x, int y);
V2 v2v(int v);
V2 v2_add(V2 a, V2 b);
V2 v2_sub(V2 a, V2 b);
V2 v2_mul(V2 a, int v);
float v2_mag(V2 a);
fV2 v2_normalize(V2 a);

fV2 fv2(float x, float y);
fV2 fv2v(float v);
fV2 fv2_add(fV2 a, fV2 b);
fV2 fv2_sub(fV2 a, fV2 b);
fV2 fv2_mul(fV2 a, float v);
fV2 fv2_div(fV2 a, float v);
float fv2_mag(fV2 a);
fV2 fv2_normalize(fV2 a);


#ifdef VEC2_IMPLEMENTATION

V2 v2(int x, int y) {
    return (V2) { x, y };
}

V2 v2v(int v) {
    return v2(v, v);
}

V2 v2_add(V2 a, V2 b) {
    return v2(a.x + b.x, a.y + b.y);
}

V2 v2_sub(V2 a, V2 b) {
    return v2_add(a, v2_mul(b, -1));
}

V2 v2_mul(V2 a, int v) {
    return v2(a.x * v, a.y * v);
}

fV2 v2_div(V2 a, int v) {
    return fv2((float) a.x / v, (float)a.y / v);
}

float v2_mag(V2 a) {   
    return sqrt(a.x*a.x + a.y*a.y);
}

fV2 v2_normalize(V2 a) {
    return v2_div(a, v2_mag(a));
}

fV2 fv2(float x, float y) {
    return (fV2) { x, y };
}

fV2 fv2v(float v) {
    return fv2(v, v);
}

fV2 fv2_add(fV2 a, fV2 b) {
    return fv2(a.x + b.x, a.y + b.y);
}

fV2 fv2_sub(fV2 a, fV2 b) {
    return fv2_add(a, fv2_mul(b, -1));
}

fV2 fv2_mul(fV2 a, float v) {
    return fv2(a.x * v, a.y * v);
}

fV2 fv2_div(fV2 a, float v) {
    assert(v != 0);
    return fv2_mul(a, 1 / v);
}

float fv2_mag(fV2 a) {   
    return sqrt(a.x*a.x + a.y*a.y);
}

fV2 fv2_normalize(fV2 a) {
    return fv2_div(a, fv2_mag(a));
}


#endif

#endif