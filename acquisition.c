#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include "lectureEcriture.h"

/***************************************************************************************************************/
/* Executable acquisition permet de faire le lien entre les demandes du terminal et le serveur d'autorisation */
/* il permet aussi de communiquer avec le serveur interbancaire */
/***************************************************************************************************************/


int main(int argc, char const *argv[])
{


    if (argc != 2)
    {
        printf("pas assez d'argument\n");
        exit(1);
    }

    int nombreTerminaux = atoi(argv[1]);

    
    //creation des tubes de communication avec Autorisation
    int descripteurTubeEnvoiVersAutorisation[2];
    int descripteurTubeRecoiDeAutorisation[2];

    char tubeEnvoiVersAutorisation[100];
    char tubeRecoiDeAutorisation[100];

    if (pipe(descripteurTubeEnvoiVersAutorisation) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }
    if (pipe(descripteurTubeRecoiDeAutorisation) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }


    
    

    //creation d'un tableau a 2 dimensions pour contenir les tubes de commincation avec les terminaux
    int tableauDeTubeEcritureDansTerminal[nombreTerminaux][2];
    int tableauDeTubeLectureDuTerminal[nombreTerminaux][2];

    //creation des tubes d'ecriture vers le terminal
    for (int b = 0; b < nombreTerminaux; b++)
    {
        pipe(tableauDeTubeEcritureDansTerminal[b]);
    }

    //creation des tubes lecture des messages ecrit par le terminal
    for (int c = 0; c < nombreTerminaux; c++)
    {
        pipe(tableauDeTubeLectureDuTerminal[c]);
    }

    int i = 0;

    while (i < nombreTerminaux)
    {
        if (fork() == 0)
        {
            printf("terminal %d cree par acquisition\n", i);

            char tubeEcritureDansTerminal[100];
            char tubeLectureDuTerminal[100];

            sprintf(tubeEcritureDansTerminal, "%d", tableauDeTubeEcritureDansTerminal[i][0]);
            sprintf(tubeLectureDuTerminal, "%d", tableauDeTubeLectureDuTerminal[i][1]);

            printf("tubeEcritureDansTerminal %s\n",tubeEcritureDansTerminal);
            printf("tubeLectureDuTerminal %s\n",tubeLectureDuTerminal);
            
            execl("terminal","terminal", tubeLectureDuTerminal, tubeEcritureDansTerminal, NULL);
            //execution de xterm sous linux (xterm ne fonctionne pas sous mac os)
            // execlp("xterm","xterm","-hold","-e","./terminal",tubeLectureDuTerminal, tubeEcritureDansTerminal, NULL);
        }

        printf("Terminal %d crée\n", i);
        i++;
    }

    if (fork() == 0)
    {
        printf("processus autorisation cree par acquisition\n");

        sprintf(tubeEnvoiVersAutorisation, "%d", descripteurTubeEnvoiVersAutorisation[0]);
        sprintf(tubeRecoiDeAutorisation, "%d", descripteurTubeRecoiDeAutorisation[1]);
        execl("autorisation", "autorisation", tubeRecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
        // execlp("xterm","xterm","-hold","-e","./autorisation",tubeRecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
    }

    int a = 0;

    while (a < nombreTerminaux)
    {

        char *demandeDuTerminal = litLigne(tableauDeTubeLectureDuTerminal[a][0]);

        printf("la demande envoyé PAR le terminal %d (ce nombre ne correspond pas vraiment au numéro exact du terminal) est : %s\n",a, demandeDuTerminal);
        ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal);

        char *reponseAutorisation = litLigne(descripteurTubeRecoiDeAutorisation[0]);

        printf("reponse de autorisation : %s\n", reponseAutorisation);
        ecritLigne(tableauDeTubeEcritureDansTerminal[a][1], reponseAutorisation);

        a++;
    }

    // close(descripteurTubeDemandeTerminal[0]);
    // close(descripteurTubeDemandeTerminal[1]);

    wait(NULL);


    return 0;
}