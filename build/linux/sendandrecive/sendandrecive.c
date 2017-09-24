/*
COMPILED WITH:
*/


#include <stdio.h>
#include <stdlib.h>
#include "MoCAAPI.h"

int main(int argc, char **argv)
{
    if(argc < 2)
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
    mocaClose();
    return 0;
}