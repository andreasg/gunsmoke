#include "sprite.h"

int ai_countdown = 0;

PyObject *boss_method = NULL;
PyObject *normal_enemy_method = NULL;

void init_ai_methods(void) 
{
    PyObject *args = NULL;
    PyObject *mymod = NULL;
    PyObject *class = NULL;
    PyObject *object = NULL;

    if (boss_method) return;
    Py_Initialize();
    if (!Py_IsInitialized()) {
       allegro_message("Failed to init python");
       return;
    }

    mymod = PyImport_ImportModule("ai");
    if (!mymod) {
        allegro_message("!mymod");
        exit(1);
        return;
    }

    /* find boss method */
    class = PyObject_GetAttrString(mymod, "Boss");
    if (!class) {
        allegro_message("!class");
        exit(1);
        return;
    }

    args = Py_BuildValue("()");
    object = PyEval_CallObject(class, args);
    boss_method = PyObject_GetAttrString(object, "update");
    Py_DECREF(args);


    /* find Assassin method */
    Py_DECREF(class);
    class = PyObject_GetAttrString(mymod, "Assassin");
    if (!class) {
        allegro_message("!class(2)");
        exit(1);
        return;
    }
    Py_DECREF(mymod);

    args = Py_BuildValue("()");
    object = PyEval_CallObject(class, args);
    normal_enemy_method = PyObject_GetAttrString(object, "update");
    Py_DECREF(args);
}


/* enemy_normal_movement: moves an (enemy) sprite 
 * according to the 'normal' rules */
void enemy_normal_movement(SPRITE * s, int tx, int ty, BITMAP *bmp, int y_offset)
{
    float tmpx, tmpy;
    char *tmps=NULL;
    PyObject *ret = NULL;
    PyObject *args = NULL;

    if (!s || !s->updating || !s->alive) return;
    if (!normal_enemy_method) return;/* init_ai_methods(); */

    args = Py_BuildValue("(ff)(ff)(ii)(ii)s#", s->x, s->y-y_offset,s->dx, s->dy, tx, ty, bmp->w, VSCREEN_H, 
                                             s->state, strlen(s->state));
    if (!args) {
        Py_DECREF(args);
        allegro_message("can build arglist");
        return;
    }
    ret = PyEval_CallObject(normal_enemy_method, args);
    PyArg_ParseTuple(ret, "ffs", &tmpx, &tmpy, &tmps);
    if (tmps) {
        strncpy(s->state, tmps, NAME_LN);
    }
    s->dx = (double)tmpx;
    s->dy = (double)tmpy;


    free(tmps);
    tmps = NULL;
    Py_DECREF(ret);
}




void boss_normal_movement(SPRITE *s, int tx, int ty, BITMAP *bmp, int y_offset)
{
    float tmpx, tmpy;
    char *tmps=NULL;
    PyObject *ret = NULL;
    PyObject *args = NULL;

    if (!s || !s->updating || !s->alive) return;
    if (!boss_method) return; /* init_ai_methods();*/

    args = Py_BuildValue("(ff)(ii)(ii)s#", s->x, s->y-y_offset, tx, ty, bmp->w, VSCREEN_H, 
                                             s->state, strlen(s->state));
    if (!args) {
        Py_DECREF(args);
        allegro_message("can build arglist");
        return;
    }
    ret = PyEval_CallObject(boss_method, args);
    PyArg_ParseTuple(ret, "ffs", &tmpx, &tmpy, &tmps);
    if (tmps) {
        strncpy(s->state, tmps, NAME_LN);
    }
    s->dx = (double)tmpx;
    s->dy = (double)tmpy;

    free(tmps);
    tmps = NULL;
    Py_DECREF(ret);
}


void free_python(void)
{
    if (Py_IsInitialized()) {
        Py_DECREF(boss_method);
        boss_method = NULL;
        Py_DECREF(normal_enemy_method);
        normal_enemy_method = NULL;
        Py_Finalize();
    }
}



/* sprite_list_kill_updating: kills all the sprite that has
 * updating=1. Used when the player dies */
void sprite_list_kill_updating(SPRITE * plyer, SPRITE * sprite_list,
                               size_t sz)
{
    size_t i;
    if (!sprite_list || !plyer)
        return;
    for (i = 0; i < sz; i++) {
        if (sprite_list[i].updating && !sprite_list[i].is_boss
            && &sprite_list[i] != plyer) {
            sprite_list[i].alive = 0;
            sprite_list[i].corpse_tick = CORPSE_LIFE + 1;
        }
    }
}


/* get_trajectory: Gets a normalized vector from point1 to point2 and 
 * puts the delta x and y into dx, dy */
void get_trajectory(double x1, double y1, double x2, double y2, double *dx,
                    double *dy)
{
    double x, y;
    double len;

    x = x1 - x2;
    y = y1 - y2;
    len = sqrt(pow(x, 2.0) + pow(y, 2.0));

    /* normalize and set the return value */
    *dx = -((1 / len) * x);     /* (negate because the world is upside down) */
    *dy = -((1 / len) * y);
}


/* touching_sprite_list: checks if s is colliding with any of the sprites in
 * sprite_list.
 * It's a bit wierd - but it returns a pointer to the sprite it touches,
 * if it is 'free' it returns NULL*/
SPRITE *touching_sprite_list(SPRITE * s, SPRITE * sprite_list, size_t sz)
{
    size_t i;
    SPRITE *cur_s;

    assert(s);
    assert(sprite_list);

    for (i = 0; i < sz; i++) {
        cur_s = &sprite_list[i];
        if (cur_s == s)
            continue;

        if (rect_touching_rect
            (s->x, s->y + (s->h - s->collision_end_y), s->w,
             s->collision_end_y, cur_s->x,
             cur_s->y + (cur_s->h - cur_s->collision_end_y), cur_s->w,
             cur_s->collision_end_y)
            && cur_s->alive) {
            return cur_s;
        }
    }
    return NULL;
}

/* find_sprite: returns the first occurance of 'name' in the list. NULL if not
 * found atall. */
SPRITE *find_sprite(const char *name, SPRITE * sprite_list, size_t sz)
{
    size_t i;

    assert(name);
    assert(sprite_list);

    for (i = 0; i < sz; i++) {
        if (strncmp(sprite_list[i].long_name, name, LONG_NAME_LN) == 0) {
            /* found */
            return &sprite_list[i];
        }
    }
    /* not found */
    return NULL;
}

int sprite_list_boss_alive(SPRITE * sprite_list, size_t sz)
{
    size_t i;
    for (i = 0; i < sz; i++) {
        if (sprite_list[i].is_boss && sprite_list[i].corpse_tick < CORPSE_LIFE) {
            return 1;
        }
    }
    /* found no alive boss */
    return 0;
}

/* move_sprite: moves the sprite within a bmp and ontop of a map.
 * set custom_speed to -1 to use the sprite default speed.
 * Returns 0 if move failed, non-zero otherwise .*/
int move_sprite(BITMAP * bmp, SPRITE * s, MAP * m, SPRITE * sprite_list,
                size_t sz, double custom_speed)
{
    double spd;
    size_t w, h;
    double y;
    double oldx, oldy;
    int face_dir;
    if (!bmp || !s || !m || !s->alive || !s->updating)
        return 0;

    w = bmp->w;
    h = VSCREEN_H;
    oldx = s->x;
    oldy = s->y;

    if (custom_speed == -1) {
        spd = s->spd;
    } else {
        spd = (double) custom_speed;
    }

    /* get the new coords */
    s->x += s->dx * spd;
    s->y += s->dy * spd;
    face_dir = SOUTH;

    y = s->y - m->y_offset + (s->h - s->collision_end_y);
    /* check if new coords are within *bmp */
    if (0 <= s->x && s->x + s->w < w && 0 <= y && y + s->h < h) {
        /* check if new coords are on a passable tile in the map */
        if (rect_on_passable(s->x, y, s->w, s->collision_end_y, m)) {
            if (sprite_list) {
                if (touching_sprite_list(s, sprite_list, sz) == NULL) {
                    /* use the updated the coords */
                    return 1;
                }
            } else {
                return 1;
            }
        }
    }

    /* can't move - use old coords */
    s->x = oldx;
    s->y = oldy;
    return 0;
}


void animate_sprite(SPRITE *s, GFX_OBJ *gfx_list, size_t gfx_list_sz)
{
     char bmpname[NAME_LN];

    if (ANIM_DELAY < ++s->anim_delay) {
        if (s->alive) {
            s->anim_delay = 0;
            if (s->end_frame <= ++s->current_frame) {
                s->current_frame = s->begin_frame;
            }
            sprintf(bmpname, "%s%i", s->name, s->current_frame);
            s->bmp = search_gfx_list(bmpname, gfx_list, gfx_list_sz);
        } else {
            if (CORPSE_LIFE < s->corpse_tick) {
                s->updating = 0;
            } else
                s->corpse_tick++;
        }
    }

}



int spawn_sprite_if_on_screen(SPRITE *s, MAP *m) 
{
     if (s->y - m->y_offset > 0) {
          s->updating = 1;
          return 1;
     }
     s->updating = 0;
     return 0;
}



void update_enemy_sprite(SPRITE *s, SPRITE *player, BITMAP *bmp, MAP *m, SPRITE *sprite_list, size_t sz,
                         GFX_OBJ *gfx_list, size_t gfx_list_sz)
{
    assert(s);

    if (s->ai_update && ai_countdown < 0) {
        s->ai_update(s, player->x+(player->w/2), player->y+(player->h/2)-m->y_offset, bmp, m->y_offset);
        ai_countdown = AI_DELAY;
    } else {
        ai_countdown--;
    }
    sprite_list = sprite_list;
    sz = sz;
    move_sprite(bmp, s, m, s, 1, -1);
    animate_sprite(s, gfx_list, gfx_list_sz);
}



void update_player_sprite(SPRITE *s, SPRITE *player, BITMAP *bmp, MAP *m, SPRITE *sprite_list, size_t sz,
                          GFX_OBJ *gfx_list, size_t gfx_list_sz)
{
    /* check if the sprite are to 'spawn' */
    if (s->new_dir) {
        if (s->facing_dir == NORTH || s->facing_dir == SOUTH) {
            s->end_frame = s->frame_count;
            s->begin_frame = 0;
        } else if (s->facing_dir == EAST) {
            s->end_frame = s->frame_count * 2;
            s->begin_frame = s->frame_count;
        } else if (s->facing_dir == WEST) {
            s->end_frame = s->frame_count * 3;
            s->begin_frame = s->frame_count * 2;
        }
        s->current_frame = s->begin_frame;
        s->new_dir = 0;
    }
    player = player;
    s->dx = 0;
    s->dy = 0;
    move_sprite(bmp, s, m, sprite_list, sz, -1);
    animate_sprite(s, gfx_list, gfx_list_sz);
}



/* update_sprite_list: moves the npc's and plays the animations. */
void update_sprite_list(BITMAP * bmp, MAP * m, SPRITE *player,
                        SPRITE * sprite_list, size_t sz, GFX_OBJ *gfx_list, size_t gfx_list_sz)
{
    size_t i;
    SPRITE *s;

    if (!bmp || !m || !sprite_list) return;

    for (i = 0; i < sz; i++) {
        s = &sprite_list[i];
        if (!spawn_sprite_if_on_screen(s, m)) continue;

        if (s->updating && s->update_sprite) {
             s->update_sprite(s, player, bmp, m, sprite_list, sz, gfx_list, gfx_list_sz);
        }
    }
}



/* blit_sprite_list: blits all the sprites in the list onto the 'bmp' bitmap */
void blit_sprite_list(BITMAP * bmp, MAP * m, SPRITE * sprite_list,
                      size_t sprite_list_sz)
{
    size_t i;
    size_t y;
    char idx_name[NAME_LN];
    SPRITE *s;
    double d;

    if (!bmp || !sprite_list || sprite_list_sz == 0)
        return;

    /* first draw all the 'dead' sprites in order of apperance */
    for (y = 0; y < HEIGHT; y++) {
        for (i = 0; i < sprite_list_sz; i++) {
            s = &sprite_list[i];
            d = (s->y - m->y_offset + (s->h - s->collision_end_y)) -
                (double) y;
            if (!s->updating || s->alive)
                continue;
            if (0 <= d && d <= 1) {
                draw_sprite(bmp, s->corpse_bmp, s->x, s->y - m->y_offset);
            }
        }
    }

    /* now the alive sprites in order of apperance */
    for (y = 0; y < HEIGHT; y++) {
        for (i = 0; i < sprite_list_sz; i++) {
            s = &sprite_list[i];
            if (!s->updating || !s->alive)
                continue;

            d = (s->y - m->y_offset + (s->h - s->collision_end_y)) -
                (double) y;
            if (0 <= d && d <= 1) {
                sprintf(idx_name, "%s%i", s->name, s->current_frame);

                /* the "shadow"... :D */
                ellipsefill(bmp, s->x + (s->w / 2), s->y + s->h - m->y_offset,
                            s->w / 3, 4, makecol(20, 20, 20));

                /* the sprite */
                draw_sprite(bmp, s->bmp, s->x, s->y - m->y_offset);
            }
        }
    }
}

/* create_sprite: init a sprite */
SPRITE *create_sprite(void)
{
    SPRITE *s = NULL;
    s = malloc(sizeof(*s));
    if (!s) {
        return NULL;
    }
    s->alive = 0;
    s->facing_dir = 0;
    s->done = 0;
    s->movement_type = MV_NORMAL_ENEMY;
    s->lifetime = LIFETIME;
    s->time = 0;
    s->returning = 0;
    s->dx = s->dy = 0.0f;
    s->decision_time = AI_DELAY;
    s->decision_tick = 0;
    s->corpse_tick = 0;
    return s;
}

/* load_gfx_for_sprite: loads all bitmaps from a sprite-sheet into a
 * gfx_list. */
GFX_OBJ *load_sprite_gfx(SPRITE * s, GFX_OBJ * gfx_list, size_t * sz)
{
    size_t i;
    size_t columns, frames;
    BITMAP *frame = NULL;
    BITMAP *spritesheet = NULL;
    char filename[LONG_NAME_LN];
    char idx_name[NAME_LN];

    sprintf(filename, "%s%s.bmp", SPRITE_GFX, s->name);
    spritesheet = load_bitmap(filename, NULL);

    columns = spritesheet->w / s->w;
    frames = columns * (spritesheet->h / s->h);

    if (frames % columns != 0) {
        allegro_message("Error in load_sprit_gfx: %s", filename);
        return gfx_list;
    }

    for (i = 0; i < frames; i++) {
        frame = grab_frame(spritesheet, s->w, s->h, columns, i);
        sprintf(idx_name, "%s%d", s->name, i);
        gfx_list = append_bmp_to_gfx_list(idx_name, frame, gfx_list, sz);
    }

    return gfx_list;
}

/* update_sprite_data: helper function to the helper
 * function complete_sprite_list. Updates 'dest' with all but the long_name
 * and the position of the sprite with data from 'src' */
void update_sprite_data(SPRITE * dest, SPRITE * src)
{
    if (dest == NULL || src == NULL) {
        return;
    }

    dest->bmp = src->bmp;
    dest->corpse_bmp = src->corpse_bmp;
    strncpy(dest->name, src->name, NAME_LN);
    dest->w = src->w;
    dest->h = src->h;
    dest->sprite_sheet_width = src->sprite_sheet_width;
    dest->corpse_frame = src->corpse_frame;
    dest->frame_count = src->frame_count;
    dest->current_frame = src->current_frame;
    dest->dir = src->dir;
    dest->spd = src->spd;
    dest->money_drop_max = src->money_drop_max;
    dest->money_drop_min = src->money_drop_min;
    dest->health = src->health;
    dest->wpn = src->wpn;
    dest->damage = src->damage;
    dest->fire_dir = src->fire_dir;
    dest->fire_clicks = src->fire_clicks;
    dest->cur_click = src->cur_click;
    dest->updating = src->updating;
    dest->alive = 1;

    dest->facing_dir = src->facing_dir;
    dest->new_dir = src->new_dir;
    dest->begin_frame = src->begin_frame;
    dest->end_frame = src->end_frame;
    dest->collision_end_y = src->collision_end_y;

    dest->lifetime = src->lifetime;
    dest->time = src->time;
    dest->returning = src->returning;
    dest->movement_type = src->movement_type;
    dest->done = src->done;
    dest->dx = src->dy;
    dest->dy = src->dy;
    dest->decision_time = src->decision_time;
    dest->decision_tick = src->decision_tick;
    dest->corpse_tick = src->corpse_tick;
    dest->is_boss = src->is_boss;


    dest->update_sprite = src->update_sprite;
    dest->ai_update = src->ai_update;
    strncpy(dest->state, src->state, NAME_LN);

}

/* complete_sprite_list: updates all the sprites outlined by load_sprite_list with
 * all the data needed from spriteindex. It also loads all the bitmaps for all the
 * sprites and adds them to the gfx_list. Note that this must be used
 * (preferably) directly after load_sprite_list! */
GFX_OBJ *complete_sprite_list(SPRITE * sprite_list, size_t sprite_list_sz,
                              GFX_OBJ * gfx_list, size_t * gfx_list_sz)
{
    size_t i, j;
    scew_tree *t = NULL;
    scew_parser *p = NULL;
    scew_element *e = NULL;
    scew_element *c = NULL;
    scew_element *r = NULL;
    SPRITE *s = NULL;
    char bmpname[NAME_LN];

    p = scew_parser_create();
    if (!scew_parser_load_file(p, SPRITE_INDEX)) {
        printf("%s\n", scew_error_expat_string(scew_error_code()));
        return NULL;
    }

    t = scew_parser_tree(p);
    r = scew_tree_root(t);
    s = create_sprite();

    for (i = 0; i < sprite_list_sz; i++) {
        if (!sprite_list[i].alive) {
            /* we have a sprite that is not loaded */
            e = scew_element_by_name(r, sprite_list[i].long_name);
            strncpy(s->long_name, sprite_list[i].long_name, LONG_NAME_LN);
            c = scew_element_by_name(e, "ShortName");
            strncpy(s->name, scew_element_contents(c), NAME_LN);
            c = scew_element_by_name(e, "Width");
            s->w = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "Height");
            s->h = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "SpriteSheetWidth");
            s->sprite_sheet_width = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "MoveFrameCount");
            s->frame_count = atoi(scew_element_contents(c));
            s->begin_frame = 0;
            s->end_frame = s->frame_count;
            c = scew_element_by_name(e, "CorpseFrame");
            s->corpse_frame = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "MoneyDropMax");
            s->money_drop_max = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "MoneyDropMin");
            s->money_drop_min = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "Health");
            s->health = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "WeaponType");
            s->wpn = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "BulletDamage");
            s->damage = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "DefaultMovementDir");
            s->dir = atoi(scew_element_contents(c));
            s->facing_dir = s->dir;
            s->new_dir = 0;
            c = scew_element_by_name(e, "FireDir");
            s->fire_dir = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "FireClicks");
            s->fire_clicks = atoi(scew_element_contents(c));
            c = scew_element_by_name(e, "MovementSpeed");
            s->spd = atof(scew_element_contents(c));
            c = scew_element_by_name(e, "CollisionHeight");
            s->collision_end_y = atof(scew_element_contents(c));
            get_trajectory(s->x + (s->w / 2), s->y + (s->h / 2), VSCREEN_W / 2,
                           VSCREEN_H, &s->dx, &s->dy);

            c = scew_element_by_name(e, "IsBoss");
            s->is_boss = atoi(scew_element_contents(c));

            c = scew_element_by_name(e, "DefaultState");
            strncpy(s->state, scew_element_contents(c), NAME_LN);



            s->cur_click = s->fire_clicks;
            s->current_frame = 0;
            s->anim_delay = 0;
            s->updating = 0;


            if (strncmp(s->name, PLAYER_NAME, NAME_LN) != 0) {
                s->update_sprite = &update_enemy_sprite;
                
                if (s->is_boss) {
                    s->ai_update = &boss_normal_movement;
                } else if (s->movement_type == MV_NORMAL_ENEMY) {
                    s->ai_update = &enemy_normal_movement;
                } else if (s->movement_type == MV_NORMAL_ENEMY){
                    s->ai_update = NULL;
                } 
            } else {
                /* player */
                s->update_sprite = &update_player_sprite;
                s->ai_update = NULL;
            }

            /* now... for all sprites of this type */
            for (j = 0; j < sprite_list_sz; j++) {
                if (!sprite_list[j].alive &&
                    strncmp(sprite_list[j].long_name, s->long_name,
                            LONG_NAME_LN) == 0) {
                    /* right kind of sprite and not complete */
                    update_sprite_data(&sprite_list[j], s);
                }
            }
            /* and now we load all the gfx for the sprite */
            gfx_list = load_sprite_gfx(s, gfx_list, gfx_list_sz);
        }
    }

    for (i=0; i<sprite_list_sz; i++) {
         s = &sprite_list[i];
         sprintf(bmpname, "%s0", s->name);
         s->bmp = search_gfx_list(bmpname, gfx_list, *gfx_list_sz);

         sprintf(bmpname, "%s%i", s->name, s->corpse_frame);
         s->corpse_bmp = search_gfx_list(bmpname, gfx_list, *gfx_list_sz);
    }

    /* doesn't work for some reason...? */
    /* scew_element_free(r); */
    scew_element_free(c);
    scew_element_free(e);
    scew_parser_free(p);
    destroy_sprite(s);
    return gfx_list;
}

/* load_sprite_list: loads all the sprites from a map into an array
 * note that the sprite_list must be set to NULL and sz to 0, else the
 * loading will fail! */
SPRITE *load_sprite_list(char *filename, SPRITE * sprite_list,
                         size_t * sprite_list_sz)
{
    scew_tree *t = NULL;
    scew_parser *p = NULL;
    scew_element *e = NULL;
    scew_element *r = NULL;
    scew_attribute *a = NULL;

    SPRITE *s = NULL;

    assert(filename);
    if (sprite_list != NULL && *sprite_list_sz != 0) {
        free(sprite_list);
        return NULL;
    }

    p = scew_parser_create();
    scew_parser_ignore_whitespaces(p, 1);
    if (!scew_parser_load_file(p, filename)) {
        printf("%s\n", scew_error_expat_string(scew_error_code()));
        return NULL;
    }
    t = scew_parser_tree(p);
    r = scew_element_by_name(scew_tree_root(t), "Sprites");
    e = scew_element_next(r, e);
    s = create_sprite();

    while (e != NULL) {
        a = scew_attribute_by_name(e, "x");
        s->x = atof(scew_attribute_value(a));
        a = scew_attribute_by_name(e, "y");
        s->y = atof(scew_attribute_value(a));
        a = scew_attribute_by_name(e, "name");
        strncpy(s->long_name, (char *) scew_attribute_value(a), LONG_NAME_LN);
        sprite_list = append_sprite_list(s, sprite_list, sprite_list_sz);
        e = scew_element_next(r, e);
    }

    /* cleanup */
    scew_attribute_free(a);
    /* doesn't work for some reason...?
     * scew_element_free(r); */
    scew_element_free(e);
    scew_parser_free(p);
    destroy_sprite(s);

    return sprite_list;
}

/* append_sprite_list: used to create a outline for a spritelist. This only
 * sets the position and the name of the sprite (the data available in
 * the mapfile). Use complete_sprite_list to load the rest of the data from
 * the sprite index */
SPRITE *append_sprite_list(SPRITE * s, SPRITE * sprite_list, size_t * sz)
{
    if (sprite_list == NULL) {
        sprite_list = malloc(sizeof(*sprite_list));
        *sz = 0;
    } else {
        sprite_list = realloc(sprite_list, (*sz + 1) * sizeof(*sprite_list));
    }

    sprite_list[*sz].x = s->x;
    sprite_list[*sz].y = s->y;
    sprite_list[*sz].alive = 0;
    strncpy(sprite_list[*sz].long_name, s->long_name, LONG_NAME_LN);

    *sz = *sz + 1;
    return sprite_list;
}

/* destroy_sprite: free sprite */
void destroy_sprite(SPRITE * s)
{
    if (s == NULL)
        return;
    else
        free(s);
}

/* destroy_sprite_list: free alot of sprites remember you still own the gfxlst*/
void destroy_sprite_list(SPRITE * sprite_list, size_t * sz)
{
    if (sprite_list == NULL || *sz == 0) {
        return;
    }
    free(sprite_list);
    sprite_list = NULL;
    *sz = 0;
}
