#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "partition.h"
#include "kruskal.h"
#include "labyrinthe.h"
#include "parcours_graphe.h"
#include "jeu.h"
#include "coeur.h"
#include "effets.h"
#include "affichage.h"
#include "animation_effets.h"



int ETAT_JEU;
int difficulte;
texte jeu_texte; 
texte facile;
texte moyen;
texte difficile;
texte quit;
texte retour_menu;
texte retour_jeu;
texte pause_mus;
texte reprise_mus;
SDL_Color blanc = {255, 255, 255, 0};
SDL_Color rouge = {255,0,0,0};
SDL_Color jaune = {255, 241, 0, 0};
donjon_t *donj = NULL;


void animation_mort(reglage_image image_laby, reglage_image perso_combat) 
{
    
    affiche_labyrinthe_image(donj->laby, image_laby); 
    afficher_piece(donj,1);
    afficher_effets(donj,0);
    animation_a_afficher(perso_combat, (largeur /2 )-1.5*perso_combat.destination.w,hauteur /2-1.5*perso_combat.destination.h,3, 3 ,0,0);
    reglage_image *map = (reglage_image *)malloc(sizeof(reglage_image));
    SDL_Surface *map_surface = convertion_renderer_surface(largeur, hauteur);

    LoadSurface(map, map_surface);
    if (!map || !map->image) {
        printf("Échec du chargement de la carte.\n");
        return;
    }

    for(int i = 0; i < 20; i++) {
        SDL_RenderClear(renderer);
        SDL_Texture * mort = animation3(renderer, map_surface, i);
        SDL_RenderCopy(renderer, mort, 0, 0);
        SDL_RenderPresent(renderer);             
        SDL_Delay(5);  
    }

    SDL_FreeSurface(map_surface);
    free(map);
}

void init_game()
{
    #ifndef DEBUG   
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        SDL_Log("Error : SDL initialisation - %s\n", SDL_GetError());
        exit(EXIT_FAILURE);
    }

    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 1024) == -1) {
        SDL_Log("Error : Mix_OpenAudio - %s\n", Mix_GetError());
        exit(EXIT_FAILURE);
    }
    

    
    SDL_GetCurrentDisplayMode(0, &user);
    largeur = user.w/2;
    hauteur = user.h/2;

    window_main = SDL_CreateWindow("Main",SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, largeur, hauteur, SDL_WINDOW_SHOWN); 

        
    if (!window_main) {
        SDL_Log("Error : SDL window 1 creation - %s\n", SDL_GetError());                
        SDL_Quit();      
        exit(EXIT_FAILURE);
    }

    if (IMG_Init(IMG_INIT_PNG) == 0){
        fprintf(stderr, "Erreur de creation de l'image: %s\n", SDL_GetError()); 
    }

    if (TTF_Init() != 0) fprintf(stderr, "Erreur d'initialisation TTF : %s\n", TTF_GetError());

    if (SDL_Init(SDL_INIT_AUDIO) == -1) {
        fprintf(stderr, "init SDL error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    
    renderer = SDL_CreateRenderer(window_main, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == 0) fprintf(stderr, "Erreur de creation d'un renderer : %s\n", SDL_GetError()); 

    #else
        //init mode text
    #endif 
}


void ecriture(texte *text, TTF_Font *font, const char *message)
{
    int iW, iH;
    text->surf = TTF_RenderText_Blended(font, message, text->color);
    text->text = SDL_CreateTextureFromSurface(renderer, text->surf);
    SDL_QueryTexture(text->text, NULL, NULL, &iW, &iH);
    SDL_RenderCopy(renderer, text->text, NULL, &text->position);
    SDL_FreeSurface(text->surf);
    text->surf = NULL;
    SDL_DestroyTexture(text->text);
    text->text = NULL;
}


void init_texte()
{
    ETAT_JEU = ACCUEIL;
                

            
    SDL_Delay(20);  

    jeu_texte.position.x = largeur/4;
    jeu_texte.position.y = hauteur / 32;
    jeu_texte.position.w = largeur/2;
    jeu_texte.position.h =  hauteur /20;

    facile.position.x = largeur/14;
    facile.position.y = hauteur / 2;
    facile.position.w = largeur/5;
    facile.position.h =  hauteur /20;

    moyen.position.x = largeur/2.5;
    moyen.position.y = hauteur / 2;
    moyen.position.w = largeur/5;
    moyen.position.h =  hauteur /20;

    difficile.position.x = largeur/1.4;
    difficile.position.y = hauteur / 2;
    difficile.position.w = largeur/5;
    difficile.position.h =  hauteur /20;

    quit.position.y = hauteur / 1.2;
    quit.position.w = largeur/4;
    quit.position.h =  hauteur /20;
    quit.position.x = largeur/2 - (quit.position.w / 2);

    reprise_mus.position.x = largeur/ 6;
    reprise_mus.position.y = hauteur / 2;
    reprise_mus.position.w = largeur/4;
    reprise_mus.position.h =  hauteur / 15;

    retour_menu.position.y = hauteur / 1.2;
    retour_menu.position.w = largeur/2;
    retour_menu.position.x = largeur / 2 - retour_menu.position.w/2;
    retour_menu.position.h =  hauteur /20;

    
    retour_jeu.position.y = hauteur / 16;
    retour_jeu.position.w = largeur/2;
    retour_jeu.position.x = largeur / 2 - retour_jeu.position.w/2;
    retour_jeu.position.h =  hauteur /20;

    pause_mus.position.x = largeur/ 2;
    pause_mus.position.y = hauteur / 2;
    pause_mus.position.w = largeur/4;
    pause_mus.position.h =  hauteur /15;


}


void ecriture_ecran_accueil(TTF_Font * font)
{
    reglage_image r = {0};
    loadPicture("./image/decor.jpg", &r.source, &r.image);
                   

    //Background
    r.offset_x = r.source.w;  
    r.offset_y = r.source.h;   
    
    r.source.x = 0;
    r.source.y = 0;
        
    r.destination.w = largeur;             
    r.destination.h = hauteur;       
    r.destination.y = 0; 
    r.destination.x = 0; 
    SDL_RenderCopy(renderer, r.image, &r.source, &r.destination);
    ecriture(&jeu_texte, font, "Labyrinthe World");
    ecriture(&facile, font, "Facile");
    ecriture(&moyen, font, "Moyen");
    ecriture(&difficile, font, "Difficile");
    ecriture(&quit, font,"Quitter");
    SDL_DestroyTexture(r.image);
}

void option_musique(TTF_Font * font)
{
    reglage_image r = {0};
    loadPicture("./image/decor.jpg", &r.source, &r.image);
                   

    //Background
    r.offset_x = r.source.w;  
    r.offset_y = r.source.h;   
    
    r.source.x = 0;
    r.source.y = 0;
        
    r.destination.w = largeur;             
    r.destination.h = hauteur;       
    r.destination.y = 0; 
    r.destination.x = 0; 
    SDL_RenderCopy(renderer, r.image, &r.source, &r.destination);
    ecriture(&retour_menu, font, "Retour au menu");
    ecriture(&reprise_mus, font, "Reprendre la musique");
    ecriture(&pause_mus, font, "Mettre en pause la musique");  
    if(donj) ecriture(&retour_jeu, font, "Retour au jeu");
    SDL_DestroyTexture(r.image);
}

void affiche_info_jeu(TTF_Font * font)
{
    SDL_Rect rectangle;    
    texte nb_piece; 
    char texte_piece_joueur[20];
    char texte_vie_joueur[20];
    char texte_attaque_joueur[20];
    char texte_piece_monstre[20];
    int somme = 0; 

    rectangle.x = 0;                                             
    rectangle.y = 0; 

    SDL_SetRenderDrawColor(renderer, 100,100,100, 125);                                 
                                      
    rectangle.w = largeur/5 + 4;                                                
    rectangle.h = hauteur/20 * 3 + 4; 
                                          
                                  
    SDL_RenderFillRect(renderer, &rectangle);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
  
    rectangle.w = largeur/5;                                                
    rectangle.h = hauteur/20 * 3;   
    SDL_RenderFillRect(renderer, &rectangle);

    rectangle.x = 2 * largeur/5;                                             
    rectangle.y = 0; 

    SDL_SetRenderDrawColor(renderer, 100,100,100, 125);                                 
                                      
    rectangle.w = largeur/5 + 4;                                                
    rectangle.h = hauteur/20 + 4; 
                                          
                                  
    SDL_RenderFillRect(renderer, &rectangle);
    SDL_SetRenderDrawColor(renderer, 0,0,0,0);
  
    rectangle.w = largeur/5;                                                
    rectangle.h = hauteur/20;   
    SDL_RenderFillRect(renderer, &rectangle);


    for(int i = 0; i < donj->nb_monstre; i++) {
        somme += donj->tab_monstres[i].piece;
    }

    nb_piece.position.x = 0;
    nb_piece.position.y = 0;
    nb_piece.position.w = largeur/5;
    nb_piece.position.h =  hauteur /20;
    nb_piece.color = blanc;

    sprintf(texte_piece_joueur,"Piece joueur   : %d", donj->joueur.piece);
    sprintf(texte_vie_joueur,"Vie joueur     : %d", donj->joueur.vie);
    sprintf(texte_attaque_joueur,"Attaque joueur : %d", donj->joueur.attaque);
    sprintf(texte_piece_monstre,"Piece monstres : %d", somme);
    ecriture(&nb_piece, font, texte_piece_joueur);
    nb_piece.position.y = hauteur /20;
    ecriture(&nb_piece, font, texte_vie_joueur);
    nb_piece.position.y += hauteur /20;
    ecriture(&nb_piece, font, texte_attaque_joueur);
    nb_piece.position.y = 0;
    nb_piece.position.x = 2 * nb_piece.position.w;
    nb_piece.color = rouge; 
    ecriture(&nb_piece, font, texte_piece_monstre);

    SDL_FreeSurface(nb_piece.surf);
}



void deplacement(reglage_image * image, int dep, int *pos_x, int *pos_y, int x_grille, int y_grille)
{
    int speed = 4;
    int distance_mur_horizontale = 30; 
    int distance_mur_verticale = 20; 
    int x_case = *pos_x % (largeur / LAB_N);
    int y_case = *pos_y % (hauteur / LAB_P);
    switch(dep) {
        case DROIT :
            if(!(donj->laby->grille[y_grille][x_grille] & EST ) || (((x_case + distance_mur_horizontale) < (largeur/LAB_N)) )) {
                *pos_x += speed;

                image->destination.x -= speed;
            }

            break;
        case GAUCHE :
            if(!(donj->laby->grille[y_grille][x_grille] & OUEST ) || ((x_case - distance_mur_horizontale) > 0)) {
            *pos_x -= speed;

            image->destination.x += speed;
            image->state.x += image->offset_x;
            image->state.x %= image->source.w;

            }
            break;
        case HAUT :
            if(!(donj->laby->grille[y_grille][x_grille] & NORD ) || ((y_case - distance_mur_verticale) > 0)) {
            *pos_y-= speed;


            image->destination.y += speed;
            image->state.x += image->offset_x;
            image->state.x %= image->source.w;
            }
            break;

        case BAS :
            if(!(donj->laby->grille[y_grille][x_grille] & SUD ) || ((y_case + distance_mur_verticale+10) < (hauteur /LAB_P))) {
            *pos_y += speed;


            image->destination.y -= speed;
            image->state.x += image->offset_x;
            image->state.x %= image->source.w;
            }
        break;    
    }
}

void update_position(int * pos_x_grille, int * pos_y_grille, int pos_x_sdl, int pos_y_sdl, int mouvement)
{         
      switch(mouvement) {
        
        case DROIT : 
            if ((( pos_x_sdl / (largeur / LAB_N )) - *pos_x_grille) != 0 ) {
                (*pos_x_grille)++;
                int direction = calcul_pos_noeud_voisin(donj->joueur.position, EST);
                deplacement_joueur_grille(donj, direction);
            }
            break;
        case GAUCHE :   
            if ((( pos_x_sdl / (largeur / LAB_N )) - *pos_x_grille) != 0 ) {
                (*pos_x_grille)--;
                int direction = calcul_pos_noeud_voisin(donj->joueur.position, OUEST);
                deplacement_joueur_grille(donj, direction);
            }
            break;
        case BAS : 
            if ((( pos_y_sdl / (hauteur/ LAB_P)) - *pos_y_grille) != 0 ) {
                (*pos_y_grille)++;
                int direction = calcul_pos_noeud_voisin(donj->joueur.position, SUD);
                deplacement_joueur_grille(donj, direction);
            }
            break;
        case HAUT : 
        if ((( pos_y_sdl / (hauteur/ LAB_P)) - *pos_y_grille) != 0 ) {
            (*pos_y_grille)--;
            int direction = calcul_pos_noeud_voisin(donj->joueur.position, NORD);
            deplacement_joueur_grille(donj, direction);
        }
        break;
    }

}

void init_terrain(int pos_x, int pos_y) {
    reglage_image *map = (reglage_image *)malloc(sizeof(reglage_image));
    SDL_Surface *map_surface = convertion_renderer_surface(largeur, hauteur);

    LoadSurface(map, map_surface);
    if (!map || !map->image) {
        printf("Échec du chargement de la carte.\n");
        return;
    }

    map->offset_x = map->source.w;
    map->offset_y = map->source.h;

    map->state.w = map->offset_x;
    map->state.h = map->offset_y;

    map->destination.w = largeur;
    map->destination.h = hauteur;

    // Découpe du src
    SDL_Rect src = {
        .x = pos_x - 1.53 * (largeur / LAB_N),
        .y = pos_y - 1.53 * (hauteur / LAB_P),
        .w = 3 * (largeur / LAB_N),
        .h = 3 * (hauteur / LAB_P)
    };
    // Destination
    SDL_Rect dst = {
        .x = 0,
        .y = 0,
        .w = map->destination.w,
        .h = map->destination.h
    };
    // Couleur de fond (noir)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    reglage_image r = {0};
    loadPicture("./image/fond2.png", &r.source, &r.image);
                   

    //Background
    r.offset_x = r.source.w;  
    r.offset_y = r.source.h;   
    
    r.source.x = 0;
    r.source.y = 0;
        
    r.destination.w = largeur;             
    r.destination.h = hauteur;       
    r.destination.y = 0; 
    r.destination.x = 0; 
    SDL_RenderCopy(renderer, r.image, &r.source, &r.destination);

    // Limites de la carte
    int map_w = map->source.w;
    int map_h = map->source.h;

    // Si src dépasse à gauche
    if (src.x < 0) {
        dst.x = -src.x * ((float)largeur / src.w);
        src.w += src.x;
        src.x = 0;
    }
    // Si src dépasse en haut
    if (src.y < 0) {
        dst.y = -src.y * ((float)hauteur / src.h);
        src.h += src.y;
        src.y = 0;
    }
    // Si src dépasse à droite
    if (src.x + src.w > map_w) {
        src.w = map_w - src.x;
    }
    // Si src dépasse en bas
    if (src.y + src.h > map_h) {
        src.h = map_h - src.y;
    }

    // Redimensionne la destination
    dst.w = src.w * ((float)largeur / (3 * (largeur / LAB_N)));
    dst.h = src.h * ((float)hauteur / (3 * (hauteur / LAB_P)));

    // Copie du bout de carte à l’écran
    SDL_RenderCopy(renderer, map->image, &src, &dst);
    SDL_FreeSurface(map_surface);
    SDL_DestroyTexture(r.image);
    free(map);
    
}

void liberer_texte(texte *t) {
    if (t->text) {
        SDL_DestroyTexture(t->text);
        t->text = NULL;
    }
    if (t->surf) {
        SDL_FreeSurface(t->surf);
        t->surf = NULL;
    }
}


void init_perso(reglage_image *perso)
{
    perso->destination.w *= 2;
    perso->destination.h *= 2;    
}



int main()
{      
    
    SDL_bool        event_utile = SDL_FALSE;
    SDL_bool        program_on = SDL_TRUE;
    SDL_Event       event;
    
    reglage_image image_laby = {0};
    reglage_image   perso = {0};
    reglage_image perso_combat = {0};
    reglage_image monstre_combat = {0};

    reglage_image   mini_map = {0};


    TTF_Font *font = NULL;
    resultat_combat res = -1;

    int position_personnage_x=0;
    int position_personnage_y=0;
    int pos_x;
    int pos_y;
    int souris_x;
    int souris_y;
    int dep = 0;
    int temps = 0;
    int indice_monstre_combat;
    int en_combat_perso = 0;
    int en_combat_monstre = 0;
    int perso_a_combattu = 0;
    int monstre_a_combattu = 0;
    int verif_collis = 0;
    
    jeu_texte.color = blanc; 
    facile.color = blanc;
    moyen.color = blanc;
    difficile.color = blanc;
    quit.color = blanc;
    retour_menu.color = blanc;
    retour_jeu.color = blanc;
    reprise_mus.color = blanc;
    pause_mus.color = blanc;

    

            //---------------------------------Boucle du programme--------------------------------------------------//

            init_game();
            Mix_Music *musique = Mix_LoadMUS("acc.wav");
            if (!musique)
            {
                fprintf(stderr, "Erreur lors du chargement de la musique : %s\n", Mix_GetError());
                return -1;
            }
            Mix_Music *musique_jeu = Mix_LoadMUS("jeu.wav");
            if (!musique)
            {
                fprintf(stderr, "Erreur lors du chargement de la musique : %s\n", Mix_GetError());
                return -1;
            }

            Mix_PlayMusic(musique, -1);


            font = TTF_OpenFont("police1.ttf", 35);
            if(!font) fprintf(stderr, "Erreur d'ouverture de la police : %s\n", TTF_GetError());

            

            loadPicture("./image/Moussemue.png", &image_laby.source, &image_laby.image);
            init_texte();

            loadPicture("./image/perso_dep.png", &perso.source, &perso.image);
            init_animation (&perso, 9, 4);
            init_param (&image_laby);

            loadPicture("./image/attaque_perso.png", &perso_combat.source, &perso_combat.image);
            init_animation (&perso_combat, 6,1);



            loadPicture("./image/attaque_monstre.png", &monstre_combat.source, &monstre_combat.image);
            init_animation (&monstre_combat, 6,1);
      
        
            if ((window_main) && (renderer)) {
        

            while(program_on) {
                
                
            

                #ifndef DEBUG
                event_utile = SDL_FALSE;

                
                const Uint8* keystates = SDL_GetKeyboardState(NULL); // Récupère l'état des touches du clavier

                //On appuie sur la touche échape pour quitter
                if (keystates[SDL_SCANCODE_ESCAPE]) ETAT_JEU = PAUSE;    
                if ((keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) && ETAT_JEU == JEU) {
                    dep = HAUT;
                }
                else if ((keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) && ETAT_JEU == JEU) {
                    dep = BAS;
                }
                else if ((keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]) && ETAT_JEU == JEU) {
                    dep = GAUCHE;
                }
                else if ((keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]) && ETAT_JEU == JEU) {
                    dep = DROIT;
                }

                if(dep) {
                    deplacement(&image_laby, dep, &position_personnage_x, &position_personnage_y, pos_x, pos_y);
                    update_position(&pos_x, &pos_y, position_personnage_x, position_personnage_y, dep);
                    
                }

                
                while(!event_utile && SDL_PollEvent(&event)) {
    
                        switch(event.type) {
                            case SDL_MOUSEBUTTONDOWN:
                                souris_x = event.motion.x;
                                souris_y = event.motion.y;
                                if(ETAT_JEU == ACCUEIL) {
                                    donj = NULL;
                                    if (souris_x >= facile.position.x && souris_x <= facile.position.x + facile.position.w && souris_y >= facile.position.y && souris_y <= facile.position.y + facile.position.h) {
                                            ETAT_JEU = JEU;
                                            difficulte = FACILE;
                                    } 
                                    if (souris_x >= moyen.position.x && souris_x <= moyen.position.x + moyen.position.w &&
                                        souris_y >= moyen.position.y && souris_y <= moyen.position.y + moyen.position.h) {
                                            ETAT_JEU = JEU;
                                            difficulte = MOYEN;
                                    } 
                                    if (souris_x >= difficile.position.x && souris_x <= difficile.position.x + difficile.position.w &&
                                        souris_y >= difficile.position.y && souris_y <= difficile.position.y + difficile.position.h) {
                                            ETAT_JEU = JEU;
                                            difficulte = DIFFICILE;
                                    } 
                                    
                                    if (souris_x >= quit.position.x && souris_x <= quit.position.x + quit.position.w &&
                                        souris_y >= quit.position.y && souris_y <= quit.position.y + quit.position.h) {
                                            program_on = 0;
                                            event_utile = SDL_TRUE;
                                        }

                                    if(ETAT_JEU == JEU) {
                                        donj = generation_donjon(difficulte);
                                        Mix_PlayMusic(musique_jeu, -1);
                                        position_personnage_y = (donj->joueur.position / LAB_N) * (largeur / LAB_N) * 2 ;
                                        position_personnage_x = (donj->joueur.position % LAB_N) * (hauteur / LAB_P) * 2;
                                        pos_y = donj->joueur.position / LAB_N;
                                        pos_x = donj->joueur.position % LAB_N;
                                    }

                                } if(ETAT_JEU == PAUSE) {
                                    
                                    if (souris_x >= retour_menu.position.x && souris_x <= retour_menu.position.x + retour_menu.position.w &&
                                        souris_y >= retour_menu.position.y && souris_y <= retour_menu.position.y + retour_menu.position.h) {
                                        ETAT_JEU = ACCUEIL;
                                        if(donj) Mix_PlayMusic(musique, -1);
                                        liberer_donjon(&donj);
                                    }
                                    
                                    if (souris_x >= retour_jeu.position.x && souris_x <= retour_jeu.position.x + retour_jeu.position.w &&
                                        souris_y >= retour_jeu.position.y && souris_y <= retour_jeu.position.y + retour_jeu.position.h) {
                                        ETAT_JEU = JEU;
                                    }

                                    
                                    if (souris_x >= pause_mus.position.x && souris_x <= pause_mus.position.x + pause_mus.position.w &&
                                        souris_y >= pause_mus.position.y && souris_y <= pause_mus.position.y + pause_mus.position.h) {
                                        Mix_PauseMusic();
                                    } 
                                    if (souris_x >= reprise_mus.position.x && souris_x <= reprise_mus.position.x + reprise_mus.position.w &&
                                        souris_y >= reprise_mus.position.y && souris_y <= reprise_mus.position.y + reprise_mus.position.h) {
                                        Mix_ResumeMusic(); 
                                    } 
                                }
                                break;


                            case SDL_MOUSEMOTION:
                                souris_x = event.motion.x;
                                souris_y = event.motion.y;
                                if(ETAT_JEU == ACCUEIL) {

                                    if (souris_x >= facile.position.x && souris_x <= facile.position.x + facile.position.w && souris_y >= facile.position.y && souris_y <= facile.position.y + facile.position.h) {
                                        facile.color = jaune;
                                    } else {
                                        facile.color = blanc;
                                    }

                                    if (souris_x >= moyen.position.x && souris_x <= moyen.position.x + moyen.position.w &&
                                        souris_y >= moyen.position.y && souris_y <= moyen.position.y + moyen.position.h) {
                                        moyen.color = jaune;
                                    } else {
                                        moyen.color = blanc;
                                    }
                                    
                                    if (souris_x >= difficile.position.x && souris_x <= difficile.position.x + difficile.position.w &&
                                        souris_y >= difficile.position.y && souris_y <= difficile.position.y + difficile.position.h) {
                                        difficile.color = jaune;
                                    } else {
                                        difficile.color = blanc;
                                    }
                                    
                                    if (souris_x >= quit.position.x && souris_x <= quit.position.x + quit.position.w &&
                                        souris_y >= quit.position.y && souris_y <= quit.position.y + quit.position.h) {
                                        quit.color = jaune;
                                    } else {
                                        quit.color = blanc;
                                    }
                                } if(ETAT_JEU == PAUSE) {
                                    
                                    if (souris_x >= retour_menu.position.x && souris_x <= retour_menu.position.x + retour_menu.position.w &&
                                        souris_y >= retour_menu.position.y && souris_y <= retour_menu.position.y + retour_menu.position.h) {
                                        retour_menu.color = jaune;
                                    } else {
                                        retour_menu.color = blanc;
                                    }

                                    if (souris_x >= retour_jeu.position.x && souris_x <= retour_jeu.position.x + retour_jeu.position.w &&
                                        souris_y >= retour_jeu.position.y && souris_y <= retour_jeu.position.y + retour_jeu.position.h) {
                                        retour_jeu.color = jaune;
                                    } else {
                                        retour_jeu.color = blanc;
                                    }

                                    
                                    if (souris_x >= pause_mus.position.x && souris_x <= pause_mus.position.x + pause_mus.position.w &&
                                        souris_y >= pause_mus.position.y && souris_y <= pause_mus.position.y + pause_mus.position.h) {
                                        pause_mus.color = jaune;
                                    } else {
                                        pause_mus.color = blanc;
                                    }

                                    if (souris_x >= reprise_mus.position.x && souris_x <= reprise_mus.position.x + reprise_mus.position.w &&
                                        souris_y >= reprise_mus.position.y && souris_y <= reprise_mus.position.y + reprise_mus.position.h) {
                                        reprise_mus.color = jaune;
                                    } else {
                                        reprise_mus.color = blanc;
                                    }
                                   
                                }   
                                break;                     
                            case SDL_QUIT : 
                                program_on = 0;
                                event_utile = SDL_TRUE;
                                break;

                                case SDL_WINDOWEVENT:
                                if (event.window.event == SDL_WINDOWEVENT_RESIZED || 
                                    event.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
                                    largeur = event.window.data1;
                                    hauteur = event.window.data2;
                                    init_texte();
                                }
                                break;
                            
                            default :
                                break;
                        }
  
                    }
                    #endif
                    
                    SDL_RenderClear(renderer);     
            if(ETAT_JEU == ACCUEIL) {
                ecriture_ecran_accueil(font);  
            } 
            if(ETAT_JEU == JEU && etat_jeu(donj) != PARTIE_GAGNEE && etat_jeu(donj) != PARTIE_PERDUE) {
                
                //affichage de la map
                affiche_labyrinthe_image(donj->laby, image_laby); 
                afficher_piece(donj,temps/3);
                afficher_effets(donj,temps/3);
                animation_perso_minimap( pos_x,  pos_y,  dep,  temps);
                animation_monstre(donj,temps);
                
                init_map( &mini_map);




                affiche_labyrinthe_image(donj->laby, image_laby); 
                afficher_piece(donj,temps/3);
                afficher_effets(donj,temps/3);

               verif_collis = verifie_collision_monstre(donj);
               if(verif_collis || en_combat_perso || en_combat_monstre) {
                    
                    if(verif_collis) indice_monstre_combat = obtenir_id_monstre_collision(donj);
                    if((int)res == -1) res = combat(donj, &(donj->joueur), &(donj->tab_monstres[indice_monstre_combat]));
                    init_terrain(position_personnage_x, position_personnage_y);
                    //affiche vie monstre
                    if(res == VICTOIRE_JOUEUR) {
                        if(!monstre_a_combattu) {
                            animation_a_afficher(monstre_combat, (largeur /2 )-1.5*monstre_combat.destination.w + perso_combat.destination.w,hauteur /2-1.5*monstre_combat.destination.h,3, 3 ,0,en_combat_monstre);
                            en_combat_perso = 0;
                            en_combat_monstre++;  
                            animation_a_afficher(perso_combat, (largeur /2 )-1.5*perso_combat.destination.w,hauteur /2-1.5*perso_combat.destination.h,3, 3 ,0,en_combat_perso);
                            if(en_combat_monstre == 6) monstre_a_combattu = 1;
                        } else {
                            animation_a_afficher(perso_combat, (largeur /2 )-1.5*perso_combat.destination.w,hauteur /2-1.5*perso_combat.destination.h,3, 3 ,0,en_combat_perso);
                            animation_a_afficher(monstre_combat, (largeur /2 )-1.5*monstre_combat.destination.w + perso_combat.destination.w,hauteur /2-1.5*monstre_combat.destination.h,3, 3 ,0,en_combat_monstre);
                            en_combat_perso++;  
                            en_combat_monstre = 0;
                            if(en_combat_perso == 6) perso_a_combattu = 1;
                        }  
                    } else {
                            if(!perso_a_combattu) {
                                animation_a_afficher(perso_combat, (largeur /2 )-1.5*perso_combat.destination.w,hauteur /2-1.5*perso_combat.destination.h,3, 3 ,0,en_combat_perso);
                                animation_a_afficher(monstre_combat, (largeur /2 )-1.5*monstre_combat.destination.w + perso_combat.destination.w,hauteur /2-1.5*monstre_combat.destination.h,3, 3 ,0,en_combat_monstre);
                                en_combat_perso++;  
                                if(en_combat_perso == 6) perso_a_combattu = 1;
                            } else {
                                animation_a_afficher(monstre_combat, (largeur /2 )-1.5*monstre_combat.destination.w + perso_combat.destination.w,hauteur /2-1.5*monstre_combat.destination.h,3, 3 ,0,en_combat_monstre);
                                en_combat_perso = 0;
                                en_combat_monstre++;  
                                animation_a_afficher(perso_combat, (largeur /2 )-1.5*perso_combat.destination.w,hauteur /2-1.5*perso_combat.destination.h,3, 3 ,0,en_combat_perso);
                                if(en_combat_monstre == 6) monstre_a_combattu = 1;
                            }           
                            
                            SDL_Delay(40);
                            if(perso_a_combattu && monstre_a_combattu) {
                                animation_mort(image_laby, perso_combat);
                                pos_y = donj->joueur.position / LAB_N;
                                pos_x = donj->joueur.position % LAB_N;
                                position_personnage_y = pos_y * (hauteur / LAB_P);
                                position_personnage_x = pos_x * (largeur / LAB_N);
                                SDL_Delay(40);
        
                            }
                        }
                        if(perso_a_combattu && monstre_a_combattu) {
                            monstre_a_combattu = 0;
                            perso_a_combattu = 0;
                            en_combat_perso = 0;
                            en_combat_monstre = 0;
                            deplacement_monstre_grille(donj, indice_monstre_combat, -1);
                            donj->tab_monstres[indice_monstre_combat].position = -1;
                            res = -1;
                            SDL_Delay(40);
                        }
                    

                } else {
                    int deplacement_fluide_monstre =(temps%20)+1;

                    animation_monstre_fluide(donj, deplacement_fluide_monstre , deplacement_fluide_monstre,temps/3);
                    
                    init_terrain(position_personnage_x, position_personnage_y);
                    if ((temps%20==19))
                    { 
                        update_tout_trajet_monstre(donj);
                        int piece_collecte = verifie_collision_piece(donj);
                        if(piece_collecte)
                            genere_tout_trajet_monstre(donj);
                    }
                    if (temps %100==1) genere_bonus_malus(donj);    //le temps% k correstpond au temps a attendre pour generer un malus  
                    
                    verifie_collision_piece(donj);      //gère la collision des pieces et des effets
                    gere_collision_bonus_malus(donj);
                    
                    debug_affiche_donjon(donj);           
                    animation_perso(dep,  temps/3);
                    en_combat_monstre = 0;
                    en_combat_perso = 0;
                    perso_a_combattu = 0;
                    monstre_a_combattu = 0;
                } 


                dep = 0;
                
                
                
                SDL_SetRenderDrawColor(renderer,255, 0, 0, 0);  
                SDL_RenderDrawPoint(renderer, (largeur /2 ),hauteur /2);        
                SDL_SetRenderDrawColor(renderer,0, 0, 0, 0);  
                      


                mini_map.destination.x = largeur - mini_map.destination.w;
                mini_map.destination.y = 0;
                
                
                
                SDL_RenderCopy(renderer, mini_map.image, &mini_map.source, &mini_map.destination);
                affiche_info_jeu(font);
                SDL_DestroyTexture(mini_map.image);
                temps ++;
                temps = temps%100;
            }    
            if(ETAT_JEU == PAUSE) option_musique(font);
            if(ETAT_JEU == FIN) {
                SDL_Delay(500);
                 ETAT_JEU = ACCUEIL;
                 if(donj) Mix_PlayMusic(musique, -1);
                    liberer_donjon(&donj);
            }
            if(donj && ETAT_JEU == JEU) {
                if(etat_jeu(donj) == PARTIE_GAGNEE) {
                    ecriture(&moyen, font, "BRAVO");
                    ETAT_JEU = FIN;
                }
                if(etat_jeu(donj) == PARTIE_PERDUE) {
                    ecriture(&moyen, font, "Dommage...");
                    ETAT_JEU = FIN;
                }
            }
                SDL_RenderPresent(renderer);     
                

            
            SDL_Delay(20);       
        } 

    }
        
    //Libération des composantes SDL
        
    #ifndef DEBUG

    liberer_texte(&jeu_texte);
    liberer_texte(&facile);
    liberer_texte(&moyen);
    liberer_texte(&difficile);
    liberer_texte(&quit);
    liberer_texte(&retour_menu);
    liberer_texte(&retour_jeu);
    liberer_texte(&pause_mus);
    liberer_texte(&reprise_mus);

    SDL_DestroyTexture(image_laby.image);
    SDL_DestroyTexture(perso.image);
    SDL_DestroyTexture(perso_combat.image);
    SDL_DestroyTexture(monstre_combat.image);

    Mix_FreeMusic(musique_jeu);
    Mix_FreeMusic(musique);


    Mix_CloseAudio();
    Mix_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window_main);
    TTF_CloseFont(font);
    IMG_Quit();
    SDL_Quit();
    #endif

    return 0;
}

