#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<unistd.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>


#include "partition.h"
#include "exercice1.h"
#include "kruskal.h"
#include "parcours_graphe.h"
#include "file.h"
#include "labyrinthe.h"
#include "tas_binaire.h"
#include "abr.h"

int type_de_terrain[NB_TERRAIN] = {DESERT, MARAIS, OCEAN, PLAINE, MONTAGNE, NORMAL};

/**
 * genere_tableau_voisin
 * @param liste_arete : la liste des arêtes de notre labyrinthe
 * @param tableau : le tableau que l'on va remplir, contenant à la ligne 0 les numéros des noeuds, à la ligne 1 leur nombre de voisin, et les 4 lignes suivantes leur voisin
 */ 
void genere_tableau_voisin(labyrinthe_t * laby, int tableau[6][LAB_N * LAB_P])
{

    int i,j;
    int nb_voisin;
    int noeud;


    for(int i = 0; i < LAB_N * LAB_P; i++){     // Initialisation de la ligne 1 et 2.
        tableau[0][i] = i;
        tableau[1][i] = 0;
    }
    for(i=0; i<LAB_P; i++){   
        for(j=0; j<LAB_N; j++){
            noeud = i * LAB_N + j;  
            nb_voisin = 2;
            if(!(laby->grille[i][j] & NORD)){   
                tableau[nb_voisin][noeud] = noeud - LAB_N;
                nb_voisin++;
            }
            if(!(laby->grille[i][j] & EST)){
                tableau[nb_voisin][noeud] = noeud + 1;
                nb_voisin++;
            }
            if(!(laby->grille[i][j] & SUD)){
                tableau[nb_voisin][noeud] = noeud + LAB_N;
                nb_voisin++;;
            }
            if(!(laby->grille[i][j] & OUEST)){
                tableau[nb_voisin][noeud] = noeud - 1;
                nb_voisin++;   
            }

            tableau[1][noeud] = nb_voisin - 2;
        }   
    }
}


int max_distance(labyrinthe_t * laby)
{
    int max = 0;
    for(int i = 0; i < LAB_N*LAB_P; i++) if(laby->etat_noeuds.distance[i] > max && laby->etat_noeuds.distance[i] != DISTANCE_INIT) max = laby->etat_noeuds.distance[i];
    

    return max;
}


void init_etat_noeud(labyrinthe_t * laby)
{
    srand(2);
    //Réinitialisation de la structure
    for(int j = 0; j < LAB_N * LAB_P; j++) {
        laby->etat_noeuds.etat_visite[j] = NON_VISITE;
        laby->terrain[j] = type_de_terrain[rand () % NB_TERRAIN];
    }
}


/**
 * bfs. 
 * @param liste_arete : La liste des arêtes du graphe. 
 * @param root : Le sommet sur lequel on applique le bfs. 
 * @return la structure contenant tous les noeuds accessible depuis l'origine et leur distance.
 */
void bfs(labyrinthe_t * laby, int root)
{
    //Réinitialisation de la structure
    for(int j = 0; j < LAB_N * LAB_P; j++) laby->etat_noeuds.etat_visite[j] = NON_VISITE;
    file_t * f = file_creer();

    int tab_voisins[6][LAB_N*LAB_P];
    int couleur;
    int current; 
    int i;

    genere_tableau_voisin(laby, tab_voisins);


    file_enfiler(f, root);
    laby->etat_noeuds.distance[root] = 0;
    laby->etat_noeuds.etat_visite[root] = EN_COURS_VISITE;
    while(!file_vide(f)) {  
        current = file_tete(f);
        for(i = 0; i < tab_voisins[1][current]; i++) {
            couleur = laby->etat_noeuds.etat_visite[tab_voisins[i + 2][current]];
            if(couleur == NON_VISITE) {
                laby->etat_noeuds.etat_visite[tab_voisins[i + 2][current]] = EN_COURS_VISITE; //Couleur de v
                laby->etat_noeuds.distance[tab_voisins[i + 2][current]] = laby->etat_noeuds.distance[current] + 1; //Distance de v
                file_enfiler(f, tab_voisins[i + 2][current]);
            }
        }
        file_defiler(f);
        laby->etat_noeuds.etat_visite[current] = EST_VISITE;

    }

    file_liberer(&f);
}



/**
 * dijkstra_initialisation
 */
void dijkstra_initialisation(int root, labyrinthe_t * laby)
{
    for(int i = 0; i < LAB_N * LAB_P; i++){
        laby->etat_noeuds.distance[i] = DISTANCE_INIT;
    }
    laby->etat_noeuds.distance[root] = 0;
}


/**
 * dijkstra
 */
void dijkstra(labyrinthe_t * laby, int root)
{
    int i, v, poids_arete, new_dist;
    int tab_voisins[6][LAB_N*LAB_P];
    noeud_t u, tmp;
    genere_tableau_voisin(laby, tab_voisins);
    dijkstra_initialisation(root, laby);
    tas_binaire_t * f = tas_creer();
    
    for(i=0; i<LAB_N * LAB_P;i++){
        tmp.num_noeud = i;
        tmp.distance = laby->etat_noeuds.distance[i];
        tmp.priorite = laby->etat_noeuds.distance[i];
        tas_inserer(f, tmp);
    }

    while(!tas_vide(f)){
        u = tas_supprimer(f);
        for(i=0; i<tab_voisins[1][u.num_noeud]; i++){
            v = tab_voisins[2+i][u.num_noeud];
            poids_arete = (laby->terrain[v] + laby->terrain[u.num_noeud]);
            new_dist = laby->etat_noeuds.distance[u.num_noeud] + poids_arete;
            if(laby->etat_noeuds.distance[v] > new_dist){
                laby->etat_noeuds.distance[v] = new_dist;
                tas_mise_a_jour(f,v,new_dist,0);
            }
        }
    }
    tas_liberer(&f);
}


float distance_manhattan(int root1, int root2)
{
    int i_root1 = root1 / LAB_N;
    int j_root1 = root1 % LAB_N;
    int i_root2 = root2 / LAB_N;
    int j_root2 = root2 % LAB_N;

    return (abs(i_root2 - i_root1) + abs(j_root2 - j_root1));
}


float distance_euclidienne(int root1, int root2)
{
    int i_root1 = root1 / LAB_N;
    int j_root1 = root1 % LAB_N;
    int i_root2 = root2 / LAB_N;
    int j_root2 = root2 % LAB_N;

    return sqrtf(pow((i_root2 - i_root1), 2) + pow((j_root2 - j_root1),2));  
}


float distance_de_tchebychev(int root1, int root2)
{
    int i_root1 = root1 / LAB_N;
    int j_root1 = root1 % LAB_N;
    int i_root2 = root2 / LAB_N;
    int j_root2 = root2 % LAB_N;

    return (abs(i_root2 - i_root1) > abs(j_root2 - j_root1) ? abs(i_root2 - i_root1) : abs(j_root2 - j_root1));
}



void a_star_init(int root, labyrinthe_t * laby)
{
    for(int i = 0; i < LAB_N * LAB_P; i++){
        laby->etat_noeuds.distance[i] = DISTANCE_INIT;
    }
    laby->etat_noeuds.distance[root] = 0;
}

/**
 * a_star
 */
void a_star(labyrinthe_t * laby, int dest, int root, float (*heuristique)(int, int), int * nb_noeud_visite){
    *nb_noeud_visite = 0;
    int i, poids, v, new_dist;
    int tab_voisins[6][LAB_N*LAB_P];
    noeud_t u, tmp;
    genere_tableau_voisin(laby, tab_voisins);
    a_star_init(root, laby);
    tas_binaire_t * f = tas_creer();
    
    for(i=0; i<LAB_N * LAB_P;i++){
        tmp.num_noeud = i;
        tmp.distance = laby->etat_noeuds.distance[i];
        tmp.priorite = tmp.distance + heuristique(tmp.num_noeud, dest);
        tas_inserer(f, tmp);
    }
    int trouve = 0;
    while (!tas_vide(f) && !trouve) {
        u = tas_supprimer(f);
        (*nb_noeud_visite)++;
        if (u.num_noeud == dest) {
            trouve = 1;
        }

        for (i = 0; i < tab_voisins[1][u.num_noeud] && !trouve; i++) {
            v = tab_voisins[2 + i][u.num_noeud];

            poids = laby->terrain[u.num_noeud] + laby->terrain[v];
            new_dist = laby->etat_noeuds.distance[u.num_noeud] + poids;

            if (new_dist < laby->etat_noeuds.distance[v]) {
                laby->etat_noeuds.distance[v] = new_dist;
                tas_mise_a_jour(f, v, new_dist, new_dist + heuristique(v, dest));
            }
        }
    }

    tas_liberer(&f);
 }


/**
 * rempli_file_du_chemin_a_parcourir 
 * @param position_de_parcour_x : une file a remplir qui permet de stoquer la coordonnée x du prochain voisin a parcourir
 * @param position_de_parcour_y : une file a remplir qui permet de stoquer la coordonnée y du prochain voisin a parcourir
 * @param x_initial : la coordonnée x du sommet initial. 
 * @param y_initial : la coordonnée y du sommet initial. 
 * @param x_final : la coordonnée x du sommet final. 
 * @param y_final : la coordonnée y du sommet final. 
 * cette fonction remplie des deux files.
 */
void rempli_file_du_chemin_a_parcourir(labyrinthe_t * laby, trajet * trajet, int x_initial, int y_initial, int x_final, int y_final)
{   

    bfs(laby, x_final+LAB_P*y_final); //mettre ici l'algo qui calcule la distance entre un point et les autres representer dans un tableau
    int continu=1;//juste pour arreter la boucle
    int noeud = x_initial+ LAB_P*y_initial;
    int tab_voisins[6][LAB_N*LAB_P];
    genere_tableau_voisin(laby, tab_voisins);

    while (laby->etat_noeuds.distance[noeud])
    {
        int i = 0;
        continu = 1;
        while (i<tab_voisins[1][noeud] && continu)
        {  
            if ((laby->etat_noeuds.distance[tab_voisins[i+2][noeud]]<= laby->etat_noeuds.distance[noeud]))
            {
                trajet->trajet_x[trajet->nb_element]=tab_voisins[i+2][noeud]%LAB_P;
                trajet->trajet_y[trajet->nb_element]=abs(tab_voisins[i+2][noeud]/LAB_P);
                trajet->nb_element++;

                noeud =tab_voisins[i+2][noeud];
                continu = 0;
            }
            i++;
        }
    }
}

void genere_position_aleatoire_de_debut_et_de_fin(int* x_final ,int * y_final)
{
    *x_final=rand()%LAB_N;
    *y_final=rand()%LAB_P;
}

labyrinthe_t * destination_connue_initialisation(int dest)
{
    int j;
    int i;
    int nb_murs;
    labyrinthe_t * laby_non_connu = (labyrinthe_t *)malloc(sizeof(labyrinthe_t));
    if(!laby_non_connu) return NULL;

    for(j = 0; j < LAB_N * LAB_P; j++) {
        laby_non_connu->etat_noeuds.etat_visite[j] = NON_VISITE;
        laby_non_connu->terrain[j] = NORMAL;
    }

    for(i = 0; i < LAB_P; i++) {
        for(j = 0; j < LAB_N; j++) {
            nb_murs = 0;
            if(!i) nb_murs += NORD;
            if(i == (LAB_P - 1)) nb_murs += SUD;
            if(!j) nb_murs += OUEST;
            if(j == (LAB_N - 1)) nb_murs += EST;
            laby_non_connu->grille[i][j] = nb_murs;
        }
    }

    bfs(laby_non_connu, dest);

    return laby_non_connu;  
}

int renvoie_noeud_prometteur(labyrinthe_t * laby, int noeud, int i, int j)
{
    int min = noeud;
    if(i > 0) { 
        if((laby->etat_noeuds.distance[noeud - LAB_N] < laby->etat_noeuds.distance[min]) && !(laby->grille[i][j] & NORD)) min = noeud - LAB_N;
    } 
    if(i != (LAB_P - 1)) {
        if((laby->etat_noeuds.distance[noeud + LAB_N] < laby->etat_noeuds.distance[min]) && !(laby->grille[i][j] & SUD)) min = noeud + LAB_N;
    }
    if(j) {
        if((laby->etat_noeuds.distance[noeud - 1] < laby->etat_noeuds.distance[min]) && !(laby->grille[i][j] & OUEST)) min = noeud - 1;
    }

    if(j != (LAB_N - 1)) {
        if((laby->etat_noeuds.distance[noeud + 1] < laby->etat_noeuds.distance[min]) && !(laby->grille[i][j] & EST)) min = noeud + 1;
    }


    return min;
}

void gestion_mur_voisin(labyrinthe_t * laby, int i_noeud_courant, int j_noeud_courant)
{
    int case_courante = laby->grille[i_noeud_courant][j_noeud_courant];
    if(case_courante & NORD && i_noeud_courant != 0){
        if(!(laby->grille[i_noeud_courant-1][j_noeud_courant] & SUD))
            laby->grille[i_noeud_courant-1][j_noeud_courant] += SUD;
    }
    if(case_courante & EST && j_noeud_courant != (LAB_N - 1)){
        if(!(laby->grille[i_noeud_courant][j_noeud_courant+1] & OUEST))
            laby->grille[i_noeud_courant][j_noeud_courant + 1] += OUEST;
    }
    if(case_courante & SUD && i_noeud_courant != (LAB_P - 1)){
        if(!(laby->grille[i_noeud_courant+1][j_noeud_courant] & NORD))
            laby->grille[i_noeud_courant+1][j_noeud_courant] += NORD;
    }
    if(case_courante & OUEST && j_noeud_courant != 0){
        if(!(laby->grille[i_noeud_courant][j_noeud_courant-1] & EST))
            laby->grille[i_noeud_courant][j_noeud_courant-1] += EST;
    }
}


trajet destination_connue(labyrinthe_t *laby, int dest, int depart)
{
    int noeud_courant = depart;
    int a_transite;
    int noeud_prometteur = depart;
    int i_noeud_courant = noeud_courant / LAB_N;
    int j_noeud_courant = noeud_courant % LAB_N;
    int nb_noeuds_visites = 0;

    trajet t = {0};

    t.nb_element = 0;
    int x = 0;
    int y = 0;

    labyrinthe_t * lab_courant = destination_connue_initialisation(dest);

    while(noeud_courant != dest) {
        a_transite = 0;
        noeud_courant = noeud_prometteur;
        i_noeud_courant = noeud_courant / LAB_N;
        j_noeud_courant = noeud_courant % LAB_N;   

        t.trajet_x[x] = i_noeud_courant;
        t.trajet_y[y] = j_noeud_courant;
        x++;
        y++;
        t.nb_element++;
        while(!a_transite) {
            lab_courant->grille[i_noeud_courant][j_noeud_courant] = laby->grille[i_noeud_courant][j_noeud_courant];
            lab_courant->terrain[noeud_courant] = laby->terrain[noeud_courant];
            gestion_mur_voisin(lab_courant, i_noeud_courant, j_noeud_courant);
            a_star(lab_courant, noeud_courant, dest,  distance_manhattan, &nb_noeuds_visites);

            noeud_prometteur = renvoie_noeud_prometteur(lab_courant, noeud_courant, i_noeud_courant, j_noeud_courant);
            if((laby->terrain[noeud_courant] + laby->terrain[noeud_prometteur]) == (lab_courant->terrain[noeud_courant] + lab_courant->terrain[noeud_prometteur])) {
                a_transite = 1;
            }
            else {
                lab_courant->terrain[noeud_prometteur] = laby->terrain[noeud_prometteur];
            }
        }
    }
    free(lab_courant);
    return t;
}

void trouve_dist_min(labyrinthe_t * G, abr * O, int *min, int *noeud_min, abr * F) {
    if (!O) {
        return;
    }

    if (G->etat_noeuds.distance[O->val] < *min && !abr_recherche(F, O->val)) {
        *min = G->etat_noeuds.distance[O->val];
        *noeud_min = O->val;
    }

    trouve_dist_min(G, O->gauche, min, noeud_min, F);
    trouve_dist_min(G, O->droite, min, noeud_min, F);
}


int trouve_prochaine_case(labyrinthe_t * laby, int noeud){
    int i,j, laby_val, voisin_min = -1;
    i = noeud/LAB_N;    
    j = noeud%LAB_N;
    laby_val = laby->grille[i][j];
    if(!(laby_val & NORD)){
        if(voisin_min == -1 || laby->etat_noeuds.distance[noeud - LAB_N] < laby->etat_noeuds.distance[voisin_min]) voisin_min = noeud - LAB_N;
    }
    if(!(laby_val & EST)){
        if(voisin_min == -1 || laby->etat_noeuds.distance[noeud + 1] < laby->etat_noeuds.distance[voisin_min]) voisin_min = noeud + 1;
    }
    if(!(laby_val & SUD)){
        if(voisin_min == -1 || laby->etat_noeuds.distance[noeud + LAB_N] < laby->etat_noeuds.distance[voisin_min]) voisin_min = noeud + LAB_N;
    }
    if(!(laby_val & OUEST)){
        if(voisin_min == -1 || laby->etat_noeuds.distance[noeud - 1] < laby->etat_noeuds.distance[voisin_min]) voisin_min = noeud - 1;
    }
    return voisin_min;
}


/**
 * graphe_inconnu_dest_inconnue
 * @param laby Le labyrinthe dans son intégralité
 * @param source Le noeud de départ
 * @param dest La destination
 * @param trace_labyrinthe booléen qui indique si on va tracer ou non sur le renderer le labyrinthe
 * Trouve une destination
 */
trajet graphe_inconnu_dest_inconnue(labyrinthe_t * laby, int source, int dest, int trace_labyrinthe)
{
    int noeud, suivant;
    int i,j, fils;
    int val_case = -1;
    int tab_voisins[4]; // 0 : nord, 1 : est, 2 : sud, 3 : ouest
    for(int k = 0; k < 4; k++){
        tab_voisins[k] = -1;
    }
    /**
     * INITIALISATION
     */
    trajet t = {0};

    t.nb_element = 0;

     labyrinthe_t G;
     for(int i = 0; i < LAB_P; i++){
        for(int j = 0; j < LAB_N; j++){
            G.grille[i][j] = -1;
            int num = i*LAB_N + j;
            G.terrain[num] = 0;
            G.etat_noeuds.distance[num] = DISTANCE_INIT;
            G.etat_noeuds.nb_visite[num] = 0;
            laby->etat_noeuds.nb_visite[num] = 0;
        }
     }
     abr * F = NULL;
     abr * O = NULL;
     noeud = source;


     /**
      * ITERATION
      */

     while(noeud!=dest){
        t.trajet_x[t.nb_element] = noeud/LAB_N;
        t.trajet_y[t.nb_element] = noeud%LAB_N;
        G.etat_noeuds.nb_visite[noeud]++;
        laby->etat_noeuds.nb_visite[noeud]++;
        t.nb_element++;
        F = abr_insertion(F, noeud);
        suivant = -1;
        i = noeud/LAB_N;
        j = noeud%LAB_N;
        G.grille[i][j] = laby->grille[i][j];
        G.terrain[noeud] = laby->terrain[noeud];
        val_case = laby->grille[i][j];
        // Détermination des voisins
        tab_voisins[0] = (!(val_case & NORD))  ? noeud - LAB_N : -1;
        tab_voisins[1] = (!(val_case & EST))   ? noeud + 1     : -1;
        tab_voisins[2] = (!(val_case & SUD))   ? noeud + LAB_N : -1;
        tab_voisins[3] = (!(val_case & OUEST)) ? noeud - 1     : -1;

        for(int k = 0; k < 4; k++){
            if(tab_voisins[k] != -1 && !abr_recherche(F,tab_voisins[k])){
                if(!abr_recherche(O,tab_voisins[k])){
                    O = abr_insertion(O, tab_voisins[k]);
                }
                if(suivant == -1){
                    suivant = tab_voisins[k];
                    noeud = suivant;
                }
            }
        }
        if(suivant == -1){
            dijkstra(&G, noeud);
            int min = DISTANCE_INIT;
            int noeud_min = -1;
            trouve_dist_min(&G, O, &min, &noeud_min, F);
            fils = noeud_min;
            // Se déplacer en prenant le plus court chemin A* dest : noeud source : fils
            int poubelle;

            G.grille[fils/LAB_N][fils%LAB_N] = laby->grille[fils/LAB_N][fils%LAB_N];

            a_star(&G, noeud, fils, distance_manhattan, &poubelle);

            while(noeud != fils){
                noeud = trouve_prochaine_case(&G, noeud);
                if(noeud!=fils){
                    t.trajet_x[t.nb_element] = noeud/LAB_N;
                    t.trajet_y[t.nb_element] = noeud%LAB_N;
                    t.nb_element++;
                    G.etat_noeuds.nb_visite[noeud]++;
                    laby->etat_noeuds.nb_visite[noeud]++;
                }
            }
            suivant = fils;
        }
        noeud = suivant;
    }
    t.trajet_x[t.nb_element] = noeud/LAB_N;
    t.trajet_y[t.nb_element] = noeud%LAB_N;
    t.nb_element++;
    laby->etat_noeuds.nb_visite[noeud]++;
    G.etat_noeuds.nb_visite[noeud]++;
    if(trace_labyrinthe)
        affiche_labyrinthe_dessin_passage_par_cases(&G);
    abr_liberer(O);
    abr_liberer(F);
    return t;
}

/**
 * Affiche un labyrinthe de façon textuelle (en affichant les murs) avec les distances
 * @param laby le labyrinthe à afficher
 * @param affiche_distance : booléen indiquant si on désire afficher les distances pour les noeuds de notre labyrinthe
 * @param dest : dans le cas d'A*, destination de l'algo, si pas A* alors dest = -1
 */
void afficher_labyrinthe_distance(labyrinthe_t * laby, int affiche_distance, int dest){
    int i, j, noeud, distance;
    char * bleu_clair = "\e[34m";
    char * reset = "\e[0m";

    for(i = 0; i < LAB_P; i++){             
        for(j = 0; j < LAB_N; j++){          
            printf("%s*%s", bleu_clair, reset);
            if(laby->grille[i][j] & NORD){
                printf("%s---%s", bleu_clair, reset);
            }
            else{
                printf("   ");
            }
        }
        printf("%s*%s\n", bleu_clair, reset);

        for(j = 0; j < LAB_N; j++){
            noeud = i * LAB_N + j;

            if(laby->grille[i][j] & OUEST){
                printf("%s|%s", bleu_clair, reset);
            }
            else{
                printf(" ");
            }

            if(affiche_distance){
                distance = laby->etat_noeuds.distance[noeud];
                if(distance != DISTANCE_INIT){
                    if(noeud == dest){
                        printf("\e[43m");
                    }
                    if(distance > 999){
                        printf("###");
                    }
                    else{
                        printf("%3d", distance);
                    }
                    if(noeud == dest){
                        printf("\e[0m");
                    }
                } else {
                    printf("  .");
                }
            } else {
                printf("  .");
            }
        }

        if(laby->grille[i][LAB_N - 1] & EST)
            printf("%s|%s", bleu_clair, reset);
        else
            printf(" ");
        printf("\n");
    }

    for(j = 0; j < LAB_N; j++){
        printf("%s*%s", bleu_clair, reset);
        if(laby->grille[LAB_P - 1][j] & SUD)
            printf("%s---%s", bleu_clair, reset);
        else
            printf("   ");
    }
    printf("%s*%s\n", bleu_clair, reset);
}


/**
 * Affiche un labyrinthe de façon textuelle (pour graphe_inconnue_dest_inconnue)
 * @param laby le labyrinthe à afficher
 * @param source : source de l'algo
 * @param dest : la destination (inconnue) de l'algo
 */
void afficher_labyrinthe_dest_inco(labyrinthe_t * laby, int source, int dest){
    int i, j, noeud;
    char * bleu_clair = "\e[34m";
    char * reset = "\e[0m";

    for(i = 0; i < LAB_P; i++){             
        for(j = 0; j < LAB_N; j++){          
            printf("%s*%s", bleu_clair, reset);
            if(laby->grille[i][j] & NORD){
                printf("%s---%s", bleu_clair, reset);
            }
            else{
                printf("   ");
            }
        }
        printf("%s*%s\n", bleu_clair, reset);

        for(j = 0; j < LAB_N; j++){
            noeud = i * LAB_N + j;

            if(laby->grille[i][j] & OUEST){
                printf("%s|%s", bleu_clair, reset);
            }
            else{
                printf(" ");
            }

            if(noeud == dest){
                printf("\e[43m");
            }
            if(noeud == source){
                printf("\e[42m");
            }
            if(laby->etat_noeuds.nb_visite[noeud] !=0){
                printf("%3d", laby->etat_noeuds.nb_visite[noeud]);
            }else{
                printf("  .");
            }
            if(noeud == dest || noeud == source){
                printf("\e[0m");
            }
        }

        if(laby->grille[i][LAB_N - 1] & EST)
            printf("%s|%s", bleu_clair, reset);
        else
            printf(" ");
        printf("\n");
    }

    for(j = 0; j < LAB_N; j++){
        printf("%s*%s", bleu_clair, reset);
        if(laby->grille[LAB_P - 1][j] & SUD)
            printf("%s---%s", bleu_clair, reset);
        else
            printf("   ");
    }
    printf("%s*%s\n", bleu_clair, reset);
}

/**
 * Affiche un labyrinthe de façon textuelle (pour destination_connue)
 * @param laby le labyrinthe à afficher
 * @param source : source de l'algo
 * @param dest : destination visée par l'algo
 * @param traj : le trajet fait par l'algo
 */
void afficher_labyrinthe_dest_connue(labyrinthe_t * laby, int source, int dest, trajet traj)
{
    int i, j,k, noeud, courant;
    char * bleu_clair = "\e[34m";
    char * reset = "\e[0m";

    for(k = 0; k < traj.nb_element; k++){    
        courant = traj.trajet_x[k] * LAB_N + traj.trajet_y[k];
        for(i = 0; i < LAB_P; i++){             
            for(j = 0; j < LAB_N; j++){          
                printf("%s*%s", bleu_clair, reset);
                if(laby->grille[i][j] & NORD){
                    printf("%s---%s", bleu_clair, reset);
                }
                else{
                    printf("   ");
                }
            }
            printf("%s*%s\n", bleu_clair, reset);

            for(j = 0; j < LAB_N; j++){
                noeud = i * LAB_N + j;

                if(laby->grille[i][j] & OUEST){
                    printf("%s|%s", bleu_clair, reset);
                }
                else{
                    printf(" ");
                }

                if(noeud == dest){
                    printf("\e[43m");
                }
                if(noeud == source){
                    printf("\e[42m");
                }
                if(noeud == courant){
                    printf("\e[45m");
                }
                printf("  .");

                if(noeud == dest || noeud == source || noeud == courant){
                    printf("\e[0m");
                }
            }

            if(laby->grille[i][LAB_N - 1] & EST)
                printf("%s|%s", bleu_clair, reset);
            else
                printf(" ");
            printf("\n");
        }

        for(j = 0; j < LAB_N; j++){
            printf("%s*%s", bleu_clair, reset);
            if(laby->grille[LAB_P - 1][j] & SUD)
                printf("%s---%s", bleu_clair, reset);
            else
                printf("   ");
        }
        printf("%s*%s\n", bleu_clair, reset);
    }
}