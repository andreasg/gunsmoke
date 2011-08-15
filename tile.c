#include "tile.h"



/* create_tile: get a chunk of memory for a tile */
TILE *create_tile(const char *name)
{
    TILE *t = NULL;
    t = (TILE*) malloc(sizeof(TILE));
    strncpy(t->name, name, NAME_LN);
    t->bmp = NULL;
    return t;
}



/* destroy_tile: free the tile */
void destroy_tile(TILE *t)
{
     if (t) {
          destroy_bitmap(t->bmp);
          free(t);
     }
}

