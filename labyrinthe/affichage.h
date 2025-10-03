#ifndef __AFFICHAGE_ANIMATIONS__
#define __AFFICHAGE_ANIMATIONS__


void afficher_piece(donjon_t *donj, int temps);
void afficher_effets(donjon_t *donj, int temps);

void animation_a_afficher(reglage_image img, int x,int y, int h, int l,int animation,int temps);
void animation_monstre(donjon_t *donj, int temps);
void animation_perso(int direction, int temps);
void animation_perso_minimap(int posx, int posy, int direction, int temps);
void animation_monstre_fluide(donjon_t * donj, int x, int y, int temps);




#endif