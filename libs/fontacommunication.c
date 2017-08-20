/*
    FontaCommunication.c
    Written by: Riccardo Fontanini
    Start date: 10 August 2017
    Note:   

*/

#include "fontacommunication.h"



void inizialize_communication()
{
    /* QPID PROTON*/
    //define the connection parameters
    pn_url_t *url = pn_url_parse("amqp://localhost:");
    const char *host = NULL;
    const char *port = NULL;
    //create container
    app_data_t container = {{0}};

    //if url is not null
    if(url)
    {
        host = pn_url_get_host(url);
        port = pn_url_get_port(url);
    }

    //copy the path in container
    strncpy(container.address, pn_url_get_path(url), sizeof(container.address));

    
    //create proacto    
    container.proactor = pn_proactor();
    
    //connects
    pn_proactor_connect(container.proactor, pn_connection(), container.address);
    
    //relase the url variable, not called anymore
    if(url)
    {
        pn_url_free(url);
    }

    //TODO non finito

}


void check_condition(pn_event_t *e, pn_condition_t *cond) 
{
  if (pn_condition_is_set(cond)) {
    exit_code = 1;
    fprintf(stderr, "%s: %s: %s\n", pn_event_type_name(pn_event_type(e)),
            pn_condition_get_name(cond), pn_condition_get_description(cond));
  }
}

/* Create a message with a map { "sequence" : number } encode it and return the encoded buffer. */
pn_bytes_t encode_message(app_data_t* app) 
{
  /* Construct a message with the map { "sequence": app.sent } */
  pn_message_t* message = pn_message();
  pn_data_put_int(pn_message_id(message), app->sent); /* Set the message_id also */
  pn_data_t* body = pn_message_body(message);
  pn_data_put_map(body);
  pn_data_enter(body);
  pn_data_put_string(body, pn_bytes(sizeof("sequence")-1, "sequence"));
  pn_data_put_int(body, app->sent); /* The sequence number */
  pn_data_exit(body);
  /* encode the message, expanding the encode buffer as needed */
  if (app->message_buffer.start == NULL) {
    static const size_t initial_size = 128;
    app->message_buffer = pn_rwbytes(initial_size, (char*)malloc(initial_size));
  }
  /* app->message_buffer is the total buffer space available. */
  /* mbuf wil point at just the portion used by the encoded message */
  pn_rwbytes_t mbuf = pn_rwbytes(app->message_buffer.size, app->message_buffer.start);
  int status = 0;
  while ((status = pn_message_encode(message, mbuf.start, &mbuf.size)) == PN_OVERFLOW) {
    app->message_buffer.size *= 2;
    app->message_buffer.start = (char*)realloc(app->message_buffer.start, app->message_buffer.size);
    mbuf.size = app->message_buffer.size;
  }

  if (status != 0) 
  {
    fprintf(stderr, "error encoding message: %s\n", pn_error_text(pn_message_error(message)));
    exit(1);
  }

  pn_message_free(message);
  return pn_bytes(mbuf.size, mbuf.start);
}


void send(app_data_t* app) 
{
  while (pn_link_credit(app->sender) > 0 && app->sent < app->message_count) 
  {
    ++app->sent;
    // Use sent counter bytes as unique delivery tag.
    pn_delivery(app->sender, pn_dtag((const char *)&app->sent, sizeof(app->sent)));
    pn_bytes_t msgbuf = encode_message(app);
    pn_link_send(app->sender, msgbuf.start, msgbuf.size);
    pn_link_advance(app->sender);
    if (app->delay && app->sent < app->message_count) 
    {
      /* If delay is set, wait for TIMEOUT event to send more */
      app->delaying = true;
      pn_proactor_set_timeout(app->proactor, app->delay);
      break;
    }
  }
}