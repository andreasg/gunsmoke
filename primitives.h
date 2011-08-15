#ifndef DIVVE_PRIMITIVES_H
#define DIVVE_PRIMITIVES_H



#include <allegro.h>



typedef struct _POINT {
    size_t x, y;
} POINT;



typedef struct RECT {
    size_t x, y;
    size_t w, h;
} RECT;



POINT get_mouse_pos(void);
int point_in_rec(POINT p, RECT r);
RECT rec(size_t x, size_t y, size_t w, size_t h);



#endif

