#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include <complex.h>


#include "effets.h"
#include "labyrinthe.h"


void transformation_de_translation( SDL_Surface *source ,float zoom, int x, int y)
{
    SDL_Surface* dst = SDL_CreateRGBSurfaceWithFormat(0, source->w, source->h, 8, source->format->format);

    // Toutes les images n'ont pas de couche alpha, il y a 3 ou 4 octets pour coder un pixel
    int pixel_size = 3 + SDL_ISPIXELFORMAT_ALPHA(dst->format->format);
    int i_src;
    int j_src;
    for (int j_dst = 0 ; j_dst < dst->h ; j_dst++){
        for (int i_dst = 0 ; i_dst < dst->w ; i_dst++) {
        // Calcul de l'antécédent dans l'image
        float complex z= i_dst + I*j_dst;
        float complex position = (x + y*I) * zoom;
        float complex centre = -(dst->w + I*dst->h) * zoom;
        float complex waw = centre* zoom -position;
        //float complex waw = centre - position ;
        //z = rotation_changement_de_centre(z, centre, M_PI);
        z = translation(z, -centre);
        //z = translation(z, -(largeur / LAB_N + hauteur / LAB_P*I) / 2 * zoom);
        //z = translation(z, -position);
        z = agrandissement(z + waw ,zoom) -waw;

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
  
    *source=*dst;
}

SDL_Surface* convertion_renderer_surface(int w, int h) {
    if (!renderer) return NULL;

    SDL_Surface* surface = SDL_CreateRGBSurfaceWithFormat(0, w, h, 32, SDL_PIXELFORMAT_ARGB8888);
    if (!surface) {
        SDL_Log("Erreur création surface: %s", SDL_GetError());
        return NULL;
    }

    // Lit le contenu de l'écran rendu
    if (SDL_RenderReadPixels(renderer, NULL, SDL_PIXELFORMAT_ARGB8888,
                             surface->pixels, surface->pitch) != 0) {
        SDL_Log("Erreur lecture pixels du renderer: %s", SDL_GetError());
        SDL_FreeSurface(surface);
        return NULL;
    }

    return surface;
}



float complex agrandissement(float complex point_central, float zoom)
{   
    return point_central/zoom;
}

float complex rotation(float complex centre_de_rotation, float teta)
{
    return centre_de_rotation*cexpf(I*teta);
}

float complex rotation_inverse(float complex centre_de_rotation, float teta)
{
    return centre_de_rotation*cexpf(-I*teta);
}

float complex translation(float complex origine, float complex deplacement)
{
    return origine-deplacement;
}

float complex rotation_changement_de_centre(float complex z, float complex nouvelle_origine, float teta)
{
    return rotation_inverse(z-nouvelle_origine,teta)+nouvelle_origine;
}

float complex transformation_rotation_centre(float complex z,float complex nouvelle_origine, float teta, float rayon)
{
    
    if (cabsf(z-nouvelle_origine)<rayon/2)
    {
       z= rotation_changement_de_centre( z,  nouvelle_origine, teta*8);
    }
    else {if (cabsf(z-nouvelle_origine)<rayon)
    {
       z= rotation_changement_de_centre( z,  nouvelle_origine, -teta*3.5);
    }}
    return z;
     
}

float complex loupe(float complex z, float complex position, float rayon, float force)
{
    z=z-position;
    if (cabsf(z)<rayon)
    {
        z=agrandissement(z,cabsf(z)/rayon*force);
    }
    
    return z+position;
}

float complex rotation_inverse_cercle(float complex z, float complex nouvelle_origine, float teta, float rayon)
{
    z=z-nouvelle_origine;
    if (cabsf(z)<rayon)
    {
    z =rotation_inverse(z-nouvelle_origine,teta)+nouvelle_origine;
    }
    return z;
}


