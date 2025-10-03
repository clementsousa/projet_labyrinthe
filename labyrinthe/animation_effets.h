#ifndef __ANIMATION_EFFETS_H__
#define __ANIMATION_EFFETS_H__

SDL_Surface* TextureToSurface(SDL_Renderer * renderer,SDL_Texture* texture, int width, int height);

void affichage_complexe(float complex z);
void animation();
SDL_Texture* animation1(SDL_Renderer *renderer, SDL_Surface *source);
SDL_Texture* animation2(SDL_Renderer *renderer, SDL_Surface *source);
SDL_Texture* animation3(SDL_Renderer *renderer, SDL_Surface *source,int time);

SDL_Texture* animation4(SDL_Renderer *renderer, SDL_Surface *source, int x,int y );
SDL_Texture* animation5(SDL_Renderer *renderer, SDL_Surface *source);

#endif
