#include <stdio.h>
#include <stdlib.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"


int main(int argc, char* argv[]){

    unsigned char message[MAX_INFO]; /* message de l'application */
    int taille_msg; /* taille du message */
    int cap_num = 16;
    paquet_t tabp[cap_num];
    paquet_t pack; /* paquet utilisé par le protocole et le pack recu*/
    paquet_t tabfin;
    tabfin.lg_info = 1;
    int evt = 0; // -1 si paquet reçu et n si Timetout
    int borne_inf,curseur,taille_fen;
    pack.lg_info=0;
    borne_inf = 0;
    curseur = 0;

    if(argc == 1) taille_fen = 7;
    else taille_fen = atoi(argv[1]);
    

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
            
            
            if (curseur == borne_inf) depart_temporisateur(100);
            curseur = inc_dans_borne(curseur,cap_num);
        }
        else {
            evt = attendre();
            if (evt == -1){ // le cas ou on a reçu un ACK
                de_reseau(&pack);
                if (verifier_controle(pack) && dans_fenetre(borne_inf, pack.num_seq, taille_fen)){ // verifier la somme de controle et aussi si le ACK est bien dans la fenetre
                    pack.num_seq = inc_dans_borne(pack.num_seq,cap_num);
                    borne_inf = pack.num_seq;

                    if (borne_inf == curseur) arret_temporisateur();
                }
            } else { // TimeOut
                int i = borne_inf;
                depart_temporisateur(100);
                while (i != curseur){
                    vers_reseau(&tabp[i]);
                    i = inc_dans_borne(i,cap_num);
                }
            }
        }
        /* lecture des donnees suivantes de la couche application */
        de_application(message, &taille_msg);
    }
    vers_reseau(&tabfin); 
    printf("[TRP] Fin execution protocole transfert de donnees (TDD).\n");
    return 0;

}

