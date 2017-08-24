/*
    FontaHTTP.h 
    Written by: Riccardo Fontanini
    Start date: 24 August 2017
    Note:   This library allow connection in http ...

*/

#include <stdio.h> /* printf, sprintf */
#include <stdlib.h> /* exit */
#include <unistd.h> /* read, write, close */
#include <string.h> /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <netdb.h> /* struct hostent, gethostbyname */

#include <pthread.h>

#ifndef _FONTAHTTP_H_
#define _FONTAHTTP_H_

// value of buffer recived (iterative beacuse this library download dinamycally)
#ifndef RCVBUFFER
#define RCVBUFFER 200
#endif

//max lenght of message to send
#ifndef SENDBUFFER
#define SENDBUFFER 1024
#endif

//lenght of host
#ifndef MAXHOSTSIZE
#define MAXHOSTSIZE 500
#endif

//seconds to wait in polling cycle of HTTP
#ifndef WAITSECONDS
#define WAITSECONDS 5
#endif

//Type that carry HTTP options
typedef struct HttpDescriptor
{
    struct hostent *server; //server info
    struct sockaddr_in serv_addr; //socket address
    int sockfd; //socket file descriptor
    char host[MAXHOSTSIZE]; //server url 
    int port; //service port (80)
    pthread_t tid; //thread id

} HttpDescriptor;

/*
Allow to open connection
sockfd -> is file descriptor variabile 
server -> defines server values
serv_addr -> defines address of server
host -> char sequence of URL
portno -> number of port (80)
return < 0 if connection fails
*/
int openConnection(HttpDescriptor *http);

/*
Sends msgtosend and put in msgrcv thre response
return < 0 if something fails and if >= 0 returns the number of bytes recived
*/
int sendANDrcv(char *msgtosend, char **msgrcv, HttpDescriptor *http);

/*
Closes the connection 
*/
void closeConnection(HttpDescriptor *http);

/*
Get the header of http message
Returns the size of header, it allocates dinamically the size of header variable
*/
int getHTTPHeader(char *message, char **header);

/*
Get the body of http message
returns the size of body,it allocates dinamically the size of body variable
*/
int getHTTPBody(char *message, char **body);


/*
Starts the poller
*/
int startPollHttp(void * arg);

#endif