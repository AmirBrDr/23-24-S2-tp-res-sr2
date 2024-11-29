#include <stdio.h>
#include <stdlib.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"
#define CAP_NUM 16


int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    
    paquet_t tabp[CAP_NUM];
    paquet_t pack; /* paquet utilisé par le protocole et le pack recu*/
    paquet_t tabfin;
    tabfin.lg_info = 1;
    int evt = 0; // -1 si paquet reçu et n si Timetout
    int borne_inf,curseur,taille_fen;
    pack.lg_info=0;
    borne_inf = 0;
    curseur = 0;


   
    taille_fen = atoi(argv[1]);
    
    
    paquet_t envoye[CAP_NUM];

    init_reseau(EMISSION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");
    /* lecture de donnees provenant de la couche application */
    de_application(message, &taille_msg);

    while (taille_msg!=0 || curseur == borne_inf) {
        
        if (dans_fenetre(borne_inf,curseur,taille_fen)){ // verifier si le curseur est bien dans la fenetre
            for (int i=0; i<taille_msg; i++) {
            tabp[curseur].info[i] = message[i]; // copier le contenu de paquet arrivé dans le tampon
            }

            tabp[curseur].num_seq = curseur;
            tabp[curseur].somme_ctrl = generer_controle(tabp[curseur]);
            vers_reseau(&tabp[curseur]);
            
            envoye[curseur]= tabp[curseur];
            depart_temporisateur_num(100,curseur);
            curseur = inc_dans_borne(curseur,CAP_NUM);
        }
        else {
            evt = attendre();
            if (evt == -1){ // le cas ou on a reçu un ACK
                de_reseau(&pack);
                if (verifier_controle(pack) && dans_fenetre(borne_inf, pack.num_seq, taille_fen)){ // verifier la somme de controle et aussi si le ACK est bien dans la fenetre
                    
                    if (borne_inf == pack.num_seq){
                        borne_inf = inc_dans_borne(borne_inf,CAP_NUM);
                    }

                    // Parcours du tableau sent pour trouver le premier paquet non acquitté dans la fenêtre d'envoi
                    envoye[pack.num_seq] = pack;
                    int premier_non_ACK = borne_inf;
                    while (premier_non_ACK < borne_inf + taille_fen && envoye[premier_non_ACK % taille_fen].type == ACK){
                        premier_non_ACK++;
                    }

                    // Si tous les paquets dans la fenêtre ont été acquittés, déplacer la fenêtre d'envoi vers le prochain paquet non acquitté
                    if (premier_non_ACK == borne_inf + taille_fen){
                        borne_inf = premier_non_ACK;
                    }

                     // Annuler tous les temporisateurs pour les paquets qui se trouvent en dehors de la nouvelle fenêtre d'envoi
                    for (int i = borne_inf - taille_fen; i < borne_inf; i++) {
                        arret_temporisateur_num(i % taille_fen);
                    }
                    

                    
                }
            } else { // TimeOut
                // Retransmission du paquet lors de son TimeOut
                vers_reseau(&tabp[evt]);
                depart_temporisateur_num(100,evt);
                
            }
        }
        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }
    vers_reseau(&tabfin); 
    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;

}

