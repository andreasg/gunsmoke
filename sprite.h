#ifndef DIVVE_SPRITE_H
#define DIVVE_SPRITE_H

#include <Python.h>

#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <scew/scew.h>
#include <math.h>

#include "defines.h"
#include "gfx.h"
#include "map.h"



/* About sprites: This is how sprites work. To load a bunch of sprites you must
 * first have a map that have given the name and position off the sprites to be in
 * the world. Use load_sprite_list to get this information and thus outline a
 * sprite_list.
 * Then the little data gotten from the mapfile must be complemented with the
 * data within SPRITE_INDEX for the sprite to behave as it should. Also the
 * correct bitmaps for each sprite must be loaded. For this we use
 * complete_sprite_list.
 * Hence! To load sprites from a mapfile we do this:
 * sprite_list = load_sprite_list(MAPFILE, sprite_list, sprite_list_length);
 * gfx_list = complete_sprite_list(sprite_list, sprite_list_length,
 *                                  gfx_list, gfx_list_length); */



typedef struct _SPRITE {
     BITMAP *bmp;
     BITMAP *corpse_bmp;

    char name[NAME_LN];
    char long_name[LONG_NAME_LN];
    size_t w, h;
    size_t sprite_sheet_width;
    size_t corpse_frame;
    double x, y;
    double dx, dy;
    size_t frame_count;
    size_t current_frame;
    int dir;
    int fire_dir;
    double spd;
    int money_drop_max;
    int money_drop_min;
    int health;
    int wpn;
    int damage;
    int updating;
    int alive;
    size_t anim_delay;
    size_t fire_clicks, cur_click;

    int new_dir;
    int facing_dir;
    size_t begin_frame;
    size_t end_frame;
    size_t collision_end_y;
    int lifetime;
    int time;
    int returning;
    int movement_type;
    int done;
    int decision_time;
    int decision_tick;
    int corpse_tick;
    int is_boss;

     void (*update_sprite)(struct _SPRITE*, struct _SPRITE*, BITMAP*, MAP*, struct _SPRITE*, size_t, GFX_OBJ*, size_t);
    void (*ai_update)(struct _SPRITE*, int, int, BITMAP*, int);
    char state[NAME_LN];
} SPRITE;


void init_ai_methods(void);
void free_python(void);
void animate_sprite(SPRITE *s, GFX_OBJ *gfx_list, size_t gxf_list_sz);
int spawn_sprite_if_on_screen(SPRITE *s, MAP *m);
void sprite_list_kill_updating(SPRITE *player, SPRITE *sprite_list, size_t sz);
void enemy_normal_movement(SPRITE *s, int tx, int ty, BITMAP *bmp, int y_offset);
SPRITE *touching_sprite_list(SPRITE *s, SPRITE *sprite_list, size_t sz);
SPRITE *find_sprite(const char *name, SPRITE *sprite_list, size_t sz);
int sprite_list_boss_alive(SPRITE *sprite_list, size_t sz);
int move_sprite(BITMAP *bmp, SPRITE *s, MAP *m, SPRITE *sprite_list,
                size_t sz, double custom_speed);
void update_sprite_list(BITMAP *bmp, MAP *m, SPRITE *player, SPRITE *sprite_list, size_t sz, GFX_OBJ *gfx_list, 
                        size_t gfx_list_sz);
void blit_sprite_list(BITMAP *bmp, MAP *m, SPRITE *sprite_list, size_t sprite_list_sz);
SPRITE *create_sprite(void);
GFX_OBJ *load_sprite_gfx(SPRITE *s, GFX_OBJ *gfx_list, size_t *sz);
void update_sprite_data(SPRITE *dest, SPRITE *src);
GFX_OBJ *complete_sprite_list(SPRITE *sprite_list, size_t sprite_list_sz,
                              GFX_OBJ *gfx_list, size_t *gfx_list_sz);
SPRITE *load_sprite_list(char *filename, SPRITE *sprite_list, size_t *sprite_list_sz);
SPRITE *append_sprite_list(SPRITE *s, SPRITE *sprite_list, size_t *sz);
void destroy_sprite(SPRITE *s);
void destroy_sprite_list(SPRITE *sprite_list, size_t *sz);
void get_trajectory(double x1, double y1, double x2, double y2, double *dx, double *dy);



#endif

