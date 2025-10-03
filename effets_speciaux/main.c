#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include <complex.h>


#include "effets.h"
#include "animation_effets.h"


int main()
{   
    double complex z = 2.2+I*2;
    z= agrandissement(z,40);
    z= translation(z,5-I*2);
    z= rotation(z,M_PI);
    printf("z == %.1f%+.1fi\n", creal(z), cimag(z));
    animation();
    //affichage_complexe(z);

    return 0;
}