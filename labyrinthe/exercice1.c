#include <stdio.h> 
#include <stdlib.h>
#include "partition.h"
#include "exercice1.h"


/**
 * genere_matrice_adj
 * @param mat : La matrice d'adjacence que l'on va remplir. 
 * Crée une matrice d'ajacence aléatoire.
 */
void genere_matrice_adj(int mat [N][N])
{
    int val;
    int i = 0;
    int j = 0;

    for(i = 0; i < N; i++) {
        for(j = 0; j<= i; j++) {
            if(i == j) mat[i][j] = 1;
            else {
                val = 0;
                if (rand() % 8 == 0) val = 1;
                mat[i][j] = val;
                mat[j][i] = val;
            }
        }
    }   
}


/**
 * affiche_matrice. 
 * @param mat : La matrice d'adjacence. 
 * Affiche la matrice d'adjacence. 
 */
void affiche_Matrice(int mat[N][N]) 
{
    int i = 0;
    int j = 0;

    for(i = 0; i < N; i++) {
        for(j = 0; j < N; j++) {
            printf("%d ", mat[i][j]);
        }

        printf("\n");
    }       

    printf("\n");
}


/**
 * cree_partition
 * @param mat : La matrice d'adjacence. 
 * @return la partition correspondant à la matrice d'adjacence. 
 */
partition_t* cree_partition(int mat[N][N]) 
{
    partition_t * p = creer(N);
    int i = 0;
    int j = 0;

    for(i = 0; i < N; i++) {
        for(j = 0; j < i; j++) {
            if(mat[i][j] == 1 && (recuperer_classe(p, i) != recuperer_classe(p, j))) fusionner(p, i, j);
        }
    }

    return p;
}

void sub_graphviz(classe_t * c, FILE * fichier, int i)
{

    fprintf(fichier, "subgraph cluster%d {\n", i);
    while(c) {
        fprintf(fichier, "%d;\n ", c->val);
        c = c->next;
    }
    fprintf(fichier, "\n");

    fprintf(fichier, "}");
}


void representation_sous_graphe(liste_classe_t *l, FILE* fichier)
{
    classe_t * classe;
    fprintf(fichier, "\n");
    int i = 0;
    while(l) {
        classe = l->data;
        sub_graphviz(classe, fichier, i++);
        l = l->next;
    }

    fprintf(fichier, "}");

}

/**
 * graphviz. 
 * @param nom_fichier : Le nom du fichier. 
 * @param mat : La matrice d'adjacence. 
 * Rempli le fichier de manière à tracer le graphe correspondant à mat dans graphviz.
 */
void graphviz(char * nom_fichier, int mat[N][N], liste_classe_t *l)
{
    FILE * fichier = fopen(nom_fichier, "w+");
    if(!fichier) fprintf(stderr, "Erreur lors de l'ouverture du fichier");
    else {

        int i = 0;
        int j = 0;

        fprintf(fichier, "graph {\n");
        for(i = 0; i < N; i++) fprintf(fichier, "%d;\n", i);
        fprintf(fichier, "\n");
        

        for(i = 0; i < N; i++) {
            for(j = 0; j < i; j++) {
                if(mat[i][j] == 1) fprintf(fichier, "%d -- %d\n",i,j);
            }
        }

        representation_sous_graphe(l, fichier);
        fclose(fichier);
    }
}