#ifndef DIVVE_HIGHSCORE_H
#define DIVVE_HIGHSCORE_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "defines.h"



typedef struct _SCORE {
     size_t score;
     char name[NAME_LN];
     struct _SCORE *next;
} SCORE;



/* loads highscores from a file into a (new) list */
SCORE *load_highscores(char *filename);

/* writes a list of highscores to a file */
void write_highscores(SCORE *scores, const char *filename);

/* use insert-sort to add an element to the score list */
void add_score_to_highscores(size_t score, char *name, SCORE *scores);

/* inits and begins a highscore list */
SCORE *create_highscores(void);

/* free a list of scores */
void destroy_highscores(SCORE *scores);



#endif
