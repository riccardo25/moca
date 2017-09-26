/*
    FontaUtil.h 
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   This library is used to group some useful function like, list folders ...

     R O T A S
     O P E R A
     T E N E T
     A R E P O
     S A T O R

*/
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>  
#include <pthread.h>

#ifndef _FONTAUTIL_H_
#define _FONTAUTIL_H_
/****************************************** MACROS ***********************************************************/
/* --------------------------- --------------------------------------------------------------*/
/*            COLORS          */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*-------------------------------------------------------------------------------------------------------*/

//max charaters of files name
#ifndef MAXNAMEFILE
#define MAXNAMEFILE 150
#endif
//maximum number of subfolders
#ifndef MAXSUBFOLDERS
#define MAXSUBFOLDERS 50
#endif
//maximum number of PIDs
#ifndef MAXOPENPIDS
#define MAXOPENPIDS 100
#endif
//maximum size of buffer in ServiceDescriptor
#ifndef MAXBUFFERSIZE
#define MAXBUFFERSIZE 5000
#endif

//timeout in seconds for service deadlock
#ifndef TIMEOUTSECONDS
#define TIMEOUTSECONDS 30
#endif

//define maximum length of user id 
#ifndef USERIDSIZE
#define USERIDSIZE 100
#endif

//maximum conversation id size
#ifndef CONVERSATIONIDSIZE
#define CONVERSATIONIDSIZE 200
#endif

//max token size
#ifndef TOKENSIZE
#define TOKENSIZE 200
#endif

//max watermark size
#ifndef WATERMARKSIZE
#define WATERMARKSIZE 20
#endif

//max read attempt
#ifndef MAXREADFROMSERVICEATTEMPTS
#define MAXREADFROMSERVICEATTEMPTS 5
#endif

/*
#ifndef DEBUG
#define DEBUG 1
#endif

#ifndef DEBUGFONTABOTCONNECTOR
#define DEBUGFONTABOTCONNECTOR
#endif
*/
//debug data from collateral conversation
/*#ifndef DEBUGDATACOLLATERALCONVERSATION
#define DEBUGDATACOLLATERALCONVERSATION
#endif

//debug what i need to write in service (message from collateral)
#ifndef DEBUGWRITEINSERVICE
#define DEBUGWRITEINSERVICE
#endif

//debug message sent to bot
#ifdef DEBUGSENDMESSAGETOBOT
#define DEBUGSENDMESSAGETOBOT
#endif*/

/****************************************** STRUCTS **********************************************************/

typedef struct ServiceDescriptor
{
    char folderName[MAXNAMEFILE]; //name of folder (and service)
    pid_t pid; //pid of creating service
    //int pipefd[2]; //pipe of service https://stackoverflow.com/questions/6171552/popen-simultaneous-read-and-write
    char absolutepath[500]; //absolutepath of executible file (insert here because thread permit only one parameter to be passed)

    pthread_t tid; //thread id

    //fd_set rfds; //non blocking api (timeout implementation)
    //struct timeval tv; //timeout structure

    char collateralConversationID[CONVERSATIONIDSIZE]; //id of collateral conversation
    char collateralConversationToken[TOKENSIZE];
    char collateralConversationWatermark[WATERMARKSIZE];
    char userID[USERIDSIZE];

} ServiceDescriptor;

/**************************************** PROTOTYPES *********************************************************/
/**
Returns the number of subfolders and put the list of folders' names in list
*/
int listfolder(char list[MAXSUBFOLDERS][MAXNAMEFILE]);

/* Show error when wrong parameters inserted */
void usage(const char *arg0);

/**
Returns 0 (false) if all folders contain a programm called like his folder
*/
int controlProgramInFolder(char list[MAXSUBFOLDERS][MAXNAMEFILE], int n_folders);

/**
Insert the name of service (in service) and control if this name is present in list (list of service)
return <0 if nameservice is not present in list or something not works good
*/ 
int insertControlNameService(char list[MAXSUBFOLDERS][MAXNAMEFILE], char *nameservice, ServiceDescriptor *service);


/**
Allocates memory for services and does some boring stuff
size is the new blocks to insert in service array: if all works, return is lengthServiceArray + size
return  -1 error allocating memory
        >0 length of array
*/
int allocateMemoryServices(ServiceDescriptor **serviceArray, int lengthServiceArray, int size, char *absolpth);

/**
Deallocates memory
*/
void deallocateMemoryServices(ServiceDescriptor **serviceArray);

/**
Returns the index of empty service (not used) that can be allocated to start a new service
return <0 in case of error,
*/
int getEmptyService(ServiceDescriptor **serviceArray, int length);

/**
Writes in SERVICE contenent of 'buffer'
returns <0 ->   -1 parameters error
                -2 not allowed to open files
                -3 buffer empty
return >= 0 if not error found, lenght of buffer wrote
*/
int mocaWriteinService(struct ServiceDescriptor *service, const char *buffer);

/**
Reads from SERVICE's buffer file

return < 0      -1 parameters error
                -2 can not open file
                -3 general error, something goes wrong
                -4 no file to read, nothing to read
return >= 0     number of bytes red ( no error found )

*/
int mocaReadfromService(struct ServiceDescriptor *service, char *buffer, int sizeBuffer);



#endif