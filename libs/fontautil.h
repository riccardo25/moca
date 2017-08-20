/*
    FontaUtil.h 
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   This library is used to group some useful function like, list folders ...

*/

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


void startService();


#endif