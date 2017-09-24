/*
    fontaMoCAProtClient.c
    Written by: Riccardo Fontanini
    Start date: 10 August 2017
    Note:  

    
    R O T A S
    O P E R A
    T E N E T
    A R E P O
    S A T O R

*/
#include "fontaMoCAProtClient.h"

int readMoCAProtMessage(char *message, struct MoCAMessage *parsedMessage)
{
    parsedMessage->type = MOCANOTFOUNDTYPE;

    if(message == NULL)
    {
        //null string
        return -1;
    }
    if(strlen(message) <= 8) //*MOCABOT
    {
        //no string 
        return -2;
    }
    //SPLIT STRING
    
    //copy the message
    char copy_message[strlen(message)+1];

    memcpy(copy_message, message, strlen(message));
    copy_message[strlen(message)] = 0;
    //create token
    char * pch = strtok(copy_message, " ");
    int i;
    for (i = 0; pch != NULL && i < MAXMOCAPARAMS + 2; i++)
    {
        if( i == 0)
        {   //first must be *MOCABOT
            if(strcmp(pch, "*MOCABOT") != 0)
            {
                return -3;
            }
        }
        else if(i == 1)
        {
            //copy the type of message in type
            if(strlen(pch)> MAXTYPELENTH - 1)
            {
                //too much large to be copied
                free(copy_message);
                return -4;
            }
            strcpy(parsedMessage->typeField, pch);
            //set type 
            if(strcmp(parsedMessage->typeField, "CREATEDCOLLATERAL") == 0)
            {
                parsedMessage->type = MOCACREATEDCOLLATERAL;
            }
            else if(strcmp(parsedMessage->typeField, "DESTROYCOLLATERAL") == 0)
            {
                parsedMessage->type = MOCADESTROYCOLLATERAL;
            }
            else
            {
                //not found type, so return false ( 0 )
                parsedMessage->type = MOCANOTFOUNDTYPE;
            }
        }
        else
        {
            //save parameters
            if(strlen(pch) > MAXMOCAPARAMSLENGTH -1)
            {
                //very huge param, not allowed
                return -5;
            }
            strcpy(parsedMessage->params[i-2], pch);

        }

        pch = strtok(NULL, " ");
    }
    return parsedMessage->type;


}

char *startMainCOnversation()
{
    return "*MOCACLIENT STARTMAINCONVERSATION";
}