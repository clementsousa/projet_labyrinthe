#ifndef __PROJET_ZZ1_FILE__
#define __PROJET_ZZ1_FILE__

#define TAILLE_MAX_FILE 500
typedef struct file{
    int tete;
    int queue;
    int taille;
    int tab[TAILLE_MAX_FILE];
}file_t;

file_t * file_creer();
int file_vide(file_t * file);
void file_enfiler(file_t * file, int element);
int file_defiler(file_t * file);
int file_tete(file_t * file);
int file_taille(file_t * file);
void file_afficher(file_t * file);
void file_liberer(file_t ** pointeur_file);

#endif
