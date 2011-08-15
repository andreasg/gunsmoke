#include "mapeditor.h"

int draw_grid = 1;
char key_down[KEY_MAX] = { };
int left_down = 0;
int middle_down = 0;
int right_down = 0;
int mode = TILE_MODE;
LIST_ELEMENT *selection = NULL;
LIST_ELEMENT *spr_elem_lst = NULL;
size_t spr_elem_lst_sz = 0;
SPRITE *sprite_list = NULL;
size_t sprite_list_sz = 0;
LIST_ELEMENT *tile_elem_lst = NULL;
size_t tile_elem_lst_sz = 0;
GFX_OBJ *gfx_list = NULL;
size_t gfx_list_sz = 0;
BITMAP *vscreen = NULL;
scew_tree *maptree = NULL;
int map_offset = 0;
int close_program = 0;
GRID_ITEM grid_item;

/* get_grid_item: updates the grid_item with fresh data */
void get_grid_item(void)
{
    int x, y;
    scew_attribute *a = NULL;
    scew_element *e = NULL;
    scew_element *sub_e = NULL;
    scew_element *r = NULL;

    POINT p;

    if (!maptree)
        return;

    p = get_mouse_pos();
    x = p.x - (p.x % TILE_W);
    y = p.y - (p.y % TILE_H) + map_offset;

    r = scew_tree_root(maptree);

    if (mode == TILE_MODE) {
        e = scew_element_by_name(r, "Tiles");
    } else if (mode == SPRITE_MODE) {
        e = scew_element_by_name(r, "Sprites");
    } else
        return;

    sub_e = scew_element_next(e, sub_e);

    while (sub_e) {
        a = scew_attribute_by_name(sub_e, "x");
        if (x == atoi(scew_attribute_value(a))) {
            a = scew_attribute_by_name(sub_e, "y");
            if (y == atoi(scew_attribute_value(a))) {
                grid_item.x = x;
                grid_item.y = y;
                grid_item.mode = mode;

                a = scew_attribute_by_name(sub_e, "name");
                strncpy(grid_item.name, scew_attribute_value(a), NAME_LN);

                if (mode == TILE_MODE) {
                    a = scew_attribute_by_name(sub_e, "passable");
                    grid_item.passable = atoi(scew_attribute_value(a));
                } else {
                    grid_item.passable = -1;
                }
                break;
            }
        }
        sub_e = scew_element_next(e, sub_e);
    }
}

/* init: init's the element lists and allegro, returns 0 upon success
 * and non-zero otherwise */
int init(void)
{
    /* init allegro */
    if (allegro_init() != 0) {
        printf("failed to initialize...");
        return 1;
    }

    set_color_depth(GFX_DEPTH);
    if (set_gfx_mode(GFX_AUTODETECT_WINDOWED,
                     EDITOR_WIN_W, EDITOR_WIN_H, 0, 0) != 0) {
        set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
        allegro_message("Failed to init a gfxmode.");
        return 1;
    }
    set_window_title("GUNPOWDER MapEditor");

    install_keyboard();
    install_mouse();
    show_mouse(screen);

    vscreen = create_bitmap(EDITOR_WIN_W, EDITOR_WIN_H);
    if (!vscreen) {
        close_program = 1;
    }

    /* init our lists */
    load_tiles();
    load_sprites();

    return 0;
}

/* load_tiles: for all tiles, load it's bitmap and create a item in the
 * element list so we can select it and add it to a map */
void load_tiles(void)
{
    size_t len, old_sz;
    size_t y;
    int passable;
    char buffer[NAME_LN];
    char tile_filename[NAME_LN];
    BITMAP *bmp = NULL;
    FILE *fsock = NULL;

    if (tile_elem_lst)
        return;                 /*already lodad */

    fsock = fopen(TILE_LIST, "rt");
    if (!fsock)
        return;

    while (!feof(fsock)) {
        /* get a filename from the list */
        /*fgets(buffer, NAME_LN, fsock);*/
        fscanf(fsock, "%s %i\n", buffer, &passable); 
        len = strlen(buffer);

        /* remove the newline */
        if (buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';

        /* build the filename and load the bitmap */
        sprintf(tile_filename, "%s%s%s", WORLD_GFX_DIR, buffer, GFX_EXT);
        bmp = load_bitmap(tile_filename, NULL);

        /* save the size so we can see if it's really appended */
        old_sz = gfx_list_sz;
        gfx_list = append_bmp_to_gfx_list(buffer, bmp, gfx_list, &gfx_list_sz);
        if (old_sz == gfx_list_sz) {
            /* the bitmap wasn't appended, destroy it */
            destroy_bitmap(bmp);
            continue;
        }

        /* build the coordinate for the new item */
        if (tile_elem_lst_sz == 0) {
            y = TILE_WIN.y;
        } else {
            y = TILE_WIN.y + tile_elem_lst_sz * LIST_ELEM_H;
        }

        /* finally add the list element to the list */
        tile_elem_lst = add_list_element(rec(TILE_WIN.x, y, LIST_ELEM_W,
                                             LIST_ELEM_H), passable,  
                                         DEFAULT_COLOR, SEL_COLOR, buffer, 
                                         bmp, tile_elem_lst,
                                         &tile_elem_lst_sz);
    }
    fclose(fsock);
}

/* load_sprite: load all the sprites in SPRITE_INDEX and their graphics */
void load_sprites(void)
{
    size_t i, y;
    char bmpname[NAME_LN];
    scew_tree *t = NULL;
    scew_parser *p = NULL;
    scew_element *e = NULL;
    scew_element *r = NULL;
    SPRITE *s = NULL;

    /* setup the parser */
    p = scew_parser_create();
    scew_parser_ignore_whitespaces(p, 1);
    if (!scew_parser_load_file(p, SPRITE_INDEX)) {
        printf("%s\n", scew_error_expat_string(scew_error_code()));
        return;
    }
    t = scew_parser_tree(p);
    r = scew_tree_root(t);
    s = create_sprite();

    /* read all the sprites from SPRITE_INDEX */
    /* parse the SPRITE_INDEX file and outline the list with their names */
    e = scew_element_next(r, e);
    while (e) {
        strncpy(s->long_name, (char *) scew_element_name(e), LONG_NAME_LN);
        s->x = s->y = 0;
        sprite_list = append_sprite_list(s, sprite_list, &sprite_list_sz);
        e = scew_element_next(r, e);
    }

    /* load all their graphics and the sprite data */
    gfx_list = complete_sprite_list(sprite_list, sprite_list_sz, gfx_list,
                                    &gfx_list_sz);

    /* create a list_element for each of them and put them in a list */
    y = SPRITE_WIN.y;
    for (i = 0; i < sprite_list_sz; i++) {
        sprintf(bmpname, "%s0", sprite_list[i].name);
        spr_elem_lst = add_list_element(rec(SPRITE_WIN.x, y, LIST_ELEM_W,
                                            LIST_ELEM_H), 0, DEFAULT_COLOR,
                                        SEL_COLOR, sprite_list[i].name,
                                        search_gfx_list(bmpname, gfx_list,
                                                        gfx_list_sz),
                                        spr_elem_lst, &spr_elem_lst_sz);
        y += LIST_ELEM_H;
    }

    /* cleanup */
    scew_element_free(r);
    destroy_sprite(s);
    scew_element_free(e);
    scew_parser_free(p);
}

/* cleanup: cleans our lists */
void cleanup(void)
{
    destroy_gfx_list(gfx_list, &gfx_list_sz);
    destroy_list_element_list(tile_elem_lst, &tile_elem_lst_sz);
    destroy_list_element_list(spr_elem_lst, &spr_elem_lst_sz);
}

/* parse_leftmouse_on_map: what happens when we leftclick the map?
 * Read below!! */
void parse_leftmouse_on_map(void)
{
    POINT p;
    size_t x, y;

    p = get_mouse_pos();

    if (selection) {
        x = p.x - (p.x % TILE_W);
        y = p.y - (p.y % TILE_H) + map_offset;

        if (mode == TILE_MODE) {
            /* add the tile as non-passable */
            modify_map_coord(x, y, selection->passable, selection->name, 0, 0, maptree);

            /* we wanna be able to draw on the map without clicking on
             * each square */
            left_down = 0;
        } else if (mode == SPRITE_MODE) {
            modify_map_coord(x, y, -1, selection->name, 1, 0, maptree);
        }
    } else {
        /* we should do some fun here :P */
    }

}

/* parse_rightmouse_on_map: what happens upon rightclick on map */
void parse_rightmouse_on_map(void)
{
    if (mode == TILE_MODE) {
        modify_map_coord(grid_item.x, grid_item.y, !grid_item.passable,
                         grid_item.name, 0, 0, maptree);
    } else if (mode == SPRITE_MODE) {
        modify_map_coord(grid_item.x, grid_item.y, -1, grid_item.name, 1, 1,
                         maptree);
    }
}

/* parse_leftclick: the mouse was leftclicked */
void parse_leftclick(void)
{
    LIST_ELEMENT *old_selection = selection;
    if ((mouse_b & 1) && !left_down) {
        /* mouse clicked */
        left_down = 1;

        if (point_in_rec(get_mouse_pos(), TILE_WIN)) {
            /* within the TILE_LIST */
            selection = list_element_clicked(get_mouse_pos(), tile_elem_lst,
                                             tile_elem_lst_sz);
            if (selection) {
                /* select in this list and disselect in the other list */
                set_pressed_element(selection, tile_elem_lst,
                                    tile_elem_lst_sz);
                set_pressed_element(selection, spr_elem_lst, spr_elem_lst_sz);
                /*set_mouse_sprite(selection->bmp); */
                mode = TILE_MODE;
            } else {
                selection = old_selection;
                old_selection = NULL;
            }
        } else if (point_in_rec(get_mouse_pos(), SPRITE_WIN)) {
            /* within SPRITE_LIST */
            selection = list_element_clicked(get_mouse_pos(), spr_elem_lst,
                                             spr_elem_lst_sz);
            if (selection) {
                /* select in this list and disselect in the other list */
                set_pressed_element(selection, spr_elem_lst, spr_elem_lst_sz);
                set_pressed_element(selection, tile_elem_lst,
                                    tile_elem_lst_sz);
                /*set_mouse_sprite(selection->bmp); */
                mode = SPRITE_MODE;
            } else {
                selection = old_selection;
                old_selection = NULL;
            }
        } else if (point_in_rec(get_mouse_pos(), MAP_WIN)) {
            /* mouse clicked on the map! */
            parse_leftmouse_on_map();
        } else if (point_in_rec(get_mouse_pos(), SPRITE_FRW)) {
            list_elem_switch_page(1, SPRITE_WIN.y, spr_elem_lst,
                                  spr_elem_lst_sz);
        } else if (point_in_rec(get_mouse_pos(), SPRITE_REW)) {
            list_elem_switch_page(0, SPRITE_WIN.y, spr_elem_lst,
                                  spr_elem_lst_sz);
        } else if (point_in_rec(get_mouse_pos(), TILE_FRW)) {
            list_elem_switch_page(1, TILE_WIN.y, tile_elem_lst,
                                  tile_elem_lst_sz);
        } else if (point_in_rec(get_mouse_pos(), TILE_REW)) {
            list_elem_switch_page(0, TILE_WIN.y, tile_elem_lst,
                                  tile_elem_lst_sz);
        } else {
            set_mouse_sprite(NULL);
        }
    } else if (!(mouse_b & 1) && left_down) {
        /* reset mouse */
        left_down = 0;
    }
}

/* parse_middleclick: use this btn as disselect */
void parse_middleclick(void)
{
    if ((mouse_b & 4) && !middle_down) {
        set_pressed_element(NULL, tile_elem_lst, tile_elem_lst_sz);
        set_pressed_element(NULL, spr_elem_lst, spr_elem_lst_sz);
        /*selection = NULL; */
        set_mouse_sprite(NULL);
        middle_down = 1;
    } else if (!(mouse_b & 4) && middle_down) {
        middle_down = 0;
    }
}

/* parse_rightclick: the mouse was rightclicked */
void parse_rightclick(void)
{
    if ((mouse_b & 2) && !right_down) {
        parse_rightmouse_on_map();
        right_down = 1;
    } else if (!(mouse_b & 2) && right_down) {
        right_down = 0;
    }
}

/* parse_mouse: parses the mouse */
void parse_mouse(void)
{
    parse_leftclick();
    parse_middleclick();
    parse_rightclick();
}

/* parse_keyboard: get keyboard input */
void parse_keyboard(void)
{
    size_t k;

    if (key[KEY_ESC] && !key_down[KEY_ESC]) {
        really_quit();
        key_down[KEY_ESC] = 1;
    }

    if (key[KEY_F1] && !key_down[KEY_F1]) {
        allegro_message
            ("ESC: quit and save\nG: Hide/Show grid\nRightMouse: Delete sprite in sprite-mode.\nRightMouse: Toggle passable in tile-mode\nUpArrow: Scroll up\nPgUp: Scroll up fast\nDownArrow: Scroll down\nPgDown: Scroll down fast");
    }

    if (key[KEY_G] && !key_down[KEY_G]) {
        draw_grid = !draw_grid;
        key_down[KEY_G] = 1;
        return;
    }

    if (key[KEY_UP] && !key_down[KEY_UP]) {
        map_offset -= SCROLL_SPEED * TILE_H;
        key_down[KEY_UP] = 1;
    }
    if (key[KEY_DOWN] && !key_down[KEY_DOWN]) {
        map_offset += SCROLL_SPEED * TILE_H;
        key_down[KEY_DOWN] = 1;
    }
    if (key[KEY_PGUP] && !key_down[KEY_PGUP]) {
        map_offset -= SCROLL_SPEED * 2 * TILE_H;
        key_down[KEY_PGUP] = 1;
    }
    if (key[KEY_PGDN] && !key_down[KEY_PGDN]) {
        map_offset += SCROLL_SPEED * 2 * TILE_H;
        key_down[KEY_PGDN] = 1;
    }

    /* reset key_down */
    for (k = 0; k < KEY_MAX; k++) {
        if (!key[k] && key_down[k])
            key_down[k] = 0;
    }
}

/* upadate_screen: blit virtualscreen to screen */
void update_screen(void)
{
    assert(vscreen);
    /* output */
    acquire_screen();
    blit(vscreen, screen, 0, 0, 0, 0, EDITOR_WIN_W, EDITOR_WIN_H);
    release_screen();
}

/* really_quit: ask if we really want to quit */
void really_quit(void)
{
    draw_yes_no_box(vscreen, "Really quit?", (EDITOR_WIN_W / 2) - 50,
                    (EDITOR_WIN_H / 2) - 50);
    while (1) {
        update_screen();
        poll_keyboard();
        if (key[KEY_Y]) {
            close_program = 1;
            break;
        } else if (key[KEY_N]) {
            break;
        }
    }
}

int main(int argc, char *argv[])
{
    scew_parser *parser = NULL;
    if (init() != 0) {
        return 1;
    }

    if (argc == 2) {
        if (exists(argv[1])) {
            parser = scew_parser_create();
            scew_parser_ignore_whitespaces(parser, 1);
            scew_parser_load_file(parser, argv[1]);
            maptree = scew_parser_tree(parser);
        } else {
            allegro_message("Cannot load file: %s", argv[1]);
            close_program = 1;
        }
    } else if (argc == 4) {
        maptree = new_map(0, MAP_WIN.w / TILE_W, atoi(argv[1]), 1, argv[2]);
        map_offset = atoi(argv[1]) * TILE_H - EDITOR_WIN_H;
    } else {
        /* print usage */
        allegro_message
            ("Usage:\n\"editor.exe <filename>\" to load a file\n \"editor.exe <height_in_tiles> <default_tile_name> <newmapfile>\" to create a new map");

        close_program = 1;
    }

    while (!close_program) {
        /* get input */
        parse_mouse();
        get_grid_item();
        if (keypressed()) {
            parse_keyboard();
        }

        /* blit virtual screen */
        clear_to_color(vscreen, DEAD_COLOR);
        draw_select_lists(vscreen);
        draw_ctrl_box(vscreen);

        update_screen();
    }

    if (argc == 2) {
        write_map(argv[1], maptree);
        scew_parser_free(parser);
    } else if (argc == 4) {
        write_map(argv[3], maptree);
        scew_tree_free(maptree);
    }
    destroy_bitmap(vscreen);
    cleanup();
    return 0;
}

END_OF_MAIN()
