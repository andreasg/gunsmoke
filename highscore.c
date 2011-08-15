#include "highscore.h"



SCORE *load_highscores(char *filename)
{

     size_t score;
     char name[NAME_LN];
     SCORE *highscores = NULL;
     FILE *fsock = fopen(filename, "rt");

     if (!fsock) {
          printf("can't open %s to read highscores", filename);
          return NULL;
     }

     highscores = create_highscores();

     while (!feof(fsock)) {
          fscanf(fsock, "%s %i\n", name, &score);
          add_score_to_highscores(score, name, highscores);
     }

     return highscores;
}



/* writes a list of highscores to a file */
void write_highscores(SCORE *scores, const char *filename)
{
     SCORE *s = NULL;
     FILE *fsock = NULL;     

     if (!scores || !filename) return;


     fsock = fopen(filename, "wt");
     if (!fsock) {
          printf("Unable to open %s for writing highscores", filename);
          return;
     }

     s = scores;
     do {
          fprintf(fsock, "%s %i\n", s->name, s->score);
          s = s->next;
     } while (s);

     fclose(fsock);
     s = NULL;
}



/* helper function for add_score_to_highscores */
SCORE *last_highscore(SCORE *scores)
{
     SCORE *tmp = NULL;
     if (!scores) return NULL;

     for(tmp = scores; tmp != NULL; tmp = tmp->next) {
          if (tmp->next == NULL) return tmp;
     }
     return NULL;
}



/* use insert-sort to add a element to the score list, 
 * ignore elements where score = 0 */
void add_score_to_highscores(size_t score, char *name, SCORE *scores)
{
     /* create a score item */
     SCORE *new_score = malloc(sizeof(*new_score));
     SCORE *tmp = NULL;

     if (!new_score || !name || !scores || score == 0) return;

/* check if we are adding the first element in a list? */
     if (scores->score == 0) {
          scores->score = score;
          strncpy(scores->name, name, NAME_LN);
          scores->next = NULL;
          return;
     }

/* if it is the first one, we can't just add it first, we have to 
 * modify the existing first element and then append a new element
 * as second with the previous first data */
     if (score > scores->score) {
          new_score->score = scores->score;
          strncpy(new_score->name, scores->name, NAME_LN);
          new_score->next = scores->next;

          scores->score = score;
          strncpy(scores->name, name, NAME_LN);
          scores->next = new_score;
          return;
     }

/* if it's the last one, set it to point to our new score, 
   and set our new score to the last */
     tmp = last_highscore(scores);
     if (!tmp) return;

     if (tmp->score > score) {
          new_score->score = score;
          strncpy(new_score->name, name, NAME_LN);
          new_score->next = NULL;

          tmp->next = new_score;
          return;
     }

/* find the item before our new score */
     for (tmp = scores; tmp->next != NULL; tmp = tmp->next) {
          if (tmp->next->score < score) {
               new_score->score = score;
               strncpy(new_score->name, name, NAME_LN);
               new_score->next = tmp->next;
               tmp->next = new_score;
               return;
          }
     }
}



/* inits and begins a highscore list */
SCORE *create_highscores(void)
{
     SCORE *s = malloc(sizeof(*s));
     if (!s) return NULL;

     s->score = 0;
     strncpy(s->name, "", NAME_LN);
     s->next = NULL;

     return s;
}



void destroy_highscores(SCORE *scores)
{
     SCORE *tmp = NULL;
     if (!scores) return;

     while (scores) {
          if (scores->next) {
               tmp = scores;
               scores = scores->next;
               free(tmp);
          } else {
               free(scores);
               break;
          }
     }
}
