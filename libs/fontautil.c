/*
    FontaUtil.c
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   

    R O T A S
    O P E R A
    T E N E T
    A R E P O
    S A T O R
*/

#include "fontautil.h"

int listfolder(char list[MAXSUBFOLDERS][MAXNAMEFILE])
{

    DIR *dir;
    int num_dir = 0;
    struct dirent *entry;

    if (!(dir = opendir(".")))
        return -1;

    while ((entry = readdir(dir)) != NULL && num_dir < MAXSUBFOLDERS)
    {
        if (entry->d_type == DT_DIR)
        {
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            {
                //if it is . (this folder), or .. not insert in list
            }
            else
            {
                printf("Found: [%s]\n", entry->d_name);
                strcpy(list[num_dir], entry->d_name);
                num_dir++;
            }
        }
        else
        {
        }
    }

    closedir(dir);
    printf("Total folders found: %d\n\n", num_dir);
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

    for (i = 0; i < n_folders; i++)
    {
        char pathname[125];
        //create the pathname
        strcpy(pathname, list[i]);
        strcat(pathname, "/");
        strcat(pathname, list[i]);
        //in windows the executible file are .exe
        #ifdef _WIN32
        strcat(pathname, ".exe");
        #endif
        printf("Service found: ");
        printf(pathname);
        printf("\n");
        if (access(pathname, F_OK) == -1)
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

int insertControlNameService(char list[MAXSUBFOLDERS][MAXNAMEFILE], char *nameservice, ServiceDescriptor *service)
{
    int i;
    for (i = 0; i < MAXSUBFOLDERS; i++)
    {

        if (strcmp(nameservice, list[i]) == 0)
        {
            strcpy((*service).folderName, nameservice);
            break;
        }
    }

    if (i >= MAXSUBFOLDERS)
    {
        //not found nameservice in list
        return -1;
    }

    return 0;
}

int allocateMemoryServices(ServiceDescriptor **serviceArray, int lengthServiceArray, int size, char *absolpth)
{
    if (lengthServiceArray < 0 || size <= 0 || size + lengthServiceArray > MAXOPENPIDS)
    {
        return -1;
    }

    //allocate dinamic memory
    if (*serviceArray == NULL && lengthServiceArray == 0)
    {
        *serviceArray = (ServiceDescriptor *)malloc(size * sizeof(ServiceDescriptor));

        if (*serviceArray == NULL)
        {
            //can not allocate memory
            return -1;
        }
    }
    else if ((*serviceArray == NULL && lengthServiceArray != 0) || (*serviceArray != NULL && lengthServiceArray == 0))
    {
        //not possible condition
        return -1;
    }
    else
    {
        //allocats new blocks
        *serviceArray = (ServiceDescriptor *)realloc(*serviceArray, (size + lengthServiceArray) * sizeof(ServiceDescriptor));
    }

    //do ripetitive stuff
    int i;
    for (i = 0; i < size + lengthServiceArray; i++)
    {
        //assign folder (service) name to null
        //*((*serviceArray)[i].folderName) = NULL;
        clearService(&((*serviceArray)[i]));
        //copy absolute path
        strcpy((*serviceArray)[i].absolutepath, absolpth);
    }

    return size + lengthServiceArray;
}

void deallocateMemoryServices(ServiceDescriptor **serviceArray)
{
    free(*serviceArray);
}

int getEmptyService(ServiceDescriptor **serviceArray, int length)
{
    int i;
    for (i = 0; i < length; i++)
    {
        if (strcmp("", (*serviceArray)[i].folderName) == 0)
        {
            return i;
        }
    }

    return -1;
}

int mocaWriteinService(struct ServiceDescriptor *service, const char *buffer)
{
    if (buffer == NULL )
    {
        return -1;
    }

    if (strlen(buffer) <= 0)
    {
        return -3;
    }

    FILE *filewrite;
    char path[800];
    sprintf(path, "/tmp/%sMOCAread.txt", service->collateralConversationID);

    //open file in writing
    int i = 0;
    //if I can not open file 5 times, there is an arror
    for (i = 0; i < MAXREADFROMSERVICEATTEMPTS; i++)
    {
        if ((filewrite = fopen(path, "w")) != NULL)
        {
            break;
        }
        usleep(100000);
    }
    //if I can not open file
    if (i == MAXREADFROMSERVICEATTEMPTS)
    {
        return -2;
    }
    //writing file
    fwrite(buffer, strlen(buffer), 1, filewrite);
    printf("Writing message: %s\n", buffer);
    //closing file
    fclose(filewrite);
    return strlen(buffer);
}

int mocaReadfromService(struct ServiceDescriptor *service, char *buffer, int sizeBuffer)
{
    if (buffer == NULL || sizeBuffer <= 0 )
    {
        return -1;
    }

    //set complete pathname
    char pathname[800];
    sprintf(pathname, "/tmp/%sMOCAwrite.txt", service->collateralConversationID);

    FILE *file;
    //control if exits buffer
    if (access(pathname, F_OK) == -1)
    {
        return -4;
    }

    //try to open
    if ((file = fopen(pathname, "r")) == NULL)
    {
        return -5;
    }

    int i, c = 'a';
    for (i = 0; i < sizeBuffer && c != EOF; i++)
    {
        c = fgetc(file);

        if (c != EOF)
            buffer[i] = c;
        else
            buffer[i] = '\0';
    }

    fclose(file);
    //clear file
    FILE *f = fopen(pathname, "w");
    if (f != NULL)
    {
        fclose(f);
    }
    return i - 1;
}