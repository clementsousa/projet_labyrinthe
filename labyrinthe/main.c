#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

#include "constante.h"
#include "partition.h"
#include "exercice1.h"
#include "kruskal.h"
#include "labyrinthe.h"
#include "parcours_graphe.h"
#include "jeu.h"
#include "effets.h"
//#include "../effets_speciaux/animation_effets.h"

void test_affichage_simple(){
    partition_t * part = creer(10);
    afficherPartition(part);
    libererPartition(&part);
}

void test_obtenir_classe(){
    partition_t * part = creer(10);
    part->tab[5]=0;
    part->tab[6]=5;
    afficherPartition(part);
    for(int i = 0; i < 10;i++){
        printf("Classe de %d : %d\n", i, recuperer_classe(part,i));
    }
    libererPartition(&part);
}


void test_fusion(){
    partition_t * part = creer(11);
    fusionner(part, 0, 1);
    fusionner(part, 2, 3);
    fusionner(part, 10, 3);
    fusionner(part, 5, 9);
    fusionner(part, 4, 6);
    fusionner(part, 8, 7);
    fusionner(part, 9, 7);
    fusionner(part, 6, 8);

    afficherPartition(part);
    for(int i = 0; i < 11;i++){
        printf("Classe de %d : %d\n", i, recuperer_classe(part,i));
    }
    libererPartition(&part);
}

void test_classe(){
    partition_t * part = creer(11);
    fusionner(part, 0, 1);
    fusionner(part, 2, 3);
    fusionner(part, 10, 3);
    fusionner(part, 5, 9);
    fusionner(part, 4, 6);
    fusionner(part, 8, 7);
    fusionner(part, 9, 7);
    fusionner(part, 6, 8);

    afficherPartition(part);
    classe_t * classe = lister_classe(part, 0);
    affiche(classe);
    libere(classe);
    libererPartition(&part);
}

void test_partition(){
    partition_t * part = creer(11);
    fusionner(part, 0, 1);
    fusionner(part, 2, 3);
    fusionner(part, 10, 3);
    fusionner(part, 5, 9);
    fusionner(part, 4, 6);
    fusionner(part, 8, 7);
    fusionner(part, 9, 7);
    fusionner(part, 6, 8);

    afficherPartition(part);
    liste_classe_t * liste = lister_partition(part);
    affiche_liste_classe(liste);
    printf("Classe 0, 4 appartient : %d\n Classe 0, 0 appartient %d", est_dans_classe(liste->data, 4), est_dans_classe(liste->data, 0));
    libere_liste_classe(liste);
    libererPartition(&part);
}

void test_matrice_adjacence(){
    int tab[N][N] = {{0}};
    
    genere_matrice_adj(tab);
    
    affiche_Matrice(tab);
    partition_t* p = cree_partition(tab);
    liste_classe_t * l = lister_partition(p);
    affiche_liste_classe(l);

    graphviz("test2.txt", tab, l);

    libererPartition(&p);
    libere_liste_classe(l);
}

void test_graphe_arete(){
    graphe_t * graphe = genere_graphe_aleatoire();
    afficher_graphe(graphe);

    partition_t * p = cree_partition_couple(graphe);
    liste_classe_t * l = lister_partition(p);
    affiche_liste_classe(l);

    graphviz_couple("test_couple.txt", graphe, l);
    libererPartition(&p);
    libere_liste_classe(l);
}

void test_fisher_yates(){
    graphe_t * graphe = genere_graphe_aleatoire();
    afficher_graphe(graphe);
    fisher_yates(graphe);
    afficher_graphe(graphe);
    liberer_graphe(graphe);
}

void test_kruskal(){
    graphe_t * graphe = (graphe_t * ) malloc(sizeof(graphe_t));
    if(!graphe){
        puts("Erreur alloc graphe");
        return;
    }
    graphe->nombre_noeud = 3;
    graphe->nombre_arete = 3;
    graphe->liste_arete[0][0] = 0;
    graphe->liste_arete[0][1] = 1;
    graphe->liste_arete[1][0] = 1;
    graphe->liste_arete[1][1] = 2;
    graphe->liste_arete[2][0] = 0;
    graphe->liste_arete[2][1] = 2;

    afficher_graphe(graphe);

    int aretes_kruskal[3][2] = {0};
    kruskal(    graphe, aretes_kruskal);
    graphe->liste_arete[0][0] = aretes_kruskal[0][0];
    graphe->liste_arete[0][1] = aretes_kruskal[0][1];
    graphe->liste_arete[1][0] = aretes_kruskal[1][0];
    graphe->liste_arete[1][1] = aretes_kruskal[1][1];
    graphe->liste_arete[2][0] = aretes_kruskal[2][0];
    graphe->liste_arete[2][1] = aretes_kruskal[2][1];
    afficher_graphe(graphe);
    liberer_graphe(graphe);
}


void test_graphe_base(){
    graphe_t * graphe = genere_graphe_base_laby(4,3);
    afficher_graphe(graphe);
    liberer_graphe(graphe);
}


void test_kruskal_proba(){
    graphe_t * graphe = genere_graphe_base_laby(4,3);
    puts("=== GRAPHE DE BASE===");
    afficher_graphe(graphe);
    int aretes_kruskal_proba[500][2] = {0};
    kruskal_probaliste(graphe, aretes_kruskal_proba, 1);
    puts("=== KRUSKAL PROBABILISTE ===");
    for(int i = 0; aretes_kruskal_proba[i][0] != aretes_kruskal_proba[i][1]; i++){
            printf("(%d, %d)\n", aretes_kruskal_proba[i][0], aretes_kruskal_proba[i][1]); // On affiche l'arête
    }
    int aretes_kruskal[500][2] = {0};
    kruskal(graphe, aretes_kruskal); // Enfaite c'est juste Kruskal avec proba = 0
    puts("=== KRUSKAL NORMAL ===");
    for(int i = 0; aretes_kruskal[i][0] != aretes_kruskal[i][1]; i++){
            printf("(%d, %d)\n", aretes_kruskal[i][0], aretes_kruskal[i][1]); // On affiche l'arête
    }
    liberer_graphe(graphe);
}
/*
void test_genere_tableau_voisin(){
    int labyrinthe_resultat[NB_MAX_ARETE][2] = {{0}};
    int longueur = LAB_N;
    int hauteur = LAB_P;

    generer_labyrinthe(labyrinthe_resultat, longueur, hauteur, 0.3);
    puts("=== LABYRINTHE SOUS FORME D ARETE ==");
    for(int i = 0; labyrinthe_resultat[i][0] != labyrinthe_resultat[i][1]; i++){
            printf("(%d, %d)\n", labyrinthe_resultat[i][0], labyrinthe_resultat[i][1]); // On affiche l'arête
    }
    int tableau[6][LAB_N*LAB_P] = {{0}};

    puts("=== LABY SOUS FORME DE TABLEAU");

    genere_tableau_voisin(tableau);

    for(int i = 0; i < hauteur*longueur; i++){
        printf("Noeud n°%d, nb_voisin : %d voisins : ", tableau[0][i], tableau[1][i]);
        for(int j = 0; j < tableau[1][i]; j++){
            printf("%d ", tableau[2+j][i]);
        }
        printf("\n");
    }
}
    */


/*
int main(int argc, char const *argv[])
{
    (void) argc;
    (void) argv;
    
    srand(2);
    test_genere_tableau_voisin();
    //test_graphe_arete();
    //test_kruskal_proba();

    return 0;
}
*/
/*

int main()
{
    SDL_bool        event_utile = SDL_FALSE;
    SDL_bool        program_on = SDL_TRUE;
    SDL_Event       event;
    reglage_image   image = {0};
    reglage_image    map = {0};
    SDL_Surface  * image_surface={0};
    image_surface = IMG_Load("./image/image2.png");
    SDL_Surface  * map_surface={0};
    map_surface = IMG_Load("./image/image2.png");

    reglage_image   perso = {0};
    reglage_image   pine = {0};
    reglage_image   croix = {0};
    reglage_image   etoile1 = {0};

    int etat =0;
    trajet traje ={0};
    init_trajet(&traje);
    
    int position_personnage_x=0;
    int position_personnage_y=0;
    int x_initial=rand()%LAB_N;
    int y_initial=rand()%LAB_P;
    int x_final;
    int y_final;
    int position_dans_trajet=0;
    int nb_noeud_visite = 0;
    
            //---------------------------------Boucle du programme--------------------------------------------------//

            init_game();
            //ecran_accueil();
            SDL_RenderPresent(renderer);

            //LoadSurface("./image/image2.png",&image,image_surface);
            
            //transformation_de_translation( map_surface , 0.1);
            
            //LoadSurface(&image,image_surface);
            


            printf("ok\n");
            //ecran_accueil();
            //SDL_RenderPresent(renderer);
            //SDL_Delay(20000);   
            int liste_arete[NB_MAX_ARETE][2] = {{0}};
            labyrinthe_t * laby = (labyrinthe_t *) malloc(sizeof(labyrinthe_t)); // Pensez à le free
            generer_labyrinthe(liste_arete,LAB_N, LAB_P, 0.3);
            init_grille(laby, liste_arete);
            
            //affiche_grille(laby->grille);
            //trajet traj;
            //traj = destination_connue(laby, 6, 2);
            //traj = graphe_inconnu_dest_inconnue(laby, 17,12);
            //SDL_RenderPresent(renderer);
            //SDL_Delay(10000);
            //destination_connue_initialisation(2);
            //affiche_grille(laby);
            loadPicture("./image/image2.png", &image.source, &image.image);
            init_param(&image);
            loadPicture("./image/dwarf.png", &perso.source, &perso.image);
            init_animation (&perso,4,9);
            loadPicture("./image/croix.png", &croix.source, &croix.image);
            init_animation(&croix,1,1);
            loadPicture("./image/pine.png", &pine.source, &pine.image);
            init_animation(&pine,1,1);
            loadPicture("./image/etoile1.png", &etoile1.source, &etoile1.image);
            init_animation(&etoile1,1,1);

            dijkstra(laby, 14);
            //affiche_labyrinthe_dessin(laby);
            int dest = rand()%(LAB_N*LAB_P);
            //a_star(laby, 14, dest, distance_euclidienne, &nb_noeud_visite);
            //printf("Nombre de noeuds visité en utilisant la distance euclidienne comme heuristique : %d\n", nb_noeud_visite);
            //a_star(laby, 14, dest, distance_manhattan, &nb_noeud_visite);
            //printf("Nombre de noeuds visité en utilisant la distance de manhattan comme heuristique : %d\n", nb_noeud_visite);
            //a_star(laby, 14, dest, distance_de_tchebychev, &nb_noeud_visite);
            //printf("Nombre de noeuds visité en utilisant la distance de tchebychev comme heuristique : %d\n", nb_noeud_visite);
            affiche_labyrinthe_dessin(laby);
            SDL_RenderPresent(renderer);   
            SDL_RenderClear(renderer);
            //affiche_labyrinthe_image(laby, image);
            //affiche_sans_SDL(laby);
            SDL_RenderPresent(renderer);
           
            SDL_Delay(500);

            genere_position_aleatoire_de_debut_et_de_fin(&x_final , &y_final);
            //rempli_file_du_chemin_a_parcourir( laby, &traj, x_initial, y_initial, x_final, y_final);

            #ifndef DEBUG       
        
            if ((window_main) && (renderer)) {
            }
            #endif
    
            while(program_on) {
                
                SDL_RenderClear(renderer);
                
           
                 
                animation_deplacement( laby,  &traje, map_surface, &map,  image, perso, etoile1, croix, pine, &x_initial,  &y_initial,  &x_final, &y_final, &position_dans_trajet, position_personnage_x, position_personnage_y);
                #ifndef DEBUG
                event_utile = SDL_FALSE;

                
                const Uint8* keystates = SDL_GetKeyboardState(NULL); // Récupère l'état des touches du clavier

                //On appuie sur la touche échape pour quitter
                if (keystates[SDL_SCANCODE_ESCAPE]) program_on = 0;

                
                while(!event_utile && SDL_PollEvent(&event)) {
    
    
                    switch(event.type) {
                        case SDL_MOUSEBUTTONDOWN:
                        
                        
                        
                            
                        case SDL_QUIT : 
                            program_on = 0;
                            event_utile = SDL_TRUE;
                            break;
                        default :
                            break;
                    }
                    
                }
                
                #endif
           
        SDL_Delay(100);            
    } 


        
    
    // Libération du labyrinthe
    //free(laby);
        
    //Libération des composantes SDL
        
    #ifndef DEBUG
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window_main);
    IMG_Quit();
    SDL_Quit();
    #endif
            
    return 0;
}
*/