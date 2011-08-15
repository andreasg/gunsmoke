#ifndef DIVVE_TILE_H
#define DIVVE_TILE_H

#include <allegro.h>
#include <malloc.h>
#include <string.h>

#include "defines.h"

typedef struct TILE {
     char name[NAME_LN];
     int x, y;
     int passable;
     BITMAP *bmp;
} TILE;



#endif

