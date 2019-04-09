#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>

#include "message.h"
#include "lectureEcriture.h"
#include "alea.h"

int main(int argc, char const *argv[])
{

int df1 = atoi(argv[1]);
int df2 = atoi(argv[2]);

// printf("executable terminal cree a partir de acquisition\n");


    //ouvre le fichier qui contient les numéros de carte
    int descripteurFichier = open("InfoClient.txt", O_RDONLY);
    char *codeCB = litLigne(descripteurFichier);

    //prend la ligne mais sans le retour chariot
    codeCB[strlen(codeCB)-1] ='\0';

    //on met en dur les 2 tubes qui vont vers acquisition
    dup2(df1,1); //ecrit dans argv1
    dup2(df2,0);//lit dans argv2

    //initilisation de la fonction alea du programme alea.h
    aleainit();
    char argentAleatoire[70];

    //Convertit le nombre aléatoire en une chaine de caractère
    sprintf(argentAleatoire, "%d", alea(5,20));
    printf("argent aleatoire : %s\n",argentAleatoire);

    //Formatage du message pour etre |...|...|...|
    char *demandeArgent = message(codeCB, "Demande", argentAleatoire);



    // int decoupage = decoupe(demandeArgent,codeCB,"demande",argentAleatoire);

    // if(decoupage==0){
    //     printf("erreur decoupage message");
    // }
    // printf("%s", demandeArgent);

    //Ecrit dans le tube la demande selon le formatage 
    ecritLigne(df1,demandeArgent);

    //Codage en dure de la lecture du tube
    char *reponse = litLigne(df2);

    printf("message lue : %s \n", reponse);
    //Formatage de la comparaison avec le message reçu d'acquisition
    char *accepte = message(codeCB, "Reponse", "1");


    // Traitement de l'affichage en fonction du retour du serveur authorisation
    // 1 -> accepté, 0 -> refusé

    if(strcmp(reponse, accepte) == 0){
        //Payement accepté
        printf("Payement accepte \n");
    }else
    {
        //Payement refusé
        printf("Payement refuse \n");
    }
    
    
    return 0;
}


