/*
    MoCAAPI.h 
    Written by: Riccardo Fontanini
    Start date: 30 August 2017
    Note:   

     R O T A S
     O P E R A
     T E N E T
     A R E P O
     S A T O R
*/
#include "MoCAAPI.h"

char mocaToken[200];

int mocaWrite( const char *buffer, int type)
{
    if (buffer == NULL )
    {
        return -1;
    }

    if (strlen(buffer) <= 0 )
    {
        return -3;
    }

    if (type == _MOCAWRITEINBOT)
    {
        FILE *filewrite;
        char pathname[500];
        sprintf(pathname, "/tmp/%sMOCAwrite.txt", mocaToken);
        //if I can not open file 5 times, there is an arror
        int i;
        for (i = 0; i < _MAXWRITEATTEMPTS; i++)
        {
            if ((filewrite = fopen(pathname, "w")) != NULL)
            {
                break;
            }
            usleep(_USECONDSWRITING);
        }
        //if I can not open file
        if (i == _MAXWRITEATTEMPTS)
        {
            return -2;
        }
        //writing file
        fwrite(buffer, 1, strlen(buffer), filewrite);
        printf("file scritto\n");
        //closing file
        fclose(filewrite);
    }
    else
    {
        //writing in console
        printf(buffer);
    }

    return strlen(buffer);
}

int mocaRead(char *buffer, int sizeBuffer, int type)
{
    if (buffer == NULL || sizeBuffer <= 0 )
    {
        return -1;
    }

    char pathname[500];
    sprintf(pathname, "/tmp/%sMOCAread.txt", mocaToken);

    if (type == _MOCAREADFROMBOT)
    {

        //control if exits buffer
        if (access(pathname, F_OK) == -1)
        {
            return -4;
        }

        int c, i = 0;
        while (1)
        {
            c = 'a';
            FILE *file;
            //try to open
            if ((file = fopen(pathname, "r")) == NULL)
            {
                //nothing
            }
            //forever read
            for (i = 0; i < sizeBuffer && c != EOF && file != NULL; i++)
            {
                c = fgetc(file);

                if (c != EOF)
                    buffer[i] = c;
                else
                    buffer[i] = '\0';
            }
            //close file
            if (file != NULL)
            {
                fclose(file);
            }

            //if read something
            if (i > 1)
                break;
            else
            {
                usleep(_USECONDSREADING); //if read nothing wait a second and then try to read someting
            }
        }

        //clear file
        FILE *f;
        f = fopen(pathname, "w");
        if (f != NULL)
        {
            fclose(f);
        }
        return i - 1;
    }
    else
    {
        fgets(buffer, sizeBuffer, stdin);
    }

    return -3;
}

void initMocaRW(char *fromMOCA)
{

    strcpy(mocaToken, fromMOCA);
    char pathnameread[800];
    sprintf(pathnameread, "/tmp/%sMOCAread.txt", mocaToken);
    char pathnamewrite[800];
    sprintf(pathnamewrite, "/tmp/%sMOCAwrite.txt", mocaToken);

    FILE *f;
    f = fopen(pathnameread, "w");
    if (f != NULL)
    {
        fclose(f);
    }

    FILE *a;
    a = fopen(pathnamewrite, "w");
    if (a != NULL)
    {
        fclose(a);
    }
}

void mocaClose()
{
    usleep(1200000);
    mocaWrite("/closeconversation", _MOCAWRITEINBOT);
}