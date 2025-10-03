#include <stdio.h>
#include <stdlib.h>
#include "partition.h"

/*--------------------------------------Classe------------------------------------*/

/**
 * cree_classe. 
 * @return une nouvelle classe.
 */
classe_t* cree_classe()
{
    return NULL;
}


/**
 * cree_maillon. 
 * @param val : La valeur à mettre dans le nouveau maillon. 
 * @return un nouveau maillon.
 */
classe_t * cree_maillon(int val)
{
    classe_t * maillon = (classe_t*)malloc(sizeof(classe_t));
    if(!maillon) {
        fprintf(stderr,"Allocation non réussie\n");
        return NULL;
    }
    maillon->next = NULL;
    maillon->val = val;

    return maillon;
}


/**
 * ajoute. 
 * @param c : La classe.
 * @param val : La valeur à mettre dans le nouveau maillon. 
 * @return une classe avec la nouvelle valeur ajoutée. 
 */
classe_t * ajoute(classe_t * c, int val)
{
    classe_t * maillon = cree_maillon(val);
    maillon->next = c;
    
    return maillon;
}


/**
 * affiche. 
 * @param c : La classe. 
 * Affiche toutes les valeurs de la classe.
 */
void affiche(classe_t *c)
{
    printf("Longueur de la classe : %d --- \n", longueur_classe(c));
    while(c) {
        printf("%d ", c->val);
        c = c->next;
    }
    printf("\n");
}


/**
 * libere. 
 * @param c : La classe. 
 * Supprime tous les éléments de la classe.
 */
void libere(classe_t *c)
{
    classe_t * tmp;

    while(c) {
        tmp = c->next;
        free(c);
        c = tmp;
    }
}


/*--------------------------------------Partition-------------------------------------------------------*/

/**
 * Créer la partition de taille n.
 * @param n taille de la partition
 * @return une partition composé des éléments {0, ..., n-1} où chaque élément est dans sa propre classe
 */
partition_t * creer(int n)
{
    partition_t * part = (partition_t *) malloc(sizeof(partition_t));
    if(!part){
        fprintf(stderr,"Erreur d'allocation de la partition.");
        exit(EXIT_FAILURE);
    }
    part->tab = (int *) malloc(n*sizeof(int));

    if(!part->tab){
        fprintf(stderr,"Erreur d'allocation du tableau dans la partition.");
        exit(EXIT_FAILURE);
    }
    for(int i = 0; i < n; i++){
        part->tab[i] = i;
    }

    part->hauteur_arbre = (int *) calloc(n, sizeof(int));

    if(!part->hauteur_arbre){
        fprintf(stderr,"Erreur d'allocation de la hauteur d'arbre dans la partition.");
        exit(EXIT_FAILURE);
    }

    part->taille = n;

    return part;
}

/**
 * Affiche la partition.
 * @param part partition que l'on souhaite afficher.
 */
void afficherPartition(partition_t * part)
{
    for(int i = 0; i < part->taille; i++){
        printf("Indice : %d -- Ancêtre : %d -- Hauteur : %d\n",i,part->tab[i], part->hauteur_arbre[i]);
    }
}


liste_classe_t * lister_partition(partition_t * part)
{
    liste_classe_t * liste = cree_liste_classe();
    int * deja_calcule = (int *) calloc(part->taille,sizeof(int));
    int i = 0;
    int etiquette_classe;
    for(i = 0; i < part->taille; i++){
        etiquette_classe = recuperer_classe(part, i);
        if(deja_calcule[etiquette_classe] == 0){
            deja_calcule[etiquette_classe]++;
            liste = ajoute_liste_classe(liste, lister_classe(part, etiquette_classe));
        }
    }
    free(deja_calcule);
    return liste;
}



/**
 * Libère la mémoire utilisée par la partition
 * @param part partition que l'on souhaite libérer.
 */
void libererPartition(partition_t ** part)
{
    free((*part)->tab);
    free((*part)->hauteur_arbre);
    free(*part);
    *part=NULL;
}


/*---------------------------------------------Fonctions sur les classes-------------------------------------*/

liste_classe_t * cree_liste_classe()
{
    return NULL;
}


/**
 * Récupère la classe d'un élément
 * @param part partition étudiée
 * @param element l'élement dont on souhaite connaitre la classe
 * @return l'étiquette de la classe de l'élément
 */
int recuperer_classe(partition_t * part, int element)
{
    if(part->tab[element] == element){
        return element;
    }
    return recuperer_classe(part, part->tab[element]);
}


void fusionner(partition_t * part, int indice1, int indice2)
{
    int Tx = recuperer_classe(part, indice1);
    int Ty = recuperer_classe(part, indice2);
    int hauteurTx = part->hauteur_arbre[Tx];
    int hauteurTy = part->hauteur_arbre[Ty];

    if(hauteurTx>hauteurTy){
        part->tab[Ty] = Tx;
    }else{
        part->tab[Tx] = Ty;
    }
    if(hauteurTx == hauteurTy){
        part->hauteur_arbre[Ty]++;
    }
}


classe_t * lister_classe(partition_t * part, int etiquette_classe)
{
    classe_t * classe = cree_classe();
    int index_classe;
    for(int i = 0; i < part->taille; i++){
        index_classe = recuperer_classe(part, i);
        if(index_classe == etiquette_classe){
            classe = ajoute(classe, i);
        }
    }
    return classe;
}


int longueur_classe(classe_t * classe)
{
    int i = 0;
    while(classe){
        i++;
        classe = classe->next;
    }
    return i;
}


void affiche_liste_classe(liste_classe_t * liste_classe)
{
    int i = 0;
    printf("%d\n",liste_classe->taille);
    while(liste_classe){
        printf("Classe n°%d : ", i);
        affiche(liste_classe->data);
        printf("\n");
        i++;
        liste_classe = liste_classe->next;
    }
}


liste_classe_t * ajoute_liste_classe(liste_classe_t * liste_classe, classe_t * classe)
{
    liste_classe_t * maillon = (liste_classe_t *) malloc(sizeof(liste_classe_t));
    if(!maillon){
        fprintf(stderr, "Erreur d'allocation de maillon");
        exit(EXIT_FAILURE);
    }
    
    maillon->next = liste_classe;
    maillon->taille = (!maillon->next) ? 1 : maillon->next->taille + 1;
    maillon->data = classe;
    return maillon;
}

int est_dans_classe(classe_t * classe, int element)
{
    while(classe){
        if(classe->val== element){
            return 1;
        }
        classe = classe->next;
    }
    return 0;
}


void libere_liste_classe(liste_classe_t * liste_classe)
{
    liste_classe_t * tmp = liste_classe;
    while(liste_classe){
        tmp = liste_classe;
        libere(liste_classe->data);
        liste_classe = liste_classe->next;
        free(tmp);
    }

}