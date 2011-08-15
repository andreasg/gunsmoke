#ifndef DIVVE_EDITOR_GUI_H
#define DIVVE_EDITOR_GUI_H


#include <scew/scew.h>

#include "primitives.h"
#include "list_element.h"
#include "defines.h"
#include "sprite.h"
#include "map.h"
#include "gfx.h"


/* omg.... all the globalvars from mapeditor.c... */
extern int mode;
extern LIST_ELEMENT *selection;
extern LIST_ELEMENT *tile_elem_lst;
extern size_t tile_elem_lst_sz;
extern LIST_ELEMENT *spr_elem_lst;
extern size_t spr_elem_lst_sz;
extern SPRITE *sprite_list;
extern size_t sprite_list_sz;
extern GFX_OBJ *gfx_list;
extern size_t gfx_list_sz;
extern scew_tree *maptree;
extern int map_offset;
extern int draw_grid;
extern struct GRID_ITEM grid_item;

typedef struct GRID_ITEM {
   int x, y;
   int w, h;
   int passable;
   int mode;
   char name[NAME_LN];
} GRID_ITEM;


void draw_sprite_select_list(BITMAP *dest);
void draw_select_lists(BITMAP *dest);
void draw_tile_ctrl_window(BITMAP *dest, LIST_ELEMENT *e_list, size_t sz);
void list_elem_switch_page(int next, int top_y, LIST_ELEMENT *e_list, size_t sz);

void draw_yes_no_box(BITMAP *dest, const char *text, size_t x, size_t y);
void draw_button(BITMAP *dest, RECT r, const char *text);
void draw_box(BITMAP *dest, RECT r);
void draw_map_from_tree(BITMAP *dest, scew_tree *t, int offset);

void draw_tilelst_ctrl_box(BITMAP *dest);
void draw_spritelst_ctrl_box(BITMAP *dest);
void draw_ctrl_box(BITMAP *dest);


#endif

