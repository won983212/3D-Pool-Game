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

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

#define UNIFORM_BINDING_MATERIAL 1
#define UNIFORM_BINDING_LIGHT 2
#define UNIFORM_BINDING_VIEWMAT 3

#define PBR_TEXTURE_INDEX_ALBEDO 0
#define PBR_TEXTURE_INDEX_METALLIC 1
#define PBR_TEXTURE_INDEX_ROUGHNESS 2
#define PBR_TEXTURE_INDEX_NORMAL 3
#define PBR_TEXTURE_INDEX_IRRADIANCEMAP 4
#define PBR_TEXTURE_INDEX_SPECULARMAP 5
#define PBR_TEXTURE_INDEX_BRDFMAP 6

#define BALL_RADIUS 1.0f