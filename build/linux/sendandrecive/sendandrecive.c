
#include <stdio.h>
#include <stdlib.h>
#include "MoCAAPI.h"

int main(int argc, char **argv)
{
    if(argc < 2)
        return -1;
    //GET first parameter
    initMocaRW(argv[1]);
    mocaWrite("Messaggio di prova", _MOCAWRITEINBOT);
    char buffer[500];
    mocaRead(buffer, 500, _MOCAREADFROMBOT);
    mocaWrite(buffer, _MOCAWRITEINBOT);
    return 0;
}