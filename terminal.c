#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "message.h"
#include "lectureEcriture.h"

int main(int argc, char const *argv[])
{

int df1 = atoi(argv[1]);
int df2 = atoi(argv[2]);

printf("executable terminal cree a partir de acquisition");

//on met en dur les 2 tubes qui vont vers acquisition


    dup2(df1,1); //ecrit dans argv1
    dup2(df2,0);//lit dans arv2


    char *demandeArgent = message("1234123412341234", "Demande", "50");

    ecritLigne(df1,demandeArgent);

    char *reponse = litLigne(df2);

    printf("message lue : %s \n", reponse);

    if(strcmp(reponse, "1") == 0){
        printf("1234123412341234|Reponse|accepte \n");
    }else
    {
        printf("1234123412341234|Reponse|refuse \n");
    }
    



    
    return 0;
}


