#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{

int df1 = atoi(argv[1]);
int df2 = atoi(argv[2]);

//on met en dur les 2 tubes qui vont vers acquisition


    dup2(df1,1); //ecrit dans argv1
    dup2(df2,0);//lit dans arv2

    char messagelue[50];

    write(df1,"test\n",5);

    read(df2,messagelue,5);

    printf("message lue : %s \n", messagelue);


    
    return 0;
}


