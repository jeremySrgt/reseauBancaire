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

// printf("executable terminal cree a partir de acquisition\n").;


    //ouvre le fichier qui contient les numéros de carte
    int descripteurFichier = open("InfoClient.txt", O_RDONLY);
    char *codeCB = litLigne(descripteurFichier);

    //prend la ligne mais sans le retour chariot
    codeCB[strlen(codeCB)-1] ='\0';

    dup2(df1,1); //ecrit dans argv1
    // dup2(0,df2);//lit dans argv2
    dup2(df2,0);

    //initilisation de la fonction alea du programme alea.h
    aleainit();
    char argentAleatoire[70];

    //Convertit le nombre aléatoire en une chaine de caractère
    sprintf(argentAleatoire, "%d", alea(5,20));
    // printf("argent aleatoire : %s\n",argentAleatoire);

    //Formatage du message pour etre |...|...|...|
    char *demandeArgent = message(codeCB, "Demande", argentAleatoire);



    // int decoupage = decoupe(demandeArgent,codeCB,"demande",argentAleatoire);

    // if(decoupage==0){
    //     printf("erreur decoupage message");
    // }
    // printf("%s", demandeArgent);

    //Ecrit dans le tube la demande selon le formatage 
    fprintf(stderr, "juste avant ecritligne de terminal\n");
    ecritLigne(df1,demandeArgent);

    fprintf(stderr, "juste avant litligne de terminal\n");
    //Codage en dure de la lecture du tube
    char *reponse = litLigne(df2);

    fprintf(stderr,"reponse recu de acquisition apres traitement par autorisation : %s",reponse);
    // printf("reponse lu %s\n",reponse);

    // printf("message lue : %s \n", reponse);
    //Formatage de la comparaison avec le message reçu d'acquisition
    char *accepte = message(codeCB, "Reponse", "1");


    // Traitement de l'affichage en fonction du retour du serveur autorisation
    // 1 -> accepté, 0 -> refusé

    if(strcmp(reponse, accepte) == 0){
        //Payement accepté
        fprintf(stderr, "payement accepte\n");
        // printf("Payement accepte \n");
    }else
    {
        //Payement refusé
        fprintf(stderr, "payement refuse\n");
        // printf("Payement refuse \n");
    }
    
    
    return 0;
}


