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


    if (argc != 2)
    {
        printf("pas assez d'argument\n");
        exit(1);
    }

    // int descripteurTubeDemandeTerminal[2]; //on lit en fd[0] et on ecrit en fd[1]
    // int descripteurTubeReponseTerminal[2];
    int descripteurTubeEnvoiVersAutorisation[2];
    int descripteurTubeRecoiDeAutorisation[2];

    // if (pipe(descripteurTubeDemandeTerminal) != 0)
    // {
    //     fprintf(stderr, "Erreur de creation du tube.\n");
    //     return EXIT_FAILURE;
    // }
    // if (pipe(descripteurTubeReponseTerminal) != 0)
    // {
    //     fprintf(stderr, "Erreur de creation du tube.\n");
    //     return EXIT_FAILURE;
    // }

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

    // char tubeEcritureTerminal[100];
    // char tubeLectureTerminal[100];
    char tubeEnvoiVersAutorisation[100];
    char tubeRecoiDeAutorisation[100];
    int nombreTerminaux = atoi(argv[1]);

    //creation d'un tableau a 2 dimensions pour contenir les tubes
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
            // sprintf(tubeEcritureTerminal, "%d", descripteurTubeReponseTerminal[0]);
            // sprintf(tubeLectureTerminal, "%d", descripteurTubeDemandeTerminal[1]);
            // execl("terminal", "terminal", tubeLectureTerminal, tubeEcritureTerminal, NULL);

            char tubeEcritureDansTerminal[100];
            char tubeLectureDuTerminal[100];

            sprintf(tubeEcritureDansTerminal, "%d", tableauDeTubeEcritureDansTerminal[i][0]);
            sprintf(tubeLectureDuTerminal, "%d", tableauDeTubeLectureDuTerminal[i][1]);

            printf("tubeEcritureDansTerminal %s\n",tubeEcritureDansTerminal);
            printf("tubeLectureDuTerminal %s\n",tubeLectureDuTerminal);
            
            execl("terminal","terminal", tubeLectureDuTerminal, tubeEcritureDansTerminal, NULL);
        }

        printf("Terminal %d crée\n", i);
        i++;
    }

    if (fork() == 0)
    {
        printf("processus autorisation cree par acquisition\n");
        //utiliser un sprintf()
        sprintf(tubeEnvoiVersAutorisation, "%d", descripteurTubeEnvoiVersAutorisation[0]);
        sprintf(tubeRecoiDeAutorisation, "%d", descripteurTubeRecoiDeAutorisation[1]);
        execl("autorisation", "autorisation", tubeRecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
    }

    printf("nous sommes dans le processus 'acquisition'\n");
    // printf("lecture de la demande : %s",litLigne(descripteurTubeDemande[0]));
    // ecritLigne(descripteurTubeReponse[1],"1");

    //!!!!! litLigne VIDE LE TUBE A LA LECTURE !!!!!!

    // printf("la demande envoyé PAR le terminal est : %s",litLigne(descripteurTubeDemandeTerminal[0]));

    int a = 0;
    printf("ACQUISITION\n");

    while (a < nombreTerminaux)
    {
        printf("BOUCLE WHILE D'ACQUISITION %d %d\n",a,tableauDeTubeLectureDuTerminal[a][0]);

        char *demandeDuTerminal = litLigne(tableauDeTubeLectureDuTerminal[a][0]);
        printf("APRES LITLIGNE d'acquisition\n");
        printf("la demande envoyé PAR le terminal %d est : %s\n",a, demandeDuTerminal);
        ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal);

        char *reponseAutorisation = litLigne(descripteurTubeRecoiDeAutorisation[0]);

        printf("reponse de autorisation : %s\n", reponseAutorisation);
        ecritLigne(tableauDeTubeEcritureDansTerminal[a][1], reponseAutorisation);

        a++;
    }


        // char *demandeDuTerminal = litLigne(tableauDeTubeLectureDuTerminal[0][0]);
        // printf("APRES LITLIGNE d'acquisition\n");
        // printf("la demande envoyé PAR le terminal [X] est : %s\n", demandeDuTerminal);
        // ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal);

        // char *reponseAutorisation = litLigne(descripteurTubeRecoiDeAutorisation[0]);

        // printf("reponse de autorisation : %s\n", reponseAutorisation);
        // ecritLigne(tableauDeTubeEcritureDansTerminal[0][1], reponseAutorisation);


        // char *demandeDuTerminal1 = litLigne(tableauDeTubeLectureDuTerminal[1][0]);
        // printf("APRES LITLIGNE d'acquisition\n");
        // printf("la demande envoyé PAR le terminal [X] est : %s\n", demandeDuTerminal1);
        // ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal1);

        // char *reponseAutorisation1 = litLigne(descripteurTubeRecoiDeAutorisation[0]);

        // printf("reponse de autorisation : %s\n", reponseAutorisation1);
        // ecritLigne(tableauDeTubeEcritureDansTerminal[1][1], reponseAutorisation1);




    // printf("just aavant ecrit ligne de acquisition\n");

    // printf("just apres ecritligne de acquisition\n");

    // close(descripteurTubeDemandeTerminal[0]);
    // close(descripteurTubeDemandeTerminal[1]);

    // dup2(1,descripteurTubeReponseTerminal[1]);

        // char *reponseAutorisation = litLigne(descripteurTubeRecoiDeAutorisation[0]);
        // // strcat(reponseAutorisation,"\n");
        //         ecritLigne(descripteurTubeReponseTerminal[1],reponseAutorisation);
        // printf("reponse de autorisation : %s",reponseAutorisation);
        // // close(descripteurTubeDemandeTerminal[1]);

    // strcat(reponseAutorisation,"\n");

    // close(descripteurTubeDemandeTerminal[1]);

    // close(descripteurTubeReponseTerminal[1]);

    // printf("la repone envoyé VERS le terminal est : %s",litLigne(descripteurTubeDemandeTerminal[0]));

    wait(NULL);


    return 0;
}