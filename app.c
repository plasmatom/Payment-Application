
void appStart()
{
	ST_transaction_t transaction;
	uint8_t error;
	uint8_t i[3] = {0,0,0};

	error = 10;
	while (error != OK)	// getting the amount for transaction
	{
		error = setMaxAmount(&transaction.terminalData); // Setting the Maximum Amount To Dra
		if (error)
		{
			printf("Invalid Maximum Amount Try Again.\n");
		}
	}

	getchar();
	printf("------------------------------------------------------------\n");

	error = 4;
	while (error != OK)  // Getting the card data and making sure that it is correct
	{
		if (!i[0])
		{
			error = getCardHolderName(&transaction.cardHolderData);
			i[0] = 1;
		}
		if (error == WRONG_NAME)
		{
			printf("Wrong Card Holder Name.\n");
			error = getCardHolderName(&transaction.cardHolderData);
			if(error)
				continue;
		}

		if (!i[1])
		{
			error = getCardExpiryDate(&transaction.cardHolderData);
			i[1] = 1;
		}
		if (error == WRONG_EXP_DATE)
		{
			printf("Wrong Expiry Date.\n");
			error = getCardExpiryDate(&transaction.cardHolderData);
			if (error)
				continue;
		}

		if (!i[2])
		{
			error = getCardPAN(&transaction.cardHolderData);
			i[2] = 1;
		}
		if (error == WRONG_PAN)
		{
			
			printf("Wrong Primary Account Number.\n");
			error = getCardPAN(&transaction.cardHolderData);
			if (error)
				continue;
		}

	}

	if (isValidCard(transaction.cardHolderData)) // valdating the primary account number for luhn
	{
		printf("Declined Invalid Card\n");
		return;
	}

	getTransactionDate(&transaction.terminalData); // getting the date of the transaction

	if (isCardExpired(transaction.cardHolderData, transaction.terminalData)) // cheking if the card is expired
	{
		printf("Declined Expired Card\n");
		return;
	}

	error = 10;
	while (error != OK)	// getting the amount for transaction
	{
		error = getTransactionAmount(&transaction.terminalData);
		if (error)
		{
			printf("Invalid Amount Try Again.\n");
		}
	}

	if (isBelowMaxAmount(&transaction.terminalData)) // checking if the amount does not exceed the maximum amount
	{
		printf("Declined Max Amount Exceeded");
		return;
	}

	error = recieveTransactionData(&transaction); // checking the data base

	if (error == DECLINED_STOLEN_CARD)
	{
		printf("Declined Invalid Account");
	}
	else if (error == DECLINED_INSUFFECIENT_FUND)
	{
		printf("Declined Insuffecient Fund");
	}
	else
		printf("Transaction Approved\n");

	saveTransaction(&transaction); // saving the transaction

	printf("\n-----------------------------------------------------------\nPress any thing to close the app.");
	scanf("%c", error);
}