#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"


int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet,pack; /* paquet utilisé par le protocole */
    int fin = 0; /* condition d'arrêt */
    int paquet_attendu = 0; // prochain paquet attendu
    pack.lg_info=0;
    
    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");
    
    /* tant que le récepteur reçoit des données */
    while ( !fin ){
        de_reseau(&paquet);
        if (verifier_controle(paquet) ){

            if(paquet.num_seq == paquet_attendu){
                /* extraction des donnees du paquet recu */
                for (int i=0; i<paquet.lg_info; i++) {
                    message[i] = paquet.info[i];
                }
                fin = vers_application(message,paquet.lg_info);

                paquet_attendu = inc(paquet_attendu);
                
            }

            pack.num_seq=paquet_attendu;
            vers_reseau(&pack);
        }
          
        
    }
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;

}

