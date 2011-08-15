#include "bullet.h"

void kill_bullet_list(BULLET * bullet_list, size_t sz)
{
    size_t i;
    if (!bullet_list)
        return;
    for (i = 0; i < sz; i++) {
        if (bullet_list[i].alive) {
            bullet_list[i].alive = 0;
        }
    }
}

/* fire: adds a bullet to a bulletlist, origin from the sprite pos */
BULLET *fire(SPRITE * s, SPRITE * target, BULLET * bullet_list,
             size_t * sz, BITMAP * bmp)
{
    int wpn;

    int dy;
    int x1, x2;

    double tmp = 0;
    double ty = 0;
    size_t d1, d2, d3;          /* temp vars for fire-dirs. Can max fire 3 bullet */
    assert(s);

    wpn = s->wpn;

    if (s->fire_dir != SOUTH) {
        dy = -BULLET_H;
    } else {
        dy = BULLET_H + s->h;
    }

    if (target) {
        get_trajectory(s->x, s->y, target->x, target->y, &tmp, &ty);
        if (ty > 0) {
            dy = BULLET_H + s->h;
        } else {
            dy = -BULLET_H;
        }
    }

    /* parse the different weapons */
    if (wpn == RIFLE) {
        if (target) {
            bullet_list =
                add_bullet(s->x + (s->w / 2), s->y + dy, target, s->damage,
                           s->fire_dir, bmp, bullet_list, sz);
        }
    } else if (wpn == GUNS) {
        /* only player can fire this weapontype */
        if (s->fire_dir == EAST) {
            d1 = NORTHNORTHEAST;
            d2 = NORTHEAST;
            x1 = s->x + (s->w / 2);
            x2 = s->x + (s->w);
        } else if (s->fire_dir == WEST) {
            d1 = NORTHWEST;
            d2 = NORTHNORTHWEST;
            x1 = s->x;
            x2 = s->x + (s->w / 2);
        } else {
            d1 = NORTH;
            d2 = NORTH;
            x1 = s->x + (BULLET_W / 2);
            x2 = s->x + s->w - (BULLET_W / 2);
        }
        bullet_list =
            add_bullet(x1, s->y + dy, NULL, s->damage, d1, bmp,
                       bullet_list, sz);
        bullet_list =
            add_bullet(x2, s->y + dy, NULL, s->damage, d2,
                       bmp, bullet_list, sz);
    } else if (wpn == SHOTGUN) {
        if (s->fire_dir == NORTH) {
            d1 = NORTHWEST;
            d2 = NORTHEAST;
            d3 = NORTH;
        } else if (s->fire_dir == SOUTH) {
            d1 = SOUTHWEST;
            d2 = SOUTHEAST;
            d3 = SOUTH;
        } else
            return bullet_list;
        bullet_list = add_bullet(s->x, s->y + dy, NULL, s->damage, d1, bmp,
                                 bullet_list, sz);
        bullet_list =
            add_bullet(s->x + (s->w), s->y + dy, NULL, s->damage, d2, bmp,
                       bullet_list, sz);
        bullet_list =
            add_bullet(s->x + (s->w / 2), s->y + dy, NULL, s->damage, d3,
                       bmp, bullet_list, sz);
    }

    return bullet_list;
}

/* damage: deals the damage of the bullet to the sprite - killing if
 * health goes <= 0 */
void damage(SPRITE * s, BULLET * b)
{
    assert(s);
    assert(b);

    if (b->alive) {
        s->health -= b->damage;
        b->alive = 0;
        if (s->health <= 0) {
            s->alive = 0;
            score += s->money_drop_max;
        }
    }
}

/* blit_bullets: blit the whole bullet-list to a bitmap */
void blit_bullets(BITMAP * bmp, MAP * m, BULLET * bullet_list, size_t sz)
{
    size_t i;
    BULLET *b = NULL;

    if (!bullet_list || !bmp || !m)
        return;

    for (i = 0; i < sz; i++) {
        b = &bullet_list[i];
        if (b->alive)
            draw_sprite(bmp, b->bmp, b->x, b->y - m->y_offset);
    }
}

/* move_bullet: moves a given bullet in it's direction. With
 * collision detectin. */
void move_bullet(BITMAP * bmp, BULLET * b, MAP * m, SPRITE * sprite_list,
                 size_t sprite_list_sz)
{
    int dir;
    size_t w, h;
    SPRITE s;
    SPRITE *hit_sprite = NULL;
    if (!bmp || !b || !m || !b->alive)
        return;

    w = bmp->w;
    h = bmp->h;
    dir = b->dir;

    if (b->dir == -1) {
        b->x += b->dx * 2 * BULLET_SPEED;
        b->y += b->dy * 2 * BULLET_SPEED;
    } else {
        /* get the new coords */
        if (dir == NORTH) {
            b->y -= 2 * BULLET_SPEED;
        } else if (dir == NORTHEAST) {
            b->y -= 2 * BULLET_SPEED;
            b->x += 2 * BULLET_SPEED;
        } else if (dir == NORTHNORTHEAST) {
            b->y -= 2 * BULLET_SPEED;
            b->x += BULLET_SPEED;
        } else if (dir == EAST) {
            b->x += BULLET_SPEED;
        } else if (dir == SOUTHEAST) {
            b->y += BULLET_SPEED;
            b->x += BULLET_SPEED;
        } else if (dir == SOUTH) {
            b->y += BULLET_SPEED;
        } else if (dir == SOUTHWEST) {
            b->y += 2 * BULLET_SPEED;
            b->x -= BULLET_SPEED;
        } else if (dir == WEST) {
            b->x -= BULLET_SPEED;
        } else if (dir == NORTHWEST) {
            b->y -= 2 * BULLET_SPEED;
            b->x -= 2 * BULLET_SPEED;
        } else if (dir == NORTHNORTHWEST) {
            b->y -= 2 * BULLET_SPEED;
            b->x -= BULLET_SPEED;
        } else {
            return;
        }
    }

    /* check if new coords are outside *bmp */
    if (!(0 <= b->x && b->x + BULLET_W < w &&
          0 <= b->y - m->y_offset && b->y - m->y_offset + BULLET_H < h)) {
        b->alive = 0;
        return;
    }
    /* check if new coords are on a non-passable tile in the map */
    if (!rect_on_passable(b->x, b->y - m->y_offset, BULLET_W, BULLET_H, m)) {
        b->alive = 0;
        return;
    }

    /* ok, bullet is free - check if it is touching a sprite,
     * ugly... but make a sprite of the bullet so I can use the
     * touching sprite fun - which only uses the x, y, w and h of the sprite */
    s.x = b->x;
    s.y = b->y;
    s.w = BULLET_W;
    s.h = BULLET_H;
    s.collision_end_y = s.h;
    hit_sprite = touching_sprite_list(&s, sprite_list, sprite_list_sz);
    if (hit_sprite && b->alive) {
        /* wohoo - the bullet scored a hit! */
        damage(hit_sprite, b);
    }
}

/* update_bullets: moves the alive bullets forward */
void update_bullets(BULLET * bullet_list, size_t * bullet_list_sz,
                    BITMAP * bmp, MAP * m, SPRITE * sprite_list,
                    size_t sprite_list_sz)
{
    size_t i;
    BULLET *b = NULL;

    if (!bullet_list || !bmp || !m || !sprite_list)
        return;

    for (i = 0; i < *bullet_list_sz; i++) {
        b = &bullet_list[i];
        if (BULLET_FLY_TIME < b->flight_time) {
            b->alive = 0;
        } else {
            b->flight_time++;
        }
        if (b->alive) {
            move_bullet(bmp, b, m, sprite_list, sprite_list_sz);
        }
    }
}


/*add_bullet: add a bullet to the bullet list */
BULLET *add_bullet(size_t x, size_t y, SPRITE * target, int damage,
                   int dir, BITMAP * bmp, BULLET * bullet_list, size_t * sz)
{
    size_t i;
    BULLET *b = NULL;

    if (!bmp)
        return bullet_list;

    if (!bullet_list) {
        /* create a small bulletlist */
        bullet_list = malloc(DEFAULT_BULLETLIST_SIZE * sizeof(*bullet_list));
        for (i=0; i<DEFAULT_BULLETLIST_SIZE; i++) {
            bullet_list[i].alive = 0;
        }       
        *sz = DEFAULT_BULLETLIST_SIZE;
    }

    /* check if we can find a dead bullet to overwrite! */
    if (bullet_list) {
        for (i = 0; i < *sz; i++) {
            b = &bullet_list[i];
            if (!b->alive) {
                /* got a dead one */
                b->x = (double) x;
                b->y = (double) y;
                b->damage = damage;
                b->dir = dir;
                b->bmp = bmp;
                b->alive = 1;
                b->flight_time = 0;
                if (target) {
                    get_trajectory(b->x, b->y, target->x + (target->w / 2),
                                   target->y + (target->h -
                                                target->collision_end_y),
                                   &b->dx, &b->dy);
                    b->dir = -1;
                } else {
                    b->dx = b->dy = -10.0f;
                }
                b = NULL;
                return bullet_list;
            }
        }
    }

    /* no dead bullets - need to make a new one */
    b = NULL;
    if (!bullet_list) {
        /* this shouldn't happen...*/
        bullet_list = malloc(sizeof(*bullet_list));
        *sz = 0;
    } else {
        bullet_list = realloc(bullet_list, (*sz + 1) * sizeof(*bullet_list));
    }
    b = &bullet_list[*sz];
    b->x = x;
    b->y = y;
    b->damage = damage;
    b->dir = dir;
    b->bmp = bmp;
    b->alive = 1;
    if (target) {
        get_trajectory(b->x, b->y, target->x, target->y, &b->dx, &b->dy);
        b->dir = -1;
    } else {
        b->dx = b->dy = -10.0f;
    }

    *sz = *sz + 1;
    return bullet_list;
 }

/* sprite_list_fire_guns: for all sprites in the list, check if it is
 * their time to fire their guns. If so - add their bullets */
BULLET *sprite_list_fire_guns(SPRITE * target, SPRITE * sprite_list,
                              size_t sprite_list_sz, BULLET * bullet_list,
                              size_t * bullet_list_sz, BITMAP * bullet_bmp)
{
    size_t i;
    SPRITE *s = NULL;

    if (!sprite_list || !bullet_bmp || !target)
        return bullet_list;

    for (i = 0; i < sprite_list_sz; i++) {
        s = &sprite_list[i];
        if (!s->alive || !s->updating || s->wpn == UNARMED) {
            continue;
        }
        if (s != player) {
            if (s->cur_click <= 0) {
                bullet_list =
                    fire(s, target, bullet_list, bullet_list_sz, bullet_bmp);
                s->cur_click = s->fire_clicks;
            } else {
                s->cur_click--;
            }
        }
    }
    return bullet_list;
}
