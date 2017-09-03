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
//#include "libs/fontacommunication.h"
#include "libs/fontahttp.h"
#include "libs/fontabotconnector.h"


/*          PROTOTYPES              */
int startNewService();
void folderController();
void startscreen();

/*      GLOBAL VARIABLES            */
char list[MAXSUBFOLDERS][MAXNAMEFILE]; //list of folders
int nFoundFolders = 0; //number of folders
char absolutePath[1000]; //absolute path of executable file
char UserID[] = "119926";
ServiceDescriptor *services; //array of services
int lengthServices = 0;//length of array services
//HttpDescriptor httpPoller; //poller info
BotConnectionParams botconnectionparam;


/*          MAIN                    */
int main(int argc, char** argv)
{
    startscreen();
    folderController();
    strcpy(botconnectionparam.userID, "sdkbk5646csd313d5sf1383sdz1c5sx1cc11");//set userid
    botconnectionparam.pollWatermark[0] = 0;//set watermark to empty string
    printf("Main process has PID: %d\n", getpid());

    /* **************************************** ALLOCATION MEMORY FOR SERVICES *****************************************/
    printf("Starting service: Allocating memory for services.. " );
    
    //allocation is dinamicaly because some calculator can go OUT of RAM, or improve the number of same services 
    //that you can afford
    if(( lengthServices = allocateMemoryServices(&services, lengthServices, MAXOPENPIDS, absolutePath) ) < 0)//insert 1 service
    {
         printf(ANSI_COLOR_RED "Error allocating memory!" ANSI_COLOR_RESET "\n");
         return -3;
    }
    printf("DONE.\tCan use %d service at the same time!\n", lengthServices);

    /* **************************************** CONVERSATION WITH BOT ************************************************/

    printf("Creating new conversation with MontessoroBOT.. " );
    
    if( openConversation(botconnectionparam.conversationId, botconnectionparam.token) <0)
    {
        printf(ANSI_COLOR_RED "Error creating connection params!" ANSI_COLOR_RESET "\n");
        return -8;

    }

    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);
    
    char *result;
    sendMessagetoBOT("ciao", botconnectionparam, &result);
    printf("Result: %s\n", result);
    free(result);

    /* ********************************************* POLL BOT *****************************************************/

    

    printf("Now I'm tring to create Poller Service... ");

    if( pthread_create( &(botconnectionparam.pollerTID), NULL, startPollBOT, (void *) &(botconnectionparam) ) )
    {
        printf(ANSI_COLOR_RED "Error creating thread of poller!" ANSI_COLOR_RESET "\n");

        return -9;
    }

    /*
                //last POLL BOT
    strcpy(httpPoller.host, "www.google.com");
    httpPoller.port = 80;
    //let's create the thread of poller
    //start new thread
    if( pthread_create( &(httpPoller.tid), NULL, startPollHttp, (void *) &(httpPoller) ) )
    {
        printf(ANSI_COLOR_RED "Error creating thread of poller!" ANSI_COLOR_RESET "\n");

        return -7;
    }*/
    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);

    /* ****************************************** SEND INFO TO MYSQL ************************************************/

    printf("Now I'm tring to publish informations to Database... ");
    
    if(publishMySQLInfo(UserID, botconnectionparam.conversationId) <0)
    {
        printf(ANSI_COLOR_RED "Error sending MySQL informations!" ANSI_COLOR_RESET "\n");
        return -12;
    }
    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);
    
    printf("\n ---------------------------------------------------------------\n\n");

    startNewService("ciao");
    startNewService("ciao");

    //wait until the thread of poller is not died
    pthread_join(botconnectionparam.pollerTID, NULL);
    deallocateMemoryServices(&services);
    pthread_exit(NULL);

    return 0;
}


int startNewService(const char *nameservice)
{
    int numberofservice = -1;
    if( (numberofservice = getEmptyService( &services, lengthServices)) <0 )
    {
        printf(ANSI_COLOR_RED "Error, no service available!" ANSI_COLOR_RESET "\n");

        return -4;
    }

    printf("Found free service %d\n", numberofservice);
    
    if(insertControlNameService(list, nameservice, &services[numberofservice]) < 0)//insert 1 service
    {
        printf(ANSI_COLOR_RED "Error, name of service not present or not enough space!" ANSI_COLOR_RESET "\n");

        return -5;
    }
    
    printf("Inserted name in services %d\nStarting service %d... \n", numberofservice, numberofservice );
    //start new thread
    if( pthread_create( &(services[numberofservice].tid), NULL, startService, (void *) &(services[numberofservice]) ) )
    {
        printf(ANSI_COLOR_RED "Error creating new thread!" ANSI_COLOR_RESET "\n");

        return -6;
    }

    return 0;

    
}


void folderController()
{
    printf("Now I'm looking into your files to discover new programs...\n");


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
        printf("All folders are checked... And it's OK! Wonderful\n");
    }
    else
    {
        printf("Problem found in folders\n");
        exit(3);
    }
}

void startscreen()
{
    printf(" ---------------------------------------------------------------\n");
    printf("|\t\t\tWELCOME TO MoCA\t\t\t\t|\n");
    printf(" ---------------------------------------------------------------\n\n\n");
    printf("This program is made by Riccardo Fontanini\n\n");

}

int publishMySQLInfo(char *UserID, char *MoCAConversation)
{
    //create connection
    HttpDescriptor mysqlAPI;
    strcpy(mysqlAPI.host, "www.riccardofonta.altervista.org");
    mysqlAPI.port = 80;
    char getRequest[800];
    char servicesstring[500];
    
    //serialize all services
    int i = 0;
    for(i= 0; i<nFoundFolders; i++)
    {
        if(i != 0)
            strcat(servicesstring, ",");
        strcat(servicesstring, list[i]);
    }
    //create get request
    sprintf(getRequest, "GET http://riccardofonta.altervista.org/montessorobot/?Function=insert&UserID=%s&MoCAConversationID=%s&Services=%s\r\n\r\n", UserID, MoCAConversation, servicesstring);
    int t = 0;
    //open connection
    if( (t = openHTTPConnection(&mysqlAPI)) < 0)
    {
        
        return -10;
    }
    //try to send GET to Riccardo Fontanini API
    char *rcv;
    if( HTTPsendANDrcv(getRequest, &rcv, &mysqlAPI) <0)
    {
        
        return -11;
    }
    //closing connection
    closeHTTPConnection(&mysqlAPI);
    //if there is an error
    if(strcmp(rcv, "{\"Execution\":\"Done\"}") != 0)
    {
        return -13;
    }
    free(rcv);
    return 0;
}