/**
 * Clément SOUSA
 * Rendu du travail Manipulation de formes géométriques 
 * Début du travail : 16/06/25
 * flags : -lSDL2 -lSDL2_ttf
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DELAY 16
#define VITESSE_HORIZONTALE 5
#define VITESSE_VERTICALE 10
#define MAX_NB_OF_SQUARE 30

typedef struct {
    SDL_Rect rect;
    int vx, vy; // vitesse de chaque carré autonome
    Uint8 color[4]; // couleur du carré
} CarreAutonome;

typedef enum {
    IMMOBILE,
    SAUT,
    GAUCHE,
    DROITE
} EtatRect;

Uint8 square_color[] = {255, 170, 66, 255}; // Choix d'une variable globale pour la couleur afin de pouvoir la modifier de partout (pareil pour vy et sol ?)
Uint8 bg_color[] = {170,66, 255, 255}; // Couleur du fond
CarreAutonome carre_autonomes[MAX_NB_OF_SQUARE]; // Les carrés qui se baladeront seul
int nb_carre = 0; // Nombre de carré(s) qui se balade seul

/**
 * Change de façon douce la teinte de la couleur passer en paramètre
 * @param color un tableau de 4 entiers représentant une couleur 
 * @param nuance La variation sur la teinte
 */
void setNewColor(Uint8 color[], int nuance){
    Uint8  max, min, r = color[0], g = color[1], b = color[2];
    int t_i;
    float t,s,v, f,l,m,n;
    char max_color;

    // Conversion en TSV pour avoir un changement de couleur doux. (+10 sur la teinte)
    max = r;
    max_color = 'r';
    if (g > max){
        max = g;
        max_color = 'g';
    } 
    if (b > max) {
        max = b;
        max_color = 'b';
    }
    min = r;
    if (g < min) {
        min = g;
    }
    if (b < min) {
        min = b;
    }


    if (min==max){
        t = 0;
    }else if(max_color == 'r'){
        t = (60 * (g-b)/(max-min) + 360)%360;
    }else if(max_color == 'g'){
        t = 60*(b-r)/(max-min) + 120;
    }else{
        t =60*(r-g)/(max-min) + 240;
    }

    s = (max == 0) ? 0 : 1-(min/max);
    v = max;

    // On peut maintenant appliquer notre +10 sur la teinte pour la transformation douce
    t = (int)(t+nuance)%360;

    // On peut repasser en RGB
    t_i = (int)(t/60)%6;
    f = t/60 - t_i;
    l = v * (1-s);
    m = v * (1-f*s);
    n = v * (1-(1-f)*s);
    switch(t_i){
        case 0:
            color[0] = v;
            color[1] = n;
            color[2] = l;
            break;
        case 1:
            color[0] = m;
            color[1] = v;
            color[2] = l;
            break;
        case 2:
            color[0] = l;
            color[1] = v;
            color[2] = n;
            break;
        case 3:
            color[0] = l;
            color[1] = m;
            color[2] = v;
            break;
        case 4:
            color[0] = n;
            color[1] = l;
            color[2] = v;
            break;
        case 5:
            color[0] = v;
            color[1] = l;
            color[2] = m;
            break;
        default:
            printf("Erreur lors de la conversion de couleur.");
            break;
    }
}

/**
 * Dessine notre fenêtre
 * @param renderer
 * @param rect notre rectangle principal
 * @param police la police de caractère que l'on souhaite utiliser
 * @param width la largeur de la fenetre
 */
void draw(SDL_Renderer * renderer, SDL_Rect * rect, TTF_Font * police, int width){
    SDL_SetRenderDrawColor(renderer,bg_color[0], bg_color[1], bg_color[2], bg_color[3]);
    SDL_RenderClear(renderer);

    // Affichage des petits carrés autonomes
    for (int i = 0; i < nb_carre; i++) {
        CarreAutonome *carre = &carre_autonomes[i];
        SDL_SetRenderDrawColor(renderer, carre->color[0], carre->color[1], carre->color[2], carre->color[3]);
        SDL_RenderFillRect(renderer, &carre->rect);
    }

    // Affichage du carré principal
    SDL_SetRenderDrawColor(renderer, square_color[0], square_color[1], square_color[2], square_color[3]);
    SDL_RenderFillRect(renderer, rect);

    // Affichage du texte explicatif
    SDL_Rect rect_text;
    SDL_Color color = {square_color[0],square_color[1],square_color[2],square_color[3]};
    SDL_Surface * surface = TTF_RenderText_Blended_Wrapped(police, "Appuyez sur n pour un nouveau carre autonome. Espace pour sauter. q/d pour gauche/droite. ECHAP pour quitter.",color, width-40);
	SDL_Texture * texttext = SDL_CreateTextureFromSurface(renderer, surface);
	rect_text.x = 20;
	rect_text.y = 20;
	rect_text.w = surface->w;
	rect_text.h = surface->h;
	SDL_QueryTexture(texttext, NULL, NULL, NULL, NULL);
	SDL_RenderCopy(renderer, texttext, NULL, &rect_text);


    SDL_RenderPresent(renderer);

    
	SDL_FreeSurface(surface);
	SDL_DestroyTexture(texttext);
}
void gravite(SDL_Rect * rect, float * vy, int sol){
    float ay = 0.1;
    (*vy) += ay;
    if(*vy>2*VITESSE_VERTICALE/3){
        *vy=2*VITESSE_VERTICALE/3;
    }
    rect->y += (int)(*vy);

    if (rect->y >= sol) {
        rect->y = sol;
        (*vy) = 0;
    }
}

/**
 * Créer un carré autonome (il se déplace seul)
 * @param width largeur de la fenêtre
 * @param height hauteur de la fenêtre
 */
void ajoutCarreAutonome(int width, int height){
    if(!(nb_carre>=MAX_NB_OF_SQUARE)){
        CarreAutonome * carre = &carre_autonomes[nb_carre];

        carre->rect.h = 0.1*height;
        carre->rect.w = 0.1*height;

        // Position aléatoire
        carre->rect.x = rand()%(width-carre->rect.w);
        carre->rect.y = rand()%((int) (height-carre->rect.h));

        // Vitesse entre 3 et 9
        carre->vx = rand()%7 + 3;
        if(carre->vx == 0){
            carre->vx=1;
        }
        carre->vy = rand()%7 + 3;
        if(carre->vy == 0){
            carre->vy=1;
        }


        carre->color[0] = rand()%255;
        carre->color[1] = rand()%255;
        carre->color[2] = rand()%255;
        carre->color[3] = 255;
        nb_carre++;
    }
}
/**
 * Met à jour la position de tout nos carrés autonomes (en fonction de leur vitesse)
 * @param width largeur de la fenêtre
 * @param height hauteur de la fenêtre
 */
void updateCarreAutonome(int width, int height){
    for (int i = 0; i < nb_carre; i++) {
        CarreAutonome *carre = &carre_autonomes[i];
        carre->rect.x += carre->vx;
        carre->rect.y += carre->vy;
        carre->rect.w = height*0.1;
        carre->rect.h = height*0.1;

        if (carre->rect.x < 0 || carre->rect.x > width - carre->rect.w ) {
            carre->vx = -carre->vx;
            carre->rect.x += carre->vx;
        }
        if (carre->rect.y < 0 || carre->rect.y > height - carre->rect.h ) {
            carre->vy = -carre->vy;
            carre->rect.y += carre->vy;
        }

        // Gestion des cas où les carrés sortent après redimensionnement
        if (carre->rect.x + carre->rect.w > width) {
            carre->rect.x = width - carre->rect.w;
        }
        if (carre->rect.y + carre->rect.h > height) {
            carre->rect.y = height - carre->rect.h;
        }
        if (carre->rect.x < 0) {
            carre->rect.x = 0;
        }
        if (carre->rect.y < 0) {
            carre->rect.y = 0;
        }
    }
}

int main(int argc, char *argv[]){

    (void)argc;
    (void)argv;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_DisplayMode screen;

    SDL_bool program_on = SDL_TRUE, event_utile = SDL_FALSE;
    SDL_Event event;
    
    EtatRect etat = IMMOBILE;

    int width, height, sol, nb_frame = 0;
    float vy = 0;

    SDL_Rect rect;

    srand(time(NULL));

    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
    if(TTF_Init()!=0) {
		SDL_Log("TTF_Init: %s\n", TTF_GetError());
		SDL_Quit();
		exit(EXIT_FAILURE);
	}
    TTF_Font * police = TTF_OpenFont("Roboto-Black.ttf", 25);
    if (!police) {
        SDL_Log("Erreur chargement police : %s", TTF_GetError());
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);
    }

    

    SDL_GetCurrentDisplayMode(0, &screen);

    window = SDL_CreateWindow("Animation", 0, 0, screen.w, screen.h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    if (window == NULL){
        SDL_Log("Error : SDL initialisation - %s\n", SDL_GetError());
        TTF_CloseFont(police);
        TTF_Quit();
        SDL_Quit();
    	exit(EXIT_FAILURE);
    }
    SDL_GetWindowSize(window, &width, &height);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL){
        SDL_Log("Error : SDL initialisation - %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_CloseFont(police);
        TTF_Quit();
        SDL_Quit();
        exit(EXIT_FAILURE);    
    }
    // Création de notre rectangle
    rect.w = height*0.2; // Carré de côté 20% de la hauteur
    rect.h = height*0.2; 
    rect.x = (width - rect.w)/2; // Placé au milieu en largeur
    rect.y = 0;  // Placé tout en bas
    sol = height - rect.h;

    while(program_on){
        event_utile = SDL_FALSE;
        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if(!(keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_SPACE] || keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A] || keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D] )){
            etat = IMMOBILE;
            nb_frame = 0;
        }else{
            if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_SPACE]){
                // On saute !
                etat = SAUT;
                nb_frame = 0;
            }
            if (keystates[SDL_SCANCODE_LEFT] || keystates[SDL_SCANCODE_A]){
                // On se déplace vers la gauche
                etat = GAUCHE;
                nb_frame = 0;
            }
            if (keystates[SDL_SCANCODE_RIGHT] || keystates[SDL_SCANCODE_D]){
                // On se déplace vers la droite
                etat = DROITE;
                nb_frame = 0;
            }
        }
        
		while(!event_utile && SDL_PollEvent(&event)) {
            switch(event.type){

				case SDL_QUIT:
					program_on=SDL_FALSE;
					event_utile = SDL_TRUE;
					break;

				case SDL_KEYDOWN:
                    setNewColor(square_color,2);
                    setNewColor(bg_color,2);

					switch(event.key.keysym.sym){
                        case SDLK_ESCAPE:
                            program_on = SDL_FALSE;
                            event_utile = SDL_TRUE;
                            break;
                        case SDLK_n:
                            ajoutCarreAutonome(width, height);
                            event_utile = SDL_TRUE;
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    break;
            }
                
        }
        switch(etat) {
            case SAUT:
                if (nb_frame < (int)(0.4 * height / VITESSE_VERTICALE)) {
                    if (rect.y > VITESSE_VERTICALE) {
                        rect.y -= VITESSE_VERTICALE;
                    } else {
                        rect.y = 0;
                    }
                    nb_frame++;
                } else {
                    etat = IMMOBILE;
                    nb_frame = 0;
                }
                break;
            case GAUCHE:
                if (nb_frame < (int)(0.1 * width / VITESSE_HORIZONTALE)) {
                    if (rect.x > VITESSE_HORIZONTALE) {
                        rect.x -= VITESSE_HORIZONTALE;
                    } else {
                        rect.x = 0;
                    }
                    nb_frame++;
                } else {
                    etat = IMMOBILE;
                    nb_frame = 0;
                }
                break;
            case DROITE:
                if (nb_frame < (int)(0.1 * width / VITESSE_HORIZONTALE)) {
                    if (rect.x < width - rect.w - VITESSE_HORIZONTALE) {
                        rect.x += VITESSE_HORIZONTALE;
                    } else {
                        rect.x = width - rect.w;
                    }
                    nb_frame++;
                } else {
                    etat = IMMOBILE;
                    nb_frame = 0;
                }
                break;
            case IMMOBILE:
            default:
                break;
        }

        // Gestion du redimensionnement de la fenêtre
        SDL_GetWindowSize(window, &width, &height);
        sol = height - rect.h;
        rect.w = height * 0.2;
        rect.h = height * 0.2;
        if (rect.x + rect.w > width){
            rect.x = width - rect.w;
        }
        if (rect.y + rect.h > height){
            rect.y = height - rect.h;
        }
        if (rect.x < 0){
            rect.x = 0;
        }
        if (rect.y < 0){
            rect.y = 0;
        }

        gravite(&rect, &vy, sol);
        updateCarreAutonome(width, height);
        draw(renderer,&rect, police, width);
        SDL_Delay(DELAY);
    }

    

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(police);
	TTF_Quit();
    SDL_Quit();
    return EXIT_SUCCESS;
}