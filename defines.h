#ifndef DIVVE_DEFINES_H
#define DIVVE_DEFINES_H


#define PLAYER_NAME "Player"

#define HIGHSCORE_FILE "data/highscore.dat"
#define HIGHSCORE_NAME_LN 12

#define DEFAULT_BULLETLIST_SIZE 12

#define PI 3.14159265
#define FONT_PATH "data\\fonts"

#define MAPDIR "data/maps"
#define DEFAULT_MAPNAME "map.m"
#define PORTRAIT_BMP "portrait.bmp"
#define OBJECTIVE_TXT "objectives.txt"
#define COMPLETE_TXT "complete.txt"
#define FIRST_MAP 0

/* 
 * states:
 * Let the first items (NEW_GAME) to (QUIT_GAME) be in 
 * this order, because they are used (in that order) for the 
 * main menu.
 */
#define NEW_GAME 0
#define HIGHSCORE 1
#define QUIT_GAME 2
#define MAIN_MENU 3
#define SHOW_MAP_OBJECTIVES 4
#define RUN_GAME 5
#define REDUCE_PLAYER_HEALTH 6
#define GAME_OVER 7
#define MAP_COMPLETE 8
#define LOAD_NEXT_MAP 9
#define GAME_FINISHED 10
#define PAUSED 11
#define ABOUT_BOX 12

#define HAT_BMP "data/gfx/hat.bmp"
#define MAINMENU_BMP "data/gfx/mainmenu.bmp"
#define SELECTION_MARKER_BMP "data/gfx/selection_marker.bmp"
#define HIGHSCORE_BMP "data/gfx/highscore.bmp"
#define OBJECTIVES_BMP "data/gfx/objective_bg.bmp"
#define MAP_COMPLETE_BMP "data/gfx/map_complete.bmp"
#define GAMEOVER_BMP "data/gfx/gameover.bmp"

#define CORPSE_LIFE 110
#define LIFETIME 1200

/* movement_types */
#define MV_NORMAL_ENEMY 1
#define MV_BOSS 2
#define AI_DELAY 15

/* type of bitmap used in game */
#define GFX_EXT ".bmp"
#define GFX_DEPTH 16

/* maxsz of idenifiernames */
#define NAME_LN 32
#define LONG_NAME_LN 64
#define LINE_LN 79
#define MAX_LINES 30

/* the amount of pixels a character will move each cycle in the gameloop */
#define STEPSIZE 3
#define MAP_SPEED 0.5 

/* max framerate */
#define FRAMETIME 60/60

/* window size */
#define WIDTH 800
#define HEIGHT 600

/* virtual screen size */
#define VSCREEN_H HEIGHT - HUD_H + 10
#define VSCREEN_W WIDTH

#define HUD_H 64
#define HUD_W WIDTH
#define HUD_BG "data/gfx/hud.bmp"
#define HEART_BMP "data/gfx/hat.bmp"
#define FONT_W 21
#define FONT_H 42

/* tile sizes */
#define TILE_H 32
#define TILE_W 32

/* movement directions */
#define NORTH 0
#define EAST 1
#define SOUTH 2
#define WEST 3
#define NORTHEAST 4
#define SOUTHEAST 5
#define SOUTHWEST 6
#define NORTHWEST 7
#define NORTHNORTHWEST 8
#define NORTHNORTHEAST 9

/* indices */
#define GFX_INDEX       "data/gfxindex.xml"
#define SPRITE_INDEX    "data/spriteindex.xml"
#define SND_INDEX       "data/sndindex.xml"


/* sprite dir */
#define WORLD_GFX_DIR "data/gfx/world/"
#define SPRITE_GFX "data/gfx/sprites/"


/* bullet bitmap info */
#define BULLET_BMP "data/gfx/sprites/bullet.bmp"
#define BULLET_W 8
#define BULLET_H 8
#define BULLET_FLY_TIME 40
#define BULLET_SPEED 3.5


/* controls the speed of the animations */
#define ANIM_DELAY 20


/* the weapons! */
#define RIFLE 0
#define GUNS 1
#define SHOTGUN 2
#define UNARMED 100



/*                *
 * EDITOR DEFINES *
 *                */
#define EDITOR_WIN_W 1024
#define EDITOR_WIN_H 768

/* all the sub windows */
#define TILE_WIN rec(911, 0, 112, 352)
#define TILE_FRW rec(1004, 352, 20, 20)
#define TILE_REW rec(911, 352, 20, 20)

#define SPRITE_WIN rec(911, 410, 112, 357)
#define SPRITE_FRW rec(1004, 390, 20, 20)
#define SPRITE_REW rec(911, 390, 20, 20)

#define DATA_WIN rec(799, 0, 112, 767)
#define MAP_WIN rec(0, 0, 800, 768)


/* data about the elements in the tile and sprit lists */
#define ELEM_PER_PAGE 11
#define LIST_ELEM_W 256
#define LIST_ELEM_H 32


/* how fast to scroll the map in the editor (in tiles)*/
#define SCROLL_SPEED 5


/* colors */
#define OUTLINE_COLOR makecol(150, 150, 150)
#define TEXT_COLOR makecol(255,255,255)
#define DEFAULT_COLOR makecol(63,63,63)
#define BG_COLOR makecol(0,0,0)
#define SEL_COLOR makecol(80,80,80)
#define DEAD_COLOR makecol(40,40,40)
#define GRID_COLOR makecol(50, 50, 80)

#define TILE_LIST "data/tilelist.dat"

/* program operating modes, now this is a but ugly. It controls what 
 * kind of item we are adding to the map - and the behaviour of the right 
 * mouse button. */
#define TILE_MODE 0
#define SPRITE_MODE 1

/*                    *
 * END EDITOR DEFINES *
 *                    */



#endif

