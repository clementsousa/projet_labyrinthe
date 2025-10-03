#ifndef __PROJET_ZZ1_JEU__
#define __PROJET_ZZ1_JEU__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


typedef struct {
    SDL_Surface * surf;
    SDL_Texture * text;
    SDL_Rect position;
    SDL_Color color;
    char * texte;
} texte;

typedef enum {
    ACCUEIL,
    PAUSE,
    JEU,
    FIN
} jeu;

typedef enum {
    HAUT = 1,
    BAS = 3,
    DROIT = 4,
    GAUCHE = 2
}deplacement_perso;

typedef enum {
    FACILE,
    MOYEN,
    DIFFICILE
} difficulte_enum;

void init_game();
void ecriture(texte *text, TTF_Font *font, const char *message);
void ecriture_ecran_accueil(TTF_Font * font);
void ecran_accueil(TTF_Font * font);

extern int ETAT_JEU;
extern texte jeu_texte; 
extern texte facile;
extern texte moyen;
extern texte difficile;
extern texte sauv;


#endif