/*
    Fontacommunication.h 
    Written by: Riccardo Fontanini
    Start date: 10 August 2017
    Note:   This library is used to allow comunication with broker

*/

#include <proton/connection.h>
#include <proton/connection_driver.h>
#include <proton/delivery.h>
#include <proton/proactor.h>
#include <proton/link.h>
#include <proton/message.h>
#include <proton/session.h>
#include <proton/transport.h>
#include <proton/url.h>


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifndef _FONTACOMMUNICATION_H_
#define _FONTACOMMUNICATION_H_

typedef char str[1024];

/*
  Base struct 
*/
typedef struct app_data_t 
{
  str address;                  //address to connect
  str container_id;             //id 
  pn_rwbytes_t message_buffer;  // buffer of the message
  int message_count;            //number og 
  int sent;
  int acknowledged;
  pn_proactor_t *proactor;
  pn_millis_t delay;
  bool delaying;
  pn_link_t *sender;
  bool finished;
} app_data_t;

/* Define the code whrn program stops*/
int exit_code;

/* Inizialize parameters and start comunication*/
void inizialize_communication();

/* Check conditions of */
void check_condition(pn_event_t *e, pn_condition_t *cond);

/* Create a message with a map { "sequence" : number } encode it and return the encoded buffer. */
pn_bytes_t encode_message(app_data_t* app);

/* Send message */
void send(app_data_t* app);

#endif