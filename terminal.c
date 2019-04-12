#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "message.h"
#include "lectureEcriture.h"
#include "alea.h"
#include "annuaire.h"

int main(int argc, char const *argv[])
{
    

int tubeEcritureDemande = atoi(argv[1]);
int tubeLectureReponse = atoi(argv[2]);



// printf("executable terminal cree a partir de acquisition\n").;

  /*****************************************************************/
  /* Création d'un annuaire composé de n banque et n client        */
  /* L'annuaire est ensuite sauvegarder dans un fichier texte      */
  /*****************************************************************/

    // AnnuaireClients *annuaireCree;
    // annuaireCree =  annuaireAleatoire(2, 20);
    // //sauvegarde annuaire crée dans le fichier InfoClient.txt pour autorisation
    // sauvegardeAnnuaire(annuaireCree, "InfoClient.txt");

    
    // On récupère l'annuaire dans une variable de type AnnuaireClient
    // On utilise cette variable afin de récupérer un numéro de CB aléatoire dans l'annuaire
    // On récupère également le solde correspondant au compte afin de l'afficher dans le terminal

    aleainit();
    int nombreAleatoire = alea(1,20);

    AnnuaireClients *annuaireClient = annuaire("InfoClient.txt");
    char *codeCbAleatoire = annuaireClient->donnees[nombreAleatoire].CB;
    fprintf(stderr, "Code Cb aleatoire : %s \n", codeCbAleatoire);

    int soldeActuelCompte = annuaireClient->donnees[nombreAleatoire].solde;
    fprintf(stderr, "Solde actuel du compte : %d", soldeActuelCompte);


    // Permet de rediriger dans l'entrée et la sortie des tubes 

    dup2(tubeEcritureDemande,1); //ecrit dans argv1
    dup2(tubeLectureReponse,0);//lit dans argv2


    // On choisit un nombre aléatoire pour une demande de montant
    // Convertit le nombre aléatoire en une chaine de caractère

    char argentAleatoire[70];
    sprintf(argentAleatoire, "%d", alea(5,100000));

    // Formatage du message pour etre selon la forme demandé |...|...|...|

    char *demandeArgent = message(codeCbAleatoire, "Demande", argentAleatoire);




    //Ecrit dans le tube la demande selon le protocole

    ecritLigne(tubeEcritureDemande,demandeArgent);
    char *reponse = litLigne(tubeLectureReponse);

    fprintf(stderr,"reponse recu de acquisition apres traitement par autorisation : %s",reponse);

    //Formatage de la comparaison avec le message reçu d'acquisition
    char *accepte = message(codeCbAleatoire, "Reponse", "1");


    // Traitement de l'affichage en fonction du retour du serveur autorisation
    // 1 -> accepté, 0 -> refusé

    if(strcmp(reponse, accepte) == 0){
        //Payement accepté
        fprintf(stderr, "---------------------payement accepte---------------------\n");
    }else
    {
        //Payement refusé
        fprintf(stderr, "---------------------payement refuse---------------------\n");
    }
    
     return 0;
 }


