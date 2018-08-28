/*
    MoCAAPI.h 
    Written by: Riccardo Fontanini
    Start date: 30 August 2017
    Note:   SDK made to help you to read/write with MoCA Client

     R O T A S
     O P E R A
     T E N E T
     A R E P O 
     S A T O R
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifndef _MOCAAPI_H_
#define _MOCAAPI_H_

/****************************************** MACROS ***********************************************************/
/**
Needed to define to write in moca Client (not in stdout)
*/
#ifndef _MOCAWRITEINBOT
#define _MOCAWRITEINBOT 1
#endif

/**
Needed to define to Write in stdout (not in moca client)
*/
#ifndef _MOCAWRITECONSOLE
#define _MOCAWRITECONSOLE 2
#endif


/**
Needed to define to read from moca Client
*/
#ifndef _MOCAREADFROMBOT
#define _MOCAREADFROMBOT 1
#endif

/**
Needed to define to read from stdin
*/
#ifndef _MOCAREADFROMCONSOLE
#define _MOCAREADFROMCONSOLE 2
#endif

/**
Max attempts to write to file
*/
#ifndef _MAXWRITEATTEMPTS
#define _MAXWRITEATTEMPTS 5
#endif

/**
microseconds to wait between two writing attempts
*/
#ifndef _USECONDSWRITING
#define _USECONDSWRITING 120000
#endif

/**
Max attempts to read from file
*/
#ifndef _MAXREADATTEMPTS
#define _MAXREADATTEMPTS 5
#endif

/**
microseconds to wait between two READING attempts
*/
#ifndef _USECONDSREADING
#define _USECONDSREADING 120000
#endif

/****************************************** STRUCTS **********************************************************/

/**************************************** PROTOTYPES *********************************************************/

/**
Writes in MOCA CLIENT contenent of 'buffer' if type is _MOCAWRITEINBOT also writes in console
returns <0 ->   -1 parameters error
                -2 not allowed to open files
                -3 buffer empty
return >= 0 if not error found, lenght of buffer wrote
*/
int mocaWrite(const char *buffer, int type);


/**
Reads from buffer file, if type is _MOCAREADFROMCONSOLE nothing append

return < 0      -1 parameters error
                -2 can not open file
                -3 general error, something goes wrong
                -4 no file to read, nothing to read
return >= 0     number of bytes red ( no error found )

*/
int mocaRead(char *buffer, int sizeBuffer, int type);

/*
Create files to comunicate with client
*/
void initMocaRW(char *fromMOCA);

/*
Closes the comunication
*/
void mocaClose();

#endif