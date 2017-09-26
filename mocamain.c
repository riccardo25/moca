/*
    MoCA Montessoro Connector Application
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:  This program is made to communicate with MontessoRoBOT. 

     R O T A S
     O P E R A
     T E N E T
     A R E P O
     S A T O R

*/
#include <stdlib.h>
#include <stdio.h>
/*#include <proton/connection.h>
#include <proton/connection_driver.h>
#include <proton/delivery.h>
#include <proton/proactor.h>
#include <proton/link.h>
#include <proton/message.h>
#include <proton/session.h>
#include <proton/transport.h>
#include <proton/url.h>*/
#include <pthread.h>
#include "libs/fontautil.h"
//#include "libs/fontacommunication.h"
#include "libs/fontahttp.h"
#include "libs/fontabotconnector.h"


/*          PROTOTYPES              */
int startNewService(const char *nameservice,struct MoCAMessage calledMessage);
void folderController();
void startscreen();
void createServiceCollateralBOTConnector(struct MoCAMessage message);

/*      GLOBAL VARIABLES            */
char list[MAXSUBFOLDERS][MAXNAMEFILE]; //list of folders
int nFoundFolders = 0; //number of folders
char absolutePath[1000]; //absolute path of executable file
char UserID[USERIDSIZE];
ServiceDescriptor *services; //array of services
int lengthServices = 0;//length of array services
//HttpDescriptor httpPoller; //poller info
BotConnectionParams botconnectionparam;


/*          MAIN                    */
// -i or -id to set userID
int main(int argc, char** argv)
{
    startscreen();
    //params selector
    //strcpy(UserID, "119926");
    if(argc > 1)
    {
        int p;
        for (p = 1; p < argc; p+=2)
        {
            if(strcmp( argv[p], "-id") == 0 || strcmp( argv[p], "-i") == 0)//Userid set
            {
                if(p+1 < argc)
                {
                    strcpy(UserID,argv[p+1]);
                    UserID[strlen(argv[p+1])] = '\0';
                    

                }
            }
        }
    }
    /*************************** PARAMS CONTROL*************************/
    if(strlen(UserID) == 0)
    {
        printf(ANSI_COLOR_RED "Error not setted UserID!" ANSI_COLOR_RESET " ./moca -i <userid> to set UserID \n\n");
        return -16;
    }
    printf("UserID: %s\n", UserID);
    folderController();
    strcpy(botconnectionparam.userID, "sdkbk5646csd313d5sf1383sdz1c5sx1cc11");//set userid
    botconnectionparam.pollWatermark[0] = 0;//set watermark to empty string
    botconnectionparam.handler = createServiceCollateralBOTConnector;//insert hendler when POLLBOT receive a service request
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
    printf(ANSI_COLOR_GREEN "DONE." ANSI_COLOR_RESET);
    printf("\tCan use %d service at the same time!\n", lengthServices);

    /* **************************************** CONVERSATION WITH BOT ************************************************/

    printf("Creating new conversation with MontessoroBOT.. " );
    
    if( openConversation(botconnectionparam.conversationId, botconnectionparam.token) <0)
    {
        printf(ANSI_COLOR_RED "Error creating connection params!" ANSI_COLOR_RESET "\n");
        return -8;

    }

    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);
    /****************************************** SENDING INFORMATION TO BOT *************************************/
    
    printf("Sending information to bot.. " );
    char *result;
    sendMessagetoBOT(startMainCOnversation(), botconnectionparam, &result);
    //printf("Result: %s\n", result);
    free(result);
    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);

    /* ********************************************* POLL BOT *****************************************************/

    

    printf("Creating Poller Service... ");

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


    /**************************************** START POLL COLLATERAL CONVERSATION **********************************/
    printf("Creating collateral conversation poller... ");
    if( pthread_create( &(botconnectionparam.pollerCollateralConversationID), NULL, startPollCollateralConversationBOT, (void *) services ) )
    {
        printf(ANSI_COLOR_RED "Error creating thread of collateral conversation poller!" ANSI_COLOR_RESET "\n");

        return -9;
    }
    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);
    /* ****************************************** SEND INFO TO MYSQL ************************************************/

    printf("Publishing informations to Database... ");
    int error = 0;
    if( (error = publishMySQLInfo(UserID, botconnectionparam.conversationId) ) <0)
    {
        printf(ANSI_COLOR_RED "Error sending MySQL informations! %d" ANSI_COLOR_RESET "\n", error );
        return -12;
    }
    printf(ANSI_COLOR_GREEN "DONE.\n" ANSI_COLOR_RESET);
    
    printf("\n ---------------------------------------------------------------\n\n");

    //wait until the thread of poller is not died
    pthread_join(botconnectionparam.pollerTID, NULL);
    deallocateMemoryServices(&services);
    pthread_exit(NULL);

    return 0;
}


int startNewService(const char *nameservice, struct MoCAMessage calledMessage)
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

    //copy the connection parameters
    strcpy(services[numberofservice].collateralConversationID, calledMessage.params[0]);
    strcpy(services[numberofservice].collateralConversationToken, calledMessage.params[1]);
    strcpy(services[numberofservice].collateralConversationWatermark, "0");
    strcpy(services[numberofservice].userID , UserID);


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
    /*printf(" ---------------------------------------------------------------\n");
    printf("|\t\t\tWELCOME TO MoCA\t\t\t\t|\n");
    printf(" ---------------------------------------------------------------\n\n\n");*/
    system("@cls||clear");
    printf(
    "\n _____ ______   ________  ________  ________     \n"
    "|\\   _ \\  _   \\|\\   __  \\|\\   ____\\|\\   __  \\    \n"
    "\\ \\  \\\\\\__\\ \\  \\ \\  \\|\\  \\ \\  \\___|\\ \\  \\|\\  \\   \n"
    " \\ \\  \\\\|__| \\  \\ \\  \\\\\\  \\ \\  \\    \\ \\   __  \\  \n"
    "  \\ \\  \\    \\ \\  \\ \\  \\\\\\  \\ \\  \\____\\ \\  \\ \\  \\ \n"
    "   \\ \\__\\    \\ \\__\\ \\_______\\ \\_______\\ \\__\\ \\__\\\n"
    "    \\|__|     \\|__|\\|_______|\\|_______|\\|__|\\|__|\n"
    "\n\n");
    printf("This program is made by Riccardo Fontanini\n\n");

}

int publishMySQLInfo(char *userID, char *MoCAConversation)
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
    sprintf(getRequest, "GET http://riccardofonta.altervista.org/montessorobot/?Function=insert&UserID=%s&MoCAConversationID=%s&Services=%s\r\n\r\n", userID, MoCAConversation, servicesstring);
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


void createServiceCollateralBOTConnector(struct MoCAMessage message)
{
    startNewService(message.params[2], message); //third parameter is the name of service
}