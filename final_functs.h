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

typedef struct DecisionPoint_struct{
    int index;
    char* timestamp;
    double* values;
} DecisionPoint;

typedef struct Currency_struct{
    int size;
    double* prices;
    EMA* smallEMA;
    EMA* largeEMA;
    DecisionPoint* decisionPoints;
} Currency;

EMA* createEMA(int period, double startValue);

void addEMAValue(EMA* ema, double price);

void destroyEMA(EMA* ema);

double EMAValueAt(EMA* ema, int index);

size_t WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp);

Response* getCurrentPrices(char* apiKey, char* tokens);

Response getNextHistorical(FILE* inputFile, char* tokens);
#endif