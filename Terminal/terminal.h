#pragma once 
#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "../Card/card.h"

/*defines macro for magic numbers*/
/********************************************************************************/
#define TERM_LEN  10
#define MAX_AMOUNT 10000
/********************************************************************************/

/*typedef to use unit8_t as a unsigned char*/
typedef unsigned char unit8_t;

/*terminal data structure*/
typedef struct ST_terminalData_t
{
	float transAmount;
	float maxTransAmount;
	unit8_t transactionData[11];
}ST_terminalData_t;

/*terminal data errors return enums*/
typedef enum EN_terminalError_t
{
	TERMINAL_OK, WORNG_DATE, EXPIRED_CARD, INVALID_CARD, INVALID_AMOUNT, EXCEED_MAX_AMOUNT, INVALID_MAX_AMOUNT
}EN_terminalError_t;

/*
*The function will read the current date from your computer and store it into terminal data with the mentioned size and format.
*Transaction date is 10 characters string in the format DD/MM/YYYY, e.g 25/06/2022.
*If the transaction date is NULL, less than 10 characters or wrong format will return WRONG_DATE error, else return OK.
*/
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData);


/*This function compares the card expiry date with the transaction date.
 *If the card expiration date is before the transaction date will return EXPIRED_CARD, else return OK.
 */
EN_terminalError_t isCardExpired(ST_cardData_t cardData, ST_terminalData_t termData);


/*This function checks if the PAN is a Luhn number or not.
 *If PAN is not a Luhn number will return INVALID_CARD, else return OK.
 */
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData);


/*
 *This function asks for the transaction amountand saves it into terminal data.
 *If the transaction amount is less than or equal to 0 will return INVALID_AMOUNT, else return OK.
 */
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData);


/*This function compares the transaction amount with the terminal max amount.
 *If the transaction amount is larger than the terminal max amount will return EXCEED_MAX_AMOUNT, else return OK.
 */
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData);


/*This function sets the maximum allowed amount into terminal data.
 *Transaction max amount is a float number.
 *If transaction max amount less than or equal to 0 will return INVALID_MAX_AMOUNT error, else return OK.
 */
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount);

#endif // !TERMINAL_H_