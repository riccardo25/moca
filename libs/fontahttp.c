#include "fontahttp.h"


int openConnection(int *sockfd, struct hostent *server, struct sockaddr_in *serv_addr, const char *host, int portno )
{
    // create the socket 
    if( (*sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0 )
    {
        //error creating socket
        return -1;
    }

    // lookup the ip address 
    if( (server = gethostbyname(host) ) == NULL)
    {
        //no suck host
        return -2;
    }

    // fill in the structure 

    memset(serv_addr,0,sizeof( struct sockaddr_in));
    serv_addr->sin_family = AF_INET;
    serv_addr->sin_port = htons(portno);

    memcpy(&(serv_addr->sin_addr.s_addr), server->h_addr, server->h_length);

    /* connect the socket */
    if ( connect( *sockfd, (struct sockaddr *) serv_addr, sizeof(*serv_addr)) < 0)
    {
        //connection error
        return -3;
    }

    return 0;

}

void closeConnection(int *sockfd)
{
    /* close the socket */
    close(*sockfd);

}


int sendANDrcv(char *msgtosend, char **msgrcv, int sockfd)
{
    int lentosend = strlen(msgtosend);

    if(lentosend <= 0)
    {
        return -1;
    }

    int bytesend = 0;
    //send message
    while (bytesend < lentosend )
    {
        int b = write(sockfd,msgtosend + bytesend, lentosend - bytesend);
        
        if (b < 0)
            return -2;
        if (b == 0)
            break;

        bytesend+=b;
    }

    *msgrcv = (char *) malloc(RCVBUFFER * sizeof(char));
    // receive the response 
    memset(*msgrcv, 0, RCVBUFFER * sizeof(char) -1 );
    //int total = (RCVBUFFER * sizeof(char)) - 1;
    int received = 0;

    int iteration = 2;
    do 
    {

        int bytes = read(sockfd, (*msgrcv)+received, ((iteration - 1) * RCVBUFFER )-received);
        
        if (bytes < 0)
            return -3;
        if (bytes == 0)
            break;
        received+=bytes;

        *msgrcv = (char *) realloc(*msgrcv, iteration * RCVBUFFER );

        if(msgrcv == NULL)
        {//if reallocation fails
            return -4;
        }

        //memset((*msgrcv) + ((iteration - 1) * RCVBUFFER), 0 , (iteration * RCVBUFFER) - 1);
        iteration++;
    } while (1);


    return received;


}


int getHTTPHeader(char *message, char **header)
{
    char *occurrence;
    
    if( (occurrence = strstr(message, "\r\n\r\n")) == NULL )
    {
        return -1;
    }

    int size = message - occurrence;
    printf("Size %d", size);
    //create header string (allocates dinamically)
    *header = (char *) malloc(size+1);
    
    strncpy(*header, message, size);

    (*header)[size-1] = 0;
    return size;
}