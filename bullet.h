#ifndef DIVVE_BULLET_H
#define DIVVE_BULLET_H


#include <math.h>

#include <allegro.h>
#include "map.h"
#include "sprite.h"
#include "gfx.h"

extern SPRITE *player;
extern int score;

typedef struct BULLET {
   double x, y;
   int damage;
   int dir;
   int alive;
   BITMAP *bmp;
   int flight_time;

   double dx, dy;
} BULLET;


void kill_bullet_list(BULLET *bullet_list, size_t sz);
BULLET *fire(SPRITE *s, SPRITE *at_s, BULLET *bullet_list, size_t *sz, BITMAP *bmp);
void damage(SPRITE *s, BULLET *b);
void blit_bullets(BITMAP *bmp, MAP *m, BULLET *bullet_list, size_t sz);
void move_bullet(BITMAP *bmp, BULLET *b, MAP *m, SPRITE *sprite_list,
                 size_t sprite_list_sz);
void update_bullets(BULLET *bullet_list, size_t *bullet_list_sz, BITMAP *bmp,
                    MAP *m, SPRITE *sprite_list, size_t sprite_list_sz);
BULLET *add_bullet(size_t x, size_t y, SPRITE *at_spr, int damage, int dir, BITMAP *bmp, BULLET *bullet_list, size_t *sz);
BULLET *sprite_list_fire_guns(SPRITE *target, SPRITE *sprite_list, size_t sprite_list_sz,
                              BULLET *bullet_list, size_t *bullet_list_sz,
                              BITMAP *bullet_bmp);



#endif

