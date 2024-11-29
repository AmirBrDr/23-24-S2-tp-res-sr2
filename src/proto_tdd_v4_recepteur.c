#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"
#define CAP_NUM 16

int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message pour l'application */
    paquet_t paquet,pack; /* paquet utilisé par le protocole */
    
    int fin = 0; /* condition d'arrêt */
    int paquet_attendu = 0; // prochain paquet attendu
    pack.lg_info=0;
    pack.num_seq = 16;
    int borne_inf,curseur = 0;

    bool paquet_recu[CAP_NUM] = {false};
    bool ack_envoyes[CAP_NUM] = { false };
    paquet_t buffer[CAP_NUM];

    int taille_fen = atoi(argv[1]);
    
    
    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");
    
    /* tant que le récepteur reçoit des données */
    while ( !fin && paquet.lg_info != 1){
        de_reseau(&paquet);
        if (verifier_controle(paquet) ){
            // Si le paquet reçu est dans la fenêtre de réception
            if (paquet.num_seq >= borne_inf && paquet.num_seq < borne_inf + taille_fen){

                // Stocker le paquet dans le tampon de réception
                buffer[paquet.num_seq] = paquet;
                paquet_recu[paquet.num_seq] = true;

                 // Envoyer un ACK pour ce paquet
                pack.num_seq = paquet.num_seq; // Numéro de séquence du paquet ACK
                vers_reseau(&pack);
                ack_envoyes[paquet.num_seq] = true;

                 // Vérifier si le paquet reçu est celui attendu
                while (paquet_recu[paquet_attendu]) {
                    // Extraire les données du paquet et les envoyer à l'application
                    for (int i = 0; i < buffer[paquet_attendu].lg_info; i++) {
                        message[i] = buffer[paquet_attendu].info[i];
                    }
                    fin = vers_application(message, buffer[paquet_attendu].lg_info);

                    // Mettre à jour les variables pour le prochain paquet attendu
                    paquet_recu[paquet_attendu] = false;
                    paquet_attendu = inc_dans_borne(paquet_attendu,CAP_NUM);
                    borne_inf = inc_dans_borne(borne_inf,CAP_NUM);
                }
            }
            
            vers_reseau(&pack); 
        }
          
        
    }
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;

}

