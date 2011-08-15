#include "gunsmoke.h"
int score = 0;
int map_complete = 0;
int state = MAIN_MENU;

MAP *m = NULL;

int current_map = FIRST_MAP;
BITMAP *buffer;
BITMAP *hud_pic = NULL;
BITMAP *heart = NULL;
BITMAP *mainmenu = NULL;
BITMAP *marker = NULL;
BITMAP *map_complete_bmp = NULL;
BITMAP *gameover_bmp = NULL;

BITMAP *boss_portrait = NULL;
BITMAP *objectives_bg = NULL;

char next_map[NAME_LN];
int terminate = 0;
int facing_ticks = 0;

SPRITE *player;
SPRITE *sprite_list = NULL;
size_t sprite_list_sz = 0;

GFX_OBJ *gfx_list = NULL;
size_t gfx_list_sz = 0;

BITMAP *bullet_bitmap = NULL;
BULLET *sprite_bullet_list = NULL;
size_t sprite_bullet_list_sz = 0;
BULLET *player_bullet_list = NULL;
size_t player_bullet_list_sz = 0;

SCORE *highscores = NULL;
char player_name[NAME_LN];

char key_down[KEY_MAX] = { 0 };

GLYPH_FACE *game_font = NULL;
GLYPH_REND *rend = NULL;

SAMPLE *fire_snd = NULL;
SAMPLE *select_snd = NULL;
MIDI *music = NULL;

/* adjusts the speed of the game logic - so things don't run away */
volatile int speed_counter = 0;
void inc_speed_counter(void)
{
     speed_counter++;
}

END_OF_FUNCTION(inc_speed_counter)

/* press_key: returns true if key k is pressed. */
int key_is_pressed(int k) 
{
     if (key[k] && !key_down[k]) {
          key_down[k] = 1;
          return 1;
     } else if (!key[k] && key_down[k]) {
          key_down[k] = 0;
     }
     return 0;
}

void blit_buffer(void);

/* get_char_from_key: parses the KEY_* and returns a char value */
void get_player_name_and_write_score(void) {
     int i;
     int caps = 0;
     int idx = 0;
     char name[HIGHSCORE_NAME_LN] = {0};

     gk_rend_set_text_color(rend, 255,255,255);
     
     while (1) {
          if (key[KEY_LSHIFT] || key[KEY_RSHIFT]) {
               caps = 1;
          } else {
               caps = 0;
          }

          for (i=KEY_A; i<KEY_9 && idx < (HIGHSCORE_NAME_LN-2); i++) {
               if (key_is_pressed(i)) {
                    idx = strlen(name);
                    if (caps) {                
                         name[idx] = (char)(scancode_to_ascii(i)-32);
                    } else {
                         name[idx] = (char)scancode_to_ascii(i);
                    }
                    name[idx+1] = '\0';
                    continue;
               }
          }

          if (key_is_pressed(KEY_BACKSPACE)) {
               name[idx] = '\0';
               if (idx > 0) {
                    idx--;
               }
          }

          if (key_is_pressed(KEY_ENTER) && strlen(name) > 0) {
               break;
          }

          if (key_is_pressed(KEY_ESC)) {
               score = 0;
               gk_rend_set_text_color(rend, 0,0,0);
               return;
          }

          rectfill(buffer, 200, 200, VSCREEN_W-200, VSCREEN_H-200, makecol(0,0,0));
          rect(buffer, 200, 200, VSCREEN_W-200, VSCREEN_H-200, makecol(255,255,255));
          gk_render_line_utf8(buffer, rend, "Enter your name:", 240, 240);
          gk_render_line_utf8(buffer, rend, name, 270, 270);
          blit_buffer();
     }
     gk_rend_set_text_color(rend, 0,0,0);
     add_score_to_highscores(score, name, highscores);
     score = 0;
}


int init_bitmaps(void)
{
     buffer = create_bitmap(WIDTH, HEIGHT);
     if (!buffer)
          return 1;
     clear_bitmap(buffer);

     hud_pic = load_bitmap(HUD_BG, NULL);
     if (!hud_pic) {
          allegro_message("can't find hud background");
          return 1;
     }

     mainmenu = load_bitmap(MAINMENU_BMP, NULL);
     if (!mainmenu) {
          allegro_message("Could'nt find mainmenu background bmp");
          return 1;
     }

     marker = load_bitmap(SELECTION_MARKER_BMP, NULL);
     if (!marker) {
          allegro_message("Couldn't find 'marker' bmp!");
          return 1;
     }

     heart = load_bitmap(HEART_BMP, NULL);
     if (!heart) {
          allegro_message("Couldn't find the heart bmp");
          return 1;
     }

     bullet_bitmap = load_bitmap(BULLET_BMP, NULL);
     if (!bullet_bitmap) {
          allegro_message("Coludn't find the bullet bmp!");
          return 1;
     }

     objectives_bg = load_bitmap(OBJECTIVES_BMP, NULL);
     if (!objectives_bg) {
          allegro_message("Can't find objectives background!");
          return 1;
     }

     map_complete_bmp = load_bitmap(MAP_COMPLETE_BMP, NULL);
     if (!map_complete_bmp) {
          allegro_message("Can't find map complete bg");
          return 1;
     }

     gameover_bmp = load_bitmap(GAMEOVER_BMP, NULL);
     if (!gameover_bmp) {
          allegro_message("Can't find gameover bitmap");
          return 1;
     }

     return 0;
}

/* init: basically inits the game */
int init(void)
{
     /* allegro stuff */
     if (allegro_init() != 0) {
          printf("failed to initialize...");
          return 1;
     }

     set_color_depth(GFX_DEPTH);
     if (set_gfx_mode(GFX_AUTODETECT, WIDTH, HEIGHT, 0, 0) != 0) {
          set_gfx_mode(GFX_TEXT, 0, 0, 0, 0);
          allegro_message("Failed to init a gfxmode.");
          return 1;
     }

     install_keyboard();
     install_timer();

     if (install_sound(DIGI_AUTODETECT, MIDI_AUTODETECT, NULL) != 0) {
          allegro_message("failed to init sound");
          return 1;
     }
     fire_snd = load_sample("data/sound/gun.wav");
     select_snd = load_sample("data/sound/select.wav");
     music = load_midi("data/sound/music.mid");

     if (!fire_snd || !select_snd || !music) {
          allegro_message("failed to find soundfile");
          return 1;
     }

     LOCK_VARIABLE(speed_counter);
     LOCK_FUNCTION(inc_speed_counter);
     set_window_title("GUNPOWDER");




     /* fonts */
     gk_set_font_path("data/fonts/");
     game_font = gk_load_face_from_file("Saddlebag.ttf", 0);
     /*game_font = gk_load_face_from_file("NASHVILL.TTF", 0); */
     if (!game_font) {
          allegro_message("Can't load font!");
          return 1;
     }

     rend = gk_create_renderer(game_font, 0);
     if (!rend) {
          allegro_message("Can't create font renderer");
          return 1;
     }

     gk_rend_set_size_pixels(rend, FONT_W, FONT_H);
     gk_rend_set_text_color(rend, 0, 0, 0);

     /* game stuff */
     if (init_bitmaps() != 0) {
          return 1;
     }

     if (!exists(SPRITE_INDEX)) {
          allegro_message("Couldn't find SPRITEINDEX!!");
          return 1;
     }

     /* highscore */
     if (exists(HIGHSCORE_FILE)) {
          highscores = load_highscores(HIGHSCORE_FILE);
     } else {
          highscores = create_highscores();
     }

     init_ai_methods();
     return 0;
}

void parse_fire_keys(void)
{
     if (!player->alive)
          return;
     if (key[KEY_LCONTROL] && key[KEY_ALT]) {
          if (player->cur_click <= 0) {
               if (player->facing_dir != NORTH) {
                    player->facing_dir = NORTH;
                    player->new_dir = 1;
                    facing_ticks = 20;
               }
               player->fire_dir = NORTH;
               player_bullet_list =
                    fire(player, NULL, player_bullet_list, &player_bullet_list_sz,
                         bullet_bitmap);
               player->cur_click = player->fire_clicks;
               play_sample(fire_snd, 128, 128, 1000, 0);
          }
     } else {
          if (key[KEY_LCONTROL]) {
               /* player fire gun */
               if (player->cur_click <= 0) {
                    if (player->facing_dir != WEST) {
                         player->facing_dir = WEST;
                         player->new_dir = 1;
                    }
                    facing_ticks = 20;
                    player->fire_dir = WEST;
                    player_bullet_list =
                         fire(player, NULL, player_bullet_list,
                              &player_bullet_list_sz, bullet_bitmap);
                    player->cur_click = player->fire_clicks;
                    play_sample(fire_snd, 128, 128, 1000, 0);
               }
          }
          if (key[KEY_ALT]) {
               /* player fire gun */
               if (player->cur_click <= 0) {
                    if (player->facing_dir != EAST) {
                         player->facing_dir = EAST;
                         player->new_dir = 1;
                    }
                    facing_ticks = 20;
                    player->fire_dir = EAST;
                    player_bullet_list =
                         fire(player, NULL, player_bullet_list,
                              &player_bullet_list_sz, bullet_bitmap);
                    player->cur_click = player->fire_clicks;
                    play_sample(fire_snd, 128, 128, 1000, 0);
               }
          }
     }
}

void parse_keyboard(void)
{
     if (!keypressed())
          return;

     if (key[KEY_ESC] && !key_down[KEY_ESC]) {
          key_down[KEY_ESC] = 1;
          if (highscores) {
               get_player_name_and_write_score();
          }
          state = MAIN_MENU;
          remove_int(inc_speed_counter);
     } else if (!key[KEY_ESC] && key_down[KEY_ESC]) {
          key_down[KEY_ESC] = 0;
     }

     parse_fire_keys();

     /* move player */
     if (key[KEY_UP]) {
          player->dx = 0.0 * player->spd;
          player->dy = -1.0 * player->spd;
          move_sprite(buffer, player, m, sprite_list, sprite_list_sz, -1);
     }
     if (key[KEY_DOWN]) {
          player->dx = 0.0 * player->spd;
          player->dy = 1.0 * player->spd;
          move_sprite(buffer, player, m, sprite_list, sprite_list_sz, -1);
     }
     if (key[KEY_RIGHT]) {
          player->dx = 1.0 * player->spd;
          player->dy = 0.0 * player->spd;
          move_sprite(buffer, player, m, sprite_list, sprite_list_sz, -1);
     }
     if (key[KEY_LEFT]) {
          player->dx = -1.0 * player->spd;
          player->dy = 0.0 * player->spd;
          move_sprite(buffer, player, m, sprite_list, sprite_list_sz, -1);
     }
     if (key_is_pressed(KEY_F2)) {
          player->health++;
     }
     if (key_is_pressed(KEY_F3)) {
          if (player->fire_clicks != 3) {
               player->fire_clicks = 3;
          } else {
               player->fire_clicks = 15;
          }
     }
}

void move_world(void)
{
     /* stop forwarding the map if we aren't alive */
     if (!player->alive)
          return;
     if (!forward_map(m))
          return;

     /* move the player along with the camera */
     player->dx = 0.0;
     player->dy = -1.0;
     if (!move_sprite(buffer, player, m, sprite_list, sprite_list_sz, MAP_SPEED)) {
          /* playes can't move with the camera - he's stuck, try to jump forward */
          player->dx = 0.0;
          player->dy = 1.0;
          if (!move_sprite
              (buffer, player, m, sprite_list, sprite_list_sz, MAP_SPEED)) {

               player->y -= TILE_H * 2;
               if (!move_sprite(buffer, player, m, player, 1, -1)) {
                    /* couldn't jump, we're stuck. Jump back and die */
                    player->y += TILE_H * 2;
                    player->alive = 0;
               }
          }
     }
}

void blit_to_buffer(void)
{
     blit_map(buffer, m, VSCREEN_W, VSCREEN_H);
     blit_sprite_list(buffer, m, sprite_list, sprite_list_sz);
     blit_bullets(buffer, m, sprite_bullet_list, sprite_bullet_list_sz);
     blit_bullets(buffer, m, player_bullet_list, player_bullet_list_sz);
     draw_hud(buffer, hud_pic, heart, player);
     line(buffer, 0, VSCREEN_H, WIDTH, VSCREEN_H, makecol(0, 0, 0));
}

void reload_map(int map)
{
     int health = 0;
     char filename[LINE_LN];
     if (player) {
          health = player->health;
     }

     sprintf(filename, "%s/%i/%s", MAPDIR, map, DEFAULT_MAPNAME);
     /*int hp = player->health; */

     if (m) {
          destroy_map(m);
          m = NULL;
     }

     if (sprite_bullet_list) {
          free(sprite_bullet_list);
          sprite_bullet_list = NULL;
     }

     if (player_bullet_list) {
          free(player_bullet_list);
          player_bullet_list = NULL;
     }

     destroy_sprite_list(sprite_list, &sprite_list_sz);
     player = NULL;
     sprite_list = NULL;
     sprite_list_sz = 0;
     destroy_gfx_list(gfx_list, &gfx_list_sz);
     gfx_list = NULL;
     gfx_list_sz = 0;

     sprite_list = load_sprite_list(filename, sprite_list, &sprite_list_sz);
     m = load_map(filename);
     if (!sprite_list)
          terminate = 1;
     if (!m)
          terminate = 1;

     gfx_list =
          complete_sprite_list(sprite_list, sprite_list_sz, gfx_list,
                               &gfx_list_sz);
     if (!gfx_list)
          terminate = 1;
     map_complete = 0;

     player = find_sprite("Player", sprite_list, sprite_list_sz);
     if (!player) {
          allegro_message("Couldn't find player in spritelist");
          terminate = 1;
     }
     player->alive = 1;

     if (boss_portrait) {
          destroy_bitmap(boss_portrait);
          boss_portrait = NULL;
     }

     sprintf(filename, "%s/%i/%s", MAPDIR, map, PORTRAIT_BMP);
     boss_portrait = load_bitmap(filename, NULL);
     if (!boss_portrait) {
          allegro_message("couldn't find boss portrait %s", filename);
     }
     current_map = map;

     if (map != FIRST_MAP) {
          /* if not first map, keep the health from prev map */
          player->health = health;
     }
}

void load_next_map(void)
{
     /* here we have completed a map - so somehow, reload and begin again */
     if (m->next_map == -1) {
          state = GAME_FINISHED;
          return;
     }

     reload_map(m->next_map);
     state = SHOW_MAP_OBJECTIVES;
}

/* clean: make things cliiiiiin */
void clean(void)
{
     if (sprite_bullet_list) {
          free(sprite_bullet_list);
     }
     if (player_bullet_list) {
          free(player_bullet_list);
     }
     destroy_bitmap(gameover_bmp);
     gameover_bmp = NULL;
     destroy_bitmap(mainmenu);
     mainmenu = NULL;
     destroy_bitmap(marker);
     marker = NULL;
     destroy_bitmap(bullet_bitmap);
     bullet_bitmap = NULL;
     destroy_bitmap(boss_portrait);
     boss_portrait = NULL;
     destroy_bitmap(objectives_bg);
     objectives_bg = NULL;
     destroy_bitmap(buffer);
     buffer = NULL;
     destroy_bitmap(map_complete_bmp);
     map_complete_bmp = NULL;

     destroy_bitmap(heart);
     heart = NULL;
     destroy_bitmap(hud_pic);
     hud_pic = NULL;
     destroy_map(m);
     m = NULL;
     destroy_sprite_list(sprite_list, &sprite_list_sz);
     sprite_list = 0;
     sprite_list_sz = 0;
     destroy_gfx_list(gfx_list, &gfx_list_sz);
     gfx_list = NULL;
     gfx_list_sz = 0;

     destroy_sample(fire_snd);
     destroy_sample(select_snd);

     free_python();

     if (highscores) {
          write_highscores(highscores, HIGHSCORE_FILE);
          destroy_highscores(highscores);
          highscores = NULL;
     }
}

void draw_main_menu(int selection)
{
     draw_sprite(buffer, mainmenu, 0, 0);
     switch (selection) {
     case NEW_GAME:
          draw_sprite(buffer, marker, 110, 180);
          break;
     case HIGHSCORE:
          draw_sprite(buffer, marker, 110, 230);
          break;
     case QUIT_GAME:
          draw_sprite(buffer, marker, 110, 280);
          break;
     default:
          break;
     }
}

void write_line(size_t y, char *txt)
{
     /*textprintf_ex(buffer, font, 35, y, makecol(200, 200, 200), -1, txt); */
     gk_render_line_utf8(buffer, rend, txt, 200, y);

}

void about_box(void)
{
     size_t y = 55;

     gk_rend_set_size_pixels(rend, 20, 30);
     gk_rend_set_text_color(rend, 200, 200, 200);

     rectfill(buffer, 25, 25, WIDTH - 25, HEIGHT - 100, makecol(0, 0, 0));
     rect(buffer, 25, 25, WIDTH - 25, HEIGHT - 100, makecol(255, 255, 255));

     gk_render_line_utf8(buffer, rend, "About GUNPOWDER:", 280, y);
     y += FONT_H;
     gk_rend_set_size_pixels(rend, 15, 25);

     write_line(y, "GUNPOWDER 0.1 is written by me, Andreas Granstrom.");
     y += 20;
     write_line(y, "It is supposed to be sort of a clone of an old NES");
     y += 20;
     write_line(y, "classic called 'GUN.SMOKE'. However - the maps and");
     y += 20;
     write_line(y, "such things have nothing in common with GUN.SMOKEs");
     y += 20;
     write_line(y, "maps etc.");
     y += 30;
     write_line(y, "Control the gunman by using the arrow keys");
     y += 20;
     write_line(y, "and press 'ctrl' to fire left, 'alt' to");
     y += 20;
     write_line(y, "fire right, and press both at the same time to fire");
     y += 20;
     write_line(y, "straight ahead.");
     y += 20;
     write_line(y, "You finnish a map by killing it's boss!");
     y += 30;
     write_line(y, "Enjoy!");

     write_line(HEIGHT - 110, "Hit Return to return");

     if (key[KEY_ENTER] && !key_down[KEY_ENTER]) {
          gk_rend_set_size_pixels(rend, FONT_W, FONT_H);
          gk_rend_set_text_color(rend, 0, 0, 0);
          state = MAIN_MENU;
          key_down[KEY_ENTER] = 1;
     } else if (!key[KEY_ENTER] && key_down[KEY_ENTER])
          key_down[KEY_ENTER] = 0;
}

void draw_hud(BITMAP * dest, BITMAP * hudpic, BITMAP * heart, SPRITE * player)
{
     int i;
     char buf[32];
     /*clear_bitmap(dest);*/
     draw_sprite(dest, hudpic, 0, VSCREEN_H);

     for (i = 0; i < player->health; i++) {
          draw_sprite(dest, heart, i * heart->w + 200, VSCREEN_H+20);
     }

     sprintf(buf, "%i", score);
     gk_render_line_utf8(dest, rend, buf, 657, VSCREEN_H+42);
}

/*
 * GAME STATES
 */

void main_menu(void)
{
     static int selection = NEW_GAME;

     draw_main_menu(selection);
     stop_midi();
     
     if (key[KEY_UP] && !key_down[KEY_UP] && (selection > 0)) {
          selection--;
          key_down[KEY_UP] = 1;
          play_sample(select_snd, 255, 128, 1000, 0);
     } else if (!key[KEY_UP] && key_down[KEY_UP])
          key_down[KEY_UP] = 0;

     if (key[KEY_DOWN] && !key_down[KEY_DOWN] && (selection < QUIT_GAME)) {
          selection++;
          key_down[KEY_DOWN] = 1;
          play_sample(select_snd, 255, 128, 1000, 0);
     } else if (!key[KEY_DOWN] && key_down[KEY_DOWN])
          key_down[KEY_DOWN] = 0;

     if (key[KEY_ENTER] && !key_down[KEY_ENTER]) {
          state = selection;
          key_down[KEY_ENTER] = 1;
          play_sample(select_snd, 255, 128, 1000, 0);
     } else if (!key[KEY_ENTER] && key_down[KEY_ENTER]) {
          key_down[KEY_ENTER] = 0;
     }

     if (key[KEY_F1] && !key_down[KEY_F1]) {
          state = ABOUT_BOX;
          key_down[KEY_F1] = 1;
     } else if (!key[KEY_F1] && key_down[KEY_F1]) {
          key_down[KEY_F1] = 0;
     }
}

void highscore(void)
{
     SCORE *s = NULL;
     char l[NAME_LN];
     size_t i, y = 120;
     rectfill(buffer, 200, 100, WIDTH - 200, HEIGHT - 100, makecol(0, 0, 0));
     rect(buffer, 200, 100, WIDTH - 200, HEIGHT - 100, makecol(250, 250, 250));

     gk_rend_set_size_pixels(rend, 13, 23);
     gk_rend_set_text_color(rend, 250, 250, 250);

     gk_render_line_utf8(buffer, rend, "Highscores:", 210, y);
     y += 30;
     if (highscores && highscores->score != 0) {
          for (i = 0, s = highscores ; i < 15 && s; i++, s = s->next) {
               sprintf(l, "%s: %i", s->name, s->score);
               gk_render_line_utf8(buffer, rend, l, 210, y);
               y += 20;
          }
     }
     gk_render_line_utf8(buffer, rend, "(Return to return)", 210, HEIGHT - 120);

     if (key[KEY_ENTER] && !key_down[KEY_ENTER]) {
          gk_rend_set_size_pixels(rend, FONT_W, FONT_H);
          gk_rend_set_text_color(rend, 0, 0, 0);
          score = 0;
          state = MAIN_MENU;
          key_down[KEY_ENTER] = 1;
     } else if (!key[KEY_ENTER] && key_down[KEY_ENTER])
          key_down[KEY_ENTER] = 0;
}

void quit_game(void)
{
     terminate = 1;
}

void new_game(void)
{
     play_looped_midi(music, 15, -1);
     reload_map(FIRST_MAP);
     state = SHOW_MAP_OBJECTIVES;
}

void textprint_message(int x, int in_y, char *msg)
{

     char lines[MAX_LINES][LINE_LN] = { {0} };
     char str[LINE_LN];
     int i;
     char *s = NULL;
     int newlines = 0;
     int walked = 0;

     int y = in_y;

     if (msg[strlen(msg) - 1] != '\n') {
          /* string is not terminated by a newline */
          sprintf(str, "%s\n", msg);
     } else {
          strncpy(str, (char *) line, LINE_LN);
     }

     /* relace newlines with NULL characters */
     for (i = 0; i < LINE_LN && str[i] != '\0'; i++) {
          if (str[i] == '\n') {
               newlines++;
               str[i] = '\0';
          }
     }

     /* Copy the splitted string into the lines array */
     s = &str[0];
     for (i = 0; i < newlines; i++) {
          strncpy(lines[i], s, LINE_LN);
          walked += strlen(s);
          s = &str[walked + i + 1];
     }

     for (i = 0; i < newlines; i++) {
          gk_render_line_utf8(buffer, rend, lines[i], x, y);
          y += FONT_H;
     }
}

void show_map_objectives(void)
{
     size_t y = 270;
     char filename[LONG_NAME_LN];
     char line[LINE_LN];
     static FILE *fsock = NULL;


     gk_rend_set_size_pixels(rend, 13, 23);
     gk_rend_set_text_color(rend, 0, 0, 0);

     if (!fsock) {
          sprintf(filename, "%s/%i/%s", MAPDIR, current_map, OBJECTIVE_TXT);
          fsock = fopen(filename, "rt");
          if (!fsock) {
               allegro_message("can't find objectives file!!");
          }
     }


     draw_sprite(buffer, objectives_bg, 0, 0);
     draw_sprite(buffer, boss_portrait, 460, 120);

     if (fsock) {
          /* draw the text */

          while (!feof(fsock)) {
               fgets(line, LINE_LN, fsock);
               if (line[strlen(line) - 1] == '\n')
                    line[strlen(line) - 1] = '\0';
               gk_render_line_utf8(buffer, rend, line, 380, y);
               y += 15;
          }
          rewind(fsock);
     }


     if (key[KEY_ENTER] && !key_down[KEY_ENTER]) {
          if (fsock)
               fclose(fsock);
          fsock = NULL;
          gk_rend_set_size_pixels(rend, FONT_W, FONT_H);
          gk_rend_set_text_color(rend, 0, 0, 0);
          state = RUN_GAME;
          install_int_ex(inc_speed_counter, BPS_TO_TIMER(60));
          key_down[KEY_ENTER] = 1;
     } else if (!key[KEY_ENTER] && key_down[KEY_ENTER])
          key_down[KEY_ENTER] = 0;
}

void run_game(void)
{
     int hp;
     /* game logic loop */
     while (speed_counter > 0 && state == RUN_GAME) {
          hp = player->health;
          /* parse keystrokes */
          parse_keyboard();

          /* update the sprites */
          update_sprite_list(buffer, m, player, sprite_list, sprite_list_sz, gfx_list, gfx_list_sz);

          /* fire and update bullets */
          sprite_bullet_list =
               sprite_list_fire_guns(player, sprite_list, sprite_list_sz,
                                     sprite_bullet_list,
                                     &sprite_bullet_list_sz, bullet_bitmap);
          update_bullets(sprite_bullet_list, &sprite_bullet_list_sz, buffer,
                         m, player, 1);
          update_bullets(player_bullet_list, &player_bullet_list_sz, buffer,
                         m, sprite_list, sprite_list_sz);

          /* shift the map */
          move_world();

          /* check if all the bosses on a map are dead - if so
           * we have finnished the map! */
          if (!sprite_list_boss_alive(sprite_list, sprite_list_sz)) {
               state = MAP_COMPLETE;
               remove_int(inc_speed_counter);
               return;
          }

          /* player fire countdown */
          if (player->cur_click > 0)
               player->cur_click--;

          /* tick down the facing timer */
          if (facing_ticks < 0 && player->facing_dir != NORTH) {
               player->facing_dir = NORTH;
               player->new_dir = 1;
          } else
               facing_ticks--;

          if (!player->alive && player->corpse_tick > CORPSE_LIFE) {
               if (highscores) {
                    get_player_name_and_write_score();
               }
               state = GAME_OVER;
               remove_int(inc_speed_counter);
               return;
          }
          if (player->health < hp) {
               state = REDUCE_PLAYER_HEALTH;
               remove_int(inc_speed_counter);
               return;
          }
          speed_counter--;
     }
     blit_to_buffer();
}

void kill_all_on_screen(void)
{
     sprite_list_kill_updating(player, sprite_list, sprite_list_sz);
     kill_bullet_list(sprite_bullet_list, sprite_bullet_list_sz);
     kill_bullet_list(player_bullet_list, player_bullet_list_sz);
}

void reduce_player_health(void)
{
     static int countdown = 500;
     char frame_name[NAME_LN];

     if (countdown > 0) {
          rectfill(buffer, 50, 50, WIDTH - 50, HEIGHT - 50, makecol(0, 0, 0));
          rect(buffer, 50, 50, WIDTH - 50, HEIGHT - 50, makecol(255, 0, 0));

          gk_rend_set_text_color(rend, 255, 20, 20);
          gk_render_line_utf8(buffer, rend, "You lost a life!", WIDTH / 2 - 100,
                              HEIGHT / 2);

          sprintf(frame_name, "%s%i", player->name, player->corpse_frame);
          draw_sprite(buffer, search_gfx_list(frame_name, gfx_list, gfx_list_sz),
                      WIDTH / 2 - 150, HEIGHT / 2 - player->h);

          countdown--;
     } else {
          countdown = 500;
          kill_all_on_screen();
          gk_rend_set_text_color(rend, 0, 0, 0);
          state = RUN_GAME;
          install_int_ex(inc_speed_counter, BPS_TO_TIMER(60));
     }
}

void blit_buffer(void)
{
     /* blit */
     acquire_screen();
     blit(buffer, screen, 0, 0, 0, 0, WIDTH, HEIGHT);
     release_screen();
}

void game_over(void)
{
     clear_bitmap(buffer);
     draw_sprite(buffer, gameover_bmp, 0, 0);
     blit_buffer();
     while (!key[KEY_ENTER]) {
          key_down[KEY_ENTER] = 1;
          state = HIGHSCORE;
          player = NULL;
     }
}

void complete_map(void)
{
     static int countdown = 800;

     if (countdown > 0) {
          draw_sprite(buffer, map_complete_bmp, 0, 0);
          countdown--;
     } else {
          countdown = 800;
          state = LOAD_NEXT_MAP;
     }
}

void game_finished(void)
{
     clear_bitmap(buffer);
     textprintf_centre_ex(buffer, font, WIDTH / 2, HEIGHT / 2 - 5,
                          makecol(255, 100, 0), -1, "GAME COMPLETE");
     textprintf_centre_ex(buffer, font, WIDTH / 2, (HEIGHT / 2) + 5,
                          makecol(255, 100, 0), -1,
                          "(Press enter to return to menu)");
     blit_buffer();
     reload_map(FIRST_MAP);
     while (!key[KEY_ENTER]) {
          if (highscores) {
               get_player_name_and_write_score();
          }
          key_down[KEY_ENTER] = 1;
          state = HIGHSCORE;
     }

}

/* update: the core in my little statemachine. returns a function
 * to the corresponding state,
 * em... this is a bad solution, there should be a statemanager, where 
 * the next state simply is a function pointer - that is, this function 
 * is completley uncecceceary! */
void (*update(void))(void)
{
     if (state == MAIN_MENU) {
          return &main_menu;
     } else if (state == GAME_OVER) {
          return &game_over;
     } else if (state == NEW_GAME) {
          return &new_game;
     } else if (state == HIGHSCORE) {
          return &highscore;
     } else if (state == RUN_GAME) {
          return &run_game;
     } else if (state == GAME_FINISHED) {
          return &game_finished;
     } else if (state == LOAD_NEXT_MAP) {
          return &load_next_map;
     } else if (state == SHOW_MAP_OBJECTIVES) {
          return &show_map_objectives;
     } else if (state == MAP_COMPLETE) {
          return &complete_map;
     } else if (state == ABOUT_BOX) {
          return &about_box;
     } else if (state == REDUCE_PLAYER_HEALTH) {
          return &reduce_player_health;
     } else {
          return &quit_game;
     }
}

/* main: the corpse in a murder scene */
int main(void)
{
     /* will point to a function executing our current state! */
     void (*execute_state)(void);

     if (init() != 0) {
          terminate = 1;
     }


     while (!terminate) {
          execute_state = update();
          execute_state();
          blit_buffer();
     }

     clean();
     return 0;
}
END_OF_MAIN()
