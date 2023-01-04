#include <stdio.h>
#include <string.h>

#include "app.h"


/*transaction data object*/
ST_transaction_t transData;
/*account data base object*/
ST_accountsDB_t accountRefrence;
/*card data object*/
ST_cardData_t cardHolderData;
/*terminal data object*/
ST_terminalData_t terminalData;

/*variable to put Max amount on memory*/
float maxAmount;

/*refere to the flow chart*/
void appStart(void)
{
	/************************************** CARD_MODULE **************************************/
	/* Call Card Functions To Get:
	 * Card Holder Name      ---> getCardHolderName
	 * Card Expiration Date  ---> getCardExpiryDate
	 * Card PAN              ---> getCardPAN
	 */
	unit8_t cardHolderNameChecker = getCardHolderName(&cardHolderData);
	unit8_t cardExpiryDateChecker = getCardExpiryDate(&cardHolderData);
	unit8_t cardPraymAcNumChecker = getCardPAN(&cardHolderData);

	/*check -Card Holder Name-, if return WRONG_NAME print wrong and try again
	 * if return CARD_OK complete the application.
	 */
	if (WRONG_NAME == cardHolderNameChecker) {
		puts("- - - Wrong Name, Try again - - -");
		cardHolderNameChecker = getCardHolderName(&cardHolderData);
	}
	else {
		//puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	}

	/*check -Card Expiration Date-, if return WRONG_EXP_DATE print wrong and try again
	 * if return CARD_OK complete the application.
	 */
	if (WRONG_EXP_DATE == cardExpiryDateChecker) {
		puts("- - - Wrong Format, Try again  with this format MM/YY - - -");
		cardExpiryDateChecker = getCardExpiryDate(&cardHolderData);
	}
	else {
		//puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	}

	/*check -Card PAN-, if return WRONG_PAN print wrong and try again
	 * if return CARD_OK complete to check if number is luhn number or not
	 * if number not luhn number print Invalid PAN and ask user to try again.
	 */
	if (WRONG_PAN == cardPraymAcNumChecker) {
		puts("- - - Wrong PAN, Try again - - -");
		cardPraymAcNumChecker = getCardPAN(&cardHolderData);
	}
	else {
		unit8_t isValidPANChecker = isValidCardPAN(&cardHolderData);
		if (INVALID_CARD == isValidPANChecker) {
			puts("- - - Invalid PAN, Try again - - -");
			cardPraymAcNumChecker = getCardPAN(&cardHolderData);
		}
		else {
			puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		}
	}
	/*****************************************************************************************/

	/************************************ TERMINAL_MODULE ************************************/
	/* Call Terminal Functions To Get:
	 * Transaction Date                            ---> getTransactionDate
	 * Is Card Expired?                            ---> isCardExpired
	 */
	getTransactionDate(&terminalData);
	unit8_t isCardExpiredChecker = isCardExpired(cardHolderData, terminalData);

	/*check is Card Expaired*/
	if (EXPIRED_CARD == isCardExpiredChecker) {
		puts("- - - Declined Expired Card - - -");
		exit(0);
	}
	else {
		//puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	}

	/* Call Terminal Functions To Get:--------------------------------------
	 * Transaction Amount                          ---> getTransactionAmount
	 * Set Transaction max Amoun                   ---> setMaxAmount
	 * compare max amount and transaction amount   ---> isBelowMaxAmount
	 */
	unit8_t maxAmountChecker = setMaxAmount(&terminalData, maxAmount);
	unit8_t getTransAmountChecker = getTransactionAmount(&terminalData);
	unit8_t beloMaxAmountChecker = isBelowMaxAmount(&terminalData);

	/*check Max Amount if it Invalid ask user to set it again*/
	if (INVALID_MAX_AMOUNT == maxAmountChecker) {
		puts("- - - Invalid Max Amount, Try again with amount > zero - - -");
		maxAmountChecker = setMaxAmount(&terminalData, maxAmount);
	}
	else {
		/*check transaction Amount if it Invalid print this*/
		if (INVALID_AMOUNT == getTransAmountChecker) {
			puts("- - - Invalid Amount - - -");
			exit(0);
		}
		else {
			/*check if transaction amount <= max amount*/
			if (EXCEED_MAX_AMOUNT == beloMaxAmountChecker) {
				puts("- - - Declined Amount Exceeding Limit - - -");
				exit(0);
			}
			else {
				//puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
			}
		}
	}
	/*****************************************************************************************/

	/************************************* SERVER_MODULE *************************************/
	/* Call SERVER Functions To Check:
	 * Is Valid Account?                           ---> getTransactionDate
	 * Is Bloked Account?                          ---> isCardExpired
	 * Is amount available                         ---> isAmountAvailable
	 */
	unit8_t isValidAccChecker = isValidAccount(&cardHolderData);
	unit8_t isBlockAccChecker = isBlockedAccount(&accountRefrence);
	unit8_t isAmountAvChecker = isAmountAvailable(&terminalData);

	/*check if function return ACCOUNT_NOT_FOUND, print  Declined Invalid Account and out program.
	 *if account founded check if Running Or BLOCKED.
	*/
	if (ACCOUNT_NOT_FOUND == isValidAccChecker) {
		puts("- - - Declined Invalid Account - - -");
		//exit(0);
	}
	else {
		/*if account founded check if Running Or BLOCKED.*/
		if (BLOCKED_ACCOUNT == isBlockAccChecker) {
			puts("- - - Declined Blocked Account - - -");
			//	exit(0);
		}
		else {
			//puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
		}
	}

	/*check if the amount available by compare it with the account balance*/
	if (LOW_BALANCE == isAmountAvChecker) {
		puts("- - - Declined Insuffecient Funds - - -");
		//exit(0);
	}
	else {
		//puts("- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -");
	}

	/* Call SERVER Functions To :
	 * save Transaction Data                       ---> saveTransaction
	 * Recieve Transaction Data                    ---> recieveTransactionData
	 */
	saveTransaction(&transData);
	transData.cardHolderData = cardHolderData; // UPDATE CARD_HOLDER_DATA
	transData.terminalData = terminalData; // UPDATE TERMINAL_DATA
	unit8_t recieveTransDataChecker = recieveTransactionData(&transData);

	/*check server returns and return the acount state*/
	if (FRAUD_CARD == recieveTransDataChecker) {
		puts("- - - FRAUD_CARD - - -");
	}
	else if (DECLINED_INSUFFECIENT_FUND == recieveTransDataChecker) {
		puts("- - - DECLINED_INSUFFECIENT_FUND - - -");
	}
	else if (DECLINED_STOLEN_CARD == recieveTransDataChecker) {
		puts("- - - DECLINED_STOLEN_CARD - - -");
	}
	else if (INTERNAL_SERVER_ERROR == recieveTransDataChecker) {
		puts("- - - INTERNAL_SERVER_ERROR - - -");
	}
	else if (APPROVED == recieveTransDataChecker) {
		puts("- - - Transaction Complete - - -");
		printf("- - - Updating Account ....\n- - - Transaction is Saved - - - \n");
		puts("############################################");
		printf("- - - Card Holder Name: %s - - -\n", cardHolderData.cardHolderName);
		printf("- - - Card PAN: %s - - -\n", cardHolderData.primaryAccountNumber);
		printf("- - - Amount You talk: %.2f - - -\n", terminalData.transAmount);
		printf("- - - Your Account Balance: %.2f - - -\n", balance);
		puts("############################################");
		exit(0);
	}
}