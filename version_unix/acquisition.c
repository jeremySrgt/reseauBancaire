#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include "lectureEcriture.h"
#include "message.h"

/***************************************************************************************************************/
/* Executable acquisition permet de faire le lien entre les demandes du terminal et le serveur d'autorisation */
/* il permet aussi de communiquer avec le serveur interbancaire */
/***************************************************************************************************************/

void *routineThreadTerminaux(void *args);
void *fonctionThreadRoutage(void *inputRoutage);

int tableauDeRoutage[100][3];
int incrementPositionTableRoutage = 0;

//allocation d'un semaphore
sem_t semaphore;

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

typedef struct argumentsRoutage
{
    int recoiDeAutorisation;
    int ecritDansLeTerminal;
} argumentsRoutage;

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

    //creation d'un semaphore nommé pour mac car semaphore non nommé ne fonctionnent pas sur mac or nous avons travaillé sur mac durant tout le projet
    // le dernnier argument donne le nombre de place du semaphore
    sem_init(&semaphore,0,1);

    //creation de n thread pour n terminaux
    pthread_t *threadsTerminaux = (pthread_t *)malloc(nombreTerminaux * sizeof(pthread_t));

    // printf("FD envoi vers Autorisation %d\n", descripteurTubeEnvoiVersAutorisation[1]);
    // printf("FD recoi de Autorisation %d\n", descripteurTuberecoiDeAutorisation[0]);

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

        int ppp = fork();

        if (ppp < 0)
        {
            fprintf(stderr, "fork erreur");
        }
        if (ppp == 0)
        {
            fprintf(stderr, "Terminal %d cree par acquisition\n", i);

            char tubeEcritureDansTerminal[100];
            char tubeLectureDuTerminal[100];
            char numeroTerminal[30];

            sprintf(tubeEcritureDansTerminal, "%d", tableauDeTubeEcritureDansTerminal[i][0]);
            sprintf(tubeLectureDuTerminal, "%d", tableauDeTubeLectureDuTerminal[i][1]);
            sprintf(numeroTerminal, "%d", i);

            //execution de execl sous mac os
            //if (execl("terminal", "terminal", tubeLectureDuTerminal, 			tubeEcritureDansTerminal, numeroTerminal, NULL) == -1)
           // {
              //  fprintf(stderr, "EXEC ERREUR\n");
           // }
            //execution de xterm sous linux (xterm ne fonctionne pas sous mac os)
            execlp("xterm","xterm","-hold","-e","./terminal",tubeLectureDuTerminal, tubeEcritureDansTerminal,numeroTerminal, NULL);
        }

        i++;
    }



    if (fork() == 0)
    {
        printf("processus autorisation cree par acquisition\n");

        sprintf(tubeEnvoiVersAutorisation, "%d", descripteurTubeEnvoiVersAutorisation[0]);
        sprintf(tuberecoiDeAutorisation, "%d", descripteurTuberecoiDeAutorisation[1]);
        //pour mac os
        //execl("autorisation", "autorisation", tuberecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
        execlp("xterm","xterm","-hold","-e","./autorisation",tuberecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
    }


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

        int numTerm = ((struct argsThreadsLiaison *)args)->numeroDuTerminal;

        //zone de debug-------
        // printf("on est dans le thread du terminal %d \n", numTerm);
        // printf("FD ecriture autorisation: %d\n", ((struct argsThreadsLiaison *)args)->ecritureVersAutorisation);
        // printf("FD lecture de Terminal: %d\n", ((struct argsThreadsLiaison *)args)->lectureDeTerminal);
        // printf("numero de Terminal : %d\n", ((struct argsThreadsLiaison *)args)->numeroDuTerminal);
        //----------

        char *demandeDuTerminal = litLigne(((struct argsThreadsLiaison *)args)->lectureDeTerminal);

        // printf("DEMANDE DU TERMINAL ASSOCIE AU THREAD %s \n", demandeDuTerminal);

        char numeroCarte[17];
        char type[20];
        char valeurTransaction[100];

        int r = decoupe(demandeDuTerminal, numeroCarte, type, valeurTransaction);
        if (r == 0)
            perror("Pb terminal");

        char numeroTerminal[100];
        sprintf(numeroTerminal, "%d", numTerm);

        //il faudrait trouver mieux pour initialiser les tableaux notamment avec des mallocs
        char fileDescripteurEcritureDansTerminali[100];
        sprintf(fileDescripteurEcritureDansTerminali, "%d", ((struct argsThreadsLiaison *)args)->ecritureDansTerminal);

        //code en dur qu'il serai bien de changer -------------------
        // strcpy(&tableauDeRoutage[numTerm][1],numeroTerminal);
        // strcpy(tableauDeRoutage[numTerm][1],"b");
        // strcpy(&tableauDeRoutage[numTerm][0],numeroCarte);
        // tableauDeRoutage[numTerm][0] = numeroCarte;
        // strcpy(&tableauDeRoutage[numTerm][2],fileDescripteurEcritureDansTerminali);


        //section critique    
        sem_wait(&semaphore);

        tableauDeRoutage[incrementPositionTableRoutage][1] = numTerm;
        tableauDeRoutage[incrementPositionTableRoutage][0] = atoi(numeroCarte);
        tableauDeRoutage[incrementPositionTableRoutage][2] = ((struct argsThreadsLiaison *)args)->ecritureDansTerminal;

        incrementPositionTableRoutage++;

        

        // printf("table de routage, numCB : %d , numTerm : %d , FD : %d \n", tableauDeRoutage[numTerm][0], tableauDeRoutage[numTerm][1], tableauDeRoutage[numTerm][2]);
        // tableauDeRoutage[numTerm][2] = fileDescripteurEcritureDansTerminali;

        printf("la demande envoyé PAR le terminal %d est : %s\n", ((struct argsThreadsLiaison *)args)->numeroDuTerminal, demandeDuTerminal);
        ecritLigne(((struct argsThreadsLiaison *)args)->ecritureVersAutorisation, demandeDuTerminal);


        //mettre le sem_post apres le ecritligne permet de s'asssurer que les demandes sont ecrites dans l'ordre dans le tube de autorisations
        //et que du coup elle seront renvoyés au bon terminal
        sem_post(&semaphore);
        //fin section critique

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
        int terminalAEnvoyer = 0;

        // printf("table de routage, numCB : %c , numTerm : %c , FD : %c \n",tableauDeRoutage[0][0],tableauDeRoutage[0][1],tableauDeRoutage[0][2]);
        // printf("Ce qu'il y avait dans le tube sortant d'autorisation : %d \n", ((struct argumentsRoutage *)inputRoutage)->recoiDeAutorisation);

        char *reponseDeAutorisation = litLigne(((struct argumentsRoutage *)inputRoutage)->recoiDeAutorisation);

        fprintf(stderr, "Entrer dans le thread de routage \n");

        // fprintf(stderr, "REPONSE DE AUTORISATION %s \n", reponseDeAutorisation);

        char numeroCarte[17];
        char type[20];
        char valeurTransaction[100];

        decoupe(reponseDeAutorisation, numeroCarte, type, valeurTransaction);

        // fprintf(stderr, "Valeur des champ recu de autorisation numeroCarte %s - type %s - valeurTransaction %s - \n", numeroCarte, type, valeurTransaction);

        // fprintf(stderr, "juste avant l'entrer dans la table de routage du thread de routage \n");

        sem_wait(&semaphore);


        //zone de debug --------
        // fprintf(stderr,"tableauDeRoutage[i][0] 1 %d \n",tableauDeRoutage[0][0]);
        // fprintf(stderr,"tableauDeRoutage[i][0] 2 %d \n",tableauDeRoutage[1][0]);
        // fprintf(stderr,"tableauDeRoutage[i][0] 3 %d \n",tableauDeRoutage[2][0]);
        //-------



        for (int i = 0; i < 100; i++)
        {

            if (tableauDeRoutage[i][0] == atoi(numeroCarte))
            {
                // fprintf(stderr,"tableauDeRoutage[i][0] AVANT MODIF %d \n",tableauDeRoutage[i][0]);
                numeroDuTerminalAEnvoyer = tableauDeRoutage[i][1];
                terminalAEnvoyer = tableauDeRoutage[i][2];
                tableauDeRoutage[i][0] = 0;
                tableauDeRoutage[i][1] = 0;
                tableauDeRoutage[i][2] = 0;

                // fprintf(stderr,"tableauDeRoutage APRES MODIF %d %d %d \n",tableauDeRoutage[i][0],tableauDeRoutage[i][1],tableauDeRoutage[i][2]);
                break;
            }
        }
        sem_post(&semaphore);

        // fprintf(stderr, "juste apres la sortie de la table de routage du thread de routage | terminal à envoyer %d \n", terminalAEnvoyer);

        // int descripteurTerminalAEnvoyer = atoi(&terminalAEnvoyer);

        ecritLigne(terminalAEnvoyer, reponseDeAutorisation);
        fprintf(stderr, "cette reponse doit rejoindre le terminale avec le message : %s \n", reponseDeAutorisation);
    }
    return NULL;
}
