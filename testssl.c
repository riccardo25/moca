#include <stdio.h>
#include <curl/curl.h>

int main(void)
{
  CURL *curl;
  CURLcode res;

  curl_global_init(CURL_GLOBAL_DEFAULT);

  curl = curl_easy_init();
  struct curl_slist *list = NULL;
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, "https://directline.botframework.com/v3/directline/conversations");
    curl_easy_setopt(curl, CURLOPT_POST, 1);

    list = curl_slist_append(list, "Authorization: Bearer Ay_aHU8nWyk.cwA.xv0.3Q-n11juq0vr4h9f4AZ89B4idQCHJ-bVJF-cA0S3kTI");
    list = curl_slist_append(list, "Content-Type: application/json");
    list = curl_slist_append(list, "Content-length: 0");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, list);

    #ifdef SKIP_PEER_VERIFICATION
        /*
        * If you want to connect to a site who isn't using a certificate that is
        * signed by one of the certs in the CA bundle you have, you can skip the
        * verification of the server's certificate. This makes the connection
        * A LOT LESS SECURE.
        *
        * If you have a CA cert for the server stored someplace else than in the
        * default bundle, then the CURLOPT_CAPATH option might come handy for
        * you.
        */
        
    #endif

    #ifdef SKIP_HOSTNAME_VERIFICATION
        /*
        * If the site you're connecting to uses a different host name that what
        * they have mentioned in their server certificate's commonName (or
        * subjectAltName) fields, libcurl will refuse to connect. You can skip
        * this check, but this will make the connection less secure.
        */
        
    #endif
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
    /* Perform the request, res will get the return code */
    res = curl_easy_perform(curl);
    
    /* Check for errors */
    if(res != CURLE_OK)
      fprintf(stderr, "curl_easy_perform() failed: %s\n",
              curl_easy_strerror(res));

    /* always cleanup */
    curl_easy_cleanup(curl);
  }

  curl_global_cleanup();

  return 0;
}