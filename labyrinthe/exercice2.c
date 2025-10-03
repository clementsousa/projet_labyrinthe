#include <stdio.h>
#include <stdlib.h>

#include "partition.h"
#include "graphe.h"


graphe_t * genere_graphe_aleatoire()
{
    graphe_t * graphe = (graphe_t *) malloc(sizeof(graphe_t));

    if(!graphe){
        fprintf(stderr, "Erreur d'allocation du graphe.");
        exit(EXIT_FAILURE);
    }
    graphe->nombre_arete = 0;
    graphe->nombre_noeud = NB_NOEUD;

    for(int i = 0; i < graphe->nombre_noeud; i++){
        for(int j = i+1; j < graphe->nombre_noeud; j++){
            if(rand()%8==0){
                graphe->liste_arete[graphe->nombre_arete][0] = i;
                graphe->liste_arete[graphe->nombre_arete][1] = j;
                graphe->nombre_arete++;
            }
        }
    }
    return graphe;
}


void afficher_graphe(graphe_t * graphe)
{
    if(graphe){
        printf("Nombre de noeud : %d\nNombre d'arête : %d\n",graphe->nombre_noeud, graphe->nombre_arete);
        for(int i = 0; i < graphe->nombre_arete; i++){
            printf("(%d, %d)\n", graphe->liste_arete[i][0], graphe->liste_arete[i][1]); // On affiche l'arête
        }
    }else{
        fprintf(stderr, "Le graphe donné en paramètres de afficher_graphe est NULL.");
        exit(EXIT_FAILURE);
    }
}


void liberer_graphe(graphe_t * graphe)
{
    free(graphe);
}


partition_t* cree_partition_couple(graphe_t *g) 
{
    partition_t * p = creer(g->nombre_noeud);
    int i = 0;

    for(i = 0; i < g->nombre_arete; i++) {
        if(recuperer_classe(p, g->liste_arete[i][0]) != recuperer_classe(p, g->liste_arete[i][1])) fusionner(p, g->liste_arete[i][0], g->liste_arete[i][1]);
    }

    return p;
}


void sub_graphviz_couple(classe_t * c, FILE * fichier, int i)
{

    fprintf(fichier, "subgraph cluster%d {\n", i);
    while(c) {
        fprintf(fichier, "%d;\n ", c->val);
        c = c->next;
    }
    fprintf(fichier, "\n");

    fprintf(fichier, "}");
}


void representation_sous_graphe_couple(liste_classe_t *l, FILE* fichier)
{
    classe_t * classe;
    fprintf(fichier, "\n");
    int i = 0;
    while(l) {
        classe = l->data;
        sub_graphviz_couple(classe, fichier, i++);
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
void graphviz_couple(char * nom_fichier, graphe_t * graphe, liste_classe_t *l)
{
    FILE * fichier = fopen(nom_fichier, "w");
    if(!fichier) fprintf(stderr, "Erreur lors de l'ouverture du fichier");
    else {

        int i = 0;

        fprintf(fichier, "graph {\n");
        for(i = 0; i < graphe->nombre_noeud; i++) { // Affichage de tout nos noeuds
            fprintf(fichier, "%d;\n", i);
        }
        fprintf(fichier, "\n");
        

        for(i = 0; i < graphe->nombre_arete; i++) {
            fprintf(fichier, "%d -- %d\n",graphe->liste_arete[i][0],graphe->liste_arete[i][1]);
        }

        representation_sous_graphe_couple(l, fichier);
        fclose(fichier);
    }
}
