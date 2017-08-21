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
        strcat(pathname, "/");
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


int insertControlNameService(char list[MAXSUBFOLDERS][MAXNAMEFILE], char *nameservice, ServiceDescriptor *service, char *absolpth)
{
    int i;
    for(i = 0; i<MAXSUBFOLDERS; i++)
    {

        if(strcmp(nameservice, list[i]) == 0)
        {
            strcpy((*service).folderName, nameservice);
            break;
        }
    }

    if(i >= MAXSUBFOLDERS)
    {
        //not found nameservice in list
        return -1;
    }

    //Wait up to TIMEOUTSECONDS seconds. 
    (*service).tv.tv_sec = TIMEOUTSECONDS;
    (*service).tv.tv_usec = 0;

    //insert absolute path

    if( ((*service).absolutepath = (char *) malloc( strlen(absolpth) * sizeof(char) )) == NULL)
    {
        return -2;
    }

    strcpy((*service).absolutepath, absolpth);
    
    return 0;
}

int startService(void *arg)
{
    //cast arguments
    ServiceDescriptor *service = (ServiceDescriptor *) arg;
    //create a pipe 
    //number of process less than MAXOPENPIDS
    if( pipe(service->pipefd) < 0)
    {
        printf(ANSI_COLOR_RED "Problem creating pipeline stop StartService" ANSI_COLOR_RESET "\n");
        return -1;
    }


    if( (service->pid = fork()) < 0)
    {
        printf(ANSI_COLOR_RED "Impossibile to create PID stop StartService" ANSI_COLOR_RESET "\n");
        return -2;
    }

    printf("Created pid %d \n", service->pid);
    
    if (service->pid == 0)
    {
        printf(ANSI_COLOR_GREEN    "Inside child process"    ANSI_COLOR_RESET "\n");
        
        // Child
        dup2(service->pipefd[0], STDIN_FILENO);
        dup2(service->pipefd[1], STDOUT_FILENO);
        dup2(service->pipefd[1], STDERR_FILENO); 
        
        char programPath[5000];
        strcpy(programPath, service->absolutepath);

        strcat(programPath, "/");
        strcat(programPath, service->folderName);
        strcat(programPath, "/");
        strcat(programPath, service->folderName);
        
        #ifdef _WIN32
            strcat(programPath, ".exe");
        #endif

        execl(programPath, "", (char*) NULL);
        // Nothing below this line should be executed by child process. If so, 
        // it means that the execl function wasn't successfull, so lets exit:
        close(service->pipefd[0]);
        exit(1);
    }

    // The code below will be executed only by parent. You can write and read
    // from the child using pipefd descriptors, and you can send signals to 
    // the process using its pid by kill() function. If the child process will
    // exit unexpectedly, the parent process will obtain SIGCHLD signal that
    // can be handled (e.g. you can respawn the child process).

    // Now, you can write to the process using pipefd[1], and read from pipefd[0]:

    #ifdef BLOCKINGAPI
        /*int bytesRead = 0;

        while(( bytesRead = read(pipefd[0], buf, sizeof(buf))) > 0)
        {
            printf("letto: ");
            printf(buf);
            printf("\n");
            printf("byte letti: %d\n", bytesRead);
        }*/

    #else        
        //not blocking api
        

        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO( &(service->rfds) );
        FD_SET( service->pipefd[0], &(service->rfds) );

        /* Wait up to five seconds. */
        service->tv.tv_sec = TIMEOUTSECONDS;
        service->tv.tv_usec = 0;

        int selectResult = 0, bytesRead = 0;

        //create buffer to read
        char buf[MAXBUFFERSIZE];
        //TIMEOUT implementation, firstly wait for pipe change with select
        while( (selectResult = select( (service->pipefd[0])+1, &(service->rfds), NULL, NULL, &(service->tv) )) > 0 )
        {
            //reading the file
            if( (bytesRead = read(service->pipefd[0], buf, sizeof(buf))) >0)
            {
                printf("Letto :");
                printf(buf);
                printf("\n");
            }

        }

        if(selectResult == 0)
        {
            kill(service->pid, 1);
            printf( ANSI_COLOR_RED "Timeout on pid %d, process killed.\n" ANSI_COLOR_RESET "\n", service->pid);
        }
        else if(selectResult < 0)
        {
            printf( ANSI_COLOR_RED "ERROR! someting goes wrong in pipe select" ANSI_COLOR_RESET "\n");
        }
        else
        {
            printf("Bye!\n");
        }
    
    #endif
    

    
    close(service->pipefd[1]);
    pthread_exit(NULL);
    return 0;
}


int allocateMemoryServices(ServiceDescriptor **serviceArray, int lenghtServiceArray, int size)
{
    if(lenghtServiceArray <0 || size <= 0 || size + lenghtServiceArray > MAXOPENPIDS)
    {
        return -1;
    }

    //allocate dinamic memory
    if(*serviceArray == NULL  && lenghtServiceArray == 0)
    {
        *serviceArray = ( ServiceDescriptor *) malloc( size * sizeof( ServiceDescriptor) );
    
        if(*serviceArray == NULL)
        {
            //can not allocate memory
            return -1;
        }
    }
    else if( (*serviceArray == NULL && lenghtServiceArray != 0) || (*serviceArray != NULL && lenghtServiceArray == 0))
    {
        //not possible condition
        return -1;
    }
    else
    {
        //allocats new blocks
        *serviceArray = ( ServiceDescriptor *) realloc(*serviceArray, (size + lenghtServiceArray) * sizeof( ServiceDescriptor) );
    }

    return size + lenghtServiceArray;
}

void deallocateMemoryServices(ServiceDescriptor **serviceArray)
{
    free(*serviceArray);
}