#ifndef __TAS_BINAIRE__
#define __TAS_BINAIRE__

#include "constante.h"
#include "parcours_graphe.h"


typedef struct tas_binaire{
    noeud_t * tas;
    int nb_element;
}tas_binaire_t;


tas_binaire_t * tas_creer();
void tas_inserer(tas_binaire_t * tas, noeud_t elem);
void tas_liberer(tas_binaire_t ** tas) ;
noeud_t tas_supprimer(tas_binaire_t *tas);
void tas_afficher(tas_binaire_t * tas);
void tas_mise_a_jour(tas_binaire_t * tas, int num_noeud, int distance, int heuristique);
void tas_remonter(tas_binaire_t *tas, int i);
int tas_vide(tas_binaire_t * tas);
#endif