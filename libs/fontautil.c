/*
    FontaUtil.c
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   

*/

#include "fontautil.h"

//to read folders
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

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
            printf("%d ", num_dir);
            
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                //if it is . (this folder), or .. not insert in list
            }
            else
            {
                printf("[%s]\n",  entry->d_name);
                strcpy(list[num_dir], entry->d_name);
                num_dir++;
            }

            
        } else 
        {

        }
    }
    closedir(dir);

    return num_dir;
}