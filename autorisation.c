#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include "lectureEcriture.h"
#include "message.h"

int main(int argc, char const *argv[])
{
    int TubeEcritureReponse = atoi(argv[1]);
    int TubeLectureDemande = atoi(argv[2]);

    int carteOK = 0;
    int montantOK = 0;

    dup2(TubeEcritureReponse,1); //ecrit dans argv1
    dup2(TubeLectureDemande,0);//lit dans argv2

    fprintf(stderr,"on est dans autorisation\n");
    //numero de carte codé et solde en dur le temps d'avoir un fichier de carte client
    char *BDDcarte = "1234123412341234";
    int soldeActuelAssocieCarte = 70;

    char *transactionDemande = litLigne(TubeLectureDemande);

    //TODO mieux initialiser les char 
    char numeroCarte[17];
    char type[20];
    char valeurTransaction[100];

    if(decoupe(transactionDemande,numeroCarte,type,valeurTransaction) == 0){
        printf("La decoupe du message de transaction a échoué");
    }

    printf("numcb : %s type : %s valeur : %s \n",numeroCarte,type,valeurTransaction);



    //ce qui va etre lu dans le tube va etre selon le protocle de message CAD XXXX|XXXX|XXXX donc il faudra proceder a une decoupe

    //convertion en int du numero de carte recu
    // int numeroCarteRecu = atoi(numeroCarte);

    int montantDemande = atol(valeurTransaction);

    //comparaison du numero de carte recu avec la BDD de carte de la banque
    if(strcmp(numeroCarte,BDDcarte) == 0 ){
        carteOK = 1;
    }
    else{
        printf("cette carte n'appartient pas à la banque \n");
    }

    //verif du solde de du compte 
    if(soldeActuelAssocieCarte >= montantDemande){
        montantOK = 1;
    }
    else
    {
        printf("Solde du compte insuffisant \n");
    }

    //si solde ok et que carte appartien a la banque alors on accepte la transaction
    if(carteOK == 1 && montantOK ==1){
        fprintf(stderr,"message envoyé par autorisation a acquisition : %s",message(numeroCarte,"Reponse","1"));
        ecritLigne(TubeEcritureReponse,message(numeroCarte,"Reponse","1"));
    }
    else
    {
        ecritLigne(TubeEcritureReponse,message(numeroCarte,"Reponse","0"));
    }
    
    
    return 0;

}