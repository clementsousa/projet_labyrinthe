#ifndef __PROJET_ZZ1_COEUR__
#define __PROJET_ZZ1_COEUR__
#include "constante.h"
#include "labyrinthe.h"
#include "jeu.h"
#include "kruskal.h"

typedef enum{
    JOUEUR = 1,
    MONSTRE_FOUINEUR = 2,
    MONSTRE_SAVANT = 4,
    PIECE = 8,
    BONUS_MALUS = 16
}entite;

typedef enum{
    DIRECTION_NORD = 1,
    DIRECTION_EST = 2,
    DIRECTION_SUD = 3,
    DIRECTION_OUEST = 4
}direction;

typedef enum{
    DEFAITE_JOUEUR = 0,
    VICTOIRE_JOUEUR = 1,
    EGALITE = 2
}resultat_combat;

typedef enum{
    PARTIE_PERDUE = 0,
    PARTIE_GAGNEE = 1,
    PARTIE_EN_COURS = 2
}etat_partie;


typedef struct{
    int id_monstre;
    int attaque;
    int vie;
    int vitesse;
    int piece;
    int est_savant;
    int position; // Le noeud sur lequel le monstre est présent
    trajet traj;
    int etape_traj;
    direction derniere_direction;
    direction direction_actuelle;
}monstre_t;

typedef struct{
    int attaque;
    int vie;
    int vitesse;
    int piece;
    int position;
    direction derniere_direction;   // Le dernier mouvement qu'à fait le joueur 
                                    // Peut-être inutile, mais peut-être utile en SDL
}joueur_t;

typedef struct{
    labyrinthe_t * laby;
    joueur_t joueur;
    entite presence_entite[LAB_P][LAB_N];
    monstre_t tab_monstres[MAX_MONSTRES]; 
    int nb_monstre;
    int nb_or_pour_gagner;
    int nb_or_restant_a_generer;
    int position_or;
}donjon_t; 

typedef struct{
    int atk;
    int vie;
    int spd;
}effet_t;

donjon_t * generation_donjon(difficulte_enum difficulte);

void genere_piece(donjon_t * donjon);

void debug_affiche_donjon(donjon_t * donjon);
void debug_affiche_trajet(trajet traj);
void affiche_map_donjon(donjon_t * donjon);

int get_nb_monstre_case(donjon_t * donjon, int est_savant, int noeud);
void deplacement_monstre_grille(donjon_t * donjon, int id_monstre , int destination);
void deplacement_joueur_grille(donjon_t * donjon, int destination);

resultat_combat combat(donjon_t * donjon, joueur_t * joueur, monstre_t * monstre);

void liberer_donjon(donjon_t ** donjon);

etat_partie etat_jeu(donjon_t * donjon);

int direction_valide(donjon_t * donjon, cardinal direction);
int calcul_pos_noeud_voisin(int noeud, cardinal direction);

int verifie_collision_piece(donjon_t * donjon);
int trouver_piece(donjon_t * donjon);

int verifie_collision_monstre(donjon_t * donjon);
int obtenir_id_monstre_collision(donjon_t * donjon);

void genere_trajet_monstre(donjon_t * donjon, int id_monstre);
void genere_tout_trajet_monstre(donjon_t * donjon);
void update_trajet_monstre(donjon_t * donjon, int id_monstre);
void update_tout_trajet_monstre(donjon_t * donjon);

void genere_bonus_malus(donjon_t * donjon);
void gere_collision_bonus_malus(donjon_t * donjon);


#endif
