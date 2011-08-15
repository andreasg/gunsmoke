#include "editor_gui.h"



/* draw_select_lists: draws the tile and sprite subwindows */
void draw_select_lists(BITMAP *dest)
{
   size_t i, sprite_top_element = 0, tile_top_element = 0;

   if (!tile_elem_lst || !spr_elem_lst) return;

   /* draw nice backgrounds */
   draw_box(dest, TILE_WIN);
   draw_box(dest, SPRITE_WIN);

   /* top of TILE list */
   for (i = 0; i < tile_elem_lst_sz; i++) {
      if (tile_elem_lst[i].r.y == TILE_WIN.y) {
         tile_top_element = i;
         break;
      }
   }
   /* top of SPRITE list */
   for (i = 0; i < spr_elem_lst_sz; i++) {
      if (spr_elem_lst[i].r.y == SPRITE_WIN.y) {
         sprite_top_element = i;
         break;
      }
   }

   /* TILE: top_element and the ones below till ELEM_PER_PAGE is reached */
   for (i = tile_top_element; i < tile_top_element + ELEM_PER_PAGE &&
         i < tile_elem_lst_sz; i++) {
      draw_list_element(dest, &tile_elem_lst[i]);
   }
   /* SPRITE: top_element and the ones below till ELEM_PER_PAGE is reached */
   for (i = sprite_top_element; i < sprite_top_element + ELEM_PER_PAGE &&
         i < spr_elem_lst_sz; i++) {
      draw_list_element(dest, &spr_elem_lst[i]);
   }

}



/* list_elem_switch_page: "flips" the page... next if next=1 else prev-page */
void list_elem_switch_page(int next, int top_y, LIST_ELEMENT *e_list, size_t sz)
{
   size_t i, top_element = 0;
   int offset = 0;

   top_element = LIST_ELEM_H * ELEM_PER_PAGE;
   offset = ELEM_PER_PAGE * LIST_ELEM_H;

   /* find what elemen is at the top of the list */
   for (i = 0; i < sz; i++) {
      if (e_list[i].r.y == (size_t)top_y) {
         top_element = i;
         break;
      }
   }

   if (next) {
      /* check if it isn't already the last page */
      if (top_element + ELEM_PER_PAGE >= sz) {
         return;
      } else {
         /* apply the new coord for all elements */
         for (i = 0; i < sz; i++) {
            e_list[i].r.y -= offset;
         }
      }
   } else {
      /* go previous - check if we aren't the first page already */
      if (top_element == 0) {
         return;
      }

      /* apply the new coords */
      for (i = 0; i < sz; i++) {
         e_list[i].r.y += offset;
      }
   }
}



/* draw_yes_no_box: just display a box with the 'text' and '(y/n)' below that */
void draw_yes_no_box(BITMAP *dest, const char *text, size_t x, size_t y)
{
   int w = text_length(font, text) + 40;
   int h = text_height(font) * 2;
   int padding = 10;

   int text_y = y + 20;

   rectfill(dest, x, y, x + w, (y + 3*h + padding), DEFAULT_COLOR);
   rect(dest, x, y, x + w, y + 3*h + padding, OUTLINE_COLOR);
   rect(dest, x + 2, y + 2, (x + w) - 2, (y + 3*h + padding) - 2, OUTLINE_COLOR);

   textprintf_centre_ex(dest, font, x + (w / 2), text_y, TEXT_COLOR,
                        DEFAULT_COLOR, "%s", text);
   textprintf_centre_ex(dest, font, x + (w / 2), text_y + h, TEXT_COLOR,
                        DEFAULT_COLOR, "(y/n)");
}



/* draw_button: draw a box with pos and dimensions of r - with 'text'
 * centered on it */
void draw_button(BITMAP *dest, RECT r, const char *text)
{
   rectfill(dest, r.x, r.y, r.x + r.w, r.y + r.h, DEFAULT_COLOR);
   rect(dest, r.x, r.y, r.x + r.w, r.y + r.h, OUTLINE_COLOR);
   textprintf_centre_ex(dest, font, r.x + (r.w / 2), r.y + (r.h / 2),
                        TEXT_COLOR, DEFAULT_COLOR, "%s", text);
}



/* draw_box: simply draws a box with a little frame */
void draw_box(BITMAP *dest, RECT r)
{
   rectfill(dest, r.x, r.y, r.x + r.w, r.y + r.h, DEAD_COLOR);
   rect(dest, r.x, r.y, r.x + r.w, r.y + r.h, OUTLINE_COLOR);
}



/* draw_spritelst_ctrl_box: draws the next and prev and title for the
 * spritelist subwindow */
void draw_spritelst_ctrl_box(BITMAP *dest)
{
   draw_button(dest, SPRITE_FRW, ">>");
   draw_button(dest, SPRITE_REW, "<<");
   textprintf_centre_ex(dest, font, SPRITE_WIN.x + (SPRITE_WIN.w / 2),
                        SPRITE_WIN.y - 15, TEXT_COLOR, DEAD_COLOR, "SPRITES");
}



/* draw_tilelst_ctrl_box: draws the next and prev and title for the
 * tilelist subwindow */
void draw_tilelst_ctrl_box(BITMAP *dest)
{
   draw_button(dest, TILE_FRW, ">>");
   draw_button(dest, TILE_REW, "<<");
   textprintf_centre_ex(dest, font, TILE_WIN.x + (TILE_WIN.w / 2),
                        TILE_WIN.h + 5, TEXT_COLOR, DEAD_COLOR, "TILES");
}




/* get_wpn_name: returns a string representation of the weaponname parsed
 * from the defines */
char *get_wpn_name(int wpn)
{
   if (wpn == RIFLE) return "rifle";
   if (wpn == GUNS) return "guns";
   if (wpn == SHOTGUN) return "shotgun";
   if (wpn == UNARMED) return "UNARMED";
   return NULL;
}



/* draw_data_box: depending on the 'mode' of the program - output alot
 * of information about selected tile etc */
void draw_data_box(BITMAP *dest)
{
   size_t x, y, offset;
   int c, bg;
   SPRITE *s = NULL;
   draw_box(dest, DATA_WIN);
   POINT p = get_mouse_pos();


   if (!dest) return;

   c = TEXT_COLOR;
   bg = DEAD_COLOR;
   x = DATA_WIN.x + 5;
   y = DATA_WIN.y + 5;
   offset = text_height(font) + 5;


   /* 'default' items to display */
   /* mouse coordinates */
   if (point_in_rec(p, MAP_WIN)) {
      textprintf_ex(dest, font, x, y, c, bg, "X: %i", p.x - (p.x % TILE_W));
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "Y: %i",
                    p.y - (p.y % TILE_H) + map_offset);
      y += offset;



      /* if mouseover tile is passable */
      if (grid_item.passable) {
         textprintf_ex(dest, font, x, y, makecol(20, 240, 20), bg, "PASSABLE");
      } else {
         textprintf_ex(dest, font, x, y, makecol(240, 20, 20), bg, "NOT-PASSABLE");
      }
      y += offset;
   }
   /* current mode */
   if (mode == TILE_MODE) {
      textprintf_ex(dest, font, x, y, c, bg, "Mode: TILE");
   } else if (mode == SPRITE_MODE) {
      textprintf_ex(dest, font, x, y, c, bg, "Mode: SPRITE");
   }
   y += offset;
   /* mode specifics */
   if (mode == TILE_MODE) {
      if (selection) {
         textprintf_ex(dest, font, x, y, c, bg, "Name: %s",
                       selection->name);
         y += offset;
      }
   } else if (mode == SPRITE_MODE && sprite_list) {
      s = find_sprite(selection->name, sprite_list, sprite_list_sz);
      if (!s)  return;
      textprintf_ex(dest, font, x, y, c, bg, "Name: %s", s->long_name);
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "Health: %i", s->health);
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "$ DropMax: %i", s->money_drop_max);
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "$ DropMin: %i", s->money_drop_min);
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "Speed: %.1f", s->spd);
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "Weapon: %s", get_wpn_name(s->wpn));
      y += offset;
      textprintf_ex(dest, font, x, y, c, bg, "BulletDamage: %i", s->damage);
      y += offset;
      return;
   }
}



/* draw_map_from_tree: out of a scew_tree it draws a representatin of the map */
void draw_map_from_tree(BITMAP *dest, scew_tree *t, int offset)
{
   size_t i, x, y;
   char name[NAME_LN];
   POINT p;
   int sprite_pass = 1;
   scew_attribute *a = NULL;
   scew_element *r = NULL;
   scew_element *e = NULL;
   scew_element *sub_e = NULL;

   if (!t || !dest) return;

   r = scew_tree_root(t);

   /* kind of ugly, but loops two times and loads all the tiles the
    * first iteration and the sprites the second iteration */
   e = scew_element_by_name(r, "Tiles");
   sub_e = scew_element_next(e, sub_e);
   for (i = 0; i < 2; i++) {
      while (sub_e) {
         a = scew_attribute_by_name(sub_e, "x");
         x = atoi(scew_attribute_value(a));
         a = scew_attribute_by_name(sub_e, "y");
         y = atoi(scew_attribute_value(a));
         p.x = x;
         p.y = y - offset;
         if (point_in_rec(p, MAP_WIN)) {
            a = scew_attribute_by_name(sub_e, "name");
            if ((int)i == sprite_pass) {
               /* the sprite iteration */
               sprintf(name, "%s0", scew_attribute_value(a));
            } else { /* tile_pass */
               sprintf(name, "%s", scew_attribute_value(a));
            }
            draw_sprite(dest, search_gfx_list(name, gfx_list, gfx_list_sz),
                        p.x, p.y);
            
            /* draw red outline around non-passable tile */
            if ((int)i != sprite_pass) {
               a = scew_attribute_by_name(sub_e, "passable");
               if (!atoi(scew_attribute_value(a))) {
                  rect(dest, p.x+1, p.y+1, p.x+TILE_W-1, p.y+TILE_H-1, makecol(255, 0,0));
               }
            }
         }
         sub_e = scew_element_next(e, sub_e);
      }

      /* since we only iterate twise, this is in 'practice' only done once */
      e = scew_element_by_name(r, "Sprites");
      sub_e = scew_element_next(e, sub_e);
   }
}



/* draw_map_grid_overlay: draws a ugly grid ontop of the map :P */
void draw_map_grid_overlay(BITMAP *dest)
{
   size_t i;
   int x, y;
   x = y = 0;

   /* draw horizontal lines */
   for (i = 0; i < (MAP_WIN.h / TILE_H); i++) {
      line(dest, 0, y, MAP_WIN.w, y, GRID_COLOR);
      y += TILE_H;
   }

   /* draw vertical lines */
   for (i = 0; i < (MAP_WIN.w / TILE_W); i++) {
      line(dest, x, 0, x, MAP_WIN.h, GRID_COLOR);
      x += TILE_W;
   }
}



/* draw_ctrl_box: draws most of the guielements */
void draw_ctrl_box(BITMAP *dest)
{
   draw_spritelst_ctrl_box(dest);
   draw_tilelst_ctrl_box(dest);
   draw_map_from_tree(dest, maptree, map_offset);
   if (draw_grid) draw_map_grid_overlay(dest);
   draw_data_box(dest);
}

