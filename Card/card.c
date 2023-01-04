#include <stdio.h>
#include <string.h>
#include "card.h"

/*
 *This function will ask for the cardholder's name and store it into card data.
 *Card holder name is 24 characters string max and 20 min.
 *If the cardholder name is NULL, less than 20 characters or more than 24 will return WRONG_NAME error, else return OK.
 */
EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	/*return check*/
	EN_cardError_t errorState = CARD_OK;

	/*I will ask a user to enter Holder name*/
	puts("please, Enter the Card Holder name: ");
	gets(cardData->cardHolderName);

	/*If the cardholder name is NULL, less than 20 characters or more than 24 will return WRONG_NAME error, else return OK.*/
	if ((strlen(cardData->cardHolderName) == NULL) || (strlen(cardData->cardHolderName) < MIN_NAME_NUM)
		|| (strlen(cardData->cardHolderName) > MAX_NAME_NUM))
	{
		errorState = WRONG_NAME;
	}
	else
	{
		errorState = CARD_OK;
	}
	return errorState;
}

/*
 *This function will ask for the card expiry date and store it in card data.
 *Card expiry date is 5 characters string in the format "MM/YY", e.g "05/25".
 *If the card expiry date is NULL, less or more than 5 characters, or has the wrong format will return WRONG_EXP_DATE error, else return OK.
 */
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	/*return check*/
	EN_cardError_t errorState = CARD_OK;

	/*I will ask a user to enter expirition date*/
	puts("please, Enter the Card Expiration Data -should be like this format MM/YY- :  ");
	gets(cardData->cardExpirationData);

	/*check expDate format is correct
	*If the card expiry date is NULL, less or more than 5 characters, or has the wrong format will return WRONG_EXP_DATE error, else return OK.
	*/
	if ((strlen(cardData->cardExpirationData) == NULL) || (strlen(cardData->cardExpirationData) != NUM_EXP)
		|| ((cardData->cardExpirationData[2]) != '/'))
	{
		errorState = WRONG_EXP_DATE;
	}
	else
	{
		errorState = CARD_OK;
	}
	return errorState;

}

/*
 *This function will ask for the card's Primary Account Number and store it in card data.
 *PAN is 20 characters alphanumeric only string 19 character max, and 16 character min.
 *If the PAN is NULL, less than 16 or more than 19 characters, will return WRONG_PAN error, else return OK.
 */
EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	/*return check*/
	EN_cardError_t errorState = CARD_OK;

	/*I will ask a user to enter PAN*/
	puts("please, Enter Primary Account Number: ");
	gets(cardData->primaryAccountNumber);

	/*If the PAN is NULL, less than 16 or more than 19 characters,
	will return WRONG_PAN error, else return OK.
	*/
	if ((strlen(cardData->primaryAccountNumber) == NULL) || (strlen(cardData->primaryAccountNumber) < MIN_PAN)
		|| (strlen(cardData->primaryAccountNumber) > MAX_PAN))
	{
		errorState = WRONG_PAN;

	}
	else
	{
		errorState = CARD_OK;
	}
	return errorState;

}