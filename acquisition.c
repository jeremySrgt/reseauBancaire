#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "lectureEcriture.h"

int main(int argc, char const *argv[])
{
    pid_t pid_terminal;
    pid_t pid_autorisation;

    int descripteurTubeDemandeTerminal[2]; //on lit en fd[0] et on ecrit en fd[1]
    int descripteurTubeReponseTerminal[2];
    int descripteurTubeEnvoiVersAutorisation[2];
    int descripteurTubeRecoiDeAutorisation[2];

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

    if(pipe(descripteurTubeEnvoiVersAutorisation) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }
     if(pipe(descripteurTubeRecoiDeAutorisation) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }

    pid_terminal = fork();

    char tubeEcritureTerminal[100];
    char tubeLectureTerminal[100];

    pid_autorisation = fork();

    char tubeEnvoiVersAutorisation[100];
    char tubeRecoiDeAutorisation[100];
    
    if(pid_terminal == 0){
        printf("terminal cree par acquisition\n");
        //utiliser un sprintf()
        sprintf(tubeEcritureTerminal,"%d",descripteurTubeReponseTerminal[0]);
        sprintf(tubeLectureTerminal,"%d",descripteurTubeDemandeTerminal[1]);
        execl("terminal","terminal",tubeLectureTerminal,tubeEcritureTerminal,NULL);
    }


    
    if(pid_autorisation == 0){
        printf("processus autorisation cree par acquisition\n");
        //utiliser un sprintf()
        sprintf(tubeEnvoiVersAutorisation,"%d",descripteurTubeEnvoiVersAutorisation[0]);
        sprintf(tubeRecoiDeAutorisation,"%d",descripteurTubeRecoiDeAutorisation[1]);
        execl("autorisation","autorisation",tubeRecoiDeAutorisation,tubeEnvoiVersAutorisation,NULL);
    }




        printf("nous sommes dans le processus 'acquisition'\n");
        // printf("lecture de la demande : %s",litLigne(descripteurTubeDemande[0]));
        // ecritLigne(descripteurTubeReponse[1],"1");
        

        printf("la demande envoyé PAR le terminal est : %s",litLigne(descripteurTubeDemandeTerminal[0]));

        ecritLigne(descripteurTubeEnvoiVersAutorisation[1],litLigne(descripteurTubeDemandeTerminal[0]));


        // close(descripteurTubeDemandeTerminal[0]);
        // close(descripteurTubeDemandeTerminal[1]);

        // dup2(1,descripteurTubeReponseTerminal[1]);

        //ne pas oublier les \n a la fin des ecriture dans les tubes

        char *reponseAutorisation = litLigne(descripteurTubeRecoiDeAutorisation[0]);
        // strcat(reponseAutorisation,"\n");
        printf("reponse autorisation : %s",reponseAutorisation);
        ecritLigne(descripteurTubeReponseTerminal[1],reponseAutorisation);
        // close(descripteurTubeDemandeTerminal[1]);

        // close(descripteurTubeReponseTerminal[1]);


        
        // printf("la repone envoyé VERS le terminal est : %s",litLigne(descripteurTubeDemandeTerminal[0]));

        wait(NULL);


    

    return 0;
}