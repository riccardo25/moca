/*
    fontabotconnector.h
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:       https://linuxprograms.wordpress.com/2010/05/20/json-c-libjson-tutorial/
                https://github.com/json-c/json-c
        Library to link some stuff



    R O T A S
    O P E R A
    T E N E T
    A R E P O
    S A T O R
*/

#include <pthread.h>
#include "fontahttp.h"
#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */
#include <json-c/json.h>//json parser
#include "fontautil.h"
#include "fontaMoCAProtClient.h"

#ifndef _FONTABOTCONNECTOR_H_
#define _FONTABOTCONNECTOR_H_
/****************************************** MACROS ***********************************************************/
/*DEBUG FUNCTION*/
#if defined DEBUG || defined DEBUGFONTABOTCONNECTOR //defined in fontautil.h
        //allow debug all data
        
        //debug data from principal conversation
        #ifndef DEBUGDATAPRINCIPALCONVERSATION
        #define DEBUGDATAPRINCIPALCONVERSATION
        #endif

        //debug data from collateral conversation
        #ifndef DEBUGDATACOLLATERALCONVERSATION
        #define DEBUGDATACOLLATERALCONVERSATION
        #endif

        //debug what i need to write in service (message from collateral)
        #ifndef DEBUGWRITEINSERVICE
        #define DEBUGWRITEINSERVICE
        #endif

        //debug message sent to bot
        #ifdef DEBUGSENDMESSAGETOBOT
        #define DEBUGSENDMESSAGETOBOT
        #endif
#endif

/*#ifndef USEPOLL
#define USEPOLL 
#endif*/


/****************************************** STRUCTS **********************************************************/
/*
Used to connect a message from MontessoroBOT to mocamain
*/
typedef void (*CollateralHandler)(struct MoCAMessage message);

typedef struct BotConnectionParams
{
        char conversationId[CONVERSATIONIDSIZE]; //conversationID to excange information with bot
        char token[TOKENSIZE]; //token used to connect to bot
        char userID[USERIDSIZE]; //id of this terminal
        pthread_t pollerTID; //thread id of poller
        pthread_t pollerCollateralConversationID;//thread id of collateral conversation poller
        char *pollWatermark[WATERMARKSIZE];
        CollateralHandler handler; //used to create a connection between BotPOLLER and mocamain
        
}BotConnectionParams;

/**************************************** PROTOTYPES *********************************************************/
/*
Start a conversation and insert in token, a retrived token from server
*/
int openConversation(char *conversationId, char *token );

/*
Starts the poller
*/
int startPollHttp(void * arg);


/*
Send message to BOT and recive information
*/
int sendMessagetoBOT(char *message, BotConnectionParams conn, char **result);

/*
Send message to BOT to comunicate closing conversation.
Return <0 failure
*/
int sendEndConversationtoBOT(BotConnectionParams conn, char **result);
/*
POLL the bot via direct API (Main CONVERSATION)
*/
int startPollBOT(void *arg);

/**
Start new Service of existing program in subfolder
Service passed
Return <0 there is an error:    -1 is pipeline creating problem
                                -2 is fork creating problem

*/
int startService(void *arg);

/**
Clear the specified service
*/
void clearService(ServiceDescriptor *service);


/**
Start new Poller that polls all collateral conversations searching new messages
Service passed
Return <0 there is an error

*/
void startPollCollateralConversationBOT(void *arg);


/**
Writes in service textToWrite

return <0 in case of error:     -1 parameters error
                                -2 writing error
*/
int writeInService(const char *textToWrite, struct ServiceDescriptor *service);

#endif