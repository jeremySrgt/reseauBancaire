#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#include "lectureEcriture.h"
#include "message.h"
#include "annuaire.h"

int main(int argc, char const *argv[])
{
// A TESTER ET A FAIRE MARCHER ///////////////////////////////////////
/////////////////////////////////////////////////////////////////////

    // char *suiteChaine = NULL;

    // int descripteurFichier = open("InfoClient.txt", O_RDONLY);
    // char *soldeDuCompte = litLigne(descripteurFichier);
    // fprintf(stderr, "Solde du compte : %s", soldeDuCompte);

    // suiteChaine = strchr(soldeDuCompte, 'd');
    // if (suiteChaine != NULL) // Si on a trouvé quelque chose
    // {
    //     printf("Voici la fin de la chaine a partir du premier d : %s \n", suiteChaine);
    // }

///////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////


    int TubeEcritureReponse = atoi(argv[1]);
    int TubeLectureDemande = atoi(argv[2]);

    int carteOK = 0;
    int montantOK = 0;

    dup2(TubeEcritureReponse,1); //ecrit dans argv1
    dup2(TubeLectureDemande,0);//lit dans argv2

    // Récupération de l'annuaire qui est dans le fichier texte
    AnnuaireClients *annuaireClient = annuaire("InfoClient.txt");
    // afficherAnnuaire(annuaireClient); 


    fprintf(stderr,"on est dans autorisation\n");
    //numero de carte codé et solde en dur le temps d'avoir un fichier de carte client
    //char *BDDcarte = "1234123412341234";
    int soldeActuelAssocieCarte = 100000;

    char *transactionDemande = litLigne(TubeLectureDemande);

    //TODO mieux initialiser les char 
    char numeroCarte[17];
    char type[20];
    char valeurTransaction[100];

    if(decoupe(transactionDemande,numeroCarte,type,valeurTransaction) == 0){
        printf("La decoupe du message de transaction a échoué");
    }

     int montantDemande = atol(valeurTransaction);


    fprintf(stderr,"numcb : %s type : %s valeur : %s \n",numeroCarte,type,valeurTransaction);

    //  Client *soldeDuCompte = client(annuaire("InfoClient.txt"), numeroCarte);


    // On recupère le code de la CB pour ensuite le comparer avec la BDD et voir si la carte existe
    if(client(annuaireClient, numeroCarte) == NULL){
        carteOK =1;
        fprintf(stderr, "gg tu peux regarder si t'a assez de thune\n");
    }else
    {
        fprintf(stderr, "C'est pas ta banque   \n");
    }
        //verif du solde de du compte 
    if(soldeActuelAssocieCarte >= montantDemande){
        montantOK = 1;
        fprintf(stderr, "Woaow la richesse \n");
    }
    else
    {
        fprintf(stderr,"Solde du compte insuffisant \n");
    }
    

//     //ce qui va etre lu dans le tube va etre selon le protocle de message CAD XXXX|XXXX|XXXX donc il faudra proceder a une decoupe

//     //convertion en int du numero de carte recu
//     // int numeroCarteRecu = atoi(numeroCarte);


//     //comparaison du numero de carte recu avec la BDD de carte de la banque
//     if(strcmp(numeroCarte,BDDcarte) == 0 ){
//         carteOK = 1;
//     }
//     else{
//         fprintf(stderr,"cette carte n'appartient pas à la banque \n");
//     }

//     //verif du solde de du compte 
//     if(soldeActuelAssocieCarte >= montantDemande){
//         montantOK = 1;
//     }
//     else
//     {
//         fprintf(stderr,"Solde du compte insuffisant \n");
//     }

// //     A FAIRE MARCHER QUAND LA VENU DE TERMINAL EST BONNE

// //     AnnuaireClients *annuaireClient = annuaireAleatoire(2, 2);
// //     afficherAnnuaire(annuaireClient);

// // if(client(annuaireClient, numeroCarte) == NULL ){
// //         carteOK = 1;
// //         fprintf(stderr, "Louuuuurd ça marche \n");
// //     }
// //     else{
// //         fprintf(stderr,"cette carte n'appartient pas à la banque \n");
// //     }

//     //verif du solde de du compte 
//     if(soldeActuelAssocieCarte >= montantDemande){
//         montantOK = 1;
//     }
//     else
//     {
//         fprintf(stderr,"Solde du compte insuffisant \n");
//     }


    //si solde ok et que carte appartien a la banque alors on accepte la transaction
    if(carteOK == 1 && montantOK ==1){
        ecritLigne(TubeEcritureReponse,message(numeroCarte,"Reponse","1"));
        fprintf(stderr,"message envoyé par autorisation a acquisition quand on accepte le payement: %s",message(numeroCarte,"Reponse","1"));

    }
    else
    {
        fprintf(stderr,"avant ecritligne de refus de payement %s \n",message(numeroCarte,"Reponse","0"));
        ecritLigne(TubeEcritureReponse,message(numeroCarte,"Reponse","0"));
        fprintf(stderr, "Messgae envoyé par autorisation a acquisition quand il y a refus de payement : %s", message(numeroCarte, "Reponse", "0"));

    }
    
    
    return 0;

}