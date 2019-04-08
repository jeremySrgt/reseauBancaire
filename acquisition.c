#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "lectureEcriture.h"

int main(int argc, char const *argv[])
{
    pid_t pid_fils;

    int descripteurTube[2]; //on lit en fd[0] et on ecrit en fd[1]

    if(pipe(descripteurTube) != 0)
    {
        fprintf(stderr, "Erreur de création du tube.\n");
        return EXIT_FAILURE;
    }

    pid_fils = fork();

    if(pid_fils == 0){
        execlp("terminal.c",descripteurTube[1],descripteurTube[0],NULL);
    }

    else
    {
        printf("nous sommes dans le processus 'acquisition'");
        printf("lecture de la demande : %s",litLigne(descripteurTube[0]));
        ecritLigne(descripteurTube[1],"1");
    }
    

    return 0;
}