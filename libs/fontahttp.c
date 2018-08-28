/*
    fontahttp.c
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:       
    
    R O T A S
    O P E R A
    T E N E T
    A R E P O
    S A T O R
*/
#include "fontahttp.h"


int openHTTPConnection(HttpDescriptor *http)
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


void closeHTTPConnection(HttpDescriptor *http)
{
    /* close the socket */
    close(http->sockfd);

}


int HTTPsendANDrcv(char *msgtosend, char **msgrcv, HttpDescriptor *http)
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

//-------------------------------------------

int openHTTPSConnection(HttpsDescriptor *http, struct curl_slist *list)
{
    //initialize curl
    curl_global_init(CURL_GLOBAL_DEFAULT);
    if( (http->curl = curl_easy_init()) == NULL)
    {
        //cannot initialize curl
        return -1;
    }
    //insert parameters
    curl_easy_setopt(http->curl, CURLOPT_URL, http->host);
    curl_easy_setopt(http->curl, CURLOPT_POST, 1);
    curl_easy_setopt(http->curl, CURLOPT_HTTPHEADER, list);
    //setup verification of secure server
    //curl_easy_setopt(http->curl, CURLOPT_SSL_VERIFYPEER, 0L);
    //verify the host  name
    //curl_easy_setopt(http->curl, CURLOPT_SSL_VERIFYHOST, 0L);
    /* send all data to this function  */
    //curl_easy_setopt(http->curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);

    return 0;

}

int HTTPSsendANDrcv(char *sendbody, char **bodyrcv, HttpsDescriptor *http)
{
    if(http->curl == NULL)
    {
        //curl is null
        return -1;
    }
    CURLcode res;
    MemoryStruct mem;
    //allocate  byte (other allocated dinamically)
    mem.size = 0;
    mem.memory = (char *) malloc(1);
    /* we pass our memory struct to the callback function */
    //curl_easy_setopt(http->curl, CURLOPT_WRITEDATA, (void *)&(mem));
    printf("prima di perform\n");
    if( (res = curl_easy_perform(http->curl)) != CURLE_OK)
    {
        //someting goes wrong
        fprintf(stderr, "curl_easy_perform() failed: \n");
        return -2;
    }

    printf("send fatto\n");

    return mem.size;
}

void closeHTTPSConnection(HttpsDescriptor *http)
{
    if(http->curl != NULL)
    {
        /* cleanup curl stuff */
        curl_easy_cleanup(http->curl);
    }

    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
}


size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  mem->memory = realloc(mem->memory, mem->size + realsize + 1);
  if(mem->memory == NULL) {
    /* out of memory! */ 
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}
