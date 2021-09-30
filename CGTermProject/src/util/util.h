#pragma once

#ifndef M_PI
#define M_PI (3.1415926535897932384626433832795f)
#define DEGTORAD(degree) ((degree) * (M_PI / 180.0f))
#define RADTODEG(radian) ((radian) * (180.0f / M_PI))
#define SIZEOF(type, len) ((len) * sizeof(type)) // sizeof multiple types
#endif

#ifdef WIN32
#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) (srand((x)))
#define randomVal(min, max) ((float)(min + (max - min) * drand48()))
#endif

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define UNIFORM_BINDING_MATERIAL 1
#define UNIFORM_BINDING_LIGHT 2
#define UNIFORM_BINDING_VIEWMAT 3

// table size is 2.84m x 1.42m
#define BALL_RENDER_STEP 30
#define BALL_RADIUS 0.44f