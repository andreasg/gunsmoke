#include "primitives.h"



/* get_mouse_pos: gives the coords of the mouse */
POINT get_mouse_pos(void)
{
    int pos;
    POINT p;

    pos = mouse_pos;
    p.x = pos >> 16;
    p.y = pos & 0x0000ffff;

    return p;
}



/* rec: rect is used by allegro... anywais, it just create a RECT */
RECT rec(size_t x, size_t y, size_t w, size_t h)
{
    RECT r;
    r.x = x;
    r.y = y;
    r.w = w;
    r.h = h;
    return r;
}


/* point_in_rec: check if the point is within the rectangle */
int point_in_rec(POINT p, RECT r)
{
    size_t x, y, w, h;
    x = r.x;
    y = r.y;
    w = r.w;
    h = r.h;

    return (x <= p.x && p.x <= x + w && y <= p.y && p.y <= y + h);
}

