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

//on met en dur les 2 tubes qui vont vers acquisition

    int descripteurFichier = open("InfoClient.txt", O_RDONLY);
    char *codeCB = litLigne(descripteurFichier);

    codeCB[strlen(codeCB)-1] ='\0';
    // char *codeCB = "123456";


    dup2(df1,1); //ecrit dans argv1
    dup2(df2,0);//lit dans argv2

    aleainit();
    char argentAleatoire[70];
    sprintf(argentAleatoire, "%d", alea(5,20));
    printf("argent aleatoire : %s\n",argentAleatoire);

    char *demandeArgent = message(codeCB, "Demande", argentAleatoire);


    // int decoupage = decoupe(demandeArgent,codeCB,"demande",argentAleatoire);

    // if(decoupage==0){
    //     printf("erreur decoupage message");
    // }
    // printf("%s", demandeArgent);

    ecritLigne(df1,demandeArgent);

    char *reponse = litLigne(df2);

    printf("message lue : %s \n", reponse);
    char *accepte = "1\n";

    if(strcmp(reponse, accepte) == 0){
        printf("%s",message(codeCB,"Reponse","accepte"));
        // printf("%s|Reponse|accepte|\n", codeCB);
    }else
    {
        printf("%s",message(codeCB,"Reponse","refuse"));
    }
    
    
    return 0;
}


