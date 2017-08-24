#include "fontahttp.h"


int openConnection(HttpDescriptor *http)
{
    // create the socket 
    if( (http->sockfd = socket(AF_INET, SOCK_STREAM, 0)) <0 )
    {
        //error creating socket
        return -1;
    }

    // lookup the ip address 
    if( (http->server = gethostbyname(http->host) ) == NULL)
    {
        //no suck host
        return -2;
    }

    // fill in the structure 

    memset( &(http->serv_addr), 0, sizeof( struct sockaddr_in));
    http->serv_addr.sin_family = AF_INET;
    http->serv_addr.sin_port = htons(http->port);

    memcpy(&(http->serv_addr.sin_addr.s_addr), http->server->h_addr, http->server->h_length);

    /* connect the socket */
    if ( connect( http->sockfd, (struct sockaddr *) &(http->serv_addr), sizeof(http->serv_addr)) < 0)
    {
        //connection error
        return -3;
    }

    return 0;

}


void closeConnection(HttpDescriptor *http)
{
    /* close the socket */
    close(http->sockfd);

}


int sendANDrcv(char *msgtosend, char **msgrcv, HttpDescriptor *http)
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
        int b = write(http->sockfd,msgtosend + bytesend, lentosend - bytesend);
        
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

        int bytes = read(http->sockfd, (*msgrcv)+received, ((iteration - 1) * RCVBUFFER )-received);
        
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

    int size = occurrence- message;
    //create header string (allocates dinamically)
    *header = (char *) malloc(size+1);  
    memcpy(*header, message, size);
    (*header)[size] = 0;

    return size;
}

int getHTTPBody(char *message, char **body)
{
    char *occurrence;

    if( (occurrence = strstr(message, "\r\n\r\n")) == NULL )
    {
        return -1;
    }

    int first = occurrence - message + 4;//charaters for header
    int size = strlen(occurrence) - 4;//4 is for \r\n\r\n
    //create body string (allocates dinamically)
    *body = (char *) malloc(size+1);  
    memcpy(*body, message + first, size);
    (*body)[size] = 0;
    
    return size;
}

int startPollHttp(void * arg)
{

    HttpDescriptor *httpdesc = (HttpDescriptor *) arg;
    while(1)
    {


        sleep(WAITSECONDS);

        int connectionerror = 0;
        //try to open connection
        if( (connectionerror = openConnection(httpdesc) ) <0)
        {
            printf("Connection error %d\n", connectionerror);
        }
        //if connection is OK
        char messagesend[SENDBUFFER];
        char *messagerecived; //the buffer of recived data is dynamic
        char *message_fmt = "GET http://%s\r\n\r\n";//this is the format to follow
        sprintf(messagesend, message_fmt, httpdesc->host);

        int byterecived = 0;
        //send and recive message
        if( (byterecived = sendANDrcv(messagesend, &messagerecived, httpdesc) ) <0 || connectionerror < 0)
        {
            printf("Send or recive error %d\n", byterecived);
        }
        else
        {
            printf("Recived %s\n\n", messagerecived);
        }

        //connection is not longer needed
        closeConnection(httpdesc);

        //do someting with message recived
        char *header;
        getHTTPBody(messagerecived, &header);

        //frees the RAM
        free(messagerecived);
        free(header);
        printf("Header %s\n\n", header);
    }

    pthread_exit(NULL);
    exit(1);
}