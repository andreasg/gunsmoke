#include "list_element.h"



LIST_ELEMENT *add_list_element(RECT rec, int passable, int color, int pressed_color,
                               const char *text, BITMAP *bmp,
                               LIST_ELEMENT *e_list, size_t *sz)
{
   LIST_ELEMENT *e = NULL;

   if (!e_list) {
      e_list = malloc(sizeof(*e_list));
      *sz = 0;
   } else {
      e_list = realloc(e_list, (*sz + 1) * sizeof(*e_list));
   }

   e = &e_list[*sz];

   e->r = rec;
   e->color = color;
   e->pressed_color = pressed_color;
   strncpy(e->name, text, NAME_LN);
   e->bmp = bmp;
   e->pressed = 0;
   e->passable = passable;

   *sz = *sz + 1;
   return e_list;
}



void draw_list_element(BITMAP *dest, LIST_ELEMENT *e)
{
   int color;
   size_t x, y, w, h;

   assert(dest);

   if (e->pressed) {
      color = e->pressed_color;
   } else {
      color = e->color;
   }
   x = e->r.x;
   y = e->r.y;
   w = e->r.w;
   h = e->r.h;

   rectfill(dest, x, y, x + w, y + h, color);
   rect(dest, x, y, x + w, y + h, OUTLINE_COLOR);

   stretch_sprite(dest, e->bmp, x + 4, y + 4, h - 8, h - 8);
   rect(dest, x + 4, y + 4, (x + 4) + (h - 8), (y + 4) + (h - 8), OUTLINE_COLOR);

   textprintf_ex(dest, font, x + h + 5, y + (h / 2), TEXT_COLOR, color, "%s", e->name);

}



/* destroy_list_element_list: free the list. NOTE this will not destroy the
 * bitmap - but then you should keep your bitmaps in a differend place then
 * the list_element... like a GFX_OBJ list ;) */
void destroy_list_element_list(LIST_ELEMENT *e_list, size_t *sz)
{
   if (!e_list) return;
   free(e_list);
   e_list = NULL;
   *sz = 0;
}



/* list_element_clicked: checks what element has been clicked. Returns
 * a pointer to the clicked one - if non has been clicked returns NULL. */
LIST_ELEMENT *list_element_clicked(POINT p, LIST_ELEMENT *e_list, size_t sz)
{
   size_t i;

   if (!e_list) return NULL;

   for (i = 0; i < sz; i++) {
      if (point_in_rec(p, e_list[i].r)) {
         return &e_list[i];
      }
   }
   return NULL;
}



/* set_pressed_element: highlights element e in list e_list,
 * automaticly disselects all other element in the list */
void set_pressed_element(LIST_ELEMENT *e, LIST_ELEMENT *e_list, size_t sz)
{
   size_t i;
   if (!e || !e_list) return;

   for (i = 0; i < sz; i++) {
      if (e != &e_list[i]) {
         e_list[i].pressed = 0;
      } else {
         e->pressed = 1;
      }
   }
}

