/*
Uses:   https://linuxprograms.wordpress.com/2010/05/20/json-c-libjson-tutorial/
        https://github.com/json-c/json-c
For json parsing
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



#ifndef TOKENSIZE
#define TOKENSIZE 200
#endif

#ifndef CONVERSATIONIDSIZE
#define CONVERSATIONIDSIZE 200
#endif

#ifndef WATERMARKSIZE
#define WATERMARKSIZE 20
#endif


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
        char *pollWatermark[WATERMARKSIZE];
        CollateralHandler handler; //used to create a connection between BotPOLLER and mocamain
        
}BotConnectionParams;



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

#endif