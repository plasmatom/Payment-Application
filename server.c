#include "server.h"

void lunGenerator(char *PAN)
{
    uint32_t sum = 0, digit, PANLenght;
    PANLenght = (rand() % 4) + 16;
    uint8_t parity = ((PANLenght - 2) % 2);

    for (int i = 0; i <= PANLenght-2 ; i++)
    {
        digit = (rand() % 10);
        PAN[i] = digit + '0';
        if ((i % 2) == parity)
            digit *= 2;
        if (digit > 9)
            digit -= 9;
        sum += digit;
    }
    if (sum % 10)
    {
        PAN[PANLenght - 1] = (10 - (sum % 10)) + '0';
    }
    else
        PAN[PANLenght - 1] = '0';

    PAN[PANLenght] = '\0';
}
void serverSideAccounts()
{
    //this function will creat accounts randomly
    //this function will be called once

    short int accNumLen;
    FILE* fAccounts;
    char accountNumber[20];
    char fileName[] = "Accounts DB.txt";

    //openning the file
    fAccounts = fopen(fileName, "w");
    srand(10);

    // generating random accounts ;
    for (int i = 0; i < 159; i++)
    {
        // generating account numbers from 16 to 19 luhn number 
        lunGenerator(accountNumber);
        //writing to the file
        fprintf(fAccounts, "%s,%f\n", accountNumber, ((rand() + 0.0) + ((rand() % 100) * 0.01)));
    }
    fclose(fAccounts);
}

float accountInDataBase(char acc[]) // returns the balance if the account exists atherwise returns -1
{
    float balance;
    char* token;
    char Accounts[50];

    FILE* fAccountDb = fopen("Accounts DB.txt", "r"); // opening the file

    while (fgets(Accounts, 50, fAccountDb) != NULL)  // getting the line which is in the form (PAN,balanc)
    {
        token = strtok(Accounts, ",");   // seperating the account number from 
        if (!(strcmp(token, acc)))       // comparing the PAN of the card with the one from the data base 
        {
            token = strtok(NULL, ",");
            balance = atof(token);
            fclose(fAccountDb);
            return balance;             // if the account found will return the balance
        }
    }
    fclose(fAccountDb);
    return -1;
}

EN_serverError_t isValidAccount(ST_cardData_t* cardData)
{
    // this function checks if the account in the data base file.
    if ((glbalance = accountInDataBase(cardData->primaryAccountNumber)) == -1)  
        return DECLINED_STOLEN_CARD;

    else
        return OK;
}

EN_serverError_t isAmountAvailable(ST_terminalData_t* termData)
{

    if (glbalance < termData->transAmount) // this function compares the balnce extracted from the data base with the amount entered
        return LOW_BALANCE;
    else
        return OK;
}


void transSeqNum(ST_transaction_t* transData)
{
    // this function will count all transactions then assign the new transaction a number

    int sequenceNum = 0;    
    char buffer[50];
    FILE* fTransactions;
    fTransactions = fopen("Transactions DB.txt", "r"); // opening the transactions folder
    if (fTransactions == NULL)                         // if there is no file 
    {
        transData->transactionSequenceNumber = sequenceNum; // asign the transaction sequence number the value 0
        return;
    }
    while (fgets(buffer, 50, fTransactions) != NULL) // other wise read line by line untill you reach the end of the file 
        sequenceNum++;                               // keep increamting the sequence number by 1

    transData->transactionSequenceNumber = sequenceNum;
}

EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
    // this function saves the transaction in the file
    transSeqNum(transData); // assigning the transaction seqnce number
    FILE* fTransactions;
    fTransactions = fopen("Transactions DB.txt", "a"); // appending to the data base

    if (fTransactions)
    {
        fprintf(fTransactions, "%d,%d,%f,%s,%s\n", transData->transactionSequenceNumber,
            transData->transState,
            transData->terminalData.transAmount,
            transData->terminalData.transactionDate,
            transData->cardHolderData.primaryAccountNumber);
        return OK;
    }
    else
        return SAVING_FAILED;

}

EN_transState_t updateAccountDB(ST_cardData_t cardData, float newBalance)
{
    // this function updates the database by creating a new file then renaming it

    FILE* tempFile;
    FILE* fAccountDb;
    char* token;
    char Accounts[50];
    char temp[50];

    fAccountDb = fopen("Accounts DB.txt", "r");         // opening the data base
    tempFile = fopen("Accounts DB temp.txt", "w");      // opening the new data base file 
    if (!fAccountDb || !tempFile)
        return INTERNAL_SERVER_ERROR;


    // this loops copies the content of the old data base to the new one execpt for the account that is wanted to be updated 
    while (fgets(Accounts, 50, fAccountDb) != NULL)
    {

        strcpy(temp, Accounts);
        token = strtok(Accounts, ",");
        if (!(strcmp(token, cardData.primaryAccountNumber)))
        {
            fprintf(tempFile, "%s,%f\n", cardData.primaryAccountNumber, newBalance);
        }
        else
            fprintf(tempFile, "%s", temp);
    }
    fclose(fAccountDb);
    fclose(tempFile);
    remove("Accounts DB.txt");  // removing the old file
    if (rename("Accounts DB temp.txt", "Accounts DB.txt")) // renaming the new file
        return INTERNAL_SERVER_ERROR;
    return APPROVED;
}

EN_serverError_t recieveTransactionData(ST_transaction_t* transData)
{
    // this function utilizes previosly declared functions

    if (isValidAccount(&transData->cardHolderData)) // if error is fount will return DECLINED_STOLEN_CARD (2)
    {
        return transData->transState = DECLINED_STOLEN_CARD;
    }

    else if (isAmountAvailable(&transData->terminalData)) // if the availble amount is insuffecient it will return DECLINED_INSUFFECIENT_FUND (1)
    {
        return transData->transState = DECLINED_INSUFFECIENT_FUND;
    }

    // Otherwise will return OK (0) and update the database 
    return transData->transState = updateAccountDB(transData->cardHolderData, glbalance - transData->terminalData.transAmount);

}