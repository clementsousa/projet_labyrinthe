#include <stdio.h>
#include <stdlib.h>


#include "kruskal.h"
#include "coeur.h"
#include "code_erreur.h"
#include "labyrinthe.h"
#include "jeu.h"
#include "parcours_graphe.h"


/**
 * Génère une position aléatoire dans le labyrinthe
 * @return un noeud aléatoire du labyrinthe
 */
int genere_position(){
    int noeud;
    noeud = rand()%(LAB_N * LAB_P);
    return noeud;
    
}

/**
 * Initialise le joueur en fonction des attributs spécifiés
 * @param donjon : le donjon (contient le joueur)
 * @param attaque : l'attaque voulue pour le joueur
 * @param vie : la vie voulue pour le joueur
 */
void genere_joueur(donjon_t * donjon, int attaque, int vie, int vitesse){
    donjon->joueur.attaque = attaque;
    donjon->joueur.derniere_direction = -1;
    donjon->joueur.piece = 0;
    donjon->joueur.vie = vie;
    donjon->joueur.vitesse = vitesse;
    donjon->joueur.position = genere_position();
}

/**
 * Change l'attaque et la vie du joueur
 * @param donjon : le donjon
 * @param attaque : l'attaque souhaitée
 * @param vie : la vie souhaitée
 */
void reinit_joueur(donjon_t * donjon, int attaque, int vie){
    donjon->joueur.attaque = attaque;
    donjon->joueur.vie = vie;
}

/**
 * Initialise un monstre
 * @param donjon : le donjon où le monstre vivra
 * @param attaque : son attaque voulue
 * @param vie : sa vie voulue
 * @param est_savant : VRAI si désigne le type B de monstre (SAVANT), FAUX SINON (FOUINEUR)
 */
void genere_monstre(donjon_t * donjon, int attaque, int vie, int vitesse, int est_savant){
    donjon->tab_monstres[donjon->nb_monstre].attaque = attaque;
    donjon->tab_monstres[donjon->nb_monstre].est_savant = est_savant;
    donjon->tab_monstres[donjon->nb_monstre].piece = 0;
    donjon->tab_monstres[donjon->nb_monstre].vie = vie;
    donjon->tab_monstres[donjon->nb_monstre].vitesse = vitesse;

    int position = genere_position(), iteration = 0;
    while (distance_manhattan(donjon->joueur.position, position) <= 1 && iteration < LAB_N* LAB_P){ // Sécurité boucle infini + tant que la distance de Manhattan n'est pas >= 2
        position = (position+1)%(LAB_N*LAB_P);
        iteration++;
    }
    
    if(iteration >= LAB_N * LAB_P){
        fprintf(stderr, "Erreur lors de la génération d'un monstre : impossible de trouver une distance convenable");
        exit(CODE_ERR_GENERATION_MONSTRE);
    }
    donjon->tab_monstres[donjon->nb_monstre].position = position;
    donjon->tab_monstres[donjon->nb_monstre].id_monstre = donjon->nb_monstre;
    donjon->tab_monstres[donjon->nb_monstre].etape_traj = 0;
    donjon->tab_monstres[donjon->nb_monstre].derniere_direction = DIRECTION_SUD;
    donjon->tab_monstres[donjon->nb_monstre].direction_actuelle = DIRECTION_SUD;

    donjon->nb_monstre++;
    donjon->tab_monstres[donjon->nb_monstre].traj.nb_element = 0;
    for(int i = 0; i < TAILLE_TRAJET; i++){
        donjon->tab_monstres[donjon->nb_monstre].traj.trajet_x[i] = 0;
        donjon->tab_monstres[donjon->nb_monstre].traj.trajet_y[i] = 0;

    }
}

/**
 * Génère le trajet d'un monstre jusqu'à la pièce
 */
void genere_trajet_monstre(donjon_t * donjon, int id_monstre)
{   
    if(id_monstre < 0 || id_monstre >= donjon->nb_monstre){
        fprintf(stderr, "Erreur id_monstre invalide dans genere_trajet_monstre");
        exit(CODE_ERR_GEN_TRAJ_ID_INV);
    }
    if(donjon->tab_monstres[id_monstre].vie <= 0){
        fprintf(stderr, "Erreur monstre invalide (mort) dans genere_trajet_monstre");
        exit(CODE_ERR_GEN_TRAJ_M_MORT);
    }
    if(donjon->tab_monstres[id_monstre].est_savant){    // Le monstre connait la destination
        donjon->tab_monstres[id_monstre].traj = destination_connue(donjon->laby, trouver_piece(donjon), donjon->tab_monstres[id_monstre].position); 
        donjon->tab_monstres[id_monstre].etape_traj = 0;

    }else{                                              // Le monstre ne connait pas la destination
        donjon->tab_monstres[id_monstre].traj = graphe_inconnu_dest_inconnue(donjon->laby, donjon->tab_monstres[id_monstre].position, trouver_piece(donjon), 0 );
        donjon->tab_monstres[id_monstre].etape_traj = 0;
    }


    // Set de direction_actuelle : 
    if(donjon->tab_monstres[id_monstre].traj.nb_element > 1){
        int next_dest = donjon->tab_monstres[id_monstre].traj.trajet_x[1] * LAB_N + donjon->tab_monstres[id_monstre].traj.trajet_y[1];
        int pos_actu = donjon->tab_monstres[id_monstre].traj.trajet_x[0] * LAB_N + donjon->tab_monstres[id_monstre].traj.trajet_y[0];
        if(next_dest == pos_actu - 1){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_OUEST;
        }
        if(next_dest == pos_actu + 1){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_EST;
        }
        if(next_dest == pos_actu - LAB_N){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_NORD;
        }
        if(next_dest == pos_actu + LAB_N){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_SUD;
        }
    }else{
        donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_SUD;

    }
}

/**
 * Génère le trajet de tout les monstres jusqu'à la pièce
 */
void genere_tout_trajet_monstre(donjon_t * donjon)
{
    for(int i = 0; i < donjon->nb_monstre; i++){
        if(donjon->tab_monstres[i].vie > 0){
            genere_trajet_monstre(donjon, i);
        }
    }
}
/**
 * Renvoie un booléen indicant si la position est valide
 * @param donjon : le donjon voulu
 * @param position : la position dont il faut valider la conformité
 * @param distance_min : La distance (de Manhattan) minimale (exclu) à laquelle la pièce doit être généré de tout monstres ou joueur.
 * @param est_pour_piece : décrit si on recherche une position valide pour une pièce ou non
 * @return Vrai si la position donné en paramètre est bien éloigné de tout monstre et joueur d'une certaine distance.
 */
int position_valide(donjon_t * donjon, int position, int distance_min, int est_pour_piece)
{
    if(distance_manhattan(position, donjon->joueur.position) < distance_min){
        return 0;
    }
    if(!est_pour_piece && distance_manhattan(position, trouver_piece(donjon)) < distance_min){
        return 0;
    }
    for(int i = 0; i < donjon->nb_monstre; i++){
        if(distance_manhattan(donjon->tab_monstres[i].position, position) < distance_min){
            return 0;
        }
    }
    return 1;
}

/**
 * Génère une pièce d'or dans le labyrinthe
 * @param donjon : le donjon dans lequel on insère la pièce
 */
void genere_piece(donjon_t * donjon)
{
    int position = genere_position(),iteration = 0;
    while (!position_valide(donjon, position, 1, 1) && iteration < LAB_N* LAB_P){
        position = (position+1)%(LAB_N*LAB_P);
        iteration++;
    }
    if(iteration >= LAB_N*LAB_P){
        fprintf(stderr, "Erreur lors de la génération d'une pièce : impossible de trouver une distance convenable");
        exit(CODE_ERR_GENERATION_PIECE);
    }
    donjon->presence_entite[position/LAB_N][position%LAB_N] += PIECE;
    donjon->nb_or_restant_a_generer--;
    donjon->position_or = position;
}

/**
 * Applique une itération sur le trajet d'un monstre du labyrinthe
 * @param donjon le donjon
 * @param id_monstre l'id du monstre que l'on veut update
 */
void update_trajet_monstre(donjon_t * donjon, int id_monstre)
{  
    int noeud, i, j;
    int case_a_se_rendre = donjon->tab_monstres[id_monstre].etape_traj + 1;
    if(donjon->tab_monstres[id_monstre].vie > 0){
        if(case_a_se_rendre < donjon->tab_monstres[id_monstre].traj.nb_element){
            i = donjon->tab_monstres[id_monstre].traj.trajet_x[case_a_se_rendre];
            j = donjon->tab_monstres[id_monstre].traj.trajet_y[case_a_se_rendre]; 
            noeud = i * LAB_N + j;
            deplacement_monstre_grille(donjon, id_monstre, noeud);
            donjon->tab_monstres[id_monstre].etape_traj++;
        }
    }
}

/**
 * Applique une update du trajet sur tout les monstres
 */
void update_tout_trajet_monstre(donjon_t * donjon)
{
    for(int i = 0; i < donjon->nb_monstre; i++){
        update_trajet_monstre(donjon, i);
    }
}

/**
 * Génère le donjon avec les monstres, joueurs, la première pièce, le labyrinthe...
 * @param difficulte : la difficulté du jeu
 * @return : le donjon avec tout les paramètres initialisés SAUF LES TRAJETS DES MONSTRES
 */
donjon_t * generation_donjon(difficulte_enum difficulte)
{
    int liste_arete[NB_MAX_ARETE][2] = {{0}};
    
    float difficulte_laby;
    int nb_monstres_fouineur, nb_monstres_savant, nb_or_pour_gagner;
    
    int atk_fouineur_supp = 0, vie_fouineur_supp = 0; // attaque et vie supp suivant la difficulté
    int atk_savant_supp = 0, vie_savant_supp = 0; // attaque et vie supp suivant la difficulté
    
    int position, x, y;

    donjon_t * donjon = (donjon_t *) malloc(sizeof(donjon_t));
    if(!donjon){
        fprintf(stderr, "Erreur lors de la génération du donjon.");
        exit(CODE_ERR_GENERATION_DONJON);
    }
    donjon->laby = (labyrinthe_t *) malloc(sizeof(labyrinthe_t));
    if(!donjon->laby){
        fprintf(stderr, "Erreur lors de l'allocation du labyrinthe.");
        exit(CODE_ERR_GENERATION_DONJON);
    }
    donjon->nb_monstre = 0;
    donjon->position_or = 0;
    // Gestion de la difficulté : 
    switch (difficulte){
        case FACILE:
            difficulte_laby = 0.8;
            nb_monstres_fouineur = 2;
            nb_monstres_savant = 1;
            nb_or_pour_gagner = 5;
            break;
        case MOYEN:
            difficulte_laby = 0.5;
            nb_monstres_fouineur = 4;
            nb_monstres_savant = 1;
            nb_or_pour_gagner = 10;
            break;
        case DIFFICILE:
            difficulte_laby = 0.3;
            nb_monstres_fouineur = 6;
            nb_monstres_savant = 2;
            nb_or_pour_gagner = 5;
            atk_fouineur_supp = ATK_MONSTRE_FOUINEUR/2;
            vie_fouineur_supp = VIE_MONSTRE_FOUINEUR/2;
            atk_savant_supp = ATK_MONSTRE_SAVANT/2;
            vie_savant_supp = VIE_MONSTRE_SAVANT/2;
            break;
        default:
            fprintf(stderr, "Erreur difficulté invalide");
            exit(CODE_ERR_DIFFICULTE_INVALIDE);
            break;
    }
    // Gestion de l'or
    donjon->nb_or_pour_gagner = nb_or_pour_gagner;
    donjon->nb_or_restant_a_generer = nb_or_pour_gagner*2 - 1;
    // Génération du labyrinthe
    generer_labyrinthe(liste_arete,LAB_N, LAB_P, difficulte_laby);
    init_grille(donjon->laby, liste_arete);
    for(int i = 0; i < LAB_P; i++){
        for(int j = 0; j < LAB_N; j++){
            donjon->presence_entite[i][j] = 0;
        }
    }
    // Génération du joueur
    genere_joueur(donjon, ATK_JOUEUR, VIE_JOUEUR, SPD_JOUEUR);
    

    // Génération des monstres fouineurs
    for (int i = 0; i < nb_monstres_fouineur; i++){
        genere_monstre(donjon, ATK_MONSTRE_FOUINEUR + atk_fouineur_supp, VIE_MONSTRE_FOUINEUR + vie_fouineur_supp, SPD_MONSTRE_FOUINEUR, 0);
    }

    // Génération des monstres savants
    for (int i = 0; i < nb_monstres_savant; i++){
        genere_monstre(donjon, ATK_MONSTRE_SAVANT + atk_savant_supp, VIE_MONSTRE_SAVANT + vie_savant_supp, SPD_MONSTRE_SAVANT, 1);
    }
    
    /*
     Génération de la position de chaque entité -- les bonus/malus sont gérés indépendamment; au départ il n'y en a pas.
    */
    // Position pièce
    genere_piece(donjon);

    // Position joueur
    position = donjon->joueur.position;
    donjon->presence_entite[position/LAB_N][position%LAB_P] += JOUEUR;

    // Position monstres
    for(int i = 0; i < donjon->nb_monstre; i++){
        monstre_t monstre = donjon->tab_monstres[i];
        position = monstre.position;
        x = position / LAB_N;
        y = position % LAB_N;
        if(monstre.est_savant){
            if(donjon->presence_entite[x][y] & MONSTRE_SAVANT){
                // Traitement "particulier" du fait qu'un monstre est déjà présent
            }else{
                donjon->presence_entite[x][y] += MONSTRE_SAVANT;
            }
        }else{
            if(donjon->presence_entite[x][y] & MONSTRE_FOUINEUR){
                // Traitement "particulier" du fait qu'un monstre est déjà présent
            }else{
                donjon->presence_entite[x][y] += MONSTRE_FOUINEUR;
            }
        }
        genere_trajet_monstre(donjon, i);
    }
    return donjon;
}

/**
 * Permet d'obtenir le nombre de monstre d'un certain type sur une même case
 * @param donjon : le donjon concerné
 * @param est_savant : Si vrai alors on recherche les SAVANT si faux alors les FOUINEURS
 * @param noeud :Le noeud où l'on veut compter le nombre de monstre 
 */
int get_nb_monstre_case(donjon_t * donjon, int est_savant, int noeud)
{
    int k,acc=0;
    if(est_savant) est_savant = 1;
    for(k = 0; k < donjon->nb_monstre; k++){
        if(donjon->tab_monstres[k].position == noeud && donjon->tab_monstres[k].est_savant == est_savant) acc++;
    }
    return acc;
}

/**
 * Déplace un monstre sur la grille (de manière propre)
 * Cette fonction ne vérifie pas que le déplacement est valide, elle se contente de l'effectuer.
 * @param donjon : le donjon
 * @param id_monstre  : identifiant unique du monstre (position dans la tableau tab_monstres)
 * @param destination : le nouveau noeud où sera présent le monstre. Si il vaut -1 alors on retire le monstre de la grille
 */
void deplacement_monstre_grille(donjon_t * donjon, int id_monstre , int destination){
    monstre_t m =  donjon->tab_monstres[id_monstre];
    entite type_monstre = (m.est_savant) ? MONSTRE_SAVANT : MONSTRE_FOUINEUR;
    int ancienne_pos = m.position;
    // Retrait de la présence du monstre sur sa case si nécessaire

    if(get_nb_monstre_case(donjon, m.est_savant, m.position) == 1){
        donjon->presence_entite[m.position/LAB_N][m.position%LAB_N] -= type_monstre;
    }

    // Ajout de la présence du monstre sur la case de destination si nécessaire

    if(destination != -1 && get_nb_monstre_case(donjon, m.est_savant, destination) == 0){
        donjon->presence_entite[destination/LAB_N][destination%LAB_N] += type_monstre;
    }

    donjon->tab_monstres[id_monstre].position = destination;
    if(destination == ancienne_pos - 1){
        donjon->tab_monstres[id_monstre].derniere_direction = DIRECTION_OUEST;
    }
    if(destination == ancienne_pos + 1){
        donjon->tab_monstres[id_monstre].derniere_direction = DIRECTION_EST;
    }
    if(destination == ancienne_pos - LAB_N){
        donjon->tab_monstres[id_monstre].derniere_direction = DIRECTION_NORD;
    }
    if(destination == ancienne_pos + LAB_N){
        donjon->tab_monstres[id_monstre].derniere_direction = DIRECTION_SUD;
    }
    int etape_traj = donjon->tab_monstres[id_monstre].etape_traj;
    if(etape_traj < donjon->tab_monstres[id_monstre].traj.nb_element - 1){
        int next_dest = donjon->tab_monstres[id_monstre].traj.trajet_x[etape_traj + 2] * LAB_N + donjon->tab_monstres[id_monstre].traj.trajet_y[etape_traj + 2];
        if(next_dest == destination - 1){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_OUEST;
        }
        if(next_dest == destination + 1){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_EST;
        }
        if(next_dest == destination - LAB_N){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_NORD;
        }
        if(next_dest == destination + LAB_N){
            donjon->tab_monstres[id_monstre].direction_actuelle = DIRECTION_SUD;
        }
    }
}

/**
 * Effectue le déplacement "propre" d'un joueur sur la grille
 * @param donjon : le donjon concerné
 * @param destination : la case où le joueur doit se rendre
 */
void deplacement_joueur_grille(donjon_t * donjon, int destination){
    if(destination >= LAB_N * LAB_P) destination = LAB_N * LAB_P - 1;
    if(destination < 0) destination = 0;
    if(destination < 0 || destination >= LAB_N * LAB_P) fprintf(stderr, "Erreur une position invalide a été donné pour déplacer le joueur. Remise en conformité faite.");

    donjon->presence_entite[donjon->joueur.position/LAB_N][donjon->joueur.position%LAB_N] -= JOUEUR;
    donjon->presence_entite[destination/LAB_N][destination%LAB_N] += JOUEUR;

    donjon->joueur.position = destination;
}


/**
 * Donne le résultat d'un combat entre un joueur et un monstre en fonction des PVs et de l'attaque des bélligérents
 * Met la position à -1 pour un monstre mort, à un endroit aléatoire pour le joueur
 * @param donjon : le donjon concerné
 * @param joueur : le joueur
 * @param monstre : le monstre
 * @return L'issue du combat (VICTOIRE_JOUEUR, DEFAITE_JOUEUR, EGALITE)
 */
resultat_combat combat(donjon_t * donjon, joueur_t * joueur, monstre_t * monstre)
{
    int iteration = 0;
    int nv_position = genere_position();
    if(!joueur || !monstre){
        fprintf(stderr, "Erreur : joueur ou monstre NULL dans combat.\n");
        exit(CODE_ERR_COMBAT_INATTENDU);
    }
    while(joueur->vie > 0 && monstre->vie > 0){ // Tant qu'aucun des deux belligerents est mort : le combat continue
        joueur->vie -= monstre->attaque;
        monstre->vie -= joueur->attaque;
    }

    if(joueur->vie <= 0){
        while(!position_valide(donjon, nv_position, 1, 0) && iteration < (LAB_N*LAB_P)){
            nv_position = (nv_position + 1) % (LAB_N * LAB_P);
            iteration++;
        }
        if(iteration >= LAB_N * LAB_P){
            fprintf(stderr, "Erreur dans la génération de la nouvelle position du joueur : aucune case valide trouvée");
            exit(CODE_ERR_POS_INTROUVABLE);
        }
        deplacement_joueur_grille(donjon, nv_position);
    }

    if(joueur->vie <= 0 && monstre->vie <= 0){ // Si les deux sont morts, alors on remet leur vie dans un état cohérent et EGALITE
        joueur->vie = 0;
        monstre->vie = 0;
        deplacement_monstre_grille(donjon, monstre->id_monstre, -1);
        donjon->nb_or_restant_a_generer += monstre->piece;
        monstre->piece = 0;
        reinit_joueur(donjon, ATK_JOUEUR, VIE_JOUEUR);
        return EGALITE;
    }
    if(joueur->vie <= 0){ // Si le joueur est mort alors ...
        joueur->vie = 0;
        reinit_joueur(donjon, ATK_JOUEUR, VIE_JOUEUR);
        return DEFAITE_JOUEUR;
    }
    if(monstre->vie <= 0){ // Si le monstre est mort alors ...
        monstre->vie = 0;
        deplacement_monstre_grille(donjon, monstre->id_monstre, -1);
        joueur->piece += monstre->piece;
        monstre->piece = 0;
        return VICTOIRE_JOUEUR;
    }
    fprintf(stderr, "Erreur : circonstance innatendu lors du combat\n");
    exit(CODE_ERR_COMBAT_INATTENDU);
}

/**
 * Libère le donjon et passe le pointeur à NULL
 * @param donjon : pointeur sur un pointeur sur un donjon
 */
void liberer_donjon(donjon_t ** donjon)
{
    free((*donjon)->laby);
    free(*donjon);
    *donjon = NULL;
}


/**
 * Retourne l'état du jeu : si les conditions de victoire ou de défaite sont réunies ou pas
 * @param donjon : le donjon
 * @return Si la partie est gagnée, perdue, ou en cours.
 */
etat_partie etat_jeu(donjon_t * donjon)
{
    int somme_piece_monstre = 0; 

    if(donjon->joueur.piece >= donjon->nb_or_pour_gagner){  // On a ramassé tout l'or nécessaire
        return PARTIE_GAGNEE;
    }
    for(int i = 0 ; i < donjon->nb_monstre; i++){           
        somme_piece_monstre += donjon->tab_monstres[i].piece;
    }
    if(somme_piece_monstre >= donjon->nb_or_pour_gagner){   // Les monstres ont ramassé tout l'or nécessaire

        return PARTIE_PERDUE;
    }
    return PARTIE_EN_COURS; // Aucune des deux équipes n'a gagné.
}

/**
 * Renvoie si le joueur peut se diriger dans la direction demandée
 * @param donjon : le donjon
 * @param direction : NORD/EST/OUEST/SUD --> la direction dans laquelle on souhaite vérifier l'accéssibilité
 */
int direction_valide(donjon_t * donjon, cardinal direction){
    int i, j;
    i = donjon->joueur.position / LAB_N;
    j = donjon->joueur.position % LAB_N;
    return !(donjon->laby->grille[i][j] & direction);
}

/**
 * Calcul la position du voisin du noeud dans la direction donnée
 * @param noeud : le noeud dont on cherche le voisin
 * @param direction : la direction dans laquelle on cherche le voisin
 * ATTENTION: Cette fonction ne vérifie pas que le noeud est valide
 * @return la position du noeud voulu
 */
int calcul_pos_noeud_voisin(int noeud, cardinal direction)
{
    switch (direction)
    {
    case NORD:
        return noeud - LAB_N;
        break;
    case EST:
        return noeud + 1;
        break;
    case SUD:
        return noeud + LAB_N;
        break;
    case OUEST:
        return noeud - 1;
        break;
    default:
        fprintf(stderr, "Erreur direction invalide dans calcul_pos_noeud_voisin");
        exit(CODE_ERR_CALC_POS_VOISIN);
        break;
    }
}

/**
 * Trouve la position de la pièce sur la grille
 * @param donjon : le donjon
 * @return le noeud qui contient LA pièce
 */
int trouver_piece(donjon_t * donjon){
    return donjon->position_or;
}

/**
 * Vérifie si la pièce a été récoltée, fait les modifications si nécesaires
 * @param donjon : le donjon
 * @return renvoie 1 si la pièce a été collectée, 0 sinon
 */
int verifie_collision_piece(donjon_t * donjon)
{
    int pos_piece = trouver_piece(donjon);
    int i = pos_piece/LAB_N , j = pos_piece%LAB_N;
    int trouve = 0;

    // Check si le joueur récolte la pièce
    if(donjon->presence_entite[i][j] & JOUEUR){
        donjon->presence_entite[i][j] -= PIECE;
        donjon->joueur.piece++;
        trouve = 1;
    }

    // Check si un monstre récolte la pièce.
    if(donjon->presence_entite[i][j] & MONSTRE_SAVANT || donjon->presence_entite[i][j] & MONSTRE_FOUINEUR){ // Monstre présent ?
        for(int k = 0; k < donjon->nb_monstre && !trouve; k++){                // On parcourt tout nos monstres pour savoir lequel
            if(donjon->tab_monstres[k].position == pos_piece){
                donjon->presence_entite[i][j] -= PIECE;
                donjon->tab_monstres[k].piece++;
                trouve = 1;
            }
        }
    }

    // Regénère une nouvelle pièce
    if(!(donjon->presence_entite[i][j] & PIECE)){   // La pièce a été ramassée
        if(donjon->nb_or_restant_a_generer > 0){
            genere_piece(donjon);
        }
    }
    return trouve;
    
}

/**
 * Vérifie si un monstre et le joueur se trouvent sur la même case.
 * @param donjon : donjon concerné
 * @return VRAI si un monstre est sur la même case qu'un joueur, FAUX sinon
 */
int verifie_collision_monstre(donjon_t * donjon)
{
    int i, j;
    i = donjon->joueur.position / LAB_N;
    j = donjon->joueur.position % LAB_N;

    if(donjon->presence_entite[i][j] & MONSTRE_FOUINEUR || donjon->presence_entite[i][j] & MONSTRE_SAVANT){
        return 1;
    }
    return 0;
}

/**
 * Permet d'obtenir l'id du monstre avec lequel le joueur est en collision
 * @param donjon : le donjon
 * @return : l'id du premier monstre trouvé qui est en collision avec le joueur, -1 si il n'y en a aucun
 */
int obtenir_id_monstre_collision(donjon_t * donjon)
{
    int i;
    for(i = 0; i < donjon->nb_monstre; i++){
        if(donjon->joueur.position == donjon->tab_monstres[i].position){
            return i;
        }
    }
    return -1;
}

/**
 * Renvoie si la position est valide pour générer un bonus : pas de monstre, de jouer, de pièce ou de bonus.
 * @param donjon : le donjon concerné
 * @param position : la position à tester
 * @return 1 si position valide, 0 sinon
 */
int position_valide_bonus_malus(donjon_t * donjon, int position)
{  
    if(position < 0 || position >= LAB_N * LAB_P) {
        return 0;
    }
    int val_case = donjon->presence_entite[position/LAB_N][position%LAB_N];
    return !(val_case & JOUEUR || val_case & MONSTRE_FOUINEUR || val_case & MONSTRE_SAVANT || val_case & PIECE || val_case & BONUS_MALUS);
}

/**
 * Génère un bonus ou un malus aléatoirement sur la map.
 * @param donjon : le donjon concerné
 */
void genere_bonus_malus(donjon_t * donjon)
{
    int i, j, iteration = 0;
    int position = genere_position();
    while(!position_valide_bonus_malus(donjon, position) && iteration < LAB_N * LAB_P){
        position = (position + 1)%(LAB_N * LAB_P);
        iteration++;
    }
    if(iteration < LAB_N * LAB_P){
        i = position / LAB_N;
        j = position % LAB_N;
        donjon->presence_entite[i][j] += BONUS_MALUS;
    }
}

/**
 * Affecte les valeurs donnés en paramètres à l'effet
 * @param effet : l'effet sur lequel on fait la modification
 * @param atk : la nouvelle attaque
 * @param vie : la nouvelle vie
 * @param spd : la nouvelle vitesse
 */
void set_effet(effet_t * effet, int atk, int vie, int spd){
    effet->atk = atk;
    effet->vie = vie;
    effet->spd = spd;
}

/**
 * Génère un effet aléatoire
 * @return un effet avec des bonus/malus aléatoires.
 */
effet_t genere_effet(){
    int id_item = rand()%9;

    effet_t effet;
    effet.atk = 0;
    effet.vie = 0;
    effet.spd = 0;


    switch (id_item){
    case 0:
        set_effet(&effet, 2, 1, 0);
        break;
    case 1:
        set_effet(&effet, 0, 0, 4);
        break;
    case 2:
        set_effet(&effet, 4, 0, 0);
        break;
    case 3:
        set_effet(&effet, 0, 3, 0);
        break;
    case 4:
        set_effet(&effet, 0, -2, 0);
        break;
    case 5:
        set_effet(&effet, 0, 0, -2);
        break;
    case 6:
        set_effet(&effet, 1, 1, 1);
        break;
    case 7:
        set_effet(&effet, -2, 0, 0);
        break;
    case 8:
        set_effet(&effet, -1, -1, -1);
        break;
    default:
        fprintf(stderr, "Comportement inattendu lors de la génération de l'effet\n");
        exit(CODE_ERR_EFFET_GEN);
        break;
    }
    return effet;
}

/**
 * Permet d'obtenir l'id du premier monstre en collision avec l'effet
 * @param donjon : le donjon concerné
 * @param position : la position de la collision
 */
int get_id_monstre_collision_effet(donjon_t * donjon, int position){
    for(int i = 0; i < donjon->nb_monstre; i++){
        if(donjon->tab_monstres[i].position == position){
            return i;
        }
    }
    fprintf(stderr, "Comportement inattendu : aucun monstre n'est en collision avec un effet à cette position.\n");
    exit(CODE_ERR_COLLISION_M_E);
}

void gere_collision_bonus_malus(donjon_t * donjon){
    int val_case;
    effet_t effet;
    int i,j;
    for(i=0; i < LAB_P; i++){
        for(j = 0; j < LAB_N; j++){
            val_case = donjon->presence_entite[i][j];
            if(val_case & BONUS_MALUS){
                if(val_case & JOUEUR){  // C'est le joueur qui récupère le bonus/malus
                    effet = genere_effet();
                    donjon->joueur.attaque += effet.atk;
                    donjon->joueur.vie += effet.vie;
                    donjon->joueur.vitesse += effet.spd;
                    if(donjon->joueur.vie <= 0) donjon->joueur.vie = 1;         // Remise dans un état cohérent des statistiques
                    if(donjon->joueur.attaque <= 0) donjon->joueur.attaque = 1;
                    if(donjon->joueur.vitesse <= 0) donjon->joueur.vitesse = 1;
                    donjon->presence_entite[i][j] -= BONUS_MALUS;

                }else if(val_case & MONSTRE_FOUINEUR || val_case & MONSTRE_SAVANT){     // C'est un monstre qui le récupère ON DIVISE LE SPEED PAR 2.
                    effet = genere_effet();
                    int id_monstre = get_id_monstre_collision_effet(donjon, i * LAB_N + j);
                    donjon->tab_monstres[id_monstre].attaque += effet.atk;
                    donjon->tab_monstres[id_monstre].vie += effet.vie;
                    donjon->tab_monstres[id_monstre].vitesse += effet.spd;
                    // Remise dans un état cohérent
                    if(donjon->tab_monstres[id_monstre].attaque <= 0 ) donjon->tab_monstres[id_monstre].attaque = 1;
                    if(donjon->tab_monstres[id_monstre].vie <= 0 ) donjon->tab_monstres[id_monstre].vie = 1;
                    if(donjon->tab_monstres[id_monstre].vitesse <= 0 ) donjon->tab_monstres[id_monstre].vitesse = 1;
                    donjon->presence_entite[i][j] -= BONUS_MALUS;

                }
            }
        }
    }
}

/**
 * Fonction d'affichage pour le débug
 */

 /**
  * Affiche les informations sur un joueur
  */
 void debug_affiche_joueur(joueur_t joueur)
 {
    printf("==Affichage Joueur==\n");
    printf("Attaque : %d\nVie : %d\nPosition : %d\nPièce : %d\nDernière position : %d\n", joueur.attaque, joueur.vie, joueur.position, joueur.piece, joueur.derniere_direction);
    printf("Vitesse : %d\n", joueur.vitesse);

}

void debug_affiche_trajet(trajet traj){
    printf("Affichage trajet :\nNb_element : %d\nX: ",traj.nb_element);
    for(int i = 0; i < traj.nb_element; i++){
        printf("%d ", traj.trajet_x[i]);
    }
    printf("\nY: ");
    for(int i = 0; i < traj.nb_element; i++){
        printf("%d ", traj.trajet_y[i]);
    }
    printf("\n");
}

void debug_affiche_monstre(monstre_t monstre)
{
    printf("===Affichage monstre===\n");
    printf("Attaque : %d\nVie : %d\nPosition : %d\nPièce : %d\nSavant : %d\nDernière position : %d\n", monstre.attaque, monstre.vie, monstre.position, monstre.piece, monstre.est_savant, monstre.derniere_direction);
    printf("Vitesse : %d\n", monstre.vitesse);
    printf("Direction actuelle : %d\n", monstre.direction_actuelle);
    printf("Itération du trajet : %d\n", monstre.etape_traj);
    debug_affiche_trajet(monstre.traj);
}

void debug_affiche_donjon(donjon_t * donjon){
    printf("===Affichage donjon===\n");
    puts("Labyrinthe : ");
    for(int i = 0; i < LAB_P; i++){
        for(int j = 0; j < LAB_N; j++){
            printf("%d ", donjon->laby->grille[i][j]);
        }
        printf("\n");
    }
    printf("Nombre d'or pour gagner %d\nNombre d'or restant à générer : %d\n", donjon->nb_or_pour_gagner, donjon->nb_or_restant_a_generer);
    debug_affiche_joueur(donjon->joueur);
    for(int i = 0; i < donjon->nb_monstre; i++){
        printf("Monstre n°%d\n", i);
        debug_affiche_monstre(donjon->tab_monstres[i]);
    }
    puts("Entité : ");
    for(int i = 0; i < LAB_P; i++){
        for(int j = 0; j < LAB_N; j++){
            if(donjon->presence_entite[i][j] & PIECE){
                printf("\e[33m");
            }
            if(donjon->presence_entite[i][j] & JOUEUR){
                printf("\e[36m");
            }
            if(donjon->presence_entite[i][j] & MONSTRE_FOUINEUR){
                printf("\e[31m");
            }
            if(donjon->presence_entite[i][j] & MONSTRE_SAVANT){
                printf("\e[31m");
            }
            if(donjon->presence_entite[i][j] & BONUS_MALUS){
                printf("\e[35m");
            }
            printf("%d ", donjon->presence_entite[i][j]);
            if(donjon->presence_entite[i][j]){
                printf("\e[0m");
            }
        }
        printf("\n");
    }
}

void affiche_map_donjon(donjon_t * donjon){
    affiche_sans_SDL(donjon->laby);
    for(int i = 0; i < LAB_P; i++){
        for(int j = 0; j < LAB_N; j++){
            if(donjon->presence_entite[i][j] & PIECE){
                printf("\e[33m");
            }
            if(donjon->presence_entite[i][j] & JOUEUR){
                printf("\e[36m");
            }
            if(donjon->presence_entite[i][j] & MONSTRE_FOUINEUR){
                printf("\e[31m");
            }
            if(donjon->presence_entite[i][j] & MONSTRE_SAVANT){
                printf("\e[31m");
            }
            printf("%d ", donjon->presence_entite[i][j]);
            if(donjon->presence_entite[i][j]){
                printf("\e[0m");
            }
        }
        printf("\n");
    }
}