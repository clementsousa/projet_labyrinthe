#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <complex.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "labyrinthe.h"
#include "coeur.h"

/*
void afficher_effets(donjon_t *donj)
{
    reglage_image  effets = {0};
    loadPicture("./image/Fireball_01 32x32.gif", &effets.source, &effets.image);
    init_animation(&effets,1,1);
   
    for(int i =0;i<LAB_N;i++)
    {
        for(int j =0;j<LAB_P;j++)
        {
            //printf("%d",donj->presence_entite[j][i]);
            if ((donj->presence_entite[j][i]) ==BONUS_MALUS)
            {
             //printf("bonjour\n");
            afficher_image(effets,i,j);
            }
        }
    }
    
    SDL_DestroyTexture(effets.image);
}*/


void animation_a_afficher(reglage_image img, int x,int y, int h, int l, int animation,int temps)
{
    SDL_Rect src = {
        .x = temps *img.offset_x,
        .y = animation * img.offset_y,
        .w = img.offset_x,
        .h = img.offset_y
    };

    SDL_Rect dst = {
        .x = x ,
        .y = y ,
        .w = img.destination.w*l,
        .h = img.destination.h*h
    };

    SDL_RenderCopy(renderer, img.image, &src, &dst);
}


void afficher_piece(donjon_t *donj, int temps)
{
    reglage_image  piece = {0};
    loadPicture("./image/pieceargent.png", &piece.source, &piece.image);
    init_animation(&piece,8,1);
    int position_piece = trouver_piece(donj);
    animation_a_afficher(piece,(position_piece%LAB_N)*largeur /LAB_N,position_piece/LAB_N *hauteur/LAB_P ,1,1,0,temps%8);
    //afficher_image(piece,position_piece%LAB_N,position_piece/LAB_N);
    SDL_DestroyTexture(piece.image);
}

void afficher_effets(donjon_t *donj, int temps)
{
    reglage_image  effets = {0};
    loadPicture("./image/bonus.png", &effets.source, &effets.image);
    init_animation(&effets,7,1);
    
    for(int i =0;i<LAB_N;i++)
    {
        for(int j =0;j<LAB_P;j++)
        {
            if ((donj->presence_entite[j][i]) ==BONUS_MALUS)
            {
                animation_a_afficher(effets,(i)*largeur /LAB_N,j*hauteur/LAB_P ,1,1,0,temps%7);
            }
        }
    }
   
    SDL_DestroyTexture(effets.image);
}

void animation_perso(int direction, int temps)
{
    reglage_image  perso_marche= {0};
    loadPicture("./image/perso_dep.png", &perso_marche.source, &perso_marche.image);
    init_animation(&perso_marche,9,4);
    
    
    reglage_image  perso_immobile= {0};
    loadPicture("./image/perso.immobile.png", &perso_immobile.source, &perso_immobile.image);
    init_animation(&perso_immobile,8,1);

    if (direction)
        animation_a_afficher(perso_marche,(largeur /2 )-1.5*perso_marche.destination.w,hauteur /2-1.5*perso_immobile.destination.h,3,3,direction-1,temps%9);
    else animation_a_afficher(perso_immobile,(largeur /2)-1.5*perso_marche.destination.w,hauteur /2-1.5*perso_immobile.destination.h,3,3,direction,temps%8);

    SDL_DestroyTexture(perso_immobile.image);
    SDL_DestroyTexture(perso_immobile.image);
}

void animation_perso_minimap(int posx, int posy, int direction, int temps)
{
    reglage_image  perso_marche= {0};
    loadPicture("./image/perso_dep.png", &perso_marche.source, &perso_marche.image);
    init_animation(&perso_marche,9,4);

    reglage_image  perso_immobile= {0};
    loadPicture("./image/perso.immobile.png", &perso_immobile.source, &perso_immobile.image);
    init_animation(&perso_immobile,8,1);

    if (direction)
        animation_a_afficher(perso_marche,posx*largeur /LAB_N,posy *hauteur/LAB_P ,1,1,direction-1,temps%9);
    else animation_a_afficher(perso_immobile,posx*largeur /LAB_N,posy *hauteur/LAB_P ,1,1,direction,temps%8);

    SDL_DestroyTexture(perso_marche.image);
    SDL_DestroyTexture(perso_immobile.image);
}

void animation_monstre(donjon_t * donj, int temps)
{
    //charge les animations de marche
    reglage_image  zombie_marche= {0};
    loadPicture("./image/zombie.marche.png", &zombie_marche.source, &zombie_marche.image);
    init_animation(&zombie_marche,9,4);
    //charge l'animation d'immobilité
    reglage_image  zombie_immobile= {0};
    loadPicture("./image/zombie_immobile.png", &zombie_immobile.source, &zombie_immobile.image);
    init_animation(&zombie_immobile,8,1);
    //affiche cette animation
    for (int i=0;i<donj->nb_monstre;i++)
    {
        if (donj->tab_monstres[i].derniere_direction)    
        animation_a_afficher(zombie_marche,((donj->tab_monstres[i].position)*largeur/LAB_P)% largeur,donj->tab_monstres[i].position/LAB_P*hauteur/LAB_N,1,1,donj->tab_monstres[i].derniere_direction-1,temps%9);
        else 
        animation_a_afficher(zombie_immobile,((donj->tab_monstres[i].position)* largeur/LAB_P)% largeur,donj->tab_monstres[i].position*hauteur/LAB_N/LAB_P,1,1,donj->tab_monstres[i].derniere_direction-1,temps%7);
        
    }

    //décharge 
    SDL_DestroyTexture(zombie_marche.image);
    SDL_DestroyTexture(zombie_immobile.image);
}

void animation_monstre_fluide(donjon_t * donj, int x, int y, int temps)
{
    //charge les animations de marche
    reglage_image  zombie_marche= {0};
    loadPicture("./image/zombie.marche.png", &zombie_marche.source, &zombie_marche.image);
    init_animation(&zombie_marche,9,4);
    //charge l'animation d'immobilité
    reglage_image  zombie_immobile= {0};
    loadPicture("./image/zombie_immobile.png", &zombie_immobile.source, &zombie_immobile.image);
    init_animation(&zombie_immobile,8,1);
    //affiche cette animation
    for (int i=0;i<donj->nb_monstre;i++)
    {
        if(donj->tab_monstres[i].position != -1) {
            if (donj->tab_monstres[i].direction_actuelle==DIRECTION_NORD)    
            animation_a_afficher(zombie_marche,(( donj->tab_monstres[i].position ) * largeur/LAB_P ) % largeur,donj->tab_monstres[i].position / LAB_P * hauteur / LAB_N-y*hauteur/LAB_P/20,1,1,0,temps%9);
            else if(donj->tab_monstres[i].direction_actuelle==DIRECTION_SUD) 
            animation_a_afficher(zombie_marche,(( donj->tab_monstres[i].position ) * largeur/LAB_P ) % largeur,donj->tab_monstres[i].position / LAB_P * hauteur / LAB_N+y*hauteur/LAB_P/20,1,1,2,temps%9);
            else if(donj->tab_monstres[i].direction_actuelle==DIRECTION_EST) 
            animation_a_afficher(zombie_marche,(( donj->tab_monstres[i].position ) * largeur/LAB_P ) % largeur+x*largeur/LAB_N/20,donj->tab_monstres[i].position / LAB_P * hauteur / LAB_N,1,1,3,temps%9);
            else if(donj->tab_monstres[i].direction_actuelle==DIRECTION_OUEST) 
            animation_a_afficher(zombie_marche,(( donj->tab_monstres[i].position ) * largeur/LAB_P ) % largeur-x*largeur/LAB_N/20,donj->tab_monstres[i].position / LAB_P * hauteur / LAB_N,1,1,1,temps%9);
        }
        //else  animation_a_afficher(zombie_immobile,(( donj->tab_monstres[i].position) * largeur/LAB_P ) % largeur+x,donj->tab_monstres[i].position * hauteur / LAB_N / LAB_P+y,1,1,donj->tab_monstres[i].direction_actuelle-1,temps%7);
        
    }

    //décharge 
    SDL_DestroyTexture(zombie_marche.image);
    SDL_DestroyTexture(zombie_immobile.image);
}
