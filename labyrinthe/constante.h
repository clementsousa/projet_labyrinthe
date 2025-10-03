#ifndef __PROJET_ZZ1_CONSTANTE__
#define __PROJET_ZZ1_CONSTANTE__


#define LAB_N 10 // Largeur du labyrinthe
#define LAB_P 10 // Hauteur du labyrinthe
#define NB_MAX_ARETE (2*LAB_N*LAB_P - LAB_P - LAB_N)
#define NB_CONFIG 16
#define MAX_DISTANCE LAB_N*LAB_P - 1
#define NB_TERRAIN 6
#define TAILLE_DU_TAS LAB_N*LAB_P
#define DISTANCE_INIT 20000000
#define TAILLE_TRAJET (LAB_N * LAB_P*10)


/**
 * Constante liés aux monstres
 */
#define MAX_MONSTRES 30
// Stats de base des monstres "fouineurs"
#define ATK_MONSTRE_FOUINEUR 2
#define VIE_MONSTRE_FOUINEUR 3
#define SPD_MONSTRE_FOUINEUR 6
// Stats de base des monstres "savants"
#define ATK_MONSTRE_SAVANT 3
#define VIE_MONSTRE_SAVANT 4
#define SPD_MONSTRE_SAVANT 2

// Stats de base des joueurs
#define ATK_JOUEUR 2
#define VIE_JOUEUR 4
#define SPD_JOUEUR 4

#endif
