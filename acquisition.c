#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    pid_t pid_fils;

    int descripteurTube[2];

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
    }
    

    return 0;
}