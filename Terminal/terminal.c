#include <stdio.h>
#include <string.h>
#include "terminal.h"
#include <time.h>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS

/*
*The function will read the current date from your computer and store it into terminal data with the mentioned size and format.
*Transaction date is 10 characters string in the format DD/MM/YYYY, e.g 25/06/2022.
*If the transaction date is NULL, less than 10 characters or wrong format will return WRONG_DATE error, else return OK.
*/
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData)
{
	/*return check*/
	EN_terminalError_t errorState = TERMINAL_OK;

	/*return time and date from PC*/
	time_t dateC = time(NULL);
	struct tm* currentDate = localtime(&dateC);

	/*I will recevie the day's date in different values */
	unit8_t day = currentDate->tm_mday;
	unit8_t month = currentDate->tm_mon + 1; // add 1 becuse system started from month zero.
	unsigned int year = currentDate->tm_year + 1900; // add 1900 becuse system late 1900 years -_-.

	/*store a day in the array as character to set the format as required*/
	if (day <= 9)
	{
		termData->transactionData[0] = ASCII_ZERO;
		termData->transactionData[1] = day + ASCII_ZERO;
	}
	else if (day > 9)
	{
		termData->transactionData[1] = (day % 10) + ASCII_ZERO;
		day /= 10;
		termData->transactionData[0] = (day % 10) + ASCII_ZERO;
	}

	/*stor '/' between day and month to set the format as required*/
	termData->transactionData[2] = '/';

	/*store a month in the array as character to set the format as required*/
	if (month <= 9)
	{
		termData->transactionData[3] = ASCII_ZERO;
		termData->transactionData[4] = month + ASCII_ZERO;
	}
	else if (month > 9)
	{
		termData->transactionData[3] = 1 + ASCII_ZERO;
		termData->transactionData[4] = (month % 10) + ASCII_ZERO;
	}

	/*stor '/' between month and year to set the format as required*/
	termData->transactionData[5] = '/';

	/*store a year in the array as character to set the format as required*/
	for (unit8_t countDown = 9; countDown >= 6; countDown--)
	{
		termData->transactionData[countDown] = (year % 10) + ASCII_ZERO;
		year /= 10;
	}
	termData->transactionData[10] = '\0';

	/*check If the transaction date is NULL, less than 10 characters or wrong format will return WRONG_DATE error, else return OK.*/
	if ((strlen(termData->transactionData) == NULL)
		|| (strlen(termData->transactionData) < TERM_LEN)
		|| (termData->transactionData[2] != '/')
		|| (termData->transactionData[5] != '/'))
	{
		errorState = WORNG_DATE;
	}
	else {
		errorState = TERMINAL_OK;
	}
	return errorState;
	puts(termData->transactionData);
}


/*This function compares the card expiry date with the transaction date.
 *If the card expiration date is before the transaction date will return EXPIRED_CARD, else return OK.
 */
EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData)
{
	/*return check*/
	EN_terminalError_t errorState = TERMINAL_OK;

	/*check if year of expiration is a same current year.
	 * check if month less than current month or larger.
	 */
	 /*************************************************************************************************************/
	if ((cardData.cardExpirationData[3] == termData.transactionData[8])
		&& (cardData.cardExpirationData[4] == termData.transactionData[9]))
	{
		/*check if month less than current month or larger.*/
		/*************************************************************************************************************/
		if (termData.transactionData[3] - ASCII_ZERO == 1)
		{
			if (cardData.cardExpirationData[0] != termData.transactionData[3])
			{
				errorState = EXPIRED_CARD;
			}
			else if ((cardData.cardExpirationData[1] < termData.transactionData[4])
				|| (cardData.cardExpirationData[1] > 2))
			{
				errorState = EXPIRED_CARD;
			}
			else
			{
				errorState = TERMINAL_OK;
			}
		}
		else if (termData.transactionData[3] - ASCII_ZERO == 0)
		{
			if (cardData.cardExpirationData[0] == termData.transactionData[3])
			{
				if (cardData.cardExpirationData[1] < termData.transactionData[4])
				{
					errorState = EXPIRED_CARD;
				}
				else
				{
					errorState = TERMINAL_OK;
				}
			}
			else if (cardData.cardExpirationData[0] - ASCII_ZERO == 1)
			{
				if (((cardData.cardExpirationData[1] - ASCII_ZERO) < 0 || (cardData.cardExpirationData[1] - ASCII_ZERO) > 2))
				{
					errorState = EXPIRED_CARD;
				}
				else
				{
					errorState = TERMINAL_OK;
				}
			}
			else
			{
				errorState = EXPIRED_CARD;
			}
		}
		else
		{
			errorState = EXPIRED_CARD;
		}
		/*************************************************************************************************************/
	}
	/*************************************************************************************************************/


	/*check if year of expiration is larger than the current year.
	 *check if a month is real month.
	 */
	 /*************************************************************************************************************/
	else if ((cardData.cardExpirationData[3] >= termData.transactionData[8])
		&& (cardData.cardExpirationData[4] > termData.transactionData[9]))
	{
		/*check if a month is real month.*/
		/*************************************************************************************************************/
		if ((cardData.cardExpirationData[0] - ASCII_ZERO) == 0)
		{
			if (((cardData.cardExpirationData[1] - ASCII_ZERO) < 1
				|| (cardData.cardExpirationData[1] - ASCII_ZERO) > 9))
			{
				errorState = EXPIRED_CARD;
			}
			else
			{
				errorState = TERMINAL_OK;
			}
		}
		else if ((cardData.cardExpirationData[0] - ASCII_ZERO) == 1)
		{
			if (((cardData.cardExpirationData[1] - ASCII_ZERO) < 0
				|| (cardData.cardExpirationData[1] - ASCII_ZERO) > 2))
			{
				errorState = EXPIRED_CARD;
			}
			else
			{
				errorState = TERMINAL_OK;
			}
		}
		else
		{
			errorState = EXPIRED_CARD;
		}
		/*************************************************************************************************************/
	}
	/*************************************************************************************************************/
	else
	{
		errorState = EXPIRED_CARD;
	}
	return errorState;
}


/*This function checks if the PAN is a Luhn number or not.
 *If PAN is not a Luhn number will return INVALID_CARD, else return OK.
 */
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
	/*return check*/
	EN_terminalError_t errorState = TERMINAL_OK;


	/*this is my implement to luhan Algorithm
	 *check sum of all value that its index ODD ----- (1)
	 *value that its index even, multiply each one by 2 and check if result >= 10, subtract 9 from result ----- (2)
	 * if sum of (1)+(2) % 10 = 0,	PAN vaild
	 */
	int counter = 0, oddSum = 0, evenEquation = 0;
	for (counter = 0; counter < strlen(cardData->primaryAccountNumber); counter++) {
		if (counter % 2 == 0)
		{
			if (((cardData->primaryAccountNumber[counter] - ASCII_ZERO) * 2) >= 10)
			{
				evenEquation += (((cardData->primaryAccountNumber[counter] - ASCII_ZERO) * 2) - 9);
			}
			else
			{
				evenEquation += ((cardData->primaryAccountNumber[counter] - ASCII_ZERO) * 2);
			}
		}
		else
		{
			oddSum += (cardData->primaryAccountNumber[counter] - ASCII_ZERO);
		}
	}
	int luhnCheck = oddSum + evenEquation;

	/*If PAN is not a Luhn number will return INVALID_CARD, else return OK*/
	if ((luhnCheck % 10 != 0))
	{
		errorState = INVALID_CARD;
	}
	else
	{
		errorState = TERMINAL_OK;
	}
	return errorState;
}


/*
 *This function asks for the transaction amountand saves it into terminal data.
 *If the transaction amount is less than or equal to 0 will return INVALID_AMOUNT, else return OK.
 */
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	/*return check*/
	EN_terminalError_t errorState = TERMINAL_OK;

	/*I will ask a user to enter Transaction amount*/
	puts("please, Enter the Transaction Amount -float number-:  ");
	scanf_s(" %f", &termData->transAmount);

	/*If the transaction amount is less than or equal to 0 will return INVALID_AMOUNT, else return OK.*/
	if (termData->transAmount == NULL)
	{
		errorState = INVALID_AMOUNT;
	}
	else
	{
		errorState = TERMINAL_OK;
	}
	return errorState;
}


/*This function compares the transaction amount with the terminal max amount.
 *If the transaction amount is larger than the terminal max amount will return EXCEED_MAX_AMOUNT, else return OK.
 */
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	/*return check*/
	EN_terminalError_t errorState = TERMINAL_OK;

	/*If the transaction amount is larger than the terminal max amount will return EXCEED_MAX_AMOUNT, else return OK.*/
	if (termData->transAmount > termData->maxTransAmount)
	{
		errorState = EXCEED_MAX_AMOUNT;
	}
	else
	{
		errorState = TERMINAL_OK;
	}
	return errorState;
}


/*This function sets the maximum allowed amount into terminal data.
 *Transaction max amount is a float number.
 *If transaction max amount less than or equal to 0 will return INVALID_MAX_AMOUNT error, else return OK.
 */
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount)
{
	maxAmount = 0;
	/*return check*/
	EN_terminalError_t errorState = TERMINAL_OK;

	/*I will ask a user to set max Amount*/
	puts("please, Enter the MAX Transaction Amount -float number-:  ");
	scanf_s(" %f", &maxAmount);

	/*If transaction max amount less than or equal to 0
	will return INVALID_MAX_AMOUNT error, else return OK.*/
	if (maxAmount <= NULL)
	{
		errorState = INVALID_MAX_AMOUNT;
	}
	else
	{
		termData->maxTransAmount = maxAmount;
		errorState = TERMINAL_OK;
	}
	return errorState;
}