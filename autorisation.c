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

/*******************************************************************************************/
/* Le serveur autorisation reçoit la demande de terminal après son passage par acquisition */
/* Il reverifie si le compte voulant retirer est bien présent dans sa banque, puis vérifie */
/* que le solde du compte est suffisant et il renvoi un message adapté.                   */
/*******************************************************************************************/


while(1){

    //Convertit en int les arguments passé au main

    int TubeEcritureReponse = atoi(argv[1]);
    int TubeLectureDemande = atoi(argv[2]);

    // On récupère l'annuaire contenant les numéro de carte et leur solde

    AnnuaireClients *annuaireClient = annuaire("InfoClient.txt");
   
    int carteOK = 0;
    int montantOK = 0;

    // Permet de rediriger dans l'entrée et la sortie des tubes

    dup2(TubeEcritureReponse,1); //ecrit dans argv1
    dup2(TubeLectureDemande,0);//lit dans argv2 

    char *transactionDemande = litLigne(TubeLectureDemande);

    fprintf(stderr,"transaction emise par le terminal %s\n",transactionDemande);

    //TODO mieux initialiser les char 
    char numeroCarte[17];
    char type[20];
    char valeurTransaction[100];

    // On decoupe le message reçu de terminal

    if(decoupe(transactionDemande,numeroCarte,type,valeurTransaction) == 0){
        printf("La decoupe du message de transaction a échoué");
    }

    int montantDemande = atol(valeurTransaction);
    fprintf(stderr,"numcb : %s type : %s valeur : %s \n",numeroCarte,type,valeurTransaction);


    // On recupère le solde du client associé au compte que l'on traite

    Client *clientAttenteAutorisation = client(annuaireClient, numeroCarte);
    int soldeClientActuel = clientAttenteAutorisation->solde;
    fprintf(stderr, "Solde du client actuel %d \n", soldeClientActuel);

    // On recupère le code de la CB pour ensuite le comparer avec l'annuaire et voir si la carte existe

    if(clientAttenteAutorisation != NULL){
        carteOK =1;
        fprintf(stderr, "Numéro de carte OK\n");
    }else
    {
        fprintf(stderr, "Ce numéro n'appartient pas à la banque \n");
    }
    //verification de la  du solde du compte

    if(soldeClientActuel >= montantDemande){
        montantOK = 1;
        fprintf(stderr, "Solde OK \n");
    }
    else
    {
        fprintf(stderr,"Solde du compte insuffisant \n");
    }
    

    // Si la carte appartient bien à cette banque et qu'il y a assez d'argent sur le code pour la transaction on autorise le retrait
    // sinon on refuse et on envoi le message correspondant selon le protocole

    if(carteOK == 1 && montantOK ==1){
        ecritLigne(TubeEcritureReponse,message(numeroCarte,"Reponse","1"));
        fprintf(stderr,"message envoyé par autorisation a acquisition quand on accepte le payement: %s",message(numeroCarte,"Reponse","1"));
    }
    else
    {
        ecritLigne(TubeEcritureReponse,message(numeroCarte,"Reponse","0"));
        fprintf(stderr, "Message envoyé par autorisation a acquisition quand il y a refus de payement : %s", message(numeroCarte, "Reponse", "0"));
    }
    
}
    return 0;

}
