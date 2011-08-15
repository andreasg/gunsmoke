#ifndef DIVVE_GFX_H
#define DIVVE_GFX_H

#include <string.h>
#include <malloc.h>
#include <allegro.h>
#include <scew/scew.h>

#include "defines.h"


typedef struct GFX_OBJ {
    char name[NAME_LN];
    BITMAP *img;
} GFX_OBJ;


GFX_OBJ *create_gfx_list(void);
BITMAP *search_gfx_list(char *name, GFX_OBJ *gfx_list, size_t sz);
GFX_OBJ *append_tile_to_gfx_list(const char *name, GFX_OBJ *gfx_list, size_t *sz);
int destroy_gfx_list(GFX_OBJ *gfx_list, size_t *sz);
/*BITMAP *load_img(const char *filename);*/
GFX_OBJ *append_bmp_to_gfx_list(const char *name, BITMAP *bmp, GFX_OBJ *gfx_list, size_t *sz);
BITMAP *grab_frame(BITMAP *src, size_t frame_w, size_t frame_h,
                   size_t columns, size_t frame);
int point_in_rect(size_t px, size_t py, size_t x, size_t y, size_t w, size_t h);
int rect_touching_rect(size_t x1, size_t y1, size_t w1, size_t h1,
                       size_t x2, size_t y2, size_t w2, size_t h2);


#endif

