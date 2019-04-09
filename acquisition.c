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

    int descripteurTubeDemandeTerminal[2]; //on lit en fd[0] et on ecrit en fd[1]
    int descripteurTubeReponseTerminal[2];

    if(pipe(descripteurTubeDemandeTerminal) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }
     if(pipe(descripteurTubeReponseTerminal) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }

    pid_fils = fork();
    char tubeEcritureTerminal[100];
    char tubeLectureTerminal[100];
    
    if(pid_fils == 0){
        printf("fils\n");
        //utiliser un sprintf()
        sprintf(tubeEcritureTerminal,"%d",descripteurTubeReponseTerminal[0]);
        sprintf(tubeLectureTerminal,"%d",descripteurTubeDemandeTerminal[1]);
        execl("terminal","terminal",tubeLectureTerminal,tubeEcritureTerminal,NULL);
    }

    else
    {
        printf("nous sommes dans le processus 'acquisition'\n");
        // printf("lecture de la demande : %s",litLigne(descripteurTubeDemande[0]));
        // ecritLigne(descripteurTubeReponse[1],"1");
        

        printf("la demande envoyé PAR le terminal est : %s",litLigne(descripteurTubeDemandeTerminal[0]));



        // close(descripteurTubeDemandeTerminal[0]);
        // close(descripteurTubeDemandeTerminal[1]);

        // dup2(1,descripteurTubeReponseTerminal[1]);

        //ne pas oublier les \n a la fin des ecriture dans les tubes
        ecritLigne(descripteurTubeReponseTerminal[1],"|1234123412341234|Reponse|1|\n");

        // close(descripteurTubeReponseTerminal[1]);


        
        // printf("la repone envoyé VERS le terminal est : %s",litLigne(descripteurTubeDemandeTerminal[0]));

        wait(NULL);
    }
    

    return 0;
}