#ifndef DIVVE_GUNSMOKE_H
#define DIVVE_GUNSMOKE_H


#include <allegro.h>
#include <stdio.h>
#include <time.h>

/*font stuff*/
#include <glyph.h>

#include "defines.h"
#include "gfx.h"
#include "tile.h"
#include "map.h"
#include "sprite.h"
#include "bullet.h"
#include "highscore.h"

int init(void);
int main(void);

void reload_map(int map);
void draw_hud(BITMAP *dest, BITMAP *hudpic, BITMAP *heart, SPRITE *player);


#endif

