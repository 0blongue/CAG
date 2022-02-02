/** final_functs.h
 * Name: Ameen Khan
 * Section: T1/2
 * Project: Final Project
*/

#ifndef FINAL_FUNCTS_H
#define FINAL_FUNCTS_H

#include <stdlib.h>
#include <stdio.h>
#include <json-c/json.h>
#include <curl/curl.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

typedef struct EMA_struct{
    int period;
    int size;
    double* values;
} EMA;

typedef struct MemoryStruct_struct{
  char* memory;
  int size;
} MemoryStruct;

typedef struct Response_struct{
    int size;
    char** tokens;
    char** timestamps;
    double* prices;
} Response;

typedef struct Currency_struct{
    int size;
    double* prices;
    EMA* smallEMA;
    EMA* largeEMA;
    DecisionPoint* decisionPoints;
} Currency;

// This was not used in this version of this program
// Could be used in the future to make the program trade automatically
typedef struct DecisionPoint_struct{
    int index;
    char* timestamp;
    double* values;
} DecisionPoint;

/**
 * @brief Create an EMA struct of a certain size based on an initial value
 * 
 * @param period the size of the EMA 
 * @param startValue the initial value
 * @return A pointer to the created struct
 */
EMA* createEMA(int period, double startValue);

/**
 * @brief add an EMA value to the list
 * 
 * @param ema a pointer to the EMA struct that will be added to
 * @param price the price to use for the next EMA value
 */
void addEMAValue(EMA* ema, double price);

/**
 * @brief safely free all memory allocated to an EMA struct
 * 
 * @param ema a pointer to the struct that should be destroyed
 */
void destroyEMA(EMA* ema);

/**
 * @brief return the value of the EMA at the specified index
 * 
 * @param ema a pointer to the EMA struct
 * @param index the point at which to measure the EMA
 * @return the value of the EMA 
 */
double EMAValueAt(EMA* ema, int index);

/**
 * @brief write the response from cURL to memory instead of a file
 * 
 * @param contents the response
 * @param size the size of the data types making up the response
 * @param nmemb the number of the data types making up the response
 * @param userp the pointer to which the data should be written
 * @return the amount of data written
 */
size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

/**
 * @brief Get the current prices of the desired currencies from the API
 * 
 * @param apiKey the user's apiKey from the config file
 * @param tokens the tokens listed in the API file
 * @return a response struct containing the information for the different currencies
 */
Response* getCurrentPrices(char* apiKey, char* tokens);

/**
 * @brief send a specified message to the user
 * 
 * @param to the user to which the message should be sent
 * @param from the email sending the message
 * @param message the file containing the message body
 * @param username the account used to send the message
 * @param password the password of the account used to send the message
 * @return the cURL response code
 */
int sendEmail(char* to, char* from, FILE* message, char* username, char* password);

#endif