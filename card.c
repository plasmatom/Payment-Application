#include "card.h"


EN_cardError_t getCardHolderName(ST_cardData_t* cardData) {

	uint8_t CHN[100];							//creating a variable to store the user entered value

	printf("Enter Card Holder Name: ");			//asking the user to enter the card holder name
	gets(CHN);								    //getting the card holder name
	size_t cardHolderName_size = strlen(CHN);	//variable to save the lenght of the enterd name

	if ((CHN == NULL)							//checking if the entered name is not pointing to null
		|| (20 > cardHolderName_size)			//checking if the entered name lenght is not less than 20 character long 
		|| (cardHolderName_size > 24))			//checking if the entered name lenght is not more than 24 character long
		return WRONG_NAME;						//if the above happend the function will return WRONG_NAME (1)

	else
	{	/*
		if the above didn't happen the function will store the enterd value in the
		cardHolderName variable and it will return Ok (0)
		*/
		strcpy(cardData->cardHolderName, CHN);
		return OK;
	}

}


EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	uint8_t expirationDate[24], temp[24];		// Declaring a variable to store entered data
	uint8_t* token;								// token pointer to store tokens in it
	printf("Enter Expiration Date MM/YY: ");	// Asking the user to enter the expiration date 
	gets(expirationDate);						// getting the expiration date
	strcpy(temp, expirationDate);				// copying the expiration date into temprory variable

	size_t cardExpirationDate_size = strlen(expirationDate);	// storing the lenght of the enterd value
	if ((cardExpirationDate_size != 5)							// checking the size of the date string
		|| expirationDate[2] != '/')							// checking if not containing '/' character
		return WRONG_EXP_DATE;									// will return WRONG_EXP_DATE(2) if the condition satisfied

	token = strtok(temp, "/");
	uint32_t month = atoi(token);  
								   // slicing the string and getting the values of expiration month and year with string "/"
	token = strtok(NULL, "\0");
	int32_t year = atoi(token);

	if ((expirationDate == NULL)				// checking the date is not null
		|| !(1 <= month)						// checking the expiration month value is between 1 and 12
		|| !(month <= 12)
		|| (year <= 0))							// checking the value of the expiration date is not 0

		return WRONG_EXP_DATE;					/* if the user entered wrong date the function will return 
												WRONG_EXP_DATE(2)
												*/
	else
	{
		// otherwise it will copy the entered data into cardExpirationDate and will return OK (0)
		strcpy(cardData->cardExpirationDate, expirationDate);
		return OK;
	}

}


EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	uint8_t PAN[80];												// Declaring a variable to store entered data
	printf("Enter PAN: ");											// Asking the user to enter the primary account number
	gets(PAN);														// getting PAN
	size_t PAN_size = strlen(PAN);									// storing the size of the PAN

	if ((PAN == NULL) || (16 > PAN_size) || (PAN_size > 19))		//Checking if the PAN is null or larger than 20 or less than 19   
		return WRONG_PAN;											// If so will return WRONG_PAN (3)

	int32_t cast;
	for (int i = 0; i < PAN_size; i++)								// Loop to check if the entered data is all numaric
	{																// If find one none numaric character the function will return  WRONG_PAN (3)
		cast = (int)(PAN[i]) - (int)('0');
		if ((cast > 9) || (cast < 0))
			return WRONG_PAN;
	}

		strcpy(cardData->primaryAccountNumber, PAN);				// If passed aLL that will copy PAN to primaryAccountNumber in cardData 
		return OK;													// Will retun OK (0)
}