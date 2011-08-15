#ifndef DIVVE_LIST_ELEMENT_H
#define DIVVE_LIST_ELEMENT_H

#include <stdio.h>
#include <allegro.h>
#include <malloc.h>
#include <assert.h>
#include <string.h>

#include "primitives.h"
#include "defines.h"

extern size_t gfx_list_sz;

void test_fun(void);

typedef struct LIST_ELEMENT {
     RECT r;
     int color;
     int pressed_color;
     int pressed;
     int alive;
     char name[NAME_LN];
     BITMAP *bmp;
     int passable;
} LIST_ELEMENT;


LIST_ELEMENT *add_list_element(RECT rect, int passable, int color, int pressed_color,
                               const char *txt, BITMAP *bmp,
                               LIST_ELEMENT *item_list, size_t *sz);
void draw_list_element(BITMAP *dest, LIST_ELEMENT *e);
void destroy_list_element_list(LIST_ELEMENT *e_list, size_t *sz);
LIST_ELEMENT *list_element_clicked(POINT p, LIST_ELEMENT *e_list, size_t sz);
void set_pressed_element(LIST_ELEMENT *e, LIST_ELEMENT *e_list, size_t sz);

#endif

