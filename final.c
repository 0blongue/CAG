/** final.c
 * Name: Ameen Khan
 * Section: T1/2
 * Project: Final Project
*/

#include "final_functs.h"

int main(){
    char* apiKey = "f7b2efb50dd0cdcc6b4ccd5f0808320b78e06194";
    char* tokens = "BTC,ETH,XRP";
    char* url = "https://api.nomics.com/v1/currencies/ticker?"
                    "key=f7b2efb50dd0cdcc6b4ccd5f0808320b78e06194"
                    "&ids=BTC,ETH,XRP&interval=1h&convert=USD&per-page=100&page=1";
    int numCurrencies = 3;
    int emaSizes[][2] = {{5, 8}, {12, 26}, {50, 100}};
    Response* response;
    Currency currencies[numCurrencies];
    for (int i = 0; i < numCurrencies; i++){
        currencies[i].size = 0;
        currencies[i].prices = (double*)malloc(sizeof(double));
    }
    unsigned long startTime = (unsigned long)time(NULL);
    unsigned long currentTime = (unsigned long)time(NULL);
    while (currentTime < startTime + 27000){
        response = getCurrentPrices(apiKey, tokens);
        for (int i = 0; i < numCurrencies; i++){
            currencies[i].prices = (double*)realloc(currencies[i].prices, 
                                            ++(currencies[i].size) * sizeof(double));

            currencies[i].prices[currencies[i].size - 1] = response->prices[i];

            if (currencies[i].size == emaSizes[i][0]){
                double sum = 0;
                for (int j = 0; j < emaSizes[i][0]; j++){
                    sum += currencies[i].prices[j];
                }
                currencies[i].smallEMA = createEMA(emaSizes[i][0], sum/currencies[i].size);
            }
            if (currencies[i].size == emaSizes[i][1]){
                double sum = 0;
                for (int j = 0; j < emaSizes[i][1]; j++){
                    sum += currencies[i].prices[j];
                }
                currencies[i].largeEMA = createEMA(emaSizes[i][1], sum/currencies[i].size);
            }
            
            if (currencies[i].size > emaSizes[i][0]){
                addEMAValue(currencies[i].smallEMA, response->prices[i]);
            }
            if (currencies[i].size > emaSizes[i][1]){
                addEMAValue(currencies[i].largeEMA, response->prices[i]);
            }

            if (currencies[i].size > emaSizes[i][1]){
                int lastHigher = EMAValueAt(currencies[i].smallEMA, currencies[i].size - 1) > EMAValueAt(currencies[i].largeEMA, currencies[i].size - 1);
                int previousHigher = EMAValueAt(currencies[i].smallEMA, currencies[i].size - 2) > EMAValueAt(currencies[i].largeEMA, currencies[i].size - 2);

                if (lastHigher ^ previousHigher){
                    char filename[25];
                    sprintf(filename, "crossings_%s.txt", response->tokens[i]);
                    FILE* outfile = fopen(filename, "w");

                    fprintf(outfile, "EMA Crossing Detected [%s] @ %s:\n", response->tokens[i], response->timestamps[i]);
                    fprintf(outfile, "Current Price: %lf\n\n", currencies[i].prices[currencies[i].size - 1]);

                    fclose(outfile);
                }
            }

            //fprintf(stderr, "%lf\n", currencies[i].prices[currencies[i].size - 1]);
        }
        //fprintf(stderr, "%c", '\n');
        free(response->tokens);
        free(response->prices);
        free(response->timestamps);
        free(response);
        currentTime = (unsigned long)time(NULL);
        sleep(10);
    }

    for (int i = 0; i < numCurrencies; i++){
        char filename[20];
        sprintf(filename, "output%d.csv", i);
        FILE* outfile = fopen(filename, "w");
        for (int j = 0; j < currencies[i].size; j++){
            fprintf(outfile, "%lf,%lf,%lf,\n", currencies[i].prices[j], EMAValueAt(currencies[i].smallEMA, j), EMAValueAt(currencies[i].largeEMA, j));
        }
        fclose(outfile);
    }
    exit(0);


    FILE* fp = fopen("test_data.csv", "r");
    int counter = 0;
    int smallSize = 5;
    int largeSize = 8;

    //TODO: make this dynamic 2D memory allocation and have each element be an array containing 
    //      time and price
    double* prices = (double*)malloc(sizeof(double)*largeSize);
    char garbage[1000];

    EMA *emaSmall, *emaLarge;


    while(counter < largeSize){
        //create the small ema when there are enough data points for an sma with the same period size
        if (counter == smallSize){
            int sum = 0;
            for (int i = 0; i < smallSize; i++){
                sum += prices[i];
            }
            emaSmall = createEMA(smallSize, sum/smallSize);
        }

        //only care about price data for now
        fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%[^\n]", garbage, garbage, garbage, garbage, garbage, garbage, &prices[counter], garbage);
        if (counter >= smallSize){
            addEMAValue(emaSmall, prices[counter]);
        }
        counter++;
    }

    //create the ema struct for the ema with the larger period size
    int sum = 0;
    for (int i = 0; i < counter; i++){
        sum += prices[i];
    }
    emaLarge = createEMA(largeSize, sum/counter);

    //calculate ema values as price data is read
    while(!feof(fp)){
        prices = (double*)realloc(prices, sizeof(double) * ++counter);
        fscanf(fp, "%[^,],%[^,],%[^,],%[^,],%[^,],%[^,],%lf,%[^\n]", garbage, garbage, garbage, garbage, garbage, garbage, &prices[counter - 1], garbage);
        addEMAValue(emaSmall, prices[counter - 1]);
        addEMAValue(emaLarge, prices[counter - 1]);
    }

    //write all values to output
    FILE* outfile = fopen("output.csv", "w");
    for (int i = 0; i < counter; i++){
        fprintf(outfile, "%lf,%lf,%lf,\n", prices[i], EMAValueAt(emaSmall, i), EMAValueAt(emaLarge, i));
    }

    //clean up memory
    free(prices);
    destroyEMA(emaSmall);
    destroyEMA(emaLarge);
    fclose(fp);
    fclose(outfile);
}