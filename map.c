#include "map.h"



/* new_map: generates an outline for a new map! passable and def_tile are the
 * defalut values for all the tiles of the map. Remember that _you_ own the tree! */
scew_tree *new_map(int id, int w, int h, int passable, char *def_tile)
{
    char value[LONG_NAME_LN]; /* temp so we can write int's to attributes */
    size_t i, x, y;

    scew_tree *t = NULL;
    scew_element *r = NULL;
    scew_element *e = NULL;
    scew_element *sub_e = NULL;

    t = scew_tree_create();
    r = scew_tree_add_root(t, "Map");

    /* map header */
    sprintf(value, "%i", id);
    e = scew_element_add(r, "Id");
    scew_element_set_contents(e, value);

    sprintf(value, "%i", h);
    e = scew_element_add(r, "Height");
    scew_element_set_contents(e, value);

    sprintf(value, "%i", w);
    e = scew_element_add(r, "Width");
    scew_element_set_contents(e, value);

    e = scew_element_add(r, "NextMap");
    scew_element_set_contents(e, "-1");

    /* outline the tiles */
    e = scew_element_add(r, "Tiles");
    for (i = 0; i < (size_t)(h*w); i++) {
        x = (i % w) * TILE_W;
        y = (i / w) * TILE_H;
        sub_e = scew_element_add(e, "Tile");
        sprintf(value, "%i", x);
        scew_element_add_attr_pair(sub_e, "x", value);
        sprintf(value, "%i", y);
        scew_element_add_attr_pair(sub_e, "y", value);
        sprintf(value, "%i", passable);
        scew_element_add_attr_pair(sub_e, "passable", value);
        scew_element_add_attr_pair(sub_e, "name", def_tile);
    }

    /* outline a sprite-base */
    e = scew_element_add(r, "Sprites");

    return t;
}



/* write_map: writes the tree to filename */
void write_map(const char *filename, scew_tree *t)
{
    if (!t) return;
    if (!scew_writer_tree_file(t, filename)) {
        printf("couldn't write map %s\n", filename);
    }
}



/* modify_map_coord: changes the tile or sprite on coord x,y to the given data.
 * nothing is on that coord - the cord and it's given data will be added
 * if sprite == 1 we will add/change data in the <Sprites> section of the
 * xml-tree. Also if sprite==1 passable does not have any relevance, sinse
 * only tiles care about that value.
 * If you wish to delete, set the 'del' flag to 1. Remember to set sprite=1
 * to delete Sprites and sprite=0 to delete (or add) tiles. */
void modify_map_coord(int x, int y, int passable, char *name, int sprite,
        int del, scew_tree *t)
{
    scew_attribute *a = NULL;
    scew_element *r = NULL;
    scew_element *e = NULL;
    scew_element *sub_e = NULL;
    scew_element *to_delete = NULL;
    int updated = 0;

    char value[256];

    if (!t) return;

    if (x % TILE_W != 0) return;
    if (y % TILE_H != 0) return;

    r = scew_tree_root(t);
    if (sprite) {
        e = scew_element_by_name(r, "Sprites");
    } else {
        e = scew_element_by_name(r, "Tiles");
    }

    /* loop throught the tree and look for the coordinate */
    sub_e = scew_element_next(e, sub_e);
    while (sub_e) {
        a = scew_attribute_by_name(sub_e, "x");
        if (atoi(scew_attribute_value(a)) == x) {
            a = scew_attribute_by_name(sub_e, "y");
            if (atoi(scew_attribute_value(a)) == y) {
                /* right coords - update! */
                if (del) {
                    to_delete = sub_e;
                    sub_e = scew_element_next(e, sub_e);
                    scew_element_del(to_delete);
                    updated = 1;
                    break;
                }

                if (!sprite) {
                    /* update (this is the only tile-secific value) */
                    sprintf(value, "%i", passable);
                    a = scew_attribute_by_name(sub_e, "passable");
                    scew_attribute_set_value(a, value);
                }
                sprintf(value, "%s", name);
                a = scew_attribute_by_name(sub_e, "name");
                scew_attribute_set_value(a, value);
                updated = 1;
                break;
            }
        }
        sub_e = scew_element_next(e, sub_e);
    }

    if (!updated && !del) {
        if (sprite) {
            sub_e = scew_element_add(e, "Sprite");
        } else {
            sub_e = scew_element_add(e, "Tile");
        }
        sprintf(value, "%i", x);
        scew_element_add_attr_pair(sub_e, "x", value);
        sprintf(value, "%i", y);
        scew_element_add_attr_pair(sub_e, "y", value);
        if (!sprite) {
            sprintf(value, "%i", passable);
            scew_element_add_attr_pair(sub_e, "passable", value);
        }
        sprintf(value, "%s", name);
        scew_element_add_attr_pair(sub_e, "name", value);
    }
}



/* forward_map: makes the world go around... :) */
int forward_map(MAP *m)
{
    assert(m);
    if (m->y_offset > TILE_H) {
        m->y_offset -= MAP_SPEED;
        return 1;
    }
    return 0;
}



/* point_on_passable: helper function to rect_on_passable */
int point_on_passable(size_t x, size_t y, MAP *m)
{
    size_t i;
    size_t mx, my;

    assert(m);

    /* find the tile */
    for (i = 0; i < m->tiles_len; i++) {
        mx = m->tiles[i].x;
        my = m->tiles[i].y - m->y_offset;
        if (mx <= x && x < mx + TILE_W && my <= y && y < my + TILE_H) {
            return m->tiles[i].passable;
        }
    }
    return 0;
}



/* rect_on_passable: simply checks if the coords are all on passable surface */
int rect_on_passable(size_t x, size_t y, size_t w, size_t h, MAP *m)
{
    return (point_on_passable(x,   y, m)   &&
            point_on_passable(x + w, y, m)   &&
            point_on_passable(x,   y + h, m) &&
            point_on_passable(x + w, y + h, m));
}



/* destroy_map: frees the memeory from the given map */
void destroy_map(MAP *m)
{
    if (m != NULL) {
        free(m->tiles);
        m->tiles = NULL;
        destroy_gfx_list(m->tile_gfx, &m->tile_gfx_sz);
    }
}



/* load_tile_bitmap: for all tiles of the map, load the appropriate
 * bitmap into a bitmap list - for each unique tile-name */
GFX_OBJ *load_tile_bitmaps(TILE *tiles, size_t sz, size_t *gfx_list_sz)
{
    int in;
    size_t i, j, gfx_sz = 0;
    GFX_OBJ *tile_gfx = create_gfx_list();
    char **name_list = {0};
    size_t name_list_sz = 0;

    if (tiles == NULL || sz < 1 || !tile_gfx) {
        return NULL;
    }

    /* get the names of all the different tiles */
    for (i = in = 0; i < sz; i++) {
        for (j = 0; j < name_list_sz; j++) {
            if (strncmp(tiles[i].name, name_list[j], NAME_LN) == 0) {
                in = 1;
                break;
            }
        }
        if (!in) {
            name_list = realloc(name_list, (name_list_sz + 1) * sizeof(*name_list));
            name_list[name_list_sz] = (char*)malloc(NAME_LN * sizeof(char));
            strncpy(name_list[name_list_sz], tiles[i].name, NAME_LN);
            name_list_sz++;
        } else {
            in = 0;
        }
    }

    /* load the img for each tile */
    for (i = gfx_sz = 0; i < name_list_sz; i++) {
        tile_gfx = append_tile_to_gfx_list(name_list[i], tile_gfx, &gfx_sz);
        free(name_list[i]);
    }

    free(name_list);
    *gfx_list_sz = gfx_sz;
    return tile_gfx;
}



/* create_map: inits a chunk of memory for a map - this function should
 * not be used outside this file... use load_map instead */
MAP *create_map(void)
{
    MAP *m;
    m = malloc(sizeof(*m));
    m->id = m->width = m->height = m->tiles_len = m->tile_gfx_sz = 0;
    m->tiles = NULL;
    m->tile_gfx = NULL;
    m->background = NULL;
    m->y_offset = 0;
    return m;
}



/* load_map: does what it says... creates a map from filename and returns it
 * gives NULL if failed */
MAP *load_map(const char *filename)
{
    scew_tree *t = NULL;
    scew_parser *p = NULL;
    scew_element *e = NULL;
    scew_element *c = NULL;
    scew_element *r = NULL;
    scew_attribute *a = NULL;

    unsigned int i = 0;
    /*size_t sz = 0;*/
    MAP *m;
    TILE *tile;

    p = scew_parser_create();
    scew_parser_ignore_whitespaces(p, 1);

    if (!scew_parser_load_file(p, filename)) {
        printf("%s\n", scew_error_expat_string(scew_error_code()));
        return NULL;
    }

    t = scew_parser_tree(p);
    r = scew_tree_root(t);
    m = create_map();

    /* read the id */
    e = scew_element_by_name(r, "Id");
    m->id = atoi(scew_element_contents(e));

    /* read height and width */
    e = scew_element_by_name(r, "Height");
    m->height = atoi(scew_element_contents(e));
    e = scew_element_by_name(r, "Width");
    m->width = atoi(scew_element_contents(e));
    m->tiles_len = m->height * m->width;

    e = scew_element_by_name(r, "NextMap");
    m->next_map = atoi(scew_element_contents(e));

    /* read all the tiles */
    m->tiles = (struct TILE*)malloc(m->tiles_len * sizeof(struct TILE));
    e = scew_element_by_name(r, "Tiles");
    tile = (TILE*)malloc(sizeof(TILE)); /* buffer tile */
    i = 0;
    while ((c = scew_element_next(e, c)) != NULL && i < m->tiles_len) {
        a = scew_attribute_by_name(c, "name");
        strncpy(tile->name, (char*)scew_attribute_value(a), NAME_LN);

        a = scew_attribute_by_name(c, "passable");
        tile->passable = atoi(scew_attribute_value(a));

        a = scew_attribute_by_name(c, "x");
        tile->x = atoi(scew_attribute_value(a));

        a = scew_attribute_by_name(c, "y");
        tile->y = atoi(scew_attribute_value(a));

        memcpy(&m->tiles[i], tile, sizeof(struct TILE));
        i++;
    }

    /* check if there were to few tiles defined in the mapfile */
    if (i++ != (m->tiles_len)) {
        /* you know... we could handle the problem?
         * - yeees, but let's not! */
        allegro_message("create_map: error in mapfile");
        destroy_map(m);
        return NULL;
    }
    m->y_offset = m->height * TILE_H - (HEIGHT - HUD_H);

    m->tile_gfx_sz = 0;
    m->tile_gfx = load_tile_bitmaps(m->tiles, m->tiles_len, &m->tile_gfx_sz);
    init_map_tiles_gfx(m);

    /* cleaning */
    scew_element_free(c);
    scew_element_free(e);
    scew_element_free(r);
    scew_parser_free(p);
    return m;
}



/* init_map_tiles_gfx: for all tiles in map, set it's BITMAP pointer to a valid BITMAP
 * within the maps gfx_list. Should only be run by load_map() */
void init_map_tiles_gfx(MAP *m)
{
     size_t i;

     for (i=0; i<m->tiles_len; i++) {
          m->tiles[i].bmp = search_gfx_list(m->tiles[i].name, m->tile_gfx,
                                            m->tile_gfx_sz);
     }
}

/* blit_map: fills dest with the correct part of the 
 * map - taking map->y_offset into account */
void blit_map(BITMAP *dest, MAP *m, int w, int h)
{

     int fst_tile = 0;
     int i = 0;
     int tiles_on_scr = (w/TILE_W) * (h/TILE_H);
     int delta = 0;
     TILE *t = NULL;

     if (!dest || !m) {
          allegro_message("blit_map: !dest || !m");
          return;
     }

     fst_tile = (m->y_offset / TILE_H) * m->width;

     if ((int)m->y_offset % TILE_H != 0) {
          /* we have to paint partial tiles at top and bottom of screen */
          delta = (int)m->y_offset % TILE_H;
          for(i = fst_tile - m->width; (int)i < fst_tile && i >= m->width; i++) {
               t = &m->tiles[i];
               blit(t->bmp, dest, 0, delta, t->x, 0, t->bmp->w, t->bmp->h);
          }

          for (i=fst_tile+tiles_on_scr; i<(int)m->tiles_len && i<(int)fst_tile+tiles_on_scr+m->width; i++) {
               t = &m->tiles[i];
               blit(t->bmp, dest, 0, 0, t->x, t->y-m->y_offset, t->bmp->w,TILE_H);
          }

     }

     /* draw the rest of the tiles */
     for(i=fst_tile; i<(int)m->tiles_len && (i-fst_tile)<tiles_on_scr; i++) {
          t = &m->tiles[i];
          draw_sprite(dest, t->bmp, t->x, t->y-m->y_offset);
     }
}
