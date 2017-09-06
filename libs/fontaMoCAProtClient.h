
#include <stdlib.h>
#include <stdio.h> /* printf, sprintf */
#include <string.h>
#ifndef _FONTAMOCAPROTCLIENT_H_
#define _FONTAMOCAPROTCLIENT_H_


/*------------------------------------------ TYPE MESSAGE ARRIVED -----------------------------------------------------*/
/*
type of message read from readMoCAProtMessage
define a createdCOllateral message
*/
#ifndef MOCACREATEDCOLLATERAL 
#define MOCACREATEDCOLLATERAL 1
#endif
/*
type of message read from readMoCAProtMessage
define a createdCOllateral message
*/
#ifndef MOCADESTROYCOLLATERAL 
#define MOCADESTROYCOLLATERAL 2
#endif
/*
type of message read from readMoCAProtMessage
define a createdCOllateral message
*/
#ifndef MOCANOTFOUNDTYPE 
#define MOCANOTFOUNDTYPE 0
#endif

/*********************************************** COSTANTS ************************************************************/

/*
MAX NUMBER OF PARAMETERS
*/
#ifndef MAXMOCAPARAMS 
#define MAXMOCAPARAMS 10
#endif

/*
MAX LENGTH OF PARAMETERS
*/
#ifndef MAXMOCAPARAMSLENGTH 
#define MAXMOCAPARAMSLENGTH 500
#endif


#ifndef MAXTYPELENTH 
#define MAXTYPELENTH 50
#endif


/**************************************************** PROTOTYPES *****************************************************/

struct MoCAMessage
{
    int type;
    char typeField[MAXTYPELENTH];
    char params[MAXMOCAPARAMS][MAXMOCAPARAMSLENGTH];
};


/**************************** READ MESSAGE ************************************************************************/
/*
Reads a message and get information about the message, returns type of message
*/
int readMoCAProtMessage(char *message, struct MoCAMessage *parsedMessage);


/**************************************************** WRITE MESSAGE ************************************************/

/*
Copy in message the right message to send
*/
char *startMainCOnversation();

#endif