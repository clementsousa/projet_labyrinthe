#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include <complex.h>


#include "effets.h"


float complex agrandissement(float complex point_central, float zoom)
{   
    return zoom*point_central;
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
    return origine+deplacement;
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


