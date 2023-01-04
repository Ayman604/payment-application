#include <stdio.h>
#include <string.h>
#include "server.h"

/* global array of ST_accountsDB_t for the valid accounts database.*/
static ST_accountsDB_t accountDB[255] =
{
	{1000.0  ,BLOCKED,   "5078034948991109"},
	{1500.5  ,BLOCKED, "392533806672556039"},
	{9000.0  ,RUNNING, "302748634527182821"},
	{12000.0 ,RUNNING, "307790134933005506"},
	{15000.5 ,RUNNING, "350276993032537351"},
	{1000.0  ,RUNNING,   "2479189249803973"},
	{500.0   ,RUNNING,"3800558864160504382"},
	{100000.0,BLOCKED,   "5807007076043875"},
	{2000.0  ,RUNNING,   "8989374615436851"},
	{55500.0 ,BLOCKED,"4057247864289864108"}
};

/* global array of ST_transaction_t.*/
ST_transaction_t transactionsDB[255] = { 0 };


/*global variable to save account number in it*/
unit8_t refAcNumber;
float balance;
/*to store sequanceNumber*/
unit32_t sequenceNumber = 0;


/*This function will take card data and validate if the account related to this card exists or not.
 *It checks if the PAN exists or not in the server's database (searches for the card PAN in the DB).
 *If the PAN doesn't exist will return ACCOUNT_NOT_FOUND and the account reference will be NULL,
 *else will return SERVER_OK and return a reference to this account in the DB.
 */
EN_serverError_t isValidAccount(ST_cardData_t* cardData)
{
	/*return check*/
	EN_serverError_t errorState = SERVER_OK;

	unit32_t countup;
	/*count and check DB array elements*/
	for (countup = 0; countup < 255; countup++)
	{
		if (strcmp((accountDB[countup].primaryAccountNumber), (cardData->primaryAccountNumber)) == 0)
		{
			refAcNumber = countup;
			errorState = SERVER_OK;
			break;

		}
		else
		{
			errorState = ACCOUNT_NOT_FOUND;
		}
	}
	return errorState;
}

/*This function takes a reference to the account into the database and verifies if it is blocked or not.
 *If the account is running it will return SERVER_OK, else if the account is blocked it will return BLOCKED_ACCOUNT.
 */
EN_serverError_t isBlockedAccount(ST_accountsDB_t* ST_accountsDB_t)
{
	/*return check*/
	EN_serverError_t errorState = SERVER_OK;

	if ((accountDB[refAcNumber].state) == BLOCKED)
	{
		errorState = BLOCKED_ACCOUNT;

	}
	else
	{
		errorState = SERVER_OK;
	}
	return errorState;
}

/*This function will take terminal data and a reference to the account in the database and
 *check if the account has a sufficient amount to withdraw or not.
 *It checks if the transaction's amount is available or not.
 *If the transaction amount is greater than the balance in the database will return LOW_BALANCE, else will return SERVER_OK.
 */
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData)
{
	/*return check*/
	EN_serverError_t errorState = SERVER_OK;


	if ((termData->transAmount) <= (accountDB[refAcNumber].balance))
	{
		errorState = SERVER_OK;

	}
	else
	{
		errorState = LOW_BALANCE;
	}
	return errorState;
}

/*This function will store all transaction data in the transactions database.
It gives a sequence number to a transaction,
this number is incremented once a transaction is processed into the server,
you must check the last sequence number in the server to give the new transaction a new sequence number.
It saves any type of transaction, APPROVED, DECLINED_INSUFFECIENT_FUND, DECLINED_STOLEN_CARD, FRAUD_CARD, INTERNAL_SERVER_ERROR.
It will list all saved transactions using the listSavedTransactions function.
Assuming that the connection between the terminal and server is always connected, then it will return SERVER_OK.*/
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	/*return check*/
	EN_serverError_t errorState = SERVER_OK;
	if (sequenceNumber < 255)
	{
		transData->transactionSequenceNumber = sequenceNumber;
		unit32_t count = transData->transactionSequenceNumber;
		transactionsDB[count].cardHolderData = transData->cardHolderData;
		transactionsDB[count].terminalData = transData->terminalData;
		transactionsDB[count].transactionSequenceNumber = transData->transactionSequenceNumber;
		transactionsDB[count].transState = transData->transState;
		sequenceNumber++; // ubdate sequance number
		errorState = SERVER_OK;
	}
	else
	{
		errorState = SAVING_FAILED;
	}

	return errorState;

}

/*This function will take all transaction data and validate its data, it contains all server logic.
 *It checks the account details and amount availability.
 *If the account does not exist return FRAUD_CARD,
 *if the amount is not available will return DECLINED_INSUFFECIENT_FUND,
 *if the account is blocked will return DECLINED_STOLEN_CARD,
 *if a transaction can't be saved will return INTERNAL_SERVER_ERROR , else returns APPROVED.
 *It will update the database with the new balance.
 */
EN_transState_t recieveTransactionData(ST_transaction_t* transData)
{
	/*return check*/
	EN_serverError_t errorState = APPROVED;
	if (isValidAccount(&transData->cardHolderData) == ACCOUNT_NOT_FOUND)
	{
		transData->transState = FRAUD_CARD;
		saveTransaction(transData);
		errorState = FRAUD_CARD;
	}
	else if (isAmountAvailable(&transData->terminalData) == LOW_BALANCE)
	{
		transData->transState = DECLINED_INSUFFECIENT_FUND;
		saveTransaction(transData);
		errorState = DECLINED_INSUFFECIENT_FUND;
	}
	else if (isBlockedAccount(&accountDB[refAcNumber]) == BLOCKED_ACCOUNT)
	{
		transData->transState = DECLINED_STOLEN_CARD;
		saveTransaction(transData);
		errorState = DECLINED_STOLEN_CARD;
	}
	else {
		transData->transState = APPROVED;
		//printf("- - - old balance: %.3f - - -\n", accountDB[refAcNumber].balance);
		accountDB[refAcNumber].balance -= transData->terminalData.transAmount;
		balance = accountDB[refAcNumber].balance;
		//printf("- - - updating balance: %.3f - - -\n", accountDB[refAcNumber].balance);

		errorState = APPROVED;
	}


	if (saveTransaction(transData) == SAVING_FAILED)
	{
		errorState = INTERNAL_SERVER_ERROR;
	}

	return errorState;

}

/*This function takes the sequence number of a transaction and returns the transaction data if found in the transactions DB
 *If the sequence number is not found, then the transaction is not found, the function will return TRANSACTION_NOT_FOUND,
 else return transaction data as well as SERVER_OK
 */
EN_serverError_t getTransaction(unit32_t transactionSequenceNumber, ST_transaction_t* transData)
{
	for (unit8_t count = 0; count < sequenceNumber; count++)
	{
		if (transactionsDB[count].transactionSequenceNumber == transactionSequenceNumber)
		{
			*transData = transactionsDB[count];
			return TERMINAL_OK;
		}
	}
	printf("TRANSACTION NOT FOUND \n");
	return TRANSACTION_NOT_FOUND;
}