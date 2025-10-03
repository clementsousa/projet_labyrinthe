/**
 * IMPORTANT : Si les constantes de pré-processeurs LAB_N et LAB_P ont été modifié. IL FAUT RECOMPILER ENTIEREMENT (rm *.o)
 * Sinon, coredump.
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>


#include "partition.h"
#include "kruskal.h"
#include "labyrinthe.h"
#include "parcours_graphe.h"
#include "file.h"
#include "effets.h"


//Déclaration des variables globales

SDL_Window *window_main = NULL;
SDL_Renderer * renderer = NULL; 
SDL_DisplayMode user;
int largeur = 0;
int hauteur = 0;
const char* murs_texte[16] = {
    "\xE2\x95\x8B", "\xE2\x94\xB3", "\xE2\x94\xAB", "\xE2\x94\x93",
    "\xE2\x94\xBB", "\xE2\x94\x81", "\xE2\x94\x9B", "\xE2\x95\xB8",
    "\xE2\x94\xA3", "\xE2\x94\x8F", "\xE2\x94\x83", "\xE2\x95\xBB",
    "\xE2\x94\x97", "\xE2\x95\xBA", "\xE2\x95\xB9", "\xE2\x80\xA2"
};


void init_grille(labyrinthe_t * laby, int liste_arete[NB_MAX_ARETE][2])
{
    srand(2);
    int type_de_terrain[NB_TERRAIN] = {DESERT, MARAIS, OCEAN, PLAINE, MONTAGNE, NORMAL};
    int i = 0;
    int j = 0;
    int noeud;

    for(i = 0; i < LAB_P; i++) {
        for(j = 0; j < LAB_N; j++) {
            noeud = LAB_N * i + j;
            laby->grille[i][j] = trouve_voisins(liste_arete, noeud);
        }
    }
    for(int i =0; i<LAB_N*LAB_P; i++){
        laby->terrain[i] = type_de_terrain[rand () % NB_TERRAIN];
    }
}

void affiche_grille(labyrinthe_t * laby)
{
    int i = 0;
    int j = 0;

    for(i = 0; i < LAB_P; i++) {
        for(j = 0; j < LAB_N; j++) {
            printf("%d ", laby->grille[i][j]);
        }
        printf("\n");
    }

    printf("\n");
}



void affiche_sans_SDL(labyrinthe_t * laby)
{
    for (int i =0; i<LAB_P; i++)
    {
        for (int j = 0;  j<LAB_N;j++)
        {
            printf("%s",murs_texte[laby->grille[i][j]]);
        }
        printf("\n");
    }

}


/*---------------------------------------------------SDL---------------------------------------------------------*/

void dessine_trait(int vertical, int est_ouest_ou_nord, int pos_x, int pos_y)
{
    int largeur_case = largeur/LAB_N;
    int hauteur_case = hauteur/LAB_P;
    pos_x *= largeur_case;
    pos_y *= hauteur_case;
    if(vertical){
        if(est_ouest_ou_nord){
            SDL_RenderDrawLine(renderer, pos_x, pos_y, pos_x, pos_y + hauteur_case); // Dessine à l'ouest
        }else{
            if(pos_x + largeur_case == largeur){
                pos_x--;
            }
            SDL_RenderDrawLine(renderer, pos_x + largeur_case, pos_y, pos_x + largeur_case, pos_y + hauteur_case); // Dessine à l'est
        }
    }else{
        if(est_ouest_ou_nord){
            SDL_RenderDrawLine(renderer, pos_x, pos_y, pos_x + largeur_case, pos_y); // Dessine nord
        }else{
            if(pos_y + hauteur_case == hauteur){
                pos_y--;
            }
            SDL_RenderDrawLine(renderer, pos_x, pos_y + hauteur_case, pos_x + largeur_case, pos_y + hauteur_case); // Dessine sud
        }
    }
}


void dessine_case(labyrinthe_t * laby)
{
    int i,j, noeud;
    float ratio_dist;
    SDL_Rect rectangle;
    int max = max_distance(laby);
    if(max==0) max = 1;

    for(i = 0; i < LAB_P; i++) {    
        for(j = 0; j < LAB_N; j++) {
            noeud = LAB_N * i + j;
            ratio_dist = (float) laby->etat_noeuds.distance[noeud] / max;
            if(laby->etat_noeuds.distance[noeud] == DISTANCE_INIT) ratio_dist = 1;
            Uint8 color[] = {0, 255 * (1-ratio_dist),255 * (1-ratio_dist), 255}; //On déclare pour avoir la même couleur pour les distances égales
            SDL_SetRenderDrawColor(renderer,color[0],color[1],color[2],255);
            rectangle.x = j * (largeur / LAB_N);
            rectangle.y = i * (hauteur / LAB_P);
            rectangle.w = largeur/LAB_N;
            rectangle.h = hauteur/LAB_P;
            SDL_RenderFillRect(renderer,&rectangle);
        }
    }
}

void affiche_labyrinthe_dessin(labyrinthe_t * laby)
{
    int i,j;
    dessine_case(laby);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255,255);
    
    for(i=0; i<LAB_P; i++){
        for(j=0; j<LAB_N; j++){
            if(laby->grille[i][j] & NORD){
                dessine_trait(0,1,j,i);
            }
            if(laby->grille[i][j] & EST){
                dessine_trait(1,0,j,i);
            }
            if(laby->grille[i][j] & SUD){
                dessine_trait(0,0,j,i);
            }
            if(laby->grille[i][j] & OUEST){
                dessine_trait(1,1,j,i);
            }
        }   
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
}



int max_visite(labyrinthe_t * laby)
{
    int max = laby->etat_noeuds.nb_visite[0];
    for(int i = 0; i < LAB_N*LAB_P; i++){
        if(laby->etat_noeuds.nb_visite[i] > max)
            max = laby->etat_noeuds.nb_visite[i];
    }
    return max;
}


void dessine_case_visite(labyrinthe_t * laby)
{
    int i,j, noeud;
    float ratio_dist;
    SDL_Rect rectangle;
    int max = max_visite(laby);
    if(max==0) max = 1;

    for(i = 0; i < LAB_P; i++) {    
        for(j = 0; j < LAB_N; j++) {
            noeud = LAB_N * i + j;
            ratio_dist = (float) laby->etat_noeuds.nb_visite[noeud] / (max+1);
            if(laby->etat_noeuds.nb_visite[noeud] == 0) ratio_dist = 1;
            Uint8 color[] = {0, 255 * (1-ratio_dist),255 * (1-ratio_dist) , 255}; //On déclare pour avoir la même couleur pour les distances égales
            SDL_SetRenderDrawColor(renderer,color[0],color[1],color[2],255);
            rectangle.x = j * (largeur / LAB_N);
            rectangle.y = i * (hauteur / LAB_P);
            rectangle.w = largeur/LAB_N;
            rectangle.h = hauteur/LAB_P;
            SDL_RenderFillRect(renderer,&rectangle);
        }
    }
}


/**
 * Trace le labyrinthe tel que connu, met un dégradé de couleur suivant si la case a été beaucoup visité ou non
 */
void affiche_labyrinthe_dessin_passage_par_cases(labyrinthe_t * laby)
{
    int i,j;
    dessine_case_visite(laby);
    SDL_SetRenderDrawColor(renderer, 255, 255, 255,255);
    
    for(i=0; i<LAB_P; i++){
        for(j=0; j<LAB_N; j++){
            if(laby->grille[i][j] != -1){
                if(laby->grille[i][j] & NORD){
                    dessine_trait(0,1,j,i);
                }
                if(laby->grille[i][j] & EST){
                    dessine_trait(1,0,j,i);
                }
                if(laby->grille[i][j] & SUD){
                    dessine_trait(0,0,j,i);
                }
                if(laby->grille[i][j] & OUEST){
                    dessine_trait(1,1,j,i);
                }
            }
            
        }   
    }
    SDL_SetRenderDrawColor(renderer,0,0,0,255);
}
/*---------------------------------------------------IMAGE---------------------------------------------------------*/

void LoadSurface(reglage_image * img, SDL_Surface * surface)
{
    img->image = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_QueryTexture(img->image, NULL, NULL, &img->source.w, &img->source.h);
}



void loadPicture(char * name, SDL_Rect * source, SDL_Texture ** img)
{
   *img = IMG_LoadTexture(renderer, name);
    if (! *img) fprintf(stderr, "Erreur du chargement de : %s\n", SDL_GetError());
    SDL_QueryTexture(*img, NULL, NULL, &source->w, &source->h);
}



void init_param(reglage_image *img)
{
    img->offset_x = img->source.w / 16;  // 16 vignettes en ligne
    img->offset_y = img->source.h;      // toute l'image est en une seule ligne

    img->state.w = img->offset_x;
    img->state.h = img->offset_y;

    img->destination.w = largeur / LAB_N;
    img->destination.h = hauteur / LAB_P;
}



void vignette_a_afficher(labyrinthe_t * laby, reglage_image img, int i, int j)
{
    int indice = laby->grille[i][j];  // doit être entre 0 et 15

    SDL_Rect src = {
        .x = indice * img.offset_x,
        .y = 0,
        .w = img.offset_x,
        .h = img.offset_y
    };

    SDL_Rect dst = {
        .x = j * img.destination.w,
        .y = i * img.destination.h,
        .w = img.destination.w,
        .h = img.destination.h
    };

    SDL_RenderCopy(renderer, img.image, &src, &dst);
}



void affiche_labyrinthe_image(labyrinthe_t * laby, reglage_image img)
{
    for (int i = 0; i < LAB_P; i++) {
        for (int j = 0; j < LAB_N; j++) {
            vignette_a_afficher(laby, img, i, j);
        }
    }
    
}


/*--------------------------------------animation de recherche de la sortie---------------------------------*/






void init_animation (reglage_image *img,int nombre_image,int nombre_animation)
{
    img->offset_x = img->source.w / nombre_image;  // 4 vignettes en ligne
    img->offset_y = img->source.h / nombre_animation;      // toute l'image est en une seule ligne

    img->state.x = 0;
    img->state.y = 0;
    img->state.w = img->offset_x;
    img->state.h = img->offset_y;

    img->destination.w = largeur / LAB_N;
    img->destination.h = hauteur / LAB_P;
}

void sprite_animation_a_afficher(reglage_image img, int i,int j, int animation)
{
    SDL_Rect src = {
        .x = img.state.x,
        .y = animation * img.offset_y,
        .w = img.offset_x,
        .h = img.offset_y
    };

    SDL_Rect dst = {
        .x = j * img.destination.w / 64,
        .y = i * img.destination.h / 64,
        .w = img.destination.w,
        .h = img.destination.h
    };

    SDL_RenderCopy(renderer, img.image, &src, &dst);
}

void anim_parcour_du_labyrinthe(reglage_image *img, int position_personnage_x, int position_personnage_y,int animation)
{
    sprite_animation_a_afficher(*img, position_personnage_y, position_personnage_x, animation);
}

void afficher_image(reglage_image img, int x, int y)
{
    SDL_Rect src = {
        .x = 0,
        .y = 0,
        .w = img.offset_x,
        .h = img.offset_y
    };

    SDL_Rect dst = {
        .x = x * largeur/LAB_N,
        .y = y * hauteur/LAB_P,
        .w = img.destination.w,
        .h = img.destination.h
    };

    SDL_RenderCopy(renderer, img.image, &src, &dst);
}


void init_trajet(trajet * t)
{
    t->nb_element=0;
}

void animation_deplacement(labyrinthe_t * laby, trajet * trajet, reglage_image * map, reglage_image image,reglage_image perso,reglage_image etoile1,reglage_image croix,reglage_image pine,int *x_initial, int *y_initial, int *x_final,int *y_final, int *position_dans_trajet, int position_personnage_x, int position_personnage_y)
{    
    affiche_labyrinthe_image(laby, image);
    if (trajet->nb_element==*position_dans_trajet)
    { 
        for (int i =0; i<trajet->nb_element;i++)//efface le trajet
        {
            vignette_a_afficher(laby, image, trajet->trajet_y[i], trajet->trajet_x[i]);
        }
        vignette_a_afficher(laby, image, *y_initial , *x_initial);
        *x_initial = *x_final;
        *y_initial = *y_final;
        (*position_dans_trajet)=0;
        trajet->nb_element=0;
        vignette_a_afficher( laby, image, *y_initial, *x_initial);    //efface le perso apres son passage
        genere_position_aleatoire_de_debut_et_de_fin( x_final , y_final);  //reregenère une position de départ et une position final
        rempli_file_du_chemin_a_parcourir( laby, trajet, *x_initial, *y_initial, *x_final, *y_final);      // redefini le trajet a parcourir
    }
            affiche_debut_fin( croix, pine, *x_initial, *y_initial, *x_final,  *y_final);

    affiche_trajet( etoile1, trajet);
    position_personnage_x = (*trajet).trajet_x[*position_dans_trajet];
    position_personnage_y = (*trajet).trajet_y[*position_dans_trajet];
    vignette_a_afficher( laby, image, position_personnage_y, position_personnage_x);
    (*position_dans_trajet)++;
    int animation=2;
    anim_parcour_du_labyrinthe(&perso, position_personnage_x*perso.destination.w/1.5, position_personnage_y*perso.destination.h,animation);     //fait bouger le perso
    init_map(map);
    SDL_RenderPresent(renderer);
    SDL_Delay(200); 
    vignette_a_afficher( laby, image, position_personnage_y, position_personnage_x);      
}

void generer_trajectoire_aleatoire(labyrinthe_t * laby, trajet * traj, int x_initial, int y_initial,int x_final, int y_final)
{
    genere_position_aleatoire_de_debut_et_de_fin(&x_final , &y_final);
    rempli_file_du_chemin_a_parcourir( laby, traj, x_initial, y_initial, x_final, y_final);      // redefini le trajet a parcourir
        
}

void affiche_debut_fin(reglage_image croix,reglage_image pine,int x_initial, int y_initial, int x_final, int y_final)
{
    afficher_image(croix,x_final,y_final);
    afficher_image(pine,x_initial,y_initial);
}

void affiche_trajet(reglage_image etoile1,trajet * traj)
{
for (int i =0; i<traj->nb_element;i++)//affiche la trajet
    {
        afficher_image(etoile1,traj->trajet_x[i],traj->trajet_y[i]);
    }
}

void init_map(reglage_image * map)
{

    SDL_Surface *map_surface= convertion_renderer_surface(largeur, hauteur);
    LoadSurface(map,map_surface);
    SDL_FreeSurface(map_surface);
    map->offset_x = map->source.w ;  
    map->offset_y = map->source.h ;

    map->state.w = map->offset_x;
    map->state.h = map->offset_y;

    map->destination.w = largeur /5;
    map->destination.h = hauteur /5; 


}
