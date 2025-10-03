#ifndef __PROJET_ZZ1_EXERCICE2__
#define __PROJET_ZZ1_EXERCICE2__

#define NB_NOEUD 10 // le nombre de noeud
#include "constante.h"
#include "partition.h"

typedef struct graphe{
    int nombre_noeud;
    int liste_arete[NB_MAX_ARETE][2];
    int nombre_arete;
}graphe_t;





graphe_t * genere_graphe_aleatoire();
void liberer_graphe(graphe_t *);
void afficher_graphe(graphe_t *);
void sub_graphviz_couple(classe_t *, FILE *, int);
void representation_sous_graphe_couple(liste_classe_t *, FILE*);
void graphviz_couple(char *, graphe_t *, liste_classe_t *);
partition_t* cree_partition_couple(graphe_t *);


#endif
