//
// Created by User on 15.05.2025.
//

#ifndef DATABASE_H
#define DATABASE_H

int initDB();
int fetchAccountData(const int *accountNumber, int *balance);
int deleteData(const int *accountNumber);
int loginToAccount(const int *accountNumber, const int *pin);
int addAccountToDB(const int *pin, const int *accountNumber);
int accountExists(int *accountNumber);
int transAmount(int *senderNumber, int *receiverNumber, int *amount);
int addTransactionToDB(int *senderNumber, int *receiverNumber, int *amount, char *description);
int fetchTransactionData(int *accountNumber);
int getAccountNumberByID(int *accountID);
int getAccountIDByNumber(int *accountNumber);
void closeDB();

#endif //DATABASE_H
