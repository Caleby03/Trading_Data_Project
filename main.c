
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>
#define FI "Webull_Orders_Records.csv"

/*
To-Do List

-create a gui
-More Stats
-Stats by day
-Time shit
-On launch tell user last time the csv was updated and prompt if they would like to update it now, y = output message with the file folder info and end
n = continue running program with older csv data
*/

/*
LL: Head -> next -> next ->
Stock: 
Stats blah blah
LL
stats blah
*/

//struct for trade
typedef struct trade{
    int save;
    char name[50]; //Symbol name 
    char ticker[5]; //Ticker for symbol traded
    int side; // Buy or Sell side for trade -1 == buy/ 1 == sell
    int filled;
    int totQty;
    float price; 
    char timePlace[50];
    char timeFilled[50];
}trade;

//Struct for Linked List
typedef struct nodeT{
    trade t;
    struct nodeT *next;
}nodeT;

typedef struct stock{
    char ticker[5];
    int numTrades;
    float pl; 
    nodeT* tl;
}stock;

typedef struct nodeS{
    stock s;
    struct nodeS *next;
}nodeS;


//Function that takes a trade string and a file pointer to return a trade object with the data properly split and assigned 
trade import(char buff[256], FILE* file){
    char* split = strtok(buff, ",");
    char items[11][50];
    int x = 0;
    //Splits the original string based on a "," deliminator then assigns values to items array
    while (split != NULL)
    {
        strcpy(items[x], split);
        split = strtok(NULL, ",");
        x++;
    }
    //When an order is marked cancelled just return and don't worry about other values like the order stats
    if(strcmp(items[3], "Cancelled") == 0){
        trade temp = {0, "/0", "/0", 0, atoi(items[4]), atoi(items[5]), atof(items[7]), "/0", "/0"};
        return temp;
    }

    //If the order say buy side mark it as such and import the trade data, otherwise leaving it marked as sell side
    if(strcmp(items[2], "Buy") == 0){
        trade temp = {1, "/0", "/0", -1, atoi(items[4]), atoi(items[5]), atof(items[7]), "/0", "/0"};
        strcpy(temp.name, items[0]);
        strcpy(temp.ticker, items[1]);
        strcpy(temp.timePlace, items[9]);
        strcpy(temp.timeFilled, items[10]);
        return temp;
    }else{
        trade temp = {1, "/0", "/0", 1, atoi(items[4]), atoi(items[5]), atof(items[7]), "/0", "/0"};
        strcpy(temp.name, items[0]);
        strcpy(temp.ticker, items[1]);
        strcpy(temp.timePlace, items[9]);
        strcpy(temp.timeFilled, items[10]);
        return temp;
    }

}

nodeS* find_stock(nodeS* sl, char tick[5]){
    nodeS* trav = sl;
    while(trav){
        if(strcmp(trav->s.ticker, tick) == 0){
            return trav;
        }
        trav = trav->next;
    }
    return NULL;
}

void trade_sort(nodeT* tlHead, nodeS* slHead){
    int x = 1;
    nodeT* tlTrav = tlHead->next;
    nodeS* slTrav = slHead;
    nodeT* sltTemp;
    while(tlTrav->next){
        nodeS* temp = find_stock(slHead, tlTrav->t.ticker);
        if(temp == NULL){
            while(slTrav->next != NULL){
                slTrav = slTrav->next;
            }
            slTrav->next = (nodeS*)malloc(sizeof(nodeS));
            slTrav = slTrav->next;
            slTrav->next = NULL;
            slTrav->s.numTrades = 1;
            slTrav->s.pl = 0.0;
            strcpy(slTrav->s.ticker,  tlTrav->t.ticker);
            slTrav->s.tl = (nodeT*)malloc(sizeof(nodeT));
            slTrav->s.tl->t = tlTrav->t;
            slTrav->s.tl->next = NULL;
        }else{
            temp->s.numTrades++;
            sltTemp = temp->s.tl;
            while(sltTemp->next != NULL){
                sltTemp = sltTemp->next;
            }
            sltTemp->next = (nodeT*)malloc(sizeof(nodeT));
            sltTemp = sltTemp->next;
            sltTemp->t = tlTrav->t;
            sltTemp->next = NULL;

            
            
        }
        tlTrav = tlTrav->next;
        x++;
    }
    return;
}

void stock_calcPL(int stockCnt, nodeS* slHead){
    nodeS* slTrav = slHead;
    
    for(int i = 0; i <= stockCnt; i++){
        nodeT* tlTrav = slTrav->s.tl;
        for(int j = 0; j < slTrav->s.numTrades; j++){
            slTrav->s.pl += (tlTrav->t.side*tlTrav->t.price*tlTrav->t.filled);
            tlTrav = tlTrav->next;
        }
        slTrav = slTrav->next;
    }
}

void print_menu(){
    printf("Trading Data Menu\n");
    printf("Please select one below:\n");
    printf("l: List all non-cancelled trades in data\n");
    printf("s: List all stocks that were traded (Unordered)\n");
    printf("d: Basic data about # of trades, stocks, and current P/L\n");
    printf("x: Exit\n");
    return;
}

int main(){
    //Opening File pointer
    FILE* fp = fopen(FI,"r");
    if(fp){
        printf("File Successfully Opened\n");
    }else{
        printf("File Failed to Open\n");
    }

    //Init variables 
    float tot = 0.0;
    char buffer[256];
    int numTotTrades = -1;
    int numFilledTrades = -2;
    int numStocksTraded = -1;

    //Init linked lists
    nodeT* tlHead = (nodeT*)malloc(sizeof(nodeT));
    tlHead->next = NULL;
    nodeT* trav = tlHead;
    nodeS* slHead = (nodeS*)malloc(sizeof(nodeS));
    slHead->s.numTrades = 0;
    strcpy(slHead->s.ticker , "Symb");
    slHead->s.tl = NULL;
    slHead->next = NULL;
    nodeS* slTrav = slHead;

    //imports the trade data from csv file and converts it to a trade object and add it the master linked list of trades
    
    while(fgets(buffer, sizeof(buffer), fp)){
        trade temp = import(buffer,fp);
         
        if(temp.save == 1){
           trav->t = temp;
           tot += trav->t.filled * trav->t.price * trav->t.side;
           trav->next = (nodeT*)malloc(sizeof(nodeT));
           trav = trav->next;
           trav->next = NULL;  
        }
        numTotTrades++;
    }

    trade_sort(tlHead, slHead);

    nodeT* tempT = tlHead;
    while(tempT != NULL){
        tempT = tempT->next;
        numFilledTrades++;
    }

    nodeS* tempS = slHead;
    while(tempS != NULL){
        tempS = tempS->next;
        numStocksTraded++;
    }

    stock_calcPL(numStocksTraded, slHead);
    
    char sel;
    while(sel != 'x'){
        print_menu();
        scanf("\n%c", &sel);
        switch(sel){
            case 'l':
            {
                nodeT* temp1 = tlHead;
                for(int i = 0; i <= numFilledTrades; i++){                    
                    if(temp1->t.side == 1){
                        if(temp1->t.price > 1){
                            printf("%d: %s %s %d/%d Filled @%.2f Placed: %s Filled: %s", i,"Sell", 
                            temp1->t.ticker, temp1->t.filled, temp1->t.totQty, temp1->t.price, temp1->t.timePlace, temp1->t.timeFilled);
                        }else{
                            printf("%d: %s %s %d/%d Filled @%.2f Placed: %s Filled: %s", i,"Sell", 
                            temp1->t.ticker, temp1->t.filled, temp1->t.totQty, temp1->t.price, temp1->t.timePlace, temp1->t.timeFilled);
                        }
                    }else if(temp1->t.side == -1){
                        if(temp1->t.price > 1){
                            printf("%d: %s %s %d/%d Filled @%.2f Placed: %s Filled: %s", i,"Buy ", 
                            temp1->t.ticker, temp1->t.filled, temp1->t.totQty, temp1->t.price, temp1->t.timePlace, temp1->t.timeFilled);
                        }else{
                            printf("%d: %s %s %d/%d Filled @%.2f Placed: %s Filled: %s", i,"Buy ", 
                            temp1->t.ticker, temp1->t.filled, temp1->t.totQty, temp1->t.price, temp1->t.timePlace, temp1->t.timeFilled);
                        }
                        
                    }
                    temp1 = temp1->next;
                }
                
                break;
            }
            case 's':
            {
                nodeS* temp2 = slHead->next;
                for(int i = 0; i < numStocksTraded; i++){
                    printf("%d: %.5s \n  # Trades %d \n  Stock P/L: $%.2f\n", i+1, temp2->s.ticker, temp2->s.numTrades, temp2->s.pl);
                    temp2  = temp2->next;
                }
                break;
            }
            case 'd':
            {
                printf("# of Trade Orders Placed: %d\n", numTotTrades);
                printf("# of Trades Filled: %d\n", numFilledTrades);
                printf("# of Stocks traded: %d\n", numStocksTraded);
                printf("P/L: %.2f\n", tot);
                break;
            }
            case 'x':
            {
                printf("Goodbye, thank you for trading!");
                break;
            }
            default:
            {
                printf("Invalid Character entered, please enter one of the options listed\n");
                break;
            }
        }
    }
    

    //Free the nodes used to store and manage the trade data
    
    while(tlHead != NULL){
        nodeT* temp = tlHead;
        tlHead = tlHead->next;
        free(temp);
        numFilledTrades++;
    }

    
    while(slHead != NULL){
        nodeS* temp = slHead;
        slHead = slHead->next;
        while(temp->s.tl != NULL){
            nodeT* temp2 = temp->s.tl;
            temp->s.tl = temp->s.tl->next;
            free(temp2);
        }
        free(temp);
        numStocksTraded++;
    }

    fclose(fp);
    return 1;
}