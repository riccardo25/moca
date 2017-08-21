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

#include <pthread.h>


#include "libs/fontautil.h"
#include "libs/fontacommunication.h"

char list[MAXSUBFOLDERS][MAXNAMEFILE];
int nFoundFolders = 0;
char absolutePath[1000]; //absolute path of executable file

ServiceDescriptor *services; //array of services
int lenghtServices = 0;//lenght of array services

int main(int argc, char** argv)
{
    //get the full path
    getcwd(absolutePath, sizeof(absolutePath));

    //try to find out program folders 
    nFoundFolders = listfolder(list);

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

    printf("\n----------------------------------\n\n");
    
    printf("Main process has PID: %d\n", getpid());

    printf("Starting service: Allocating memory for services.. " );
    
    if(( lenghtServices = allocateMemoryServices(&services, lenghtServices, 1) ) < 0)//insert 1 service
    {
         printf(ANSI_COLOR_RED "Error allocating memory!" ANSI_COLOR_RESET "\n");
         return -3;
    }

    printf("DONE\n");

    if(insertControlNameService(list, "ciao", &services[0], absolutePath) < 0)//insert 1 service
    {
        printf(ANSI_COLOR_RED "Error, name of service not present or not enough space!" ANSI_COLOR_RESET "\n");
        deallocateMemoryServices(&services);
        return -4;
    }
    
    printf("Inserted name in service %d\nStarting service %d... \n", lenghtServices-1, lenghtServices-1 );
    
    
    //start new thread
    if( pthread_create( &(services[0].tid), NULL, startService, (void *) &services[0] ) )
    {
        printf(ANSI_COLOR_RED "Error creating new thread!" ANSI_COLOR_RESET "\n");
        deallocateMemoryServices(&services);
        return -5;
    }

    sleep(1);
    deallocateMemoryServices(&services);
    pthread_exit(NULL);
    return 0;
}