/*
    MoCA Montessoro Connector Application
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:   This program uses Qpid-Proton, download from repository: https://github.com/apache/qpid-proton
            To test this program you need ActiveMQ (version 5.15 tested) you can find it at: http://activemq.apache.org/activemq-5150-release.html

*/
#include <stdlib.h>
#include <stdio.h>

#include <proton/connection.h>
#include <proton/connection_driver.h>
#include <proton/delivery.h>
#include <proton/proactor.h>
#include <proton/link.h>
#include <proton/message.h>
#include <proton/session.h>
#include <proton/transport.h>
#include <proton/url.h>


#include "libs/fontautil.h"
#include "libs/fontacommunication.h"

char list[MAXSUBFOLDERS][MAXNAMEFILE];
char absolutePath[5000]; //absolute path of executable file
pid_t pid = NULL; 
int pipefd[2]; //for pipe https://stackoverflow.com/questions/6171552/popen-simultaneous-read-and-write
FILE* output;
char buf[255];

int main(int argc, char** argv)
{
    //get the full path
    getcwd(absolutePath, sizeof(absolutePath));

    //try to find out program folders 
    int nFoundFolders = listfolder(list);

    if(nFoundFolders <1)
    {
        printf("Not found any folder\n");
        exit(1);
    }
    else if ( nFoundFolders >MAXSUBFOLDERS)
    {
        printf("Too much folders!\n");
        exit(2);
    }

    //section of code that search executive files in found folders
    if( controlProgramInFolder(list, nFoundFolders))
    {
        printf("All folders OK\n");
    }
    else
    {
        printf("Problem found in folders\n");
        exit(3);
    }

    printf("----------------------------------\n");

    //test per l'esecuzione de vari file TODO: Rimuovere appena testato
    //number of process less than MAXOPENPIDS
    pipe(pipefd);

    pid = fork();

    printf("pid %d \n", pid);
    if (pid == 0)
    {
        printf(ANSI_COLOR_GREEN    "Inside child process"    ANSI_COLOR_RESET "\n");
        
        // Child
        dup2(pipefd[0], STDIN_FILENO);
        dup2(pipefd[1], STDOUT_FILENO);
        dup2(pipefd[1], STDERR_FILENO); 
        
        char programPath[5000];
        strcpy(programPath, absolutePath);
        strcat(programPath, "/ciao/ciao");

        execl(programPath, "", (char*) NULL);
        // Nothing below this line should be executed by child process. If so, 
        // it means that the execl function wasn't successfull, so lets exit:
        exit(1);
    }

    // The code below will be executed only by parent. You can write and read
    // from the child using pipefd descriptors, and you can send signals to 
    // the process using its pid by kill() function. If the child process will
    // exit unexpectedly, the parent process will obtain SIGCHLD signal that
    // can be handled (e.g. you can respawn the child process).

    // Now, you can write to the process using pipefd[1], and read from pipefd[0]:

    #ifdef BLOCKINGAPI
        int bytesRead = 0;

        while(( bytesRead = read(pipefd[0], buf, sizeof(buf))) > 0)
        {
            printf("letto: ");
            printf(buf);
            printf("\n");
            printf("byte letti: %d\n", bytesRead);
        }

    #else        
        //not blocking api
        fd_set rfds;
        struct timeval tv;

        /* Watch stdin (fd 0) to see when it has input. */
        FD_ZERO(&rfds);
        FD_SET(pipefd[0], &rfds);

        /* Wait up to five seconds. */
        tv.tv_sec = 5;
        tv.tv_usec = 0;

        int selectResult = 0, bytesRead = 0;

        //TIMEOUT implementation, firstly wait for pipe change
        while( (selectResult = select(pipefd[0]+1, &rfds, NULL, NULL, &tv)) > 0 )
        {
            //reading the file
            if( (bytesRead = read(pipefd[0], buf, sizeof(buf))) >0)
            {
                printf("Letto :");
                printf( buf);
                printf("\n");
            }

        }

        if(selectResult == 0)
        {
            kill(pid, 1);
            printf(ANSI_COLOR_RED "Timeout on pid %d, process killed.\n" ANSI_COLOR_RESET, pid);
        }
        else if(selectResult < 0)
        {
            printf(ANSI_COLOR_RED    "ERROR! someting goes wrong in pipe select"    ANSI_COLOR_RESET "\n");
        }
        else
        {
            printf("Bye!\n");
        }
    
    #endif
    
    close(pipefd[0]);
    close(pipefd[1]);
    



    return 0;
}