/** final_functs.c
 * Name: Ameen Khan
 * Section: T1/2
 * Project: Final Project
*/

#include "final_functs.h"

EMA* createEMA(int period, double startValue){
    EMA* ema = (EMA*)malloc(sizeof(EMA));
    ema->period = period;
    ema->size = period;
    ema->values = (double*)malloc(sizeof(double)*ema->size);
    for(int i = 0; i < ema->size; i++){
        ema->values[i] = startValue;
    }
    return ema;
}

void addEMAValue(EMA* ema, double price){
    double multiplier = 2.0/((double)ema->period + 1);
    ema->size++;
    ema->values = (double*)realloc(ema->values, sizeof(double) * ema->size);
    ema->values[ema->size - 1] = price * multiplier + ema->values[ema->size - 2] * (1 - multiplier);
}

void destroyEMA(EMA* ema){
    free(ema->values);
    free(ema);
}

double EMAValueAt(EMA* ema, int index){
    return ema->values[index];
}

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp){
  int realsize = size * nmemb;
  MemoryStruct *mem = (MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

Response* getCurrentPrices(char* apiKey, char* tokens){
    char* baseURL = "https://api.nomics.com/v1/currencies/ticker?key=&ids=&interval=1h&convert=USD&per-page=100&page=1";
    int urlLength = strlen(baseURL) + strlen(apiKey) + strlen(tokens);
    char* url = (char*)malloc(urlLength + 1);
    sprintf(url, "https://api.nomics.com/v1/currencies/ticker?key=%s&ids=%s&interval=1h&convert=USD&per-page=100&page=1", apiKey, tokens);
    
    CURL *curl;
    CURLcode res;
    curl = curl_easy_init();

    MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;
     if(curl) {
        //Set the URL to the API call
        curl_easy_setopt(curl, CURLOPT_URL, url);

        //For HTTPS
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

        //Write the returned data to memory
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&chunk);

        //Perform the request and save the response code
        res = curl_easy_perform(curl);
        //printf("\nResult of Operation: %d\n", res);
    }
    curl_easy_cleanup(curl);

    json_object *root = json_tokener_parse(chunk.memory);
    Response *response = (Response*)malloc(sizeof(Response));
    response->size = json_object_array_length(root);
    response->tokens = (char**)malloc(sizeof(char*)*response->size);
    response->timestamps = (char**)malloc(sizeof(char*)*response->size);
    response->prices = (double*)malloc(sizeof(double)*response->size);
    //Exit program if data cannot be read
    if (!root)
        exit(-1);
    
    for(int i = 0; i < response->size; i++){
        json_object *current = json_object_array_get_idx(root, i);
        json_object *price, *timestamp, *id;
        json_object_object_get_ex(current, "price", &price);
        json_object_object_get_ex(current, "price_timestamp", &timestamp);
        json_object_object_get_ex(current, "id", &id);

        response->tokens[i] = (char*)malloc(sizeof(char)*strlen(json_object_get_string(id)) + 1);
        strcpy(response->tokens[i], json_object_get_string(id));

        response->timestamps[i] = (char*)malloc(sizeof(char)*strlen(json_object_get_string(timestamp)));
        strcpy(response->timestamps[i], json_object_get_string(timestamp) + 1);
        response->prices[i] = json_object_get_double(price);
    }
    json_object_put(root);
    free(chunk.memory);
    return response;
}