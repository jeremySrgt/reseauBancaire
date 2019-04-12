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

struct argumentsRoutage{
        int recoiDeAutorisation;
        int ecritDansLeTerminal;
        
        
};

void *fonctionThreadRoutage(void *inputRoutage);

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

    struct argumentsRoutage *argumentRoutageInitialisation = (struct argumentsRoutage *)malloc(sizeof(struct argumentsRoutage));

    pthread_t threadRoutage;
    pthread_create(&threadRoutage, NULL, fonctionThreadRoutage, (void*)argumentRoutageInitialisation);

    argumentRoutageInitialisation->recoiDeAutorisation = descripteurTuberecoiDeAutorisation[0];

    argumentRoutageInitialisation->ecritDansLeTerminal = tableauDeTubeEcritureDansTerminal[1][1];

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
        sprintf(tuberecoiDeAutorisation, "%d", descripteurTuberecoiDeAutorisation[1]);
        execl("autorisation", "autorisation", tuberecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
        // execlp("xterm","xterm","-hold","-e","./autorisation",tuberecoiDeAutorisation, tubeEnvoiVersAutorisation, NULL);
    }

    int a = 0;

    while (a < nombreTerminaux)
    {

        char *demandeDuTerminal = litLigne(tableauDeTubeLectureDuTerminal[a][0]);

        printf("la demande envoyé PAR le terminal %d (ce nombre ne correspond pas vraiment au numéro exact du terminal) est : %s\n",a, demandeDuTerminal);
        ecritLigne(descripteurTubeEnvoiVersAutorisation[1], demandeDuTerminal);

        char *reponseAutorisation = litLigne(descripteurTuberecoiDeAutorisation[0]);

        printf("reponse de autorisation : %s\n", reponseAutorisation);
        ecritLigne(tableauDeTubeEcritureDansTerminal[a][1], reponseAutorisation);

        a++;
    }


    wait(NULL);


    return 0; 
}

void *fonctionThreadRoutage(void *inputRoutage){

    printf("Entrer dans le thread de routage \n");
    printf("Ce qui y avait dans le tube sortant d'autorisation : %d \n", ((struct argumentsRoutage*)inputRoutage)-> recoiDeAutorisation);

    char *reponseDeAutorisation =litLigne(((struct argumentsRoutage*)inputRoutage)-> recoiDeAutorisation);
    ecritLigne((((struct argumentsRoutage*)inputRoutage)-> ecritDansLeTerminal), reponseDeAutorisation);
    printf("cette reponse doit rejoindre le terminale avec le message : %s \n",reponseDeAutorisation);


    return NULL;
}