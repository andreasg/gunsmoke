#ifndef DIVVE_MAP_EDITOR
#define DIVVE_MAP_EDITOR



#include <allegro.h>
#include <scew/scew.h>

#include "defines.h"
#include "gfx.h"
#include "tile.h"
#include "map.h"
#include "sprite.h"


#include "list_element.h"
#include "editor_gui.h"
#include "primitives.h"


   
int init(void);
void get_grid_item(void);
void load_tiles(void);
void load_sprites(void);
void cleanup(void);
void parse_leftclick(void);
void parse_middleclick(void);
void parse_rightclick(void);
void parse_mouse(void);
void parse_keyboard(void);
void update_screen(void);
void really_quit(void);



#endif

