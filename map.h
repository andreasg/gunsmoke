#ifndef DIVVE_MAP_H
#define DIVVE_MAP_H



#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <scew/scew.h>

#include "gfx.h"
#include "tile.h"



typedef struct MAP {
   int id;
   int width;  /* in tiles */
   int height; /* in tiles */
   int next_map;
   struct TILE *tiles;
   size_t tiles_len;
   size_t tile_gfx_sz;
   struct GFX_OBJ *tile_gfx;
   BITMAP *background;
   double y_offset; /* holds the offset - so we can move the world, not the player */
} MAP;



void write_map(const char *filename, scew_tree *t);
scew_tree *new_map(int id, int w, int h, int passable, char *def_tile);
void modify_map_coord(int x, int y, int passable, char *name, int sprite, int del, scew_tree *t);
int forward_map(MAP *m);
int point_on_passable(size_t x, size_t y, MAP *m);
int rect_on_passable(size_t x, size_t y, size_t w, size_t h, MAP *m);
struct MAP *create_map(void);
struct MAP *load_map(const char *filename);
void destroy_map(MAP *m);
struct GFX_OBJ *load_tile_bitmaps(TILE *tiles, size_t sz, size_t *gfx_list_sz);
void blit_map(BITMAP *dest, MAP *m, int w, int h);
void init_map_tiles_gfx(MAP *m);
/* write this! */
/* hint valid_map(MAP *m); */



#endif

