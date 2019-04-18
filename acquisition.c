#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "lectureEcriture.h"
#include "message.h"

/***************************************************************************************************************/
/* Executable acquisition permet de faire le lien entre les demandes du terminal et le serveur d'autorisation */
/* il permet aussi de communiquer avec le serveur interbancaire */
/***************************************************************************************************************/

void *routineThreadTerminaux(void *args);
void *fonctionThreadRoutage(void *inputRoutage);

int tableauDeRoutage[100][3];

struct argsThreadsLiaison
{
    int ecritureVersAutorisation;
    int lectureDeTerminal;
    int ecritureDansTerminal;
    int numeroDuTerminal;

    //ces champs sont juste pour les test
    // int recoiDeAutorisation;
    // int ecritVersTerminal;
};

struct argumentsRoutage
{
    int recoiDeAutorisation;
    int ecritDansLeTerminal;
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
    int descripteurTuberecoiDeAutorisation[2];

    char tubeEnvoiVersAutorisation[100];
    char tuberecoiDeAutorisation[100];

    if (pipe(descripteurTubeEnvoiVersAutorisation) != 0)
    {
        fprintf(stderr, "Erreur de creation du tube.\n");
        return EXIT_FAILURE;
    }
    if (pipe(descripteurTuberecoiDeAutorisation) != 0)
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

    //creation de n thread pour n terminaux
    pthread_t *threadsTerminaux = (pthread_t *)malloc(nombreTerminaux * sizeof(pthread_t));

    printf("FD envoi vers Autorisation %d\n", descripteurTubeEnvoiVersAutorisation[1]);
    printf("FD recoi de Autorisation %d\n", descripteurTuberecoiDeAutorisation[0]);

    //champs de test
    // argumentsDuThread->recoiDeAutorisation = descripteurTuberecoiDeAutorisation[0];

    //pas à la bonne place
    // for (int d = 0; d < nombreTerminaux; d++)
    // {
    //     pthread_create(&threadsTerminaux[d], NULL, routineThreadTerminaux, (void *) argumentThreadLiaison);
    // }
    struct argumentsRoutage *argumentRoutageInitialisation = (struct argumentsRoutage *)malloc(sizeof(struct argumentsRoutage));

    pthread_t threadRoutage;
    argumentRoutageInitialisation->recoiDeAutorisation = descripteurTuberecoiDeAutorisation[0];
    pthread_create(&threadRoutage, NULL, fonctionThreadRoutage, (void *)argumentRoutageInitialisation);

    int i = 0;

    while (i < nombreTerminaux)
    {

        struct argsThreadsLiaison *argumentsDuThread = (struct argsThreadsLiaison *)malloc(sizeof(struct argsThreadsLiaison));

        argumentsDuThread->ecritureVersAutorisation = descripteurTubeEnvoiVersAutorisation[1];

        argumentsDuThread->lectureDeTerminal = tableauDeTubeLectureDuTerminal[i][0];
        argumentsDuThread->ecritureDansTerminal = tableauDeTubeEcritureDansTerminal[i][1];
        argumentsDuThread->numeroDuTerminal = i;
        //champs de test thread
        // argumentsDuThread->ecritVersTerminal = tableauDeTubeEcritureDansTerminal[i][1];

        pthread_create(&threadsTerminaux[i], NULL, routineThreadTerminaux, (void *)argumentsDuThread);

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

    printf("table de routage (acq), numCB : %d , numTerm : %d , FD : %d \n", tableauDeRoutage[0][0], tableauDeRoutage[0][1], tableauDeRoutage[0][2]);

    if (fork() == 0)
    {
        printf("processus autorisation cree par acquisition\n");

        sprintf(tubeEnvoiVersAutorisation, "%d", descripteurTubeEnvoiVersAutorisation[0]);
        sprintf(tuberecoiDeAutorisation, "%d", descripteurTuberecoiDeAutorisation[1]);
        execl("autorisation", "autorisation", tuberecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
        // execlp("xterm","xterm","-hold","-e","./autorisation",tuberecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
    }

    // int a = 0;

    // while (a < nombreTerminaux)
    // {

    //     char *demandeDuTerminal = litLigne(tableauDeTubeLectureDuTerminal[a][0]);

    //     printf("la demande envoyé PAR le terminal %d (ce nombre ne correspond pas vraiment au numéro exact du terminal) est : %s\n", a, demandeDuTerminal);
    //     ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal);

    // char *reponseAutorisation = litLigne(descripteurTuberecoiDeAutorisation[0]);

    //     printf("reponse de autorisation : %s\n", reponseAutorisation);
    //     ecritLigne(tableauDeTubeEcritureDansTerminal[a][1], reponseAutorisation);

    //     a++;
    // }

    for (int i = 0; i < nombreTerminaux; i++)
    {
        pthread_join(threadsTerminaux[i], NULL);
    }

    wait(NULL);

    return 0;
}

void *routineThreadTerminaux(void *args)
{
    while (1)
    {
        printf("on est dans le thread d'un terminal\n");

        printf("FD ecriture autorisation: %d\n", ((struct argsThreadsLiaison *)args)->ecritureVersAutorisation);
        printf("FD lecture de Terminal: %d\n", ((struct argsThreadsLiaison *)args)->lectureDeTerminal);
        printf("numero de Terminal : %d\n", ((struct argsThreadsLiaison *)args)->numeroDuTerminal);

        char *demandeDuTerminal = litLigne(((struct argsThreadsLiaison *)args)->lectureDeTerminal);

        printf("DEMANDE DU TERMINAL ASSOCIE AU THREAD %s \n", demandeDuTerminal);

        char numeroCarte[17];
        char type[20];
        char valeurTransaction[100];

        decoupe(demandeDuTerminal, numeroCarte, type, valeurTransaction);
        int numTerm = ((struct argsThreadsLiaison *)args)->numeroDuTerminal;
        char numeroTerminal[100];
        sprintf(numeroTerminal, "%d", numTerm);

        //il faudrait trouver mieux pour initialiser les tableaux notamment avec des mallocs
        char fileDescripteurEcritureDansTerminali[100];
        sprintf(fileDescripteurEcritureDansTerminali, "%d", ((struct argsThreadsLiaison *)args)->ecritureDansTerminal);

        //code en dur qui serai bien de changer -------------------
        // strcpy(&tableauDeRoutage[numTerm][1],numeroTerminal);
        // strcpy(tableauDeRoutage[numTerm][1],"b");
        // strcpy(&tableauDeRoutage[numTerm][0],numeroCarte);
        // tableauDeRoutage[numTerm][0] = numeroCarte;
        // strcpy(&tableauDeRoutage[numTerm][2],fileDescripteurEcritureDansTerminali);

        tableauDeRoutage[numTerm][1] = numTerm;
        tableauDeRoutage[numTerm][0] = atoi(numeroCarte);
        tableauDeRoutage[numTerm][2] = ((struct argsThreadsLiaison *)args)->ecritureDansTerminal;

        printf("table de routage, numCB : %d , numTerm : %d , FD : %d \n", tableauDeRoutage[numTerm][0], tableauDeRoutage[numTerm][1], tableauDeRoutage[numTerm][2]);
        // tableauDeRoutage[numTerm][2] = fileDescripteurEcritureDansTerminali;

        printf("la demande envoyé PAR le terminal %d est : %s\n", ((struct argsThreadsLiaison *)args)->numeroDuTerminal, demandeDuTerminal);
        ecritLigne(((struct argsThreadsLiaison *)args)->ecritureVersAutorisation, demandeDuTerminal);

        //champs pour test

        // char *reponseAutorisation = litLigne(((struct argsThreadsLiaison*)args)->recoiDeAutorisation);

        // printf("reponse de autorisation : %s\n", reponseAutorisation);
        // ecritLigne(((struct argsThreadsLiaison*)args)->ecritVersTerminal, reponseAutorisation);
    }
    return NULL;
}

void *fonctionThreadRoutage(void *inputRoutage)
{
    while (1)
    {
        int numeroDuTerminalAEnvoyer;
        int terminalAEnvoyer;
        printf("Entrer dans le thread de routage \n");

        // printf("table de routage, numCB : %c , numTerm : %c , FD : %c \n",tableauDeRoutage[0][0],tableauDeRoutage[0][1],tableauDeRoutage[0][2]);
        // printf("Ce qu'il y avait dans le tube sortant d'autorisation : %d \n", ((struct argumentsRoutage *)inputRoutage)->recoiDeAutorisation);

        char *reponseDeAutorisation = litLigne(((struct argumentsRoutage *)inputRoutage)->recoiDeAutorisation);

        printf("REPONSE DE AUTORISATION %s \n", reponseDeAutorisation);

        char numeroCarte[17];
        char type[20];
        char valeurTransaction[100];

        decoupe(reponseDeAutorisation, numeroCarte, type, valeurTransaction);

        for (int i = 0; i < 100; i++)
        {
            if (tableauDeRoutage[i][0] == atoi(numeroCarte))
            {
                numeroDuTerminalAEnvoyer = tableauDeRoutage[i][1];
                terminalAEnvoyer = tableauDeRoutage[i][2];
            }
        }

        // int descripteurTerminalAEnvoyer = atoi(&terminalAEnvoyer);

        ecritLigne(terminalAEnvoyer, reponseDeAutorisation);
        printf("cette reponse doit rejoindre le terminale avec le message : %s \n", reponseDeAutorisation);
    }
    return NULL;
}

//TODO tableau de routage à initialiser et à incrémenter dans les threads + faire la liaison entre threads de routage et tous le programme