#include <stdio.h>
#include <stdlib.h>
#include "abr.h"


abr * abr_creer_noeud(int val) 
{
    abr * new = (abr*)malloc(sizeof(abr));
    if(!new) return NULL;

    new->val = val;
    new->gauche = NULL;
    new->droite = NULL;

    return new;
}

abr * abr_insertion(abr * root, int val) 
{
    if(!root) {
        return abr_creer_noeud(val);
    } else {
        if(val > root->val) root->droite = abr_insertion(root->droite, val);
        else root->gauche = abr_insertion(root->gauche, val);
    }

    return root;
}

abr * abr_val_min(abr * root) 
{
    if(!root) return NULL;
    if(root->gauche) return abr_val_min(root->gauche);
    else return root;
}

abr * abr_val_max(abr * root) 
{
    if(!root) return NULL;
    if(root->droite) return abr_val_max(root->droite);
    else return root;
}

int abr_est_feuille(abr * root)
{
    return (root->gauche == NULL && root->droite == NULL);
}

abr * abr_supprimer(abr * root, int val) 
{
    if(root->val == val) {
        if(abr_est_feuille(root)) { //feuille
            free(root);
            return NULL;

        } else if (root->gauche == NULL || root->droite == NULL) { //un enfant
            abr * tmp = root;
            if(root->gauche) root = root->gauche;
            else if(root->droite) root = root->droite;
            
            free(tmp);

        } else { //deux enfants
            abr * new = abr_val_min(root->droite);
            root->val = new->val;  
            root->droite = abr_supprimer(root->droite, new->val); // supprime le nouveau noeud dans son ancienne position
        }
        
        return root;

    } else {
        if(root->gauche) root->gauche = abr_supprimer(root->gauche, val);
        if(root->droite) root->droite = abr_supprimer(root->droite, val);
    }

    return root;
}

void abr_liberer(abr * root) 
{
    if(root) {
        abr_liberer(root->gauche);
        abr_liberer(root->droite);
        free(root);
    }
}


void aux(abr * root) 
{
    if(root) {
        printf("%d ", root->val);
        printf("[");
        aux(root->gauche);
        printf("]");
        printf("[");
        aux(root->droite);
        printf("]");
    }
}

void abr_afficher(abr * root)
 {
    aux(root);
    printf("\n");
}

int abr_recherche(abr * root, int val)
{
    if(!root){
        return 0;
    }
    if(root->val == val){
        return 1;
    }
    if(root->val > val){
        return abr_recherche(root->gauche, val);
    }
    if(root->val < val){
        return abr_recherche(root->droite, val);
    }
    return 0;
}

