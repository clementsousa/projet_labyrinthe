#ifndef __PROJET_ZZ1_PARTITION__
#define __PROJET_ZZ1_PARTITION__

typedef struct partition{
    int * tab;  
    int * hauteur_arbre;
    int taille;
}partition_t;

typedef struct classe{
    int val;
    struct classe *next;
} classe_t;

typedef struct liste_classe{
    classe_t * data;
    struct liste_classe * next;
    int taille;
}liste_classe_t;



/* DECLARATIONS DES METHODES */


classe_t* cree_classe();
classe_t * cree_maillon(int val);
classe_t * ajoute(classe_t * c, int val);
void affiche(classe_t *c);
void libere(classe_t *c);

void affiche_liste_classe(liste_classe_t *);
liste_classe_t * ajoute_liste_classe(liste_classe_t *, classe_t *);
liste_classe_t * cree_liste_classe();
void libere_liste_classe(liste_classe_t *);

partition_t * creer(int);
int recuperer_classe(partition_t *, int);
int longueur_classe(classe_t * );
void fusionner(partition_t *, int, int);
classe_t * lister_classe(partition_t *, int);
liste_classe_t * lister_partition(partition_t *);
void afficherPartition(partition_t *);
void libererPartition(partition_t **);

int est_dans_classe(classe_t *, int);



#endif