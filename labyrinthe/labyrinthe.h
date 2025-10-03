#ifndef __PROJET_ZZ1_LABYRINTHE__
#define __PROJET_ZZ1_LABYRINTHE__

#include "file.h"
#include "constante.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

typedef struct {
    SDL_Texture *image;
    SDL_Rect source;                    // Rectangle définissant la zone totale de la planche
    SDL_Rect window_dimensions;         // Rectangle définissant la fenêtre, on n'utilisera que largeur et hauteur
    SDL_Rect destination;               // Rectangle définissant où la zone_source doit être déposée dans le renderer
    SDL_Rect state;                     // Rectangle de la vignette en cours dans la planche 
    int offset_x;  
    int offset_y;
} reglage_image;


typedef enum {
    DESERT = 6,
    MARAIS = 12,
    OCEAN = 15,
    PLAINE = 2,
    MONTAGNE = 9,
    NORMAL = 1,
}TYPE_TERRAIN;

typedef struct S {
    int distance[LAB_N * LAB_P];
    int etat_visite[LAB_N * LAB_P];
    int nb_visite[LAB_N*LAB_P];
}S;

typedef struct {
    int grille[LAB_P][LAB_N];
    TYPE_TERRAIN terrain[LAB_P*LAB_N];
    S etat_noeuds;
}labyrinthe_t;

typedef struct trajet{
    int nb_element;
    int trajet_x[TAILLE_TRAJET];
    int trajet_y[TAILLE_TRAJET];
}trajet;

extern SDL_Window *window_main;
extern SDL_Renderer *renderer;
extern SDL_DisplayMode user;
extern int largeur;
extern int hauteur;
extern const char* murs_texte[16];

void init_game();
void LoadSurface(reglage_image * img, SDL_Surface * surface);
void loadPicture(char * name, SDL_Rect * source, SDL_Texture ** img);
void affiche_sans_SDL(labyrinthe_t * laby);
void init_param(reglage_image * img);
void init_grille(labyrinthe_t * laby, int liste_arete[NB_MAX_ARETE][2]);
void vignette_a_afficher(labyrinthe_t * laby, reglage_image img, int x, int y);
void affiche_labyrinthe_image(labyrinthe_t * laby,reglage_image img);
void dessine_trait(int vertical, int est_ouest_ou_nord, int pos_x, int pos_y);
void affiche_labyrinthe_dessin(labyrinthe_t * laby);
void affiche_labyrinthe_dessin_passage_par_cases(labyrinthe_t * laby);
void affiche_grille(labyrinthe_t * laby);
void dessine_case(labyrinthe_t * laby);

void init_animation (reglage_image *img,int nombre_image,int nombre_animation);
void sprite_animation_a_afficher(reglage_image img, int i,int j, int animation);
void anim_parcour_du_labyrinthe(reglage_image *img, int position_personnage_x, int position_personnage_y,int animation);
void afficher_image(reglage_image img, int x, int y);
void init_image (reglage_image *img);
void init_trajet(trajet * t);
void animation_deplacement(labyrinthe_t * laby, trajet * trajet, reglage_image * map, reglage_image image,reglage_image perso,reglage_image etoile1,reglage_image croix,reglage_image pine,int *x_initial, int *y_initial, int *x_final,int *y_final, int *position_dans_trajet, int position_personnage_x, int position_personnage_y);
void generer_trajectoire_aleatoire(labyrinthe_t * laby, trajet * traj, int x_initial, int y_initial,int x_final, int y_final);
void affiche_debut_fin(reglage_image croix,reglage_image pine,int x_initial, int y_initial, int x_final, int y_final);
void affiche_trajet(reglage_image etoile1,trajet * traj);
void init_map( reglage_image * map);



#endif
