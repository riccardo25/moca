/*
    FontaUtil.h 
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   This library is used to group some useful function like, list folders ...

*/

#include <stdlib.h>
#include <stdio.h>

//to read folders
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>  
#include <pthread.h>

#ifndef _FONTAUTIL_H_
#define _FONTAUTIL_H_

/* ---------------------------*/
/*            COLORS          */
#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"
/*------------------------------*/


//max charaters of files name
#define MAXNAMEFILE 150
//maximum number of subfolders
#define MAXSUBFOLDERS 50
//define this to use Blocking API, to use NON Blocking, comment following line
//#define BLOCKINGAPI 1
//maximum number of PIDs
#define MAXOPENPIDS 50
//maximum size of buffer in ServiceDescriptor
#define MAXBUFFERSIZE 255
//timeout in seconds for service deadlock
#define TIMEOUTSECONDS 10


typedef struct ServiceDescriptor
{
    char folderName[MAXNAMEFILE]; //name of folder (and service)
    pid_t pid; //pid of creating service
    int pipefd[2]; //pipe of service https://stackoverflow.com/questions/6171552/popen-simultaneous-read-and-write
    char absolutepath[500]; //absolutepath of executible file (insert here because thread permit only one parameter to be passed)

    pthread_t tid; //thread id

    fd_set rfds; //non blocking api (timeout implementation)
    struct timeval tv; //timeout structure


} ServiceDescriptor;


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
Start new Service of existing program in subfolder
Service passed
Return <0 there is an error:    -1 is pipeline creating problem
                                -2 is fork creating problem

*/
int startService(void *arg);

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
Clear the specified service
*/
void clearService(ServiceDescriptor *service);

#endif