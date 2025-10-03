/**
 * Clément SOUSA
 * Rendu du travail Manipulation de sprites 
 * Début du travail : 17/06/25
 * flags : -lSDL2 -lSDL2_image -lSDL2_ttf
 * -DDEBUG pour activer le mode graphique
 */
/**
 * La version sans SDL est brutalement faite, étant donné que je n'avais au départ pas compris la consigne qu'il fallait
 * que notre programme de cet exercice soit jouable sans SDL. 
 * Je ne pense pas avoir malheureuselement le temps de faire ça plus proprement. 
 * (comme la gestion du décor ou j'aurai préféré avoir des structure pour chaque élément, qui
 *  possède leur propre vitesse et ainsi pouvoir faire du code beaucoup plus propre et compact
 *  à l'aide de tableau).
 */


#include <stdio.h>
#define SIZE_GRILLE 7

#ifndef DEBUG
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

/**
 * Essayer de sortir proprement de SDL (il manque la libération de certaines textures.)
 */
void exitSDL(int sortieNormal, const char * errorMsg, SDL_Texture * texture, SDL_Renderer * renderer, SDL_Window * window, TTF_Font * police){
    if(!sortieNormal){
        SDL_Log("Error : SDL initialisation - %s\n", errorMsg); 
    }
    if(texture) 
        SDL_DestroyTexture(texture);
    if(renderer)
        SDL_DestroyRenderer(renderer);
    if(window)
        SDL_DestroyWindow(window);
    if(police)
        TTF_CloseFont(police);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    exit(EXIT_FAILURE);
}

/**
 * Charge une texture à partir d'une image.
 * Fonction issue du cours.
 */
SDL_Texture *load_texture_from_image(char *file_image_name, SDL_Window *window, SDL_Renderer *renderer, TTF_Font * police)
{
    SDL_Surface *my_image = NULL;   // Variable de passage
    SDL_Texture *my_texture = NULL; // La texture

    my_image = IMG_Load(file_image_name); // Chargement de l'image dans la surface

    if (my_image == NULL)
        exitSDL(0, "Chargement de l'image impossible", NULL, renderer, window, police);

    my_texture = SDL_CreateTextureFromSurface(renderer, my_image); // Chargement de l'image de la surface vers la texture
    SDL_FreeSurface(my_image);                                     // la SDL_Surface ne sert que comme élément transitoire
    if (my_texture == NULL)
        exitSDL(0, "Echec de la transformation de la surface en texture", NULL, renderer, window, police);

    return my_texture;
}
#endif

void afficheGrille(int grille[]){
    for (int i = 0; i < SIZE_GRILLE; i++){
        printf("%d ", grille[i]);
    }
    printf("\n");
}
/**
 * Renvoie vrai si il y a collision
 */
int deplaceRocher(int grille[]){
    for(int i = 0; i < SIZE_GRILLE; i++){
        if(grille[i] == 2){
            if(i==1){
                grille[1] = 0;
                return 1;
            }else{
                grille[i-1] = 2;
                grille[i] = 0;
            }
        }
    }
    return 0;
}
int main(int argc, char const *argv[])
{
    (void)argc;
    (void)argv;
    // On suppose que notre width vaut 1200 par défaut.  --> grille de 7 cases; 1 case ~200px
    int grille[SIZE_GRILLE] = {0};
    grille[0] = 1; // notre joueur
    grille[SIZE_GRILLE-1] = 2; // le rocher (ennemi)
    int acc = 0;
    int program_on = 1;
    int score = 0;
    int is_jumping = 0;

    #ifndef DEBUG
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_DisplayMode screen;
    SDL_Texture * sprite_right  = NULL;
    TTF_Font * police = NULL;
    SDL_Event event;
    SDL_Texture * sol = NULL, * montagne = NULL, * nuage = NULL, * arbre = NULL, * jump = NULL, * rocher = NULL;
    int width, height;
    int montagne_offset_x = 0;
    int hauteur_sol;
    int jump_frame_parcourue = 0;
    float vy = 0;
    float gravite = 1.2;
    float force_du_saut = -18.0;
    /**
     * INITIALISATION SDL
     */

    if (SDL_Init(SDL_INIT_VIDEO) != 0){
        exitSDL(0, SDL_GetError(), sprite_right, renderer, window, police);
    }
    if (IMG_Init(IMG_INIT_PNG) == 0){
        exitSDL(0, "Erreur lors de l'initialisation de SDL/IMG", sprite_right, renderer, window, police);
    }
    if (TTF_Init() < 0) {
        exitSDL(0, "Couldn't initialize SDL TTF", sprite_right, renderer, window, police);
    }
    SDL_GetCurrentDisplayMode(0, &screen);
    window = SDL_CreateWindow("Animation", 0, 0, 1267, 712, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
    
    if (window == NULL){
        exitSDL(0, SDL_GetError(), sprite_right, renderer, window, police);
    }
    SDL_GetWindowSize(window, &width, &height);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL){
        exitSDL(0, SDL_GetError(), sprite_right, renderer, window, police); 
    }
    police = TTF_OpenFont("Roboto-Black.ttf", 16);
	if (!police) {
        SDL_Log("Erreur chargement police : %s", TTF_GetError());
        exitSDL(0, SDL_GetError(), sprite_right, renderer, window, police); 
    }




    /**
     * Chargement des images
     */

    sprite_right = load_texture_from_image("pics/personnage_right.png", window, renderer, police);
    jump = load_texture_from_image("pics/jump.png", window, renderer, police);
    sol = load_texture_from_image("pics/sol.png", window, renderer, police);
    montagne = load_texture_from_image("pics/montagne.png", window, renderer, police);
    nuage = load_texture_from_image("pics/nuage.png",window, renderer, police);
    arbre = load_texture_from_image("pics/arbre.png",window,renderer, police);
    rocher = load_texture_from_image("pics/rock.png",window,renderer, police);


    SDL_Rect run_sprite_right_source = {0}, sprite_destination = {0}, run_sprite_state = {0};
    SDL_Rect jump_sprite_right_source = {0}, jump_sprite_state = {0};

    SDL_Rect sol_source = {0}, sol_destination = {0};
    SDL_Rect montagne_source = {0}, montagne_dest = {0};
    SDL_Rect nuage_source = {0}, nuage_dest = {0};
    SDL_Rect arbre_source = {0}, arbre_dest = {0};
    SDL_Rect rocher_source = {0}, rocher_dest = {0};
    
    SDL_QueryTexture(sprite_right, NULL, NULL, &run_sprite_right_source.w, &run_sprite_right_source.h);
    SDL_QueryTexture(jump, NULL, NULL, &jump_sprite_right_source.w, &jump_sprite_right_source.h);

    SDL_QueryTexture(sol, NULL, NULL, &sol_source.w, &sol_source.h);
    SDL_QueryTexture(montagne, NULL, NULL, &montagne_source.w, &montagne_source.h);
    SDL_QueryTexture(nuage, NULL, NULL, &nuage_source.w, &nuage_source.h);
    SDL_QueryTexture(arbre, NULL, NULL, &arbre_source.w, &arbre_source.h);
    SDL_QueryTexture(rocher, NULL, NULL, &rocher_source.w, &rocher_source.h);


    nuage_dest.x = 0.3 * width;
    arbre_dest.x = width/2;
    sol_destination.h = height/10;



    /**
     * Préparation des frames du sprite
     */

    // Run
    int run_nb_images = 8;
    float zoom = 3;
    int run_offset_x = run_sprite_right_source.w/run_nb_images,
        offset_y = run_sprite_right_source.h;

    run_sprite_state.x = 0;   // quelle zone on prend
    run_sprite_state.y = 0;
    run_sprite_state.w = run_offset_x;
    run_sprite_state.h = offset_y;

    // Jump
    int jump_nb_images = 5;
    int jump_offset_x = jump_sprite_right_source.w/jump_nb_images;
    int jump_offset_y = jump_sprite_right_source.h;

    jump_sprite_state.x = 0;
    jump_sprite_state.y = 0;
    jump_sprite_state.w = jump_offset_x;
    jump_sprite_state.h = jump_offset_y;

    sprite_destination.w = run_offset_x * zoom; // taille destination
    sprite_destination.h = offset_y * zoom;

    sprite_destination.y = height - sol_destination.h - sprite_destination.h;



    int speed = 15*width/700;
    rocher_dest.x = width;
    #endif

    int collision = 0;
    while(program_on){
        #ifndef DEBUG
        acc += speed;
        if(acc > (width)/(SIZE_GRILLE) && !collision){ //Met à jour la grille car on considère que le rocher a été assez longtemps dans une case.
            collision = deplaceRocher(grille);
            acc = 0;
        }
        #endif
        #ifdef DEBUG
        afficheGrille(grille);
        scanf("%d", &is_jumping);
        if(is_jumping > 1){
            is_jumping = 1;
        }
        collision = deplaceRocher(grille);
        if(collision && !is_jumping){
            printf("Vous avez perdu ! Score : %d", score);
            program_on = 0;
        }
        if(collision && is_jumping){
            score++;
            grille[SIZE_GRILLE-1] = 2;
        }
    }
        #endif
        #ifndef DEBUG
        if(collision && !is_jumping){
            program_on = 0;
        }

        // Mise à jour de la taille des éléments au cas où la fenêtre  été resize.
        SDL_GetWindowSize(window, &width, &height);
        sol_destination.w = width;
        sol_destination.h = height/10;
        sol_destination.y = height - sol_destination.h;
        hauteur_sol =  sol_destination.y;

        montagne_dest.h = 8*height/10;
        montagne_dest.w = width;
        montagne_dest.y = height - sol_destination.h - montagne_dest.h;

        nuage_dest.h = 0.2*height;
        nuage_dest.w = 0.2 * width;
        nuage_dest.y = 0.1 * width;
        

        arbre_dest.h = 0.5*height;
        arbre_dest.w = 0.2 * width;
        arbre_dest.y = sol_destination.y - arbre_dest.h;

        rocher_dest.h = 0.1*height;
        rocher_dest.w = 0.1 * width;
        rocher_dest.y = height - sol_destination.h - rocher_dest.h;

        if(!is_jumping){
            sprite_destination.y = height - sol_destination.h - sprite_destination.h;
        }
        



        const Uint8* keystates = SDL_GetKeyboardState(NULL);
        if (!is_jumping && (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_SPACE])) {
            is_jumping = 1;
            jump_frame_parcourue = 0;
            vy = force_du_saut;
        }

        run_sprite_state.x += run_offset_x;
        run_sprite_state.x %= run_sprite_right_source.w;
        
        if (keystates[SDL_SCANCODE_ESCAPE]){
            program_on = 0;
        }
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                program_on = 0;
            }
        }

        // Gestion du mouvement du décor

        montagne_offset_x -= (speed/7.5) > 0? (int) speed/7.5: 1 ;
        // Si notre montagne est entièrement sorti
        if (montagne_offset_x + montagne_source.w < 0){ 
            montagne_offset_x = 0;
        }
    
        arbre_dest.x -= speed/2;
        nuage_dest.x -= (speed/15) > 0? (int) speed/15: 1 ;
        rocher_dest.x -= speed;


        if (arbre_dest.x + arbre_dest.w < 0)
            arbre_dest.x = width;
        if (nuage_dest.x + nuage_dest.w < 0)
            nuage_dest.x = width;
        


        if (is_jumping) {
            sprite_destination.y += vy;
            vy += gravite;

            if (vy <= 0 && jump_frame_parcourue < jump_nb_images) {  // On joue l'animation et on garde la dernière image tant qu'on est en l'air
                jump_sprite_state.x = jump_frame_parcourue * jump_offset_x;
                jump_frame_parcourue++;
            } 
            jump_sprite_state.y = 0;
            jump_sprite_state.w = jump_offset_x;
            jump_sprite_state.h = jump_offset_y;

            if (sprite_destination.y >= hauteur_sol - sprite_destination.h) { // On touche le sol
                if(collision){ // On refait apparaitre un caillou quand l'utilisateur a cessé de sauter POUR SYNCHRONISER 
                    grille[SIZE_GRILLE-1] = 2;
                    acc = 0;
                    rocher_dest.x = width - rocher_dest.w;
                    collision = 0;
                    score++;
                }
                
                sprite_destination.y = hauteur_sol - sprite_destination.h;
                is_jumping = 0;
                vy = 0;
            }
        }
        SDL_SetRenderDrawColor(renderer, 176, 216, 230,255); // bleu ciel : dessin du ciel
        SDL_RenderClear(renderer);
        

        SDL_RenderCopy(renderer, nuage, NULL, &nuage_dest); // nuage : dernière couche

        montagne_dest.x = montagne_offset_x;
        SDL_RenderCopy(renderer, montagne, NULL, &montagne_dest); // montagne : avant-dernière couche

        montagne_dest.x = montagne_offset_x + montagne_source.w;
        SDL_RenderCopy(renderer, montagne, NULL, &montagne_dest);

        SDL_RenderCopy(renderer, arbre, NULL, &arbre_dest); // arbre : couche 2
        SDL_RenderCopy(renderer,sol,NULL,&sol_destination);  // sol : couche "1"
        SDL_RenderCopy(renderer, rocher, NULL, &rocher_dest); // rocher : couche 1
        if(is_jumping){ // personnage (si il saute ou non) : couche 1
            SDL_RenderCopy(renderer,jump, &jump_sprite_state, &sprite_destination);
        }else{
            SDL_RenderCopy(renderer,sprite_right,&run_sprite_state,&sprite_destination);
        }
        

        /**
         * Affichage du score
         * Pour la gestion du texte, issue du cours en ligne.
        */
        char score_text[50];
        snprintf(score_text, 50, "Score: %d", score);  // RenderText n'accepte pas les chaines formatées, on doit donc convertir notre score
        SDL_Color color = {20, 0, 40, 255};                                  // la couleur du texte
        SDL_Surface* text_surface = NULL;                                    // la surface  (uniquement transitoire)
        text_surface = TTF_RenderText_Blended(police, score_text, color); // création du texte dans la surface
        if (text_surface == NULL) exitSDL(0, "Can't create text surface", sprite_right, renderer, window,police);

        SDL_Texture* text_texture = NULL;                                    // la texture qui contient le texte
        text_texture = SDL_CreateTextureFromSurface(renderer, text_surface); // transfert de la surface à la texture
        if (text_texture == NULL) {
            SDL_FreeSurface(text_surface);
            exitSDL(0, "Can't create texture from surface", sprite_right, renderer, window,police);
        }
        SDL_FreeSurface(text_surface);                                       // la texture ne sert plus à rien
        
        SDL_Rect pos = {0, 0, 0, 0};                                         // rectangle où le texte va être prositionné
        SDL_QueryTexture(text_texture, NULL, NULL, &pos.w, &pos.h);          // récupération de la taille (w, h) du texte
        SDL_RenderCopy(renderer, text_texture, NULL, &pos);                  // Ecriture du texte dans le renderer
        SDL_DestroyTexture(text_texture);                                    // On n'a plus besoin de la texture avec le texte
       
        SDL_RenderPresent(renderer);
        SDL_Delay(40);  // Diminuer cette valeur si on veut un jeu plus fluide (et plus rapide)
    }


    /**
     * Libération des textures
     */

    SDL_DestroyTexture(arbre);
    SDL_DestroyTexture(nuage);
    SDL_DestroyTexture(montagne);
    SDL_DestroyTexture(sol);
    SDL_DestroyTexture(jump);
    SDL_DestroyTexture(rocher);
    exitSDL(1, NULL, sprite_right, renderer, window, police);
    sprite_right = NULL;
    renderer = NULL;
    window = NULL;
    return 0;
    #endif
}
