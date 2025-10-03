#ifndef __PROJET_ZZ1_PARCOURS__
#define __PROJET_ZZ1_PARCOURS__

#include "constante.h"
#include "labyrinthe.h"


typedef enum 
{
    NON_VISITE,
    EN_COURS_VISITE,
    EST_VISITE
} etat_noeud;



typedef struct {
    int num_noeud;
    int distance;
    int priorite;
}noeud_t;

extern S etat_noeuds;
void genere_tableau_voisin(labyrinthe_t * laby, int tableau[6][LAB_N * LAB_P]);
void bfs(labyrinthe_t * laby, int root);
int max_distance(labyrinthe_t * laby);
float distance_manhattan(int root1, int root2);
float distance_euclidienne(int root1, int root2);
float distance_de_tchebychev(int root1, int root2);
void dijkstra_initialisation(int root, labyrinthe_t * laby);
void dijkstra(labyrinthe_t * laby, int root);
void a_star(labyrinthe_t * laby, int dest, int root, float (*heuristique)(int, int), int * nb_noeud_visite);
labyrinthe_t * destination_connue_initialisation(int dest);
trajet destination_connue(labyrinthe_t *laby, int dest, int depart);
void rempli_file_du_chemin_a_parcourir(labyrinthe_t * laby, trajet * trajet, int x_initial, int y_initial, int x_final, int y_final);
void genere_position_aleatoire_de_debut_et_de_fin( int* x_final ,int * y_final);
trajet graphe_inconnu_dest_inconnue(labyrinthe_t * laby, int source, int dest, int trace_labyrinthe);
void afficher_labyrinthe_distance(labyrinthe_t *laby, int affiche_distance, int dest);
void afficher_labyrinthe_dest_inco(labyrinthe_t * laby, int source, int dest);
void afficher_labyrinthe_dest_connue(labyrinthe_t * laby, int source, int dest, trajet traj);

#endif