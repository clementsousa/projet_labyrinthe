#ifndef __PROJET_ZZ1_EXERCICE1__
#define __PROJET_ZZ1_EXERCICE1__

#define N 6


void genere_matrice_adj(int mat [N][N]);
void affiche_Matrice(int mat[N][N]);
void sub_graphviz(classe_t * c, FILE * fichier, int i);
void graphviz(char * nom_fichier, int mat[N][N], liste_classe_t *l);
void representation_sous_graphe(liste_classe_t *l, FILE* fichier);
partition_t* cree_partition(int mat[N][N]);


#endif
