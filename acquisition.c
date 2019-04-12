#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "lectureEcriture.h"

/***************************************************************************************************************/
/* Executable acquisition permet de faire le lien entre les demandes du terminal et le serveur d'autorisation */
/* il permet aussi de communiquer avec le serveur interbancaire */
/***************************************************************************************************************/

void *routineThreadTerminaux(void *args);

struct argsThreadsLiaison
{
    int ecritureVersAutorisation;
    int lectureDeTerminal;
    int numeroDuTerminal;
    //ces champs sont juste pour les test
    int recoiDeAutorisation;
    int ecritVersTerminal;
};

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

    struct argsThreadsLiaison *argumentsDuThread = (struct argsThreadsLiaison *)malloc(sizeof(struct argsThreadsLiaison));
    //creation de n thread pour n terminaux
    pthread_t *threadsTerminaux = (pthread_t *)malloc(nombreTerminaux * sizeof(pthread_t));

    printf("FD envoi vers Autorisation %d\n",descripteurTubeEnvoiVersAutorisation[1]);
    printf("FD recoi de Autorisation %d\n",descripteurTubeRecoiDeAutorisation[0]);

    argumentsDuThread->ecritureVersAutorisation = descripteurTubeEnvoiVersAutorisation[1];
    //champs de test
    argumentsDuThread->recoiDeAutorisation = descripteurTubeRecoiDeAutorisation[0];

    //pas à la bonne place
    // for (int d = 0; d < nombreTerminaux; d++)
    // {
    //     pthread_create(&threadsTerminaux[d], NULL, routineThreadTerminaux, (void *) argumentThreadLiaison);
    // }

    int i = 0;

    while (i < nombreTerminaux)
    {
        
        argumentsDuThread->lectureDeTerminal = tableauDeTubeLectureDuTerminal[i][0];
        argumentsDuThread->numeroDuTerminal = i;
        //champs de test thread
        argumentsDuThread->ecritVersTerminal = tableauDeTubeEcritureDansTerminal[i][1];

        
        pthread_create(&threadsTerminaux[i], NULL, routineThreadTerminaux, (void *) argumentsDuThread);


        if (fork() == 0)
        {
            printf("terminal %d cree par acquisition\n", i);

            char tubeEcritureDansTerminal[100];
            char tubeLectureDuTerminal[100];

            sprintf(tubeEcritureDansTerminal, "%d", tableauDeTubeEcritureDansTerminal[i][0]);
            sprintf(tubeLectureDuTerminal, "%d", tableauDeTubeLectureDuTerminal[i][1]);


            printf("tubeEcritureDansTerminal %s\n", tubeEcritureDansTerminal);
            printf("tubeLectureDuTerminal %s\n", tubeLectureDuTerminal);

            execl("terminal", "terminal", tubeLectureDuTerminal, tubeEcritureDansTerminal, NULL);
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

    // int a = 0;

    // while (a < nombreTerminaux)
    // {

    //     char *demandeDuTerminal = litLigne(tableauDeTubeLectureDuTerminal[a][0]);

    //     printf("la demande envoyé PAR le terminal %d (ce nombre ne correspond pas vraiment au numéro exact du terminal) est : %s\n", a, demandeDuTerminal);
    //     ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal);

    //     char *reponseAutorisation = litLigne(descripteurTubeRecoiDeAutorisation[0]);

    //     printf("reponse de autorisation : %s\n", reponseAutorisation);
    //     ecritLigne(tableauDeTubeEcritureDansTerminal[a][1], reponseAutorisation);

    //     a++;
    // }

    // close(descripteurTubeDemandeTerminal[0]);
    // close(descripteurTubeDemandeTerminal[1]);

    wait(NULL);

    return 0;
}

void *routineThreadTerminaux(void *args){

    printf("on est dans le thread\n");

    printf("FD ecriture autorisation: %d\n", ((struct argsThreadsLiaison*)args)->ecritureVersAutorisation);
    printf("FD lecture de Terminal: %d\n", ((struct argsThreadsLiaison*)args)->lectureDeTerminal);
    printf("numero de Terminal : %d\n", ((struct argsThreadsLiaison*)args)->numeroDuTerminal);

    char *demandeDuTerminal = litLigne(((struct argsThreadsLiaison*)args)->lectureDeTerminal);

        printf("la demande envoyé PAR le terminal %d est : %s\n",((struct argsThreadsLiaison*)args)->numeroDuTerminal , demandeDuTerminal);
        ecritLigne(((struct argsThreadsLiaison*)args)->ecritureVersAutorisation, demandeDuTerminal);

        //champs pour test

        char *reponseAutorisation = litLigne(((struct argsThreadsLiaison*)args)->recoiDeAutorisation);

        printf("reponse de autorisation : %s\n", reponseAutorisation);
        ecritLigne(((struct argsThreadsLiaison*)args)->ecritVersTerminal, reponseAutorisation);

}