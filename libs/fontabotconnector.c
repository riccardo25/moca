
#include "fontabotconnector.h"

int openConversation(char *conversationId, char *token)
{
    CURL *curl;
    CURLcode res; //code of curl

    struct MemoryStruct chunk; //variable to pass contenent and size of message
    chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;            /* no data at this point */

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    struct curl_slist *list = NULL;
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, "https://directline.botframework.com/v3/directline/conversations"); //default API
        curl_easy_setopt(curl, CURLOPT_POST, 1);

        list = curl_slist_append(list, "Authorization: Bearer Ay_aHU8nWyk.cwA.xv0.3Q-n11juq0vr4h9f4AZ89B4idQCHJ-bVJF-cA0S3kTI"); //authorization, app secret
        list = curl_slist_append(list, "Content-Type: application/json");                                                        //type
        list = curl_slist_append(list, "Content-length: 0");                                                                     //no message
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);                                                                        //insert the list options
                                                                                                                                 /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //verify peer
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //and verify host
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return -1;
        }

        //printf("%lu bytes retrieved\n", (long)chunk.size);
        char *data = (char *)malloc((long)chunk.size + 1); //copy data
        memcpy(data, chunk.memory, (long)chunk.size);
        data[(long)chunk.size] = 0;
        free(chunk.memory); //free the memory
        //printf("Data:%s\n", data);

        //parse for token

        json_object *jobj = NULL;
        struct json_object *val;

        if ((jobj = json_tokener_parse(data)) == NULL)
        {
            //error parsing
            printf("Error parsing JSON\n");
            return -2;
        }

        //search conversation ID
        if ((val = json_object_object_get(jobj, "conversationId")) == NULL)
        {
            printf("Error not found JSON property\n");
            return -3;
        }
        else
        {
            //found id
            //printf("ID: %s\n", json_object_get_string(val));
            if (strlen(json_object_get_string(val)) < CONVERSATIONIDSIZE)
                strcpy(conversationId, json_object_get_string(val));
        }

        //search TOKEN
        if ((val = json_object_object_get(jobj, "token")) == NULL)
        {
            printf("Error not found JSON property\n");
            return -3;
        }
        else
        {
            //found id
            //printf("TOKEN: %s\n", json_object_get_string(val));
            if (strlen(json_object_get_string(val)) < TOKENSIZE)
                strcpy(token, json_object_get_string(val));
        }

        /* always cleanup */
        curl_easy_cleanup(curl);
        free(data); //clean data
        free(val);
        free(jobj);
        /* we're done with libcurl, so clean it up */
        curl_global_cleanup();
        return 1;
    }

    return -1;
}

int startPollHttp(void *arg)
{

    HttpDescriptor *httpdesc = (HttpDescriptor *)arg;
    while (1)
    {
        sleep(WAITSECONDS);

        int connectionerror = 0;
        //try to open connection
        if ((connectionerror = openHTTPConnection(httpdesc)) < 0)
        {
            printf("Connection error %d\n", connectionerror);
        }
        //if connection is OK
        char messagesend[SENDBUFFER];
        char *messagerecived;                        //the buffer of recived data is dynamic
        char *message_fmt = "GET http://%s\r\n\r\n"; //this is the format to follow
        sprintf(messagesend, message_fmt, httpdesc->host);

        int byterecived = 0;
        //send and recive message
        if ((byterecived = HTTPsendANDrcv(messagesend, &messagerecived, httpdesc)) < 0 || connectionerror < 0)
        {
            printf("Send or recive error %d\n", byterecived);
        }
        else
        {
            printf("Recived %s\n\n", messagerecived);
        }

        //connection is not longer needed
        closeHTTPConnection(httpdesc);

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

int sendMessagetoBOT(char *message, BotConnectionParams conn, char **result)
{
    CURL *curl;
    CURLcode res;                          //code of curl
    struct MemoryStruct chunk;             //variable to pass contenent and size of message
    chunk.memory = malloc(1);              /* will be grown as needed by the realloc above */
    chunk.size = 0;                        /* no data at this point */
    curl_global_init(CURL_GLOBAL_DEFAULT); //setup curl options
    curl = curl_easy_init();
    struct curl_slist *list = NULL;

    if(conn.conversationId == NULL || message == NULL)
    {
        //no convesation ID found or message
        return -1;
    }
    //lets create JSON to send
    json_object *jsontosend = json_object_new_object();
    /*
    {
        "type": "message",
        "from": 
        {
            "id": "user1"
        },
        "text": "hello"
    }
    */
    //insert type of message
    json_object_object_add(jsontosend,"type", json_object_new_string("message"));
    //json_object_object_add(jsontosend,"textFormat", json_object_new_string("plain"));
    //insert from message
    json_object *fromid = json_object_new_object();
    json_object_object_add(fromid,"id", json_object_new_string(conn.userID));
    json_object_object_add(jsontosend,"from", fromid);
    //insert text
    json_object_object_add(jsontosend,"text", json_object_new_string(message));
    //printf("creato: %s\n", json_object_to_json_string(jsontosend));
    if (curl)
    {
        char url[500];
        sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities", conn.conversationId);
        curl_easy_setopt(curl, CURLOPT_URL, url); //default API
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        printf("Url to send message: %s\n", url);

        
        char token[500];
        sprintf(token, "Authorization: Bearer %s", conn.token);
        printf(token);
        list = curl_slist_append(list, token); //authorization, app secret
        list = curl_slist_append(list, "Content-Type: application/json");   
        char length[100];
        sprintf(length, "Content-length: %d", (int) strlen(json_object_to_json_string(jsontosend)));
        list = curl_slist_append(list, length);//no message
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);                                                                        //insert the list options
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_object_to_json_string(jsontosend)); /* send all data to this function  */
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        /* we pass our 'chunk' struct to the callback function */
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //verify peer
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //and verify host
        /* Perform the request, res will get the return code */
        res = curl_easy_perform(curl);
        /* Check for errors */
        if (res != CURLE_OK)
        {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
            return -2;
        }
        //put message json in result
        if( (*result = (char *) malloc((long)chunk.size + 1) ) == NULL)//allocate data
        {
            return -3; //problem in allocating memory
        } 
        //copy value
        memcpy(*result, chunk.memory, (long)chunk.size);
        (*result)[(long)chunk.size] = 0;//put last charater to null
        free(chunk.memory); //free the memory
        //printf("Data:%s\n", data);
        /* always cleanup */
        curl_easy_cleanup(curl);
        free(jsontosend);
    }
    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
    return 1;
}


int startPollBOT(void *arg)
{
    BotConnectionParams *params = (BotConnectionParams *) arg;

    while (1)
    {
        sleep(WAITSECONDS);
        CURL *curl;
        CURLcode res;                          //code of curl
        struct MemoryStruct chunk;             //variable to pass contenent and size of message
        chunk.memory = malloc(1);              /* will be grown as needed by the realloc above */
        chunk.size = 0;                        /* no data at this point */
        curl_global_init(CURL_GLOBAL_DEFAULT); //setup curl options
        curl = curl_easy_init();
        struct curl_slist *list = NULL;
        /*char *re;
        int test = 0;
        if( (test = sendMessagetoBOT("come va?", *params, &re) ) < 0)
        {
            printf("Errore %d", test);
        }
        printf("%s\n", re);
        free(re);*/

        if(params->conversationId == NULL)
        {
            //no convesation ID found or message
            pthread_exit(-1);
            exit(1);
        }

        if (curl)
        {
            char url[500];
            if(strlen(params->pollWatermark) > 0)
            {
                sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities?watermark=%s", params->conversationId, params->pollWatermark);
            }
            else
            {
                sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities", params->conversationId);
            }

            
            curl_easy_setopt(curl, CURLOPT_URL, url); //default API
            char token[500];
            sprintf(token, "Authorization: Bearer %s", params->token);
            list = curl_slist_append(list, token); //authorization, app secret
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);    
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
            /* we pass our 'chunk' struct to the callback function */
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //verify peer
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //and verify host
            //perform curl get
            if ( (res = curl_easy_perform(curl)) != CURLE_OK)
            {
                fprintf(stderr, "poll failed: %s\n", curl_easy_strerror(res));
                pthread_exit(-2);
                exit(1);
            }

            char *result;
            if( (result = (char *) malloc((long)chunk.size + 1) ) == NULL)//allocate data
            {
                pthread_exit(-3);
                exit(1);
            } 
            //copy value
            memcpy(result, chunk.memory, (long)chunk.size);
            result[(long)chunk.size] = 0;//put last charater to null
            free(chunk.memory); //free the memory
            printf("Data:%s\n", result);

            struct json_object *jobj = NULL;
    
            if ((jobj = json_tokener_parse(result)) == NULL)
            {
                //error parsing
                pthread_exit(-4);
                exit(1);
            }
            struct json_object *val = NULL;
            //search conversation ID
            if ((val = json_object_object_get(jobj, "watermark")) != NULL)
            {
                if (strlen(json_object_get_string(val)) < WATERMARKSIZE)
                    strcpy(params->pollWatermark, json_object_get_string(val));
                printf("watermark %s\n", params->pollWatermark);
            }
            free(val);

            /* always cleanup */
            free(jobj);
            curl_easy_cleanup(curl);
            free(result);
            curl_global_cleanup();
        }
    }

    pthread_exit(NULL);
    exit(1);
}