#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "tas_binaire.h"


tas_binaire_t * tas_creer()
{
    tas_binaire_t * tas_binaire = (tas_binaire_t *) malloc(sizeof(tas_binaire_t));
    if(!tas_binaire){
        fprintf(stderr,"Erreur d'allocation du tas");
        exit(EXIT_FAILURE);
    }
    tas_binaire->tas = (noeud_t *) calloc(TAILLE_DU_TAS, sizeof(noeud_t));
    if(!tas_binaire->tas){
        fprintf(stderr,"Erreur d'allocation du tableau du tas");
        exit(EXIT_FAILURE);
    }
    tas_binaire->nb_element=0;
    return tas_binaire;
}

void tas_inserer(tas_binaire_t * tas, noeud_t elem)
{
    int i = tas->nb_element; //nombre d'element du tas
    tas->tas[i]=elem;
    tas->nb_element++;
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (tas->tas[i].priorite < tas->tas[parent].priorite) {
            noeud_t temp = tas->tas[i];
            tas->tas[i] = tas->tas[parent];
            tas->tas[parent] = temp;
            i = parent;
        } 
        else {
            break;
        }
    }
}

void tas_liberer(tas_binaire_t ** tas) 
{
    free((*tas)->tas);
    free(*tas);
    *tas = NULL;
}

noeud_t tas_supprimer(tas_binaire_t *tas)
{
    int nb_element = tas->nb_element;
    tas->nb_element--;
    noeud_t valeur = tas->tas[0];
    int i;
    int continu =1;

    if (nb_element)
    {
        tas->tas[0] = tas->tas[nb_element-1];
        i = 0;
        while ((2 * i + 2 < nb_element)&&continu)
        {
            if ((tas->tas[i].priorite > tas->tas[2 * i + 1].priorite)|| (tas->tas[i].priorite > tas->tas[2 * i + 2].priorite))
            {
                noeud_t tmp = tas->tas[i];

                if ((2 * i + 2 >= nb_element) || (tas->tas[2 * i + 1].priorite < tas->tas[2 * i + 2].priorite))
                {
                    tas->tas[i] = tas->tas[2 * i + 1];
                    tas->tas[2 * i + 1] = tmp;
                    i = 2 * i + 1;
                }
                else
                {
                    tas->tas[i] = tas->tas[2 * i + 2];
                    tas->tas[2 * i + 2] = tmp;
                    i = 2 * i + 2;
                }
            }
            else continu = 0;
        }
    }
    return valeur;
}

void tas_afficher(tas_binaire_t * tas) 
{
    printf("nb_elem : %d\n", tas->nb_element);
    for (int i = 0; i < tas->nb_element; i++) {
        printf("Noeud : %d Priorité : %d\n", tas->tas[i].num_noeud, tas->tas[i].priorite);
    }
    printf("\n");
}

void tas_remonter(tas_binaire_t *tas, int i)
{
    while (i > 0) {
        int parent = (i - 1) / 2;
        if (tas->tas[i].priorite < tas->tas[parent].priorite) {
            noeud_t temp = tas->tas[i];
            tas->tas[i] = tas->tas[parent];
            tas->tas[parent] = temp;
            i = parent;
        } else {
            break;
        }
    }
}


void tas_mise_a_jour(tas_binaire_t * tas, int num_noeud, int distance, int heuristique)
{
    for(int i = 0; i < tas->nb_element; i++){
        if(tas->tas[i].num_noeud == num_noeud){
            tas->tas[i].distance = distance;
            tas->tas[i].priorite = distance + heuristique;
            tas_remonter(tas,i);
            break;
        }
    }
}

int tas_vide(tas_binaire_t * tas){
    return tas->nb_element==0;
}