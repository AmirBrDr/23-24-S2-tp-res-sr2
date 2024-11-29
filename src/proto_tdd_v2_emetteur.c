#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"



int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    paquet_t paquet,pack; /* paquet utilisé par le protocole et le pack recu*/
    int prochain_paquet = 0; // prochain paquet attendu
    int evt = 0; // -1 si paquet reçu et n si Timetout
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
        paquet.num_seq = prochain_paquet;
        paquet.lg_info = taille_msg;
        paquet.type = DATA;
        paquet.somme_ctrl=generer_controle(paquet);

        vers_reseau(&paquet);

        depart_temporisateur(100);
        evt = attendre();
        while (evt != -1){
            vers_reseau(&paquet);
            depart_temporisateur(100);
            evt = attendre();
        }
        de_reseau(&pack);


        arret_temporisateur();
        prochain_paquet = inc(prochain_paquet);
        
        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }
    
    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;

}

