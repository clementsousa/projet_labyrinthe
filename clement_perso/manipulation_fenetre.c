/**
 * Clément SOUSA
 * Rendu du travail Manipulation de fenêtres graphiques. 
 * Début du travail : 16/06/25
 * flags : -lSDL2 -lSDL2_ttf
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 400
#define MAX_SECONDARY_WINDOW 10


void drawWindow(SDL_Window * window, TTF_Font * police, SDL_Renderer * renderer){
	SDL_Rect rect;
	int width, height;
	/* on prépare/efface le renderer */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	SDL_GetWindowSize(window, &width, &height);

	SDL_RenderFillRect(renderer,&rect);
	

	SDL_Color couleur = {255,255,255,255};
	SDL_Surface * surface = TTF_RenderText_Blended_Wrapped(police, "Appuyez sur zqsd/fleches directionnelles pour deplacer la fenetre dans un coin. Espace pour ouvrir une nouvelle fenetre (max 10). p et m pour augmenter/diminuer la taille de la fenetre. ECHAP pour quitter.",couleur, width-40);
	SDL_Texture * texttext = SDL_CreateTextureFromSurface(renderer, surface);
	rect.x = 20;
	rect.y = 20;
	rect.w = surface->w;
	rect.h = surface->h;
	SDL_QueryTexture(texttext, NULL, NULL, NULL, NULL);
	SDL_RenderCopy(renderer, texttext, NULL, &rect);
    SDL_RenderPresent(renderer);
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texttext);

}

/**
 * Déplace la fenêtre dans un des 4 coins suivant la direction demandée.
 * @param window : la fenêtre à déplacer
 * @param direction : 0,1,2,3 respectivement : haut,bas,gauche,droite
 */
void moveWindow(SDL_Window * window, int direction){
	int x, y;
	int width, height;
	SDL_DisplayMode mode;

	SDL_GetWindowPosition(window, &x, &y);
	SDL_GetWindowSize(window, &width, &height);
	SDL_GetCurrentDisplayMode(0, &mode);
	
	switch (direction){
		case 0:
			SDL_SetWindowPosition(window, x, 0);
			break;
		case 1:
			SDL_SetWindowPosition(window, x, mode.h - height);
			break;
		case 2:
			SDL_SetWindowPosition(window, 0, y);
			break;
		case 3:
			SDL_SetWindowPosition(window, mode.w - width, y);
			break;
		default:
			break;
	}
}


/**
 * Redimensionne la fenêtre.
 * @param window : la fenêtre à redimensionner
 * @param diminution : 0 si on veut augmenter, 1 si on veut diminuer
 * @param pourcentage : de combien par rapport à la fenêtre d'origine on augmente (afin de conserver les proportions de base)
 */
void resizeWindow(SDL_Window * window, int diminution, int pourcentage){
	int x, y;
	int width, height;
	int new_width, new_height;
	int width_delta;
	int height_delta;
	SDL_DisplayMode mode;


	SDL_GetWindowPosition(window, &x, &y);
	SDL_GetWindowSize(window, &width, &height);
	SDL_GetCurrentDisplayMode(0, &mode);


	width_delta = WINDOW_WIDTH * (float) pourcentage/100;
	height_delta = WINDOW_HEIGHT * (float) pourcentage/100;


	if(diminution){
		new_width = width - width_delta;
		new_height = height - height_delta;

		SDL_SetWindowSize(window, new_width, new_height);
		if(x==mode.w - width){ // Reposition de la fenêtre
			x = mode.w - new_width;
			SDL_SetWindowPosition(window, x, y);
		}
		if(y == mode.h - height){ // Reposition de la fenêtre
			y = mode.h - new_height;
			SDL_SetWindowPosition(window, x, y); 
		}
	}else{ // on augmente
		SDL_SetWindowSize(window, width+width_delta, height + height_delta);
		// Le repositionnement de la fenêtre est automatique visiblement
	}
}


/**
 * Ouvre une nouvelle fenêtre, 2 fois moins grande.
 * @param window : la fenêtre principale, qui nous servira pour positionner la nouvelle fenêtre
 */
void openNewWindow(SDL_Window * windows[], SDL_Window * window, int * nb_created){
	if(*nb_created < MAX_SECONDARY_WINDOW){
		int width, height;
		SDL_GetWindowSize(window, &width, &height);
		windows[*nb_created] = SDL_CreateWindow("Fenêtre secondaire - on ne peut rien faire", 0, 0, WINDOW_WIDTH - 0.5*width, WINDOW_HEIGHT - 0.5*height, SDL_WINDOW_RESIZABLE);
		(*nb_created)++;
	}
	
}


int main(int argc, char * argv[]){
	(void) argc;
	(void) argv;
  
	SDL_Window * window1 = NULL;
	SDL_bool program_on = SDL_TRUE;
	SDL_bool event_utile = SDL_FALSE;
	SDL_Event event; 
	SDL_Window * secondary_windows[MAX_SECONDARY_WINDOW];
	SDL_Renderer *renderer;


	for(int i = 0; i < MAX_SECONDARY_WINDOW; i++){
		secondary_windows[i] = NULL;
	}
	int nb_secondary_window_created = 0;

  	if(SDL_Init(SDL_INIT_VIDEO) != 0){
    	SDL_Log("Error : SDL initialisation - %s\n", SDL_GetError());
    	exit(EXIT_FAILURE);
  	}
	if(TTF_Init()!=0) {
		SDL_Log("TTF_Init: %s\n", TTF_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}

	TTF_Font * police = TTF_OpenFont("Roboto-Black.ttf", 16);
	if (!police) {
        SDL_Log("Erreur chargement police : %s", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

  	window1 = SDL_CreateWindow("Fenêtre principale", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE);

  	if(window1 == NULL){
    	// Erreur lors de la création de la première fenêtre
    	SDL_Log("Error : SDL window 1 creation - %s\n", SDL_GetError());
    	SDL_Quit();                   
		TTF_Quit();
    	exit(EXIT_FAILURE);
  	}
		renderer = SDL_CreateRenderer(window1, -1, SDL_RENDERER_ACCELERATED ); 
	if (renderer == 0) {
		SDL_Log("Erreur d'initialisation de la SDL : %s\n", SDL_GetError());
		SDL_DestroyWindow(window1);
		SDL_Quit();
		TTF_Quit();
		exit(EXIT_FAILURE);
	}	

	while(program_on){
		event_utile = SDL_FALSE;
		while(!event_utile && SDL_PollEvent(&event)) {

			switch(event.type){
				case SDL_QUIT:
					program_on=SDL_FALSE;
					event_utile = SDL_TRUE;
					break;
				case SDL_KEYDOWN:
					switch(event.key.keysym.sym){
						case SDLK_ESCAPE:
							program_on = SDL_FALSE;
							event_utile = SDL_TRUE;
							break;
						case SDLK_UP:
						case SDLK_z:
							// On met la fenêtre en haut par rapport à la position actuelle
							moveWindow(window1, 0);
							event_utile = SDL_TRUE;
							break;
						case SDLK_DOWN:
						case SDLK_s:
							// On met la fenêtre en bas
							moveWindow(window1, 1);
							event_utile = SDL_TRUE;
							break;
						case SDLK_LEFT:
						case SDLK_q:
							// On met la fenêtre à gauche
							moveWindow(window1, 2);
							event_utile = SDL_TRUE;
							break;
						case SDLK_RIGHT:
						case SDLK_d:
							// On met la fenêtre en haut
							moveWindow(window1, 3);
							event_utile = SDL_TRUE;
							break;
						case SDLK_SPACE:
							// Ouvrir une nouvelle fenêtre
							openNewWindow(secondary_windows ,window1, &nb_secondary_window_created);
							event_utile = SDL_TRUE;
							break;
						case SDLK_p:
							// Redimensionnement de la fenêtre (+10%) par rapport à la taille d'origine
							resizeWindow(window1, 0,10);
							event_utile = SDL_TRUE;
							break;
						case SDLK_m:
							// Redimensionnement de la fenêtre (-10%) par rapport à la taille d'origine
							resizeWindow(window1,1,10);
							event_utile = SDL_TRUE;
							break;
						default:
							break;
					}
				default:
					break;
			}
		}
		drawWindow(window1, police, renderer);
		SDL_Delay(50); 
	}
	for(int i = nb_secondary_window_created; i >= 0; i--){
		if(secondary_windows[i] != NULL){
			SDL_DestroyWindow(secondary_windows[i]);
		}
	}
	SDL_DestroyRenderer(renderer);
  	SDL_DestroyWindow(window1);
	TTF_CloseFont(police);
	TTF_Quit();
  	SDL_Quit();

  	return 0;
}