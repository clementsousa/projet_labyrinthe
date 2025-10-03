#ifndef __PROJET_ZZ1_ABR__
#define __PROJET_ZZ1_ABR__

typedef struct abr{
	int val;
	struct abr * gauche;
	struct abr * droite;
}abr;

abr * abr_creer_noeud(int val);
abr * abr_insertion(abr * root, int val);
abr * abr_val_min(abr * root);
abr * abr_val_max(abr * root);
int abr_est_feuille(abr * root);
abr * abr_supprimer(abr * root, int val);
void abr_liberer(abr * root);
void abr_afficher(abr * root);
int abr_recherche(abr * root, int val);

#endif