/**
 * === NOTE IMPORTANTE ===
 * Ce fichier va uniquement servir pour les tests des fonctions liés à coeur.c
 * Il doit pouvoir être supprimé à tout moment.
 */
#include "coeur.h"
#include "jeu.h"
#include "kruskal.h"
#include "parcours_graphe.h"

void test_genere_donjon(){
    donjon_t * donjon = generation_donjon(FACILE);
    debug_affiche_donjon(donjon);
    free(donjon->laby);
    free(donjon);
}

void test_compte_case(){
    donjon_t * donjon = generation_donjon(FACILE);
    for(int i = 0; i < donjon->nb_monstre; i++){
        donjon->tab_monstres[i].position = 0;
    }
    printf("%d\n", get_nb_monstre_case(donjon, 0, 0));
    free(donjon->laby);
    free(donjon);
}

void test_deplacement_monstre(){
    donjon_t * donjon = generation_donjon(FACILE);
    debug_affiche_donjon(donjon);
    deplacement_monstre_grille(donjon, 0 , 0);
    debug_affiche_donjon(donjon);
    free(donjon->laby);
    free(donjon);
}
void test_deplacement_joueur(){
    donjon_t * donjon = generation_donjon(FACILE);
    debug_affiche_donjon(donjon);
    deplacement_joueur_grille(donjon, 0);
    debug_affiche_donjon(donjon);
    free(donjon->laby);
    free(donjon);
}

void test_combat(){
    donjon_t * donjon = generation_donjon(FACILE);
    debug_affiche_donjon(donjon);
    printf("Egalité normalement : %d\n", combat(donjon, &(donjon->joueur), &(donjon->tab_monstres[0])));
    
    donjon->joueur.vie = 100;
    donjon->tab_monstres[0].vie = 5;
    printf("Victoire normalement : %d\n", combat(donjon, &(donjon->joueur), &(donjon->tab_monstres[0])));
    donjon->joueur.vie = 10;
    donjon->tab_monstres[0].vie = 100;
    printf("Défaite normalement : %d\n", combat(donjon, &(donjon->joueur), &(donjon->tab_monstres[0])));

    liberer_donjon(&donjon);
}

void test_direction_valide(){
    donjon_t * donjon = generation_donjon(FACILE);
    debug_affiche_donjon(donjon);
    deplacement_joueur_grille(donjon, 9);
    printf("Attendu : 0 DIRECTION EST à partir de 9 : %d\n", direction_valide(donjon, EST));
    printf("Attendu : 1 DIRECTION OUEST à partir de 9 : %d\n", direction_valide(donjon, OUEST));
    liberer_donjon(&donjon);

}

void test_gen_trajet_monstre(){
    donjon_t * donjon = generation_donjon(FACILE);
    debug_affiche_donjon(donjon);
    genere_trajet_monstre(donjon, 2);
    debug_affiche_trajet(donjon->tab_monstres[2].traj);
    liberer_donjon(&donjon);
}


void test_jeu()
{
    donjon_t * donjon = generation_donjon(FACILE);
    int choix;
    int direction;
    int piece_collecte = 0;
    //debug_affiche_donjon(donjon);
    while(etat_jeu(donjon) == PARTIE_EN_COURS){
//        deplacement_joueur_grille(donjon, trouver_piece(donjon) - LAB_N); 
       /* if(rand()%3){
            genere_bonus_malus(donjon);
        }*/
        debug_affiche_donjon(donjon);

        printf("0 = pas bouger, 1 = nord, 2 = est, 4 = sud, 8 = ouest\n");
        scanf("%d", &choix);
        if(choix == NORD || choix == EST ||choix == SUD || choix == OUEST){
            if(direction_valide(donjon, choix)){
                direction = calcul_pos_noeud_voisin(donjon->joueur.position, choix);
                deplacement_joueur_grille(donjon, direction);
            }
        }
        update_tout_trajet_monstre(donjon);
        

        while(verifie_collision_monstre(donjon)){
            int id_monstre = obtenir_id_monstre_collision(donjon);
            resultat_combat res = combat(donjon, &(donjon->joueur), &(donjon->tab_monstres[id_monstre]));
            switch (res)
            {
            case VICTOIRE_JOUEUR:
                printf("\e[32m Victoire du joueur contre le monstre n°%d\n\e[0m", id_monstre);
                break;
            case DEFAITE_JOUEUR:
                printf("\e[31m Défaite du joueur contre le monstre n°%d\n\e[0m", id_monstre);
                break;
            case EGALITE:
                printf("\e[33m Egalité du joueur contre le monstre n°%d\n\e[0m", id_monstre);
                break;
            default:
                break;
            }
        }
        gere_collision_bonus_malus(donjon);
        piece_collecte = verifie_collision_piece(donjon);
        if(piece_collecte){
            printf("\e[33m PIECE COLLECTE\e[0m");
            genere_tout_trajet_monstre(donjon);
        }
    }
    if(etat_jeu(donjon) == PARTIE_GAGNEE){
        printf("\e[32mFélicitation, vous avez gagné la partie !\n\e[0m");
    }else{
        printf("\e[31mVous avez perdu. Dommage !\n\e[0m");
    }
    liberer_donjon(&donjon);
}

void test_collision_meme_type(){
    donjon_t * donjon = generation_donjon(DIFFICILE);
    deplacement_monstre_grille(donjon, 6, 3);
    deplacement_monstre_grille(donjon, 7, 3);

    debug_affiche_donjon(donjon);
    liberer_donjon(&donjon);
}

void test_effet_saturation_de_bonus(){
    donjon_t * donjon = generation_donjon(FACILE);
    for(int i = 0; i < 100; i++){
        genere_bonus_malus(donjon);
    }
    debug_affiche_donjon(donjon);
    liberer_donjon(&donjon);
}
/*
int main()
{
    srand(2);
    test_jeu();
    return 0;
}
*/