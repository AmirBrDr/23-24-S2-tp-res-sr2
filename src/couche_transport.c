#include <stdio.h>
#include "couche_transport.h"
#include "services_reseau.h"
#include "application.h"
#include "stdbool.h"

/* ************************************************************************** */
/* *************** Fonctions utilitaires couche transport ******************* */
/* ************************************************************************** */

// RAJOUTER VOS FONCTIONS DANS CE FICHIER...
int generer_controle(paquet_t paquet){ // Faire XOR sur lg_info , num_seq , type et chaque octet de info

    int s = paquet.lg_info ^ paquet.num_seq;
    s = s ^ paquet.type;
    for (int i = 0; i < paquet.lg_info; i++)
    {
        s = s ^ paquet.info[i];
    }


    return s;
}

bool verifier_controle(paquet_t paquet){ // verifier que les sommes de controle sont egaux

    generer_controle(paquet);
    return paquet.somme_ctrl == generer_controle(paquet);

}

int inc(int n){
    if (n==0) return 1;
    else return 0;
}

int inc_dans_borne(int n, int borne){
    n++;
    if (n == borne) n = 0;
    return n;
}
/* ===================== Fenêtre d'anticipation ============================= */

/*--------------------------------------*/
/* Fonction d'inclusion dans la fenetre */
/*--------------------------------------*/
int dans_fenetre(unsigned int inf, unsigned int pointeur, int taille) {

    unsigned int sup = (inf+taille-1) % SEQ_NUM_SIZE;

    return
        /* inf <= pointeur <= sup */
        ( inf <= sup && pointeur >= inf && pointeur <= sup ) ||
        /* sup < inf <= pointeur */
        ( sup < inf && pointeur >= inf) ||
        /* pointeur <= sup < inf */
        ( sup < inf && pointeur <= sup);
}
