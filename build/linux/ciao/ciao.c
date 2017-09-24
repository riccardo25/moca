

//compile gcc -g -Wall -o ciao ciao.c
#include <stdlib.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    printf("Ciao!\n");
    char buffer[5000];

    fgets(buffer, 5000, stdin);
    printf("uscio %s\n", buffer);
    return 0;
}