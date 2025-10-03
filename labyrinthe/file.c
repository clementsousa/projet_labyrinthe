#include <stdio.h>
#include <stdlib.h>

#include "file.h"

/**
 * creer_file
 * @return une file vide
 */
file_t * file_creer(){

    file_t * file = (file_t *) malloc(sizeof(file_t));
    if(!file){
        fprintf(stderr, "Erreur lors de l'allocation de la file.");
        exit(EXIT_FAILURE);
    }

    file->tete = 0;
    file->queue = 0;
    file->taille = 0;

    return file;
}

/**
 * file_vide
 * @param file : la file dont on veut savoir si elle est vide ou non
 * @return "booléen" si la file est vide ou non
 */
int file_vide(file_t * file){
    if(!file){
        fprintf(stderr, "Erreur file NULL dans file_vide.");
        exit(EXIT_FAILURE);
    }
    return file->taille == 0;
}

/**
 * enfiler
 * @param file : la file où l'on souhaite insérer la valeur
 * @param element : la valeur à insérer
 */
void file_enfiler(file_t * file, int element){
    if(!file){
        fprintf(stderr, "Erreur file NULL dans enfiler.");
        exit(EXIT_FAILURE);
    }
    if(file->taille == TAILLE_MAX_FILE){
        fprintf(stderr, "Erreur file pleine dans enfiler.");
        exit(EXIT_FAILURE);
    }

    file->tab[file->queue] = element;
    file->queue = (file->queue+1)%TAILLE_MAX_FILE;
    file->taille++;
}

/**
 * defiler
 * @param file : la file que l'on souhaite défiler
 * @return : la valeur défiler
 */
int file_defiler(file_t * file){
    if(!file){
        fprintf(stderr, "Erreur file NULL dans defiler.");
        exit(EXIT_FAILURE);
    }
    if(file_vide(file)){
        fprintf(stderr, "Erreur file vide dans defiler.");
        exit(EXIT_FAILURE);
    }
    int element = file->tab[file->tete];
    file->tete = (file->tete + 1)%TAILLE_MAX_FILE;
    file->taille--;
    return element;
}
/**
 * file_tete
 * @param file : la file concernée
 * @return la valeur de la tete de la file, sans la retirer
 */
int file_tete(file_t * file){
    if(!file){
        fprintf(stderr, "Erreur file NULL dans defiler.");
        exit(EXIT_FAILURE);
    }
    if(file_vide(file)){
        fprintf(stderr, "Erreur file vide dans file_tete. Impossible de lire la tête.");
        exit(EXIT_FAILURE);
    }
    return file->tab[file->tete];
}

/**
 * file_taille
 * @param file : la file concernée
 * @return la taille de la file
 */
int file_taille(file_t * file){
    if(!file){
        fprintf(stderr, "Erreur file NULL dans file_taille.");
        exit(EXIT_FAILURE);
    }
    return file->taille;
}

/**
 * afficher_file
 * @param file : file que l'on souhaite afficher
 */
void file_afficher(file_t * file){
    if(!file){
        fprintf(stderr, "Erreur file NULL dans afficher_file.");
        exit(EXIT_FAILURE);
    }
    printf("Taille de la file : %d\n", file->taille);
    for(int i = file->tete; i != file->queue; i++){
        i %= TAILLE_MAX_FILE;
        printf("%d ", file->tab[i]);
    }
    printf("\n");
}


/**
 * liberer_file
 * @param pointeur_file : un pointeur sur la file à libérer
 * Met à NULL le pointeur une fois la liste libérée.
 */
void file_liberer(file_t ** pointeur_file){
    if(!pointeur_file){
        fprintf(stderr, "Erreur file pointeur_file dans liberer_file.");
        exit(EXIT_FAILURE);
    }
    free(*pointeur_file);
    *pointeur_file = NULL;
}