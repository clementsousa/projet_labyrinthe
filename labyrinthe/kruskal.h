#ifndef __PROJET_ZZ1_KRUSKAL__
#define __PROJET_ZZ1_KRUSKAL__

typedef enum {
    NORD = 1,
    EST = 2,
    SUD = 4,
    OUEST = 8
} cardinal;

typedef struct {
    int nord;
    int sud;
    int est;
    int ouest;
    int nombre;
} voisins_noeud;
#include "constante.h"
#include "graphe.h"

void fisher_yates(graphe_t *);
int trouve_voisins(int liste_arete[NB_MAX_ARETE][2], int noeud);
void echange(int * a, int * b);
void kruskal(graphe_t *g, int A[][2]);
void kruskal_probaliste(graphe_t *g, int A[][2], float proba);
void ajoute_arete(graphe_t * graphe, int sommet1, int sommet2);
graphe_t * genere_graphe_base_laby(int longueur, int largeur);
void generer_labyrinthe(int labyrinthe_resultat[NB_MAX_ARETE][2], int longueur, int hauteur, float densite);


#endif