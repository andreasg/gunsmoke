#include "gfx.h"



/* create_gfx_lits: initializes the list
 * remember to keep count of how long the list is! */
GFX_OBJ *create_gfx_list(void)
{
    GFX_OBJ *obj = NULL;
    obj = malloc(sizeof(*obj));
    return obj;
}



/* search_gfx_list: looks in the external var gfx_list for *name and returns
 * it's BITMAP pointer, if not found in the list it returns a blank
 * bitmap, with TILE_W and TILE_H as it's size */
BITMAP *search_gfx_list(char *name, GFX_OBJ *gfx_list, size_t sz)
{
    size_t i;
    if (!gfx_list) {
         allegro_message("gfx_list==NULL\nname=%s\nsz=%i", name, sz);
         return NULL;
    }
    for (i = 0; i < sz; i++) {
         if (gfx_list[i].name[0] == name[0] && strcmp(gfx_list[i].name, name) == 0) {
              return gfx_list[i].img;
        }
    }
    allegro_message("search faied.\nname=%s\nsz=%i", name, sz);
    return NULL;
}



/* append_tile_to_gfx_list: reallocates the list to fit another element
 * the pointer returned may point to another address then the one
 * passed. 
 * NOTE: Bascially, what differs this from append_bmp_to_gfx_list is that this function
 * builds a filename (with path) from the 'name' of a tile. And then simply calls
 * the previoulsy mentioned function. */
GFX_OBJ *append_tile_to_gfx_list(const char *name, GFX_OBJ *gfx_list, size_t *sz)
{
    char filename[LONG_NAME_LN];
    BITMAP *bmp = NULL;
    sprintf(filename, "%s%s%s", WORLD_GFX_DIR, name, GFX_EXT);

    bmp = load_bitmap(filename, NULL);
    if (bmp) {
         gfx_list = append_bmp_to_gfx_list(name, bmp, gfx_list, sz);
         return gfx_list;
    } else {
         return NULL;
    }
}



/* empty_gfx_list: clears the whole list */
int destroy_gfx_list(GFX_OBJ *gfx_list, size_t *sz)
{
    size_t i;
    if (gfx_list == NULL && *sz == 0) {
        return 0;
    }
    for (i = 0; i < *sz; i++) {
        destroy_bitmap(gfx_list[i].img);
        gfx_list[i].img = NULL;
    }
    free(gfx_list);
    *sz = 0;
    return 0;
}

/* append_bmp_to_gfx_list: adds a new element to the gfx_list with the bmp and name
 * given. Note that it does only copy the pointer, not all the bmp data, so
 * use destroy_gfx_list to free the bmp - otherwise if you destroy the bitmap before
 * the list - the list will have a BITMAP pointer to 'nowhere'! */
GFX_OBJ *append_bmp_to_gfx_list(const char *name, BITMAP *bmp, GFX_OBJ *gfx_list, size_t *sz)
{
    size_t i;
    if (gfx_list == NULL) {
        gfx_list = malloc(sizeof(*gfx_list));
        *sz = 0;
    } else {
        /* check so we are not adding a duplicate */
        for (i = 0; i < *sz; i++) {
            if (strncmp(gfx_list[i].name, name, NAME_LN) == 0) {
                /* name already in list */
                return gfx_list;
            }
        }
        gfx_list = realloc(gfx_list, (*sz + 1) * sizeof(*gfx_list));
    }
    strncpy(gfx_list[*sz].name, name, NAME_LN);
    gfx_list[*sz].img = bmp;
    *sz = *sz + 1;
    return gfx_list;
}



/* grab_frame: grabs framenumber 'frame' from a spritesheet */
BITMAP *grab_frame(BITMAP *src, size_t frame_w, size_t frame_h,
                   size_t columns, size_t frame)
{
    size_t x, y;
    BITMAP *dest;

    dest = create_bitmap(frame_w, frame_h);
    clear_bitmap(dest);

    if (src) {
        x = (frame % columns) * frame_w;
        y = (frame / columns) * frame_h;
        blit(src, dest, x, y, 0, 0, frame_w, frame_h);
    }
    return dest;
}



/* point_in_rect: check if the point is within the rectangle */
int point_in_rect(size_t px, size_t py, size_t x, size_t y, size_t w, size_t h)
{
    return (x <= px && px < x + w && y <= py && py < y + h);
}



/* rect_touching_rect: checks if the two rectangles are touching... */
int rect_touching_rect(size_t x1, size_t y1, size_t w1, size_t h1,
                       size_t x2, size_t y2, size_t w2, size_t h2)
{
    return (point_in_rect(x1,    y1, x2, y2, w2, h2) ||
            point_in_rect(x1 + w1, y1, x2, y2, w2, h2) ||
            point_in_rect(x1,    y1 + h1, x2, y2, w2, h2) ||
            point_in_rect(x1 + w1, y1 + h1, x2, y2, w2, h2));
}

