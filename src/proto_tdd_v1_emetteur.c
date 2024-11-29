#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"




int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    paquet_t paquet,pack; /* paquet utilisé par le protocole et le pack recu*/
    pack.lg_info=0;
    pack.type = NACK;

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");
    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    while (taille_msg!=0) {
        

        /* construction paquet */
        for (int i=0; i<taille_msg; i++) {
            paquet.info[i] = message[i];
        }
        paquet.lg_info = taille_msg;
        paquet.type = DATA;
        paquet.somme_ctrl=generer_controle(paquet);

       
        
        do {
            vers_reseau(&paquet);
            de_reseau(&pack);
        } while (pack.type == NACK);
        
        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }
    
    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;

}

