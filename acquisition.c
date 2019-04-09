#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "lectureEcriture.h"

int main(int argc, char const *argv[])
{
    pid_t pid_fils;

    int descripteurTubeDemande[2]; //on lit en fd[0] et on ecrit en fd[1]
    int descripteurTubeReponse[2];

    if(pipe(descripteurTubeDemande) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }
     if(pipe(descripteurTubeReponse) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }

    pid_fils = fork();
    
    if(pid_fils == 0){
        printf("fils\n");
        execlp("terminal.c","terminal.c","1","0",NULL);
    }

    else
    {
        printf("nous sommes dans le processus 'acquisition'\n");
        // printf("lecture de la demande : %s",litLigne(descripteurTubeDemande[0]));
        // ecritLigne(descripteurTubeReponse[1],"1");

        wait(NULL);
    }
    

    return 0;
}