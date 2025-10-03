#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include <complex.h>


#include "effets.h"
#include "animation_effets.h"



void affichage_complexe(float complex z)
{
       
    
    SDL_bool program_on = SDL_TRUE; 
    SDL_Event event;
    SDL_Window * window   = NULL; // fenetre utilisee par le programme
    SDL_Renderer * renderer = NULL;

    

    if (SDL_Init(SDL_INIT_VIDEO) == -1)  /* SDL_INIT_EVERYTHING */ 
    {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); 
    } 
    
    window = SDL_CreateWindow(
       "magnifique Fenêtre :)",                    // codage en utf8, donc accents possibles
       0, 0,                                  // coin haut gauche en haut gauche de l'écran
       1000, 600,                              // largeur = 400, hauteur = 300
       SDL_WINDOW_RESIZABLE); 
    
    if (window == NULL) //si la fenetre a pu se créer
    {
        fprintf(stderr, "Erreur a la creation de la fenetre : %s\n", SDL_GetError()); 
    } else 
    {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED ); // | SDL_RENDERER_PRESENTVSYNC 
        if (renderer == 0) {
            fprintf(stderr, "Erreur de creation d'un renderer : %s\n", SDL_GetError());
        }
    }
    int w;
    int h;
   
    //affichage de la fenetre
  
    while(program_on)
    {
        //clear le rendu

        SDL_SetRenderDrawColor(renderer,255,255,255,255);
        SDL_RenderClear(renderer);

        //affichage du rendu:

        SDL_GetWindowSize(window, &w, &h);
        SDL_SetRenderDrawColor(renderer,0,0,0,0);
        SDL_RenderDrawLine(renderer,w/2,0,w/2,h);
        SDL_RenderDrawLine(renderer,0,h/2,w,h/2);
        SDL_SetRenderDrawColor(renderer,255,0,0,0);
        SDL_RenderDrawLine(renderer,w/2,h/2,w/2+creal(z), h/2+cimag(z));
   
        /*lance la bouche d'evenement et gère tout les evenement a traiter*/
        
        if (SDL_PollEvent(&event)) 
        {   
            switch(event.type) 
            {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)  
                    {
                        case SDL_WINDOWEVENT_CLOSE:  
                            printf("appui sur la croix\n");
                            break;
                        default:
                           
                    }   
                break;
                case SDL_MOUSEBUTTONDOWN:
                    printf("Clic :%d %d\n", event.button.x, event.button.y);
                    
                    break;
                case SDL_QUIT : 
                    printf("On quitte\n");    
                    program_on = SDL_FALSE;
            }
            
        }
        SDL_RenderPresent(renderer);    
        SDL_Delay(10);
    }
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    SDL_Quit();
}


SDL_Texture* animation1(SDL_Renderer *renderer, SDL_Surface *source )
{
  SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, source->w, source->h, 8, source->format->format);

  // Toutes les images n'ont pas de couche alpha, il y a 3 ou 4 octets pour coder un pixel
    int pixel_size = 3 + SDL_ISPIXELFORMAT_ALPHA(dst->format->format);
    int i_src;
    int j_src;
    for (int j_dst = 0 ; j_dst < dst->h ; j_dst++){
        for (int i_dst = 0 ; i_dst < dst->w ; i_dst++) {
        // Calcul de l'antécédent dans l'image
        float complex z= i_dst + I*j_dst;
        float complex position=dst->w/2 +dst->h/2 *I;
        z= rotation_changement_de_centre( z,  position, cabs((z-position)/100)+position);
        i_src = creal(z);
        j_src = cimag(z);
        
        if (i_src<=0) i_src = 1;
        if (j_src<=0) j_src = 1;
        if (i_src>=dst->w) i_src = dst->w-1;
        if (j_src>=dst->h) j_src = dst->h-1;

        // copie du pixel s'il est dans l'image
        if ((pixel_size*(j_src*source->w+i_src) < dst->w * dst->h * pixel_size )&& (i_src>0)&&(j_src>0)&&(i_src<dst->w)&&(j_src<dst->h)){
            char *pixels_dst = &((char *)dst->pixels)[pixel_size * (j_dst * dst->w + i_dst)];
            char *pixels_source = &((char *)source->pixels)[pixel_size * (j_src * source->w + i_src)];
        memcpy(pixels_dst, pixels_source, pixel_size);
      }
    }
  }

  return SDL_CreateTextureFromSurface(renderer, dst);
}


SDL_Texture* animation2(SDL_Renderer *renderer, SDL_Surface *source ){
  SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, source->w, source->h, 8, source->format->format);

  // Toutes les images n'ont pas de couche alpha, il y a 3 ou 4 octets pour coder un pixel
    int pixel_size = 3 + SDL_ISPIXELFORMAT_ALPHA(dst->format->format);
    int i_src;
    int j_src;
    for (int j_dst = 0 ; j_dst < dst->h ; j_dst++){
        for (int i_dst = 0 ; i_dst < dst->w ; i_dst++) {
        // Calcul de l'antécédent dans l'image
        float complex z= i_dst + I*j_dst;
        float complex position=dst->w/2 +dst->h/2 *I;
        float rayon = 100;
        z= transformation_rotation_centre( z,  position, cabs((z-position)/rayon)+position,rayon);
        i_src = creal(z);
        j_src = cimag(z);
        
        

        // copie du pixel s'il est dans l'image
        if (i_src<=0) i_src = 1;
        if (j_src<=0) j_src = 1;
        if (i_src>=dst->w) i_src = dst->w-1;
        if (j_src>=dst->h) j_src = dst->h-1;
        if ((pixel_size*(j_src*source->w+i_src) < dst->w * dst->h * pixel_size )&& (i_src>0)&&(j_src>0)&&(i_src<dst->w)&&(j_src<dst->h)){
            char *pixels_dst = &((char *)dst->pixels)[pixel_size * (j_dst * dst->w + i_dst)];
            char *pixels_source = &((char *)source->pixels)[pixel_size * (j_src * source->w + i_src)];
        memcpy(pixels_dst, pixels_source, pixel_size);
      }
    }
  }

  return SDL_CreateTextureFromSurface(renderer, dst);
}

SDL_Texture* animation3(SDL_Renderer *renderer, SDL_Surface *source ,int time)
{
    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, source->w, source->h, 8, source->format->format);

    // Toutes les images n'ont pas de couche alpha, il y a 3 ou 4 octets pour coder un pixel
    int pixel_size = 3 + SDL_ISPIXELFORMAT_ALPHA(dst->format->format);
    int i_src;
    int j_src;
    for (int j_dst = 0 ; j_dst < dst->h ; j_dst++){
        for (int i_dst = 0 ; i_dst < dst->w ; i_dst++) {
        // Calcul de l'antécédent dans l'image
        float complex z= i_dst + I*j_dst;
        float complex position=dst->w/2 +dst->h/2 *I;
        z= rotation_changement_de_centre( z,  position, cabs((z-position)*time/3000)+position);
        i_src = creal(z);
        j_src = cimag(z);
        
        // copie du pixel s'il est dans l'image
       /* if (i_src<=0) i_src = 1;
        if (j_src<=0) j_src = 1;
        if (i_src>=dst->w) i_src = dst->w-1;
        if (j_src>=dst->h) j_src = dst->h-1;*/
        if ((pixel_size*(j_src*source->w+i_src) < dst->w * dst->h * pixel_size )&& (i_src>0)&&(j_src>0)&&(i_src<dst->w)&&(j_src<dst->h)){
            char *pixels_dst = &((char *)dst->pixels)[pixel_size * (j_dst * dst->w + i_dst)];
            char *pixels_source = &((char *)source->pixels)[pixel_size * (j_src * source->w + i_src)];
        memcpy(pixels_dst, pixels_source, pixel_size);
      }
    }
  }
  SDL_Texture * texture = SDL_CreateTextureFromSurface(renderer, dst);
  SDL_FreeSurface(dst);
  return texture;
}

SDL_Texture* animation4(SDL_Renderer *renderer, SDL_Surface *source, int x,int y ){
    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, source->w, source->h, 8, source->format->format);

    // Toutes les images n'ont pas de couche alpha, il y a 3 ou 4 octets pour coder un pixel
    int pixel_size = 3 + SDL_ISPIXELFORMAT_ALPHA(dst->format->format);
    int i_src;
    int j_src;
    float complex position=x/2+y/2*I;
    float rayon = 100;
    for (int j_dst = 0 ; j_dst < dst->h ; j_dst++){
        for (int i_dst = 0 ; i_dst < dst->w ; i_dst++) {
        // Calcul de l'antécédent dans l'image
        float complex z= i_dst + I*j_dst;
        
        
        z= loupe( z,  position, rayon,1);
        i_src = creal(z);
        j_src = cimag(z);

        // copie du pixel s'il est dans l'image
        if (i_src<=0) i_src = 1;
        if (j_src<=0) j_src = 1;
        if (i_src>=dst->w) i_src = dst->w-1;
        if (j_src>=dst->h) j_src = dst->h-1;
        if ((pixel_size*(j_src*source->w+i_src) < dst->w * dst->h * pixel_size )&& (i_src>0)&&(j_src>0)&&(i_src<dst->w)&&(j_src<dst->h)){
            char *pixels_dst = &((char *)dst->pixels)[pixel_size * (j_dst * dst->w + i_dst)];
            char *pixels_source = &((char *)source->pixels)[pixel_size * (j_src * source->w + i_src)];
        memcpy(pixels_dst, pixels_source, pixel_size);
      }
    }
  }

  return SDL_CreateTextureFromSurface(renderer, dst);
}

SDL_Texture* animation5(SDL_Renderer *renderer, SDL_Surface *source)
{
    SDL_Surface *dst = SDL_CreateRGBSurfaceWithFormat(0, source->w, source->h, 8, source->format->format);

    // Toutes les images n'ont pas de couche alpha, il y a 3 ou 4 octets pour coder un pixel
    int pixel_size = 3 + SDL_ISPIXELFORMAT_ALPHA(dst->format->format);
    int i_src;
    int j_src;
    for (int j_dst = 0 ; j_dst < dst->h ; j_dst++){
        for (int i_dst = 0 ; i_dst < dst->w ; i_dst++) {
        // Calcul de l'antécédent dans l'image
        float complex z= i_dst + I*j_dst;
        
        i_src = creal(z);
        j_src = cimag(z);
        
        // copie du pixel s'il est dans l'image
        if (i_src<=0) i_src = 1;
        if (j_src<=0) j_src = 1;
        if (i_src>=dst->w) i_src = dst->w-1;
        if (j_src>=dst->h) j_src = dst->h-1;
        if ((pixel_size*(j_src*source->w+i_src) < dst->w * dst->h * pixel_size )&& (i_src>0)&&(j_src>0)&&(i_src<dst->w)&&(j_src<dst->h)){
            char *pixels_dst = &((char *)dst->pixels)[pixel_size * (j_dst * dst->w + i_dst)];
            char *pixels_source = &((char *)source->pixels)[pixel_size * (j_src * source->w + i_src)];
        memcpy(pixels_dst, pixels_source, pixel_size);
      }
    }
  }
  
  return SDL_CreateTextureFromSurface(renderer, dst);
}



void animation()
{
    SDL_bool program_on = SDL_TRUE; 
    SDL_Event event;
    SDL_Window * window   = NULL; // fenetre utilisee par le programme
    SDL_Renderer * renderer = NULL;
  

    if (SDL_Init(SDL_INIT_VIDEO) == -1)  /* SDL_INIT_EVERYTHING */ 
    {
        fprintf(stderr, "Erreur d'initialisation de la SDL : %s\n", SDL_GetError()); 
    } 
    
    window = SDL_CreateWindow(
       "magnifique Fenêtre :)",                    // codage en utf8, donc accents possibles
       0, 0,                                  // coin haut gauche en haut gauche de l'écran
       1000, 600,                              // largeur = 400, hauteur = 300
       SDL_WINDOW_RESIZABLE); 
    
    if (window == NULL) //si la fenetre a pu se créer
    {
        fprintf(stderr, "Erreur a la creation de la fenetre : %s\n", SDL_GetError()); 
    } else 
    {
        renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED ); // | SDL_RENDERER_PRESENTVSYNC 
        if (renderer == 0) {
            fprintf(stderr, "Erreur de creation d'un renderer : %s\n", SDL_GetError());
        }
    }
    
    SDL_Surface *surf = IMG_Load("./cameleon.jpg");
    if (!surf) {
    fprintf(stderr, "Erreur chargement image : %s\n", IMG_GetError());
    }
    SDL_Texture* image=SDL_CreateTextureFromSurface(renderer, surf);

    int time =0;
    int mousex=1;
    int mousey=1;
    
    //gestion de la fenetre

    while(program_on){

        /*lance la bouche d'evenement et gère tout les evenement a traiter*/
          
        //SDL_Texture* image =animation2(renderer, surf);
        //SDL_Texture* image =animation3(renderer, surf,time);
        //image =animation4(renderer, surf,mousex,mousey);
        //image = animation5(renderer, surf,time,mousex);
        SDL_Rect source = {0},
         window_dimensions = {0};

        SDL_GetWindowSize(window, &window_dimensions.w, &window_dimensions.h);

        SDL_QueryTexture(image, NULL, NULL, &source.w, &source.h);

        SDL_RenderCopy(renderer, image, &source, &source);
        

        if (SDL_PollEvent(&event)) 
        {   
            switch(event.type) 
            {
                case SDL_WINDOWEVENT:
                    switch (event.window.event)  
                    {
                        case SDL_WINDOWEVENT_CLOSE:  
                            printf("appui sur la croix\n");
                            break;
                        default:
                           
                    }   
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    printf("Clic :%d %d\n", event.button.x, event.button.y);
                    
                    
                    break;
                case SDL_QUIT : 
                    printf("On quitte\n");    
                    program_on = SDL_FALSE;
                }
          
        }SDL_GetMouseState( &mousex, &mousey);
        time++;
        SDL_RenderPresent(renderer);
        SDL_Delay(5);   
        SDL_SetRenderDrawColor(renderer,0,255,0,255);
        SDL_RenderClear(renderer);  
        
    }
    
    
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window)   SDL_DestroyWindow(window);
    if (image) SDL_DestroyTexture(image); 
    IMG_Quit();
    SDL_Quit();

}