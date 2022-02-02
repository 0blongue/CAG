/** final.c
 * Name: Ameen Khan
 * Section: T1/2
 * Project: Final Project
*/

#include "final_functs.h"

int main(int argc, char* argv[]){
    if (argc > 2){
        fprintf(stdout, "No more than 1 argument expected - terminating\n");
        return -1;
    }

    char* apiKey = (char*)malloc(1);
    char* tokens = (char*)malloc(1);
    char* username = (char*)malloc(1);
    char* password = (char*)malloc(1);
    int** emaSizes = (int**)malloc(1);
    char* mailingFileName = (char*)malloc(1);

    // Default to config.json if an argument is not provided
    char* configFilename = argc == 2 ? argv[argc - 1] : "config.json";

    // Read the configuration file
    json_object *root = json_object_from_file(configFilename);

    if (!root){
        fprintf(stderr, "Unable to read \"%s\"- terminating\n", configFilename);
        return -1;
    }

    json_object* APIKEY = json_object_object_get(root, "APIKEY");
    apiKey = (char*)realloc(apiKey, strlen(json_object_get_string(APIKEY)) + 1);
    strcpy(apiKey, json_object_get_string(APIKEY));

    json_object* MAILINGLIST = json_object_object_get(root, "mailingList");
    mailingFileName = (char*)realloc(mailingFileName, 
        strlen(json_object_get_string(MAILINGLIST)) + 1);
    strcpy(mailingFileName, json_object_get_string(MAILINGLIST));

    json_object* USERNAME = json_object_object_get(root, "emailUsername");
    username = (char*)realloc(username, strlen(json_object_get_string(USERNAME)) + 1);
    strcpy(username, json_object_get_string(USERNAME));

    json_object* PASSWORD = json_object_object_get(root, "emailPassword");
    password = (char*)realloc(password, strlen(json_object_get_string(PASSWORD)) + 1);
    strcpy(password, json_object_get_string(PASSWORD));

    json_object* currencyArr = json_object_object_get(root, "currencies");
    int numCurrencies = json_object_array_length(currencyArr);

    strcpy(tokens, "\0");
    for (int i = 0; i < numCurrencies; i++){
        json_object* current = json_object_array_get_idx(currencyArr, i);

        json_object* token = json_object_object_get(current, "token");
        tokens = (char*)realloc(tokens, strlen(json_object_get_string(token)) + 2);
        strcat(tokens, json_object_get_string(token));
        strcat(tokens, ",");

        emaSizes = (int**)realloc(emaSizes, (i + 1) * sizeof(int*));
        emaSizes[i] = (int*)malloc(2 * sizeof(int));
        json_object* emaSmallSize = json_object_object_get(current, "emaSmallSize");
        json_object* emaLargeSize = json_object_object_get(current, "emaLargeSize");
        emaSizes[i][0] = json_object_get_int(emaSmallSize);
        emaSizes[i][1] = json_object_get_int(emaLargeSize);
    }
    json_object_put(root);
    tokens[strlen(tokens) - 1] = '\0';
    

    // Read the emails from the mailing list file
    char** mailingList;
    int numEmails = 0;
    char buffer[50];
    FILE* mailingFile = fopen(mailingFileName, "r");
    while(!feof(mailingFile)){
        int success = fscanf(mailingFile, "%s", buffer);
        if (success == 1){
            mailingList = (char**)realloc(mailingList, ++numEmails * sizeof(char*));
            mailingList[numEmails - 1] = (char*)malloc(strlen(buffer) + 1);
            strcpy(mailingList[numEmails - 1], buffer);
        }
    }
    fclose(mailingFile);

    Response* response;
    // Set up the structs for keeping track of currencies
    Currency currencies[numCurrencies];
    for (int i = 0; i < numCurrencies; i++){
        currencies[i].size = 0;
        currencies[i].prices = (double*)malloc(sizeof(double));
    }

    // Keep track of when the program was started and what the current time is
    unsigned long startTime = (unsigned long)time(NULL);
    unsigned long currentTime = (unsigned long)time(NULL);
    while (currentTime < startTime + 1800){
        response = getCurrentPrices(apiKey, tokens);
        for (int i = 0; i < numCurrencies; i++){

            // Add to the list of prices
            currencies[i].prices = (double*)realloc(currencies[i].prices, 
                                            ++(currencies[i].size) * sizeof(double));

            currencies[i].prices[currencies[i].size - 1] = response->prices[i];

            // Create the small EMA if there are enough data points to do so
            if (currencies[i].size == emaSizes[i][0]){
                double sum = 0;
                for (int j = 0; j < emaSizes[i][0]; j++){
                    sum += currencies[i].prices[j];
                }
                currencies[i].smallEMA = createEMA(emaSizes[i][0], sum/currencies[i].size);
            }

            // Create the large EMA if there are enough data points to do so
            if (currencies[i].size == emaSizes[i][1]){
                double sum = 0;
                for (int j = 0; j < emaSizes[i][1]; j++){
                    sum += currencies[i].prices[j];
                }
                currencies[i].largeEMA = createEMA(emaSizes[i][1], sum/currencies[i].size);
            }
            
            // Add EMA to small and/or large EMA lists if there are enough data points to do so
            if (currencies[i].size > emaSizes[i][0]){
                addEMAValue(currencies[i].smallEMA, response->prices[i]);
            }
            if (currencies[i].size > emaSizes[i][1]){
                addEMAValue(currencies[i].largeEMA, response->prices[i]);
            }

            // Check to see if the EMA lines crossed each other
            if (currencies[i].size > emaSizes[i][1]){
                // This will be true if the small EMA was higher than the  large EMA 
                // at the last measurement
                int lastHigher = EMAValueAt(currencies[i].smallEMA, currencies[i].size - 1) 
                                > EMAValueAt(currencies[i].largeEMA, currencies[i].size - 1);

                // This will be true if the small EMA was higher than the  large EMA 
                // at the second to last measurement
                int previousHigher = EMAValueAt(currencies[i].smallEMA, currencies[i].size - 2) 
                                    > EMAValueAt(currencies[i].largeEMA, currencies[i].size - 2);

                // If a different EMA was on top between the last and second to last measurement,
                // the lines must have crossed. Alert the user.
                if (lastHigher ^ previousHigher){
                    char filename[25];
                    sprintf(filename, "crossings_%s.txt", response->tokens[i]);
                    FILE* outfile = fopen(filename, "w");

                    fprintf(outfile, "\r\n\r\nEMA Crossing Detected [%s] @ %s:\n", 
                                        response->tokens[i], 
                                        response->timestamps[i]);

                    fprintf(outfile, "Current Price: %lf\n\n", 
                                        currencies[i].prices[currencies[i].size - 1]);

                    fclose(outfile);

                    FILE* emailFile = fopen(filename, "r");
                    for(int j = 0; j < numEmails; j++){
                        sendEmail(mailingList[i], username, emailFile, username, password);
                    }
                    fclose(emailFile);
                }
            }
            // Print the current size (for testing purposes only)
            fprintf(stdout, "%d\n", currencies[i].size - 1);
        }
        fprintf(stdout, "%c", '\n');

        // Free dynamically allocated memory for the response
        free(response->tokens);
        free(response->prices);
        free(response->timestamps);
        free(response);

        // Update the time
        currentTime = (unsigned long)time(NULL);
        // Wait for prices to update
        sleep(10);
    }

    // Write the price and EMA data for each currency to a csv file
    for (int i = 0; i < numCurrencies; i++){
        char filename[20];
        sprintf(filename, "output%d.csv", i);
        FILE* outfile = fopen(filename, "w");
        for (int j = 0; j < currencies[i].size; j++){
            fprintf(outfile, "%lf,%lf,%lf,\n", 
                currencies[i].prices[j], 
                EMAValueAt(currencies[i].smallEMA, j), 
                EMAValueAt(currencies[i].largeEMA, j));
        }
        fclose(outfile);
    }

    // Free all dynamically allocated memory
    free(apiKey);
    free(tokens);
    free(username);
    free(password);
    free(emaSizes);
    free(mailingFileName);
    free(configFilename);
    free(mailingList);
    
    return 0;
}