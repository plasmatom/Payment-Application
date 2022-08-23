#include "terminal.h"


EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
    time_t curdate;                                                               //creating time_t variable
    uint8_t curDATE[11];                                                          //buffer that points to the data
    struct tm* dateInfo;                                                          //creating struct tm variable to store the date in it
    time(&curdate);                                                               //initailzing curdate to the current date and time 
    dateInfo = localtime(&curdate);                                               //initliazing dateInfo to the local time
    strftime(termData->transactionDate, 11, "%d/%m/20%y", dateInfo);              //changing the format and storing in curDate
    return OK;


}

EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{


    char* token1, * token2;                                         // tokens to slice the strings

    token1 = strtok(termData.transactionDate, "/");                 // passing the day
    token1 = strtok(NULL, "/");                                     // passing the month
    short int CurMonth = atoi(token1);                              // stroing current month value
    token1 = strtok(NULL, "/");                                     // passing the year
    short int CurYear = atoi(token1) - 2000;                        // storing the current year value

    token2 = strtok(cardData.cardExpirationDate, "/");              // passing the  month
    short int ExpMonth = atoi(token2);                              // stroring the expiry month value
    token2 = strtok(NULL, "\0");                                    // passing the year     
    short int ExpYear = atoi(token2);                               // storing the expiry year value

    if (CurYear > ExpYear)                                              // comparing between the current date and expiry date
        return EXPIRED_CARD;
    else if (CurYear == ExpYear && CurMonth >= ExpMonth)
        return EXPIRED_CARD;
    else
        return OK;
}
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
    float Amount;                                    // Variable to store the withdrow amount
    printf("Enter transaction amount: ");            // asking the user to Enter the amount wanted
    scanf("%f", &Amount);                            // getting the amount from the user

    if (Amount <= 0.0)                              // checking if the amount is less than or equal to zero
        return INVALID_AMOUNT;                      // if so will return INVALID_AMOUNT (4)
    else
    {
        termData->transAmount = Amount;             // otherwise will save the amount in transAmount 
        return OK;                                  // will return OK (0)
    }
}

EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
    if (termData->maxTransAmount >= termData->transAmount) // the function compares between the maxmimum transaction allowed and the request amount
        return OK;                                         // if the requested transaction amount is less than or equal to the maximum allowed
                                                           // it will return OK (0)
    else
        return EXCEED_MAX_AMOUNT;                          // Otherwise it will return EXCEED_MAX_AMOUNT (5)
}

EN_terminalError_t setMaxAmount(ST_terminalData_t* termData)
{
 
    float MaxAmount;                                    // This function for setting the maximum transaction allowed
    printf("Set Maximum Amount: ");                      // wil ask the user to set the mismum amount
    scanf("%f", &MaxAmount);                            
    if (MaxAmount <= 0.0)                               // if the maximum amount is less than or equal to 0
        return INVALID_MAX_AMOUNT;                      // will return INVALID_MAX_AMOUNT (6)

    else
    {
        termData->maxTransAmount = MaxAmount;           // otherwise will save the maxmount and return OK (0)
        return OK;
    }
        
}

EN_terminalError_t isValidCard(ST_cardData_t cardData)
{
    size_t PANLenght = strlen(cardData.primaryAccountNumber);
    uint32_t sum = (int)(cardData.primaryAccountNumber[PANLenght-1])- (int)('0');
    uint32_t digit;
    uint8_t parity = ((PANLenght - 2) % 2);

    for (int i = 0; i <= PANLenght - 2; i++)
    {
        digit = (int)(cardData.primaryAccountNumber[i]) - (int)('0');
        if (i % 2 == parity)
            digit *= 2;
        if (digit > 9)
            digit -= 9;
        sum += digit;
    }

    if ((sum % 10) == 0)
        return OK;
    else
        return INVALID_CARD;
}
