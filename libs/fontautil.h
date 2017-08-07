/*
    FontaUtil.h 
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   This library is used to group some useful function like, list folders ...

*/

#ifndef _FONTAUTIL_H_
#define _FONTAUTIL_H_


    #define MAXNAMEFILE 150
    #define MAXSUBFOLDERS 50
    /**
    Returns the number of subfolders and pass the list of names
    */
    int listfolder(char list[MAXSUBFOLDERS][MAXNAMEFILE]);


#endif