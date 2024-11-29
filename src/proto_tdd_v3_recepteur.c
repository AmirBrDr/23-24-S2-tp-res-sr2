#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet,pack; /* paquet utilisé par le protocole */
    int fin = 0; /* condition d'arrêt */
    int paquet_attendu = 0; // prochain paquet attendu
    int cap_num = 16;
    pack.lg_info=0;
    pack.num_seq = 16;
    
    
    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");
    
    /* tant que le récepteur reçoit des données */
    while ( !fin && paquet.lg_info != 1){
        de_reseau(&paquet);
        if (verifier_controle(paquet) ){

            if(paquet.num_seq == paquet_attendu){
                /* extraction des donnees du paquet recu */
                for (int i=0; i<paquet.lg_info; i++) {
                    message[i] = paquet.info[i];
                }
                fin = vers_application(message,paquet.lg_info);   
                pack.num_seq=paquet_attendu;   // mettre a jour le num_seq de ACK si on a recu le paquet en sequence 
                paquet_attendu = inc_dans_borne(paquet_attendu,cap_num); 
            }
            
            vers_reseau(&pack); 
        }
          
        
    }
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;

}

