/*
    FontaUtil.c
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   

*/

#include "fontautil.h"
#include <stdlib.h>
#include <stdio.h>

//to read folders
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>


int listfolder(char list[MAXSUBFOLDERS][MAXNAMEFILE])
{
    
    DIR *dir;
    int num_dir = 0;
    struct dirent *entry;

    if (!(dir = opendir(".")))
        return -1;
    
    while ((entry = readdir(dir)) != NULL && num_dir < MAXSUBFOLDERS ) 
    {
        if (entry->d_type == DT_DIR) 
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                //if it is . (this folder), or .. not insert in list
            }
            else
            {
                printf("Found: [%s]\n",  entry->d_name);
                strcpy(list[num_dir], entry->d_name);
                num_dir++;
            }

            
        } else 
        {

        }
    }


    closedir(dir);
    printf("Folders found: %d\n", num_dir);
    return num_dir;
}


void usage(const char *arg0) 
{
  fprintf(stderr, "Usage: %s [-a url] [-m message-count] [-d delay-ms]\n", arg0);
  exit(1);
}


int controlProgramInFolder(char list[MAXSUBFOLDERS][MAXNAMEFILE], int n_folders)
{
    //cycle all folders
    int i;

    for (i=0; i< n_folders; i++)
    {
        char pathname[125];
        //create the pathname
        strcpy(pathname, list[i]);
        strcat(pathname, "\/");
        strcat(pathname, list[i]);

        //in windows the executible file are .exe
        #ifdef _WIN32
            strcat(pathname, ".exe");
        #endif

        printf(pathname);
        printf("\n");

        if(access(pathname, F_OK) == -1 )
        {
            //file don't exits
            printf("FILE ");
            printf(pathname);
            printf(" not exists!");
            return 0;
        }
    }
    //no errors found 
    return 1;
    
}

void startService()
{
    
}