/*
    fontabotconnector.c
    Written by: Riccardo Fontanini
    Start date: 7 August 2017
    Note:       

    R O T A S
    O P E R A
    T E N E T
    A R E P O
    S A T O R
*/

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
        //printf("Header %s\n\n", header);
    }

    pthread_exit(NULL);
    exit(1);
}

int sendMessagetoBOT(char *message, BotConnectionParams conn, char **result)
{

    #ifdef DEBUGSENDMESSAGETOBOT
    printf("Sending %s\n", message);
    #endif
    CURL *curl;
    CURLcode res;                          //code of curl
    struct MemoryStruct chunk;             //variable to pass contenent and size of message
    chunk.memory = malloc(1);              /* will be grown as needed by the realloc above */
    chunk.size = 0;                        /* no data at this point */
    curl_global_init(CURL_GLOBAL_DEFAULT); //setup curl options
    curl = curl_easy_init();
    struct curl_slist *list = NULL;

    if (conn.conversationId == NULL || message == NULL)
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
    json_object_object_add(jsontosend, "type", json_object_new_string("message"));
    //json_object_object_add(jsontosend,"textFormat", json_object_new_string("plain"));
    //insert from message
    json_object *fromid = json_object_new_object();
    json_object_object_add(fromid, "id", json_object_new_string(conn.userID));
    json_object_object_add(jsontosend, "from", fromid);
    //insert text
    json_object_object_add(jsontosend, "text", json_object_new_string(message));
    //printf("creato: %s\n", json_object_to_json_string(jsontosend));
    if (curl)
    {
        char url[500];
        sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities", conn.conversationId);
        curl_easy_setopt(curl, CURLOPT_URL, url); //default API
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        char token[500];
        sprintf(token, "Authorization: Bearer %s", conn.token);
        list = curl_slist_append(list, token); //authorization, app secret
        list = curl_slist_append(list, "Content-Type: application/json");
        char length[100];
        sprintf(length, "Content-length: %d", (int)strlen(json_object_to_json_string(jsontosend)));
        list = curl_slist_append(list, length);                                             //no message
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);                                   //insert the list options
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
        if ((*result = (char *)malloc((long)chunk.size + 1)) == NULL) //allocate data
        {
            return -3; //problem in allocating memory
        }
        //copy value
        memcpy(*result, chunk.memory, (long)chunk.size);
        (*result)[(long)chunk.size] = 0; //put last charater to null
        free(chunk.memory);              //free the memory
        //printf("Data:%s\n", data);
        /* always cleanup */
        curl_easy_cleanup(curl);
        free(jsontosend);
    }
    /* we're done with libcurl, so clean it up */
    curl_global_cleanup();
    return 1;
}

int sendEndConversationtoBOT(BotConnectionParams conn, char **result)
{
    CURL *curl;
    CURLcode res;                          //code of curl
    struct MemoryStruct chunk;             //variable to pass contenent and size of message
    chunk.memory = malloc(1);              /* will be grown as needed by the realloc above */
    chunk.size = 0;                        /* no data at this point */
    curl_global_init(CURL_GLOBAL_DEFAULT); //setup curl options
    curl = curl_easy_init();
    struct curl_slist *list = NULL;

    if (conn.conversationId == NULL)
    {
        //no convesation ID found or message
        return -1;
    }
    //lets create JSON to send
    json_object *jsontosend = json_object_new_object();
    /*
    {
        "type": "endOfConversation",
        "from": 
        {
            "id": "user1"
        },
    }
    */
    //insert type of message
    json_object_object_add(jsontosend, "type", json_object_new_string("endOfConversation"));
    //json_object_object_add(jsontosend,"textFormat", json_object_new_string("plain"));
    //insert from message
    json_object *fromid = json_object_new_object();
    json_object_object_add(fromid, "id", json_object_new_string(conn.userID));
    json_object_object_add(jsontosend, "from", fromid);
    if (curl)
    {
        char url[500];
        sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities", conn.conversationId);
        curl_easy_setopt(curl, CURLOPT_URL, url); //default API
        curl_easy_setopt(curl, CURLOPT_POST, 1);
        char token[500];
        sprintf(token, "Authorization: Bearer %s", conn.token);
        list = curl_slist_append(list, token); //authorization, app secret
        list = curl_slist_append(list, "Content-Type: application/json");
        char length[100];
        sprintf(length, "Content-length: %d", (int)strlen(json_object_to_json_string(jsontosend)));
        list = curl_slist_append(list, length);                                             //no message
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);                                   //insert the list options
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
        if ((*result = (char *)malloc((long)chunk.size + 1)) == NULL) //allocate data
        {
            return -3; //problem in allocating memory
        }
        //copy value
        memcpy(*result, chunk.memory, (long)chunk.size);
        (*result)[(long)chunk.size] = 0; //put last charater to null
        free(chunk.memory);              //free the memory
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
    BotConnectionParams *params = (BotConnectionParams *)arg;

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

        if (params->conversationId == NULL)
        {
            //no convesation ID found or message
            pthread_exit(-1);
            exit(1);
        }

        if (curl)
        {
            char url[500];
            if (strlen(params->pollWatermark) > 0)
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
            if ((res = curl_easy_perform(curl)) != CURLE_OK)
            {
                fprintf(stderr, "poll failed: %s\n", curl_easy_strerror(res));
                pthread_exit(-2);
                exit(1);
            }

            char *result;
            if ((result = (char *)malloc((long)chunk.size + 1)) == NULL) //allocate data
            {
                pthread_exit(-3);
                exit(1);
            }
            //copy value
            memcpy(result, chunk.memory, (long)chunk.size);
            result[(long)chunk.size] = 0; //put last charater to null
            free(chunk.memory);           //free the memory

            #ifdef DEBUGDATAPRINCIPALCONVERSATION
            printf("Data:%s\n", result);
            #endif

            struct json_object *jobj = NULL;

            if ((jobj = json_tokener_parse(result)) == NULL)
            {
                //error parsing
                pthread_exit(-4);
                exit(1);
            }

            //search array
            json_object *arr = NULL;
            int n_messages = 0; //number of messages in te activities
            if ((arr = json_object_object_get(jobj, "activities")) != NULL)
            {
                n_messages = json_object_array_length(arr);
                int i = 0;
                //get i message (in jason format)
                for (i = 0; i < n_messages; i++)
                {
                    json_object *jsonmess = NULL;
                    if ((jsonmess = json_object_array_get_idx(arr, i)) != NULL)
                    {
                        struct json_object *text = NULL;
                        //search text field
                        if ((text = json_object_object_get(jsonmess, "text")) != NULL)
                        {
                            //create an empty message
                            struct MoCAMessage mocamessage;
                            if (readMoCAProtMessage(json_object_get_string(text), &mocamessage) <= 0)
                            {
    #ifdef DEBUGDATAPRINCIPALCONVERSATION
                                printf("Not recognized \n");
    #endif
                            }
                            else
                            {
                                //if BOT has created a collateral conversation
                                if (mocamessage.type == MOCACREATEDCOLLATERAL)
                                {
                                    printf("Created collateral with Token: %s\n", mocamessage.params[1]);
                                    params->handler(mocamessage);
                                }
                            }
                        }
                        free(text);
                    }
                    free(jsonmess);
                }
            }
            free(arr);
            struct json_object *val = NULL;
            //search conversation ID
            if ((val = json_object_object_get(jobj, "watermark")) != NULL)
            {
                if (strlen(json_object_get_string(val)) < WATERMARKSIZE)
                {
                    strcpy(params->pollWatermark, json_object_get_string(val));
                    /*int water = atoi(params->pollWatermark);
                    water++;//added 1
                    sprintf(params->pollWatermark, "%d", water);*/
                }
            }

    #ifdef DEBUGDATAPRINCIPALCONVERSATION
            printf("watermark %s\n", params->pollWatermark);
    #endif
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

int startService(void *arg)
{
    //cast arguments
    ServiceDescriptor *service = (ServiceDescriptor *)arg;
    //create a pipe
    //number of process less than MAXOPENPIDS

    //creating bot connection parameters to send messages
    BotConnectionParams collateralConnectionParams;

    strcpy(collateralConnectionParams.conversationId, service->collateralConversationID);
    strcpy(collateralConnectionParams.token, service->collateralConversationToken);
    strcpy(collateralConnectionParams.userID, service->userID);

    //send first message in collateral
    char *result;
    sendMessagetoBOT("/collateralclientfirstmessage", collateralConnectionParams, &result);
    free(result);

    //exec fork
    if ((service->pid = fork()) < 0)
    {
        printf(ANSI_COLOR_RED "Impossibile to create PID stop StartService" ANSI_COLOR_RESET "\n");
        return -2;
    }

    printf("Created pid %d \n", service->pid);

    if (service->pid == 0)
    {
        printf(ANSI_COLOR_GREEN "Inside child process" ANSI_COLOR_RESET "\n");

        char programPath[5000];
        strcpy(programPath, service->absolutepath);

        strcat(programPath, "/");
        strcat(programPath, service->folderName);
        strcat(programPath, "/");
        strcat(programPath, service->folderName);

        #ifdef _WIN32
        strcat(programPath, ".exe");
        #endif

        execl(programPath, "", service->collateralConversationID, (char *)NULL);
        // Nothing below this line should be executed by child process. If so,
        // it means that the execl function wasn't successfull, so lets exit:
        exit(1);
    }

    // you can send signals to
    // the process using its pid by kill() function. If the child process will
    // exit unexpectedly, the parent process will obtain SIGCHLD signal that
    // can be handled (e.g. you can respawn the child process).
    //not blocking api
    int selectResult = 0, bytesRead = 0, numofread = 0;
    char buffer[MAXBUFFERSIZE];
    for (numofread = 0; numofread < TIMEOUTSECONDS; numofread++)
    {
        int tot = mocaReadfromService(service, buffer, MAXBUFFERSIZE);
        if (tot > 0)
        {
            printf("%s letti: %d %s\n", service->folderName, tot, buffer);
            //if a close conversation message arrives from service
            if (strcmp(buffer, "/closeconversation") == 0)
            {
                //service close
                selectResult = 1;
                break;
            }
            else
            {
                //normal message arrived from service
                numofread = 0;
                char *result;
                char standardcollateral[] = "/collateralclient";
                char newbuf[tot + strlen(standardcollateral) + 1];
                sprintf(newbuf, "%s%s", standardcollateral, buffer);
                sendMessagetoBOT(newbuf, collateralConnectionParams, &result);
                free(result);
            }
        }
        usleep(1000000);
    }
    //if selectResult is 0, send timeout error
    if (selectResult == 0)
    {
        kill(service->pid, 1);
        printf(ANSI_COLOR_RED "Timeout on pid %d, process killed.\n" ANSI_COLOR_RESET "\n", service->pid);
    }
    else if (selectResult < 0)
    {
        printf(ANSI_COLOR_RED "ERROR! someting goes wrong!" ANSI_COLOR_RESET "\n");
    }
    else
    {
        printf(ANSI_COLOR_GREEN "Process end!\n" ANSI_COLOR_RESET "\n");
    }
    //send closing conversation
    char *res;
    sendEndConversationtoBOT(collateralConnectionParams, &res);
    free(res);
    //clear the variable of service
    clearService(service);
    pthread_exit(NULL);
    return 0;
}

void clearService(ServiceDescriptor *service)
{
    //clean the name of service
    strcpy(service->folderName, "");

    strcpy(service->collateralConversationID, "");
    strcpy(service->collateralConversationToken, "");
    strcpy(service->collateralConversationWatermark, "0");
    strcpy(service->userID, "");
}

void startPollCollateralConversationBOT(void *arg)
{
    //cast arguments
    ServiceDescriptor *services = (ServiceDescriptor *)arg;

    while (1)
    {
        int s;

        sleep(WAITCOLLATERALPOLLSECONDS);

        for (s = 0; s < MAXOPENPIDS; s++)
        {
            //if there is no service available to this s iterator, there are no others services available, so I can exit from the cycle
            if (services[s].collateralConversationID == NULL || strcmp(services[s].collateralConversationID, "") <= 0)
            {
                break;
            }
            /** START POLLING CONVERSATION **/
            CURL *curl;
            CURLcode res;                          //code of curl
            struct MemoryStruct chunk;             //variable to pass contenent and size of message
            chunk.memory = malloc(1);              /* will be grown as needed by the realloc above */
            chunk.size = 0;                        /* no data at this point */
            curl_global_init(CURL_GLOBAL_DEFAULT); //setup curl options
            curl = curl_easy_init();
            struct curl_slist *list = NULL;

            if (curl)
            {
                char url[500];
                if (strlen(services[s].collateralConversationWatermark) > 0)
                {
                    sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities?watermark=%s", services[s].collateralConversationID, services[s].collateralConversationWatermark);
                }
                else
                {
                    sprintf(url, "https://directline.botframework.com/v3/directline/conversations/%s/activities", services[s].collateralConversationID);
                }

                curl_easy_setopt(curl, CURLOPT_URL, url); //default API
                char token[500];
                sprintf(token, "Authorization: Bearer %s", services[s].collateralConversationToken);
                list = curl_slist_append(list, token); //authorization, app secret
                curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);
                curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
                /* we pass our 'chunk' struct to the callback function */
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); //verify peer
                curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L); //and verify host
                //perform curl get
                if ((res = curl_easy_perform(curl)) != CURLE_OK)
                {
                    fprintf(stderr, "poll failed: %s\n", curl_easy_strerror(res));
                    break;
                }

                char *result;
                if ((result = (char *)malloc((long)chunk.size + 1)) == NULL) //allocate data
                {
                    fprintf(stderr, "can not allocate memory for response\n");
                    break;
                }
                //copy value
                memcpy(result, chunk.memory, (long)chunk.size);
                result[(long)chunk.size] = 0; //put last charater to null
                free(chunk.memory);           //free the memory

    #ifdef DEBUGDATACOLLATERALCONVERSATION
                printf("Data from conversation %s :%s\n", services[s].collateralConversationID, result);
    #endif

                struct json_object *jobj = NULL;

                if ((jobj = json_tokener_parse(result)) == NULL)
                {
                    fprintf(stderr, "Error parsing result of polling");
                    break;
                }

                //search array
                json_object *arr = NULL;
                int n_messages = 0; //number of messages in te activities
                if ((arr = json_object_object_get(jobj, "activities")) != NULL)
                {
                    n_messages = json_object_array_length(arr);
                    int i = 0;
                    //get i message (in jason format)
                    for (i = 0; i < n_messages; i++)
                    {
                        json_object *jsonmess = NULL;
                        if ((jsonmess = json_object_array_get_idx(arr, i)) != NULL)
                        {
                            struct json_object *text = NULL;
                            //search text field
                            if ((text = json_object_object_get(jsonmess, "text")) != NULL)
                            {
                                //copy the text
                                int len = strlen(json_object_get_string(text));
                                char txtstring[len + 1];
                                strcpy(txtstring, json_object_get_string(text));
                                txtstring[len] = '\0';
                                //if it starts with /collateralbot: the message comes from BOT
                                if (strncmp("/collateralbot", txtstring, strlen("/collateralbot")) == 0)
                                {
                                    //chunk first /collateralbot charaters
                                    char *newtxt = &txtstring[strlen("/collateralbot")];

    #ifdef DEBUGWRITEINSERVICE
                                    printf("let's writing stuff in service:%s\n", newtxt);
    #endif
                                    //lets write something in service stdin
                                    writeInService((const char *)newtxt, &(services[s]));
                                }
                            }
                            free(text);
                        }
                        free(jsonmess);
                    }
                }
                free(arr);
                struct json_object *val = NULL;
                //search conversation ID
                if ((val = json_object_object_get(jobj, "watermark")) != NULL)
                {
                    if (strlen(json_object_get_string(val)) < WATERMARKSIZE)
                    {
                        strcpy(services[s].collateralConversationWatermark, json_object_get_string(val));

                        /*if(n_messages > 0)
                        {
                            int water = atoi(services[s].collateralConversationWatermark);
                            water += n_messages;//added 1
                            sprintf(services[s].collateralConversationWatermark, "%d", water);
                        }*/
                    }
                }

                free(val);

                /* always cleanup */
                free(jobj);
                curl_easy_cleanup(curl);
                free(result);
                curl_global_cleanup();
            }
        }
    }
}

int writeInService(const char *textToWrite, struct ServiceDescriptor *service)
{
    if (service == NULL || textToWrite == NULL || strcmp(textToWrite, "") <= 0)
    {
        return -1;
    }

    if (mocaWriteinService(service, textToWrite) < 0)
    {
        printf("Error writing... \n");
        return -2;
    }
    return 0;
}