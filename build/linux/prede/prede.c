/*
COMPILED WITH: gcc -g -Wall -o prede prede.c
*/

#include <stdio.h>
#include <stdlib.h>
#include "MoCAAPI.h"

//need to compute avolution of species
void model(int x1, int x2, int *newX1, int *newX2);

//global values for model
double a = 0.20;
double b = 0.005;
double c = 0.0005;
double d = 0.2;

//global values start 
int x1_0 = 500;
int x2_0 = 20;

int main(int argc, char **argv)
{
    int polli = x1_0;
    int volpi = x2_0;

    if(argc < 2)
        return -1;
    //GET first parameter and setup comunication
    initMocaRW(argv[1]);
    int i = 0;
    for(i = 0; i<20; i++)
    {
        char s[50];
        sprintf(s, "polli: %d volpi: %d\n", polli, volpi);
        mocaWrite(s, _MOCAWRITEINBOT);
        model(polli, volpi, &polli, &volpi);
        sleep(2);
    }

    //close comunication
    mocaClose();
    /*if(argc < 2)
        return -1;
    //GET first parameter and setup comunication
    initMocaRW(argv[1]);
    

    
    //write message TO bot
    mocaWrite("Messaggio di prova", _MOCAWRITEINBOT);
    char buffer[500];
    //read message to bot
    mocaRead(buffer, 500, _MOCAREADFROMBOT);
    //retransmit message to bot
    mocaWrite(buffer, _MOCAWRITEINBOT);

    
    //close comunication
    mocaClose();*/
    return 0;
}

void model(int x1, int x2, int *newX1, int *newX2)
{
    
    *newX1 = x1 + (int)( a * (double)x1 - b * (double)x1 * (double)x2 );

    *newX2 = x2 + (int)( c * (double)x1 * (double)x2 - d * (double)x2);
}