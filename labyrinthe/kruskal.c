#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "partition.h"
#include "kruskal.h"
#include "labyrinthe.h"


void echange(int * a, int * b)
{
    int tmp = *a;
    *a = *b;
    *b = tmp;
}


int trouve_voisins(int liste_arete[NB_MAX_ARETE][2], int noeud)
{
    voisins_noeud v = {0};
    int i = 0;

    for(i = 0; i < NB_MAX_ARETE && liste_arete[i][1] != liste_arete[i][0] ; i++)
    {
        if(liste_arete[i][0] == noeud) {
            if(liste_arete[i][1] > noeud) {
                if(liste_arete[i][1] == (noeud + 1)) v.est = EST;
                else v.sud = SUD;
            } else {
                if(liste_arete[i][1] == (noeud - 1)) v.ouest = OUEST;
                else v.nord = NORD;    
            }

        } if(liste_arete[i][1] == noeud) {
            if(liste_arete[i][0] > noeud) {
                if(liste_arete[i][0] == (noeud + 1)) v.est = EST;
                else v.sud = SUD;
            } else {
                if(liste_arete[i][0] == (noeud - 1)) v.ouest = OUEST;
                else v.nord = NORD;                
            }
        }
    }

    v.nombre = 15 - (v.est + v.ouest + v.sud + v.nord);
    
    return v.nombre;
}


void fisher_yates(graphe_t * g)
{
    int i = 0;
    int j = 0;
    
    srand(2);

    for(i = g->nombre_arete - 1; i >=1; i--) {
        j = rand() % (i+1);
        echange(&(g->liste_arete[i][0]), &(g->liste_arete[j][0]));
        echange(&(g->liste_arete[i][1]), &(g->liste_arete[j][1]));
    }
}


void kruskal(graphe_t *g, int A[][2])
{
    int i = 0;
    int compteur_A = 0;

    //Etape 1 
    fisher_yates(g);

    //Etape 2
    partition_t * p = creer(g->nombre_noeud);

    //Etape 4
    for(i = 0; i < g->nombre_arete; i++) {
        if(recuperer_classe(p, g->liste_arete[i][0]) != recuperer_classe(p, g->liste_arete[i][1])) {
            fusionner(p, g->liste_arete[i][0], g->liste_arete[i][1]);
            A[compteur_A][0] = g->liste_arete[i][0];
            A[compteur_A][1] = g->liste_arete[i][1];
            compteur_A++;
        }
    }


}


/**
 * kruskal_probabiliste. 
 * @param g : graphe. 
 * @param A : La liste d'arêtes.
 * @param proba : proba appartient à [0, 1], exprime la densité du labyrinthe  : 0 = arbre, 1 = aucun mur
 * Applique l'algo de Kruskal avec probabilité.
 */
void kruskal_probaliste(graphe_t *g, int A[][2], float proba)
{
    if(proba < 0){
        fprintf(stderr, "Probabilité invalide pour Kruskal_probabiliste");
        exit(EXIT_FAILURE);
    }
    int i = 0;
    int compteur_A = 0;
    int ajoute = 0; // utile pour éviter la duplication de code
    //Etape 1 
    fisher_yates(g);

    //Etape 2
    partition_t * p = creer(g->nombre_noeud);

    //Etape 4
    for(i = 0; i < g->nombre_arete; i++) {
        if(recuperer_classe(p, g->liste_arete[i][0]) != recuperer_classe(p, g->liste_arete[i][1])) {
            ajoute = 1;
        }else{
            float nb_aleatoire = (float) rand() / (float)RAND_MAX;
            if( nb_aleatoire <= proba){
                ajoute = 1;
            }
        }
        if(ajoute){
            ajoute = 0;
            fusionner(p, g->liste_arete[i][0], g->liste_arete[i][1]);
            A[compteur_A][0] = g->liste_arete[i][0];
            A[compteur_A][1] = g->liste_arete[i][1];
            compteur_A++;
        }
    }
    libererPartition(&p);
}


void ajoute_arete(graphe_t * graphe, int sommet1, int sommet2)
{
    int nb_arete = graphe->nombre_arete;
    graphe->liste_arete[nb_arete][0] = sommet1;
    graphe->liste_arete[nb_arete][1] = sommet2;
    graphe->nombre_arete++;
}


/**
 * Génère un graphe où chaque noeud possède une arete vers ses (4) voisins
 */
graphe_t * genere_graphe_base_laby(int longu, int hauteur)
{
    graphe_t * graphe = (graphe_t *) malloc(sizeof(graphe_t));
    if(!graphe){
        fprintf(stderr, "Erreur d'allocation du graphe dans la génération du labyrinthe");
    }
    int j = 0;
    int i = 0;
    graphe->nombre_arete=0;
    graphe->nombre_noeud = longu*hauteur;
    for(j = 0; j < hauteur; j++){ // Pour chaque ligne
        for(i = 0; i < longu; i++){ // Pour chaque noeud sur la ligne
            if(i!=longu-1){
                ajoute_arete(graphe, j*longu + i, (j*longu) + i +1);
            }
            if(j!=hauteur-1){
                ajoute_arete(graphe, j*longu + i, (j* longu) + i  +longu);
            }        
        } 
    }
    
    return graphe;
}


/**
 * Génère un labyrinthe
 * @param labyrinthe_resultat : liste d'arête, initialisé à 0 
 * @param longueur : longueur du labyrinthe
 * @param hauteur : hauteur du labyrinthe
 * @param densite : nombre flottant entre 0 et 1, exprime la densité des murs, 0 = arbre couvrant, 1 = pas de mur
 */
void generer_labyrinthe(int labyrinthe_resultat[NB_MAX_ARETE][2], int longueur, int hauteur, float densite)
{
    graphe_t * graphe = genere_graphe_base_laby(longueur,hauteur);
    kruskal_probaliste(graphe, labyrinthe_resultat, densite);
    FILE * fichier = fopen("labyrinthe.dot", "w");
    if(!fichier) fprintf(stderr, "Erreur lors de l'ouverture du fichier");
    else {

        int i = 0;

        fprintf(fichier, "graph {\n");
        for(i = 0; i < LAB_N*LAB_P; i++) { // Affichage de tout nos noeuds
            fprintf(fichier, "%d;\n", i);
        }
        fprintf(fichier, "\n");
        

        for(i = 0; i < NB_MAX_ARETE && labyrinthe_resultat[i][0] != labyrinthe_resultat[i][1] ; i++) {
            fprintf(fichier, "%d -- %d\n",labyrinthe_resultat[i][0],labyrinthe_resultat[i][1]);
        }
        fprintf(fichier,"}");
        fclose(fichier);
    }

    liberer_graphe(graphe);
}
