#include <stdio.h>
#include <stdlib.h>
#include <mysql.h>
#include "include/database.h"

#include "include/utils.h"

#ifndef MYSQL_SSL_MODE_DISABLED
#define MYSQL_SSL_MODE_DISABLED 2
#endif

MYSQL *conn = NULL;

//check database connection
int initDB() {
    conn = mysql_init(NULL);
    if (conn == NULL) {
        fprintf(stderr, "mysql_init() failed\n");
        return 0;
    }

    unsigned int ssl_mode = MYSQL_SSL_MODE_DISABLED;
    mysql_options(conn, MYSQL_OPT_SSL_MODE, &ssl_mode);

    if (mysql_real_connect(conn, "213.165.95.218", "kloss", "sql", "bank", 3306, NULL, 0) == NULL) {
        fprintf(stderr, "Verbindung fehlgeschlagen: %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

//fetch account data
int fetchAccountData(const int *accountNumber, int *balance) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT id, balance, account_number FROM accounts WHERE account_number = %d", *accountNumber);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Abfrage fehlgeschlagen: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *res = mysql_store_result(conn);

    MYSQL_ROW row = mysql_fetch_row(res);
    if (row == NULL) {
        mysql_free_result(res);
        return 0;
    }
    *balance = atoi(row[1]);

    mysql_free_result(res);
    return 1;
}

int deleteData(const int *accNum) {
    char query[256];
    snprintf(query, sizeof(query),
        "DELETE FROM accounts WHERE account_number = %d", *accNum);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

int loginToAccount(const int *accountNumber, const int *pin) {
    char query[256];
    snprintf(query, sizeof(query),
        "SELECT 1 FROM accounts WHERE account_number = %d AND password = %d LIMIT 1", *accountNumber, *pin);
    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);

    MYSQL_ROW row = mysql_fetch_row(result);

    int res = (*row[0] == '1') ? 1 : 0;

    return res; //1 wenn eingeloggt, 0 wenn nicht
}

int getAccountIDByNumber(int *accountNumber) {
    char query[256];
    int id;

    snprintf(query, sizeof(query),
        "SELECT id FROM accounts WHERE account_number = %d", *accountNumber);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row==NULL) {
        mysql_free_result(result);
        return 0;
    }
    id = atoi(row[0]);

    mysql_free_result(result);
    return id;
}

int getAccountNumberByID(int *accountID) {
    char query[256];
    int id;

    snprintf(query, sizeof(query),
        "SELECT account_number FROM accounts WHERE id = %d", *accountID);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row = mysql_fetch_row(result);
    if (row==NULL) {
        mysql_free_result(result);
        return 0;
    }
    id = atoi(row[0]);

    mysql_free_result(result);
    return id;
}

int fetchTransactionData(int *accountNumber) {
    char query[256];

    int id = getAccountIDByNumber(accountNumber);

    snprintf(query, sizeof(query),
        "SELECT * FROM transactions WHERE account_id = %d", id);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage: %s\n", mysql_error(conn));
        return 0;
    }

    MYSQL_RES *result = mysql_store_result(conn);
    MYSQL_ROW row;

    while ((row = mysql_fetch_row(result)) != NULL) {
        int related_id = atoi(row[6]);
        int othID = getAccountNumberByID(&related_id);
        char *labelAccount = strcmp(row[2], "transfer_out") == 0 ? "Ziel" : "Quelle";
        printf("Kontonummer: %d\n", *accountNumber);
        printf("%s: %d\n", labelAccount, othID);
        printf("Art:\t\t%s\n", row[2]);
        printf("Betrag: %s\n", row[3]);
        printf("Zeitstempfel: %s\n", row[4]);
        printf("Verwendungszweck: %s\n", row[5]);
    }
    mysql_free_result(result);
    return 1;
}

int addTransactionToDB(int *senderNumber, int *receiverNumber, int *amount, char *description) {
    char query[256];

    int senderID = getAccountIDByNumber(senderNumber);
    int receiverID = getAccountIDByNumber(receiverNumber);

    snprintf(query, sizeof(query),
        "INSERT INTO transactions (account_id, type, amount, description, related_account_id) VALUES (%d, 'transfer_out', %d, '%s', %d)",
        senderID, *amount, description, receiverID);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage (sender): %s\n", mysql_error(conn));
        return 0;
    }

    snprintf(query, sizeof(query),
        "INSERT INTO transactions (account_id, type, amount, description, related_account_id) VALUES (%d, 'transfer_in', %d, '%s', %d)",
        receiverID, *amount, description, senderID);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage (empfaenger): %s\n", mysql_error(conn));
        return 0;
    }

    return 1;
}

int transAmount(int *senderNumber, int *receiverNumber, int *amount) {
    char query[256];
    snprintf(query, sizeof(query),
        "UPDATE accounts SET balance = balance - %d WHERE account_number = %d", *amount, *senderNumber);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage (sender): %s\n", mysql_error(conn));
        return 0;
    }

    snprintf(query, sizeof(query),
        "UPDATE accounts SET balance = balance + %d WHERE account_number = %d", *amount, *receiverNumber);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage (empfaenger): %s\n", mysql_error(conn));
    }
    return 1;
}

//checks if account number is already in use
int accountExists(int *accountNumber) {
    char query[256];
    snprintf(query, sizeof(query),
             "SELECT 1 FROM accounts WHERE account_number = %d LIMIT 1", *accountNumber);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Fehler bei der Abfrage: %s\n", mysql_error(conn));
    }

    MYSQL_RES *result = mysql_store_result(conn);
    int exists = mysql_num_rows(result) > 0;
    mysql_free_result(result);

    return exists; //0 frei
}

//adds account to database
int addAccountToDB(const int *pin, const int *accountNumber) {

    char query[256];
    snprintf(query, sizeof(query),
             "INSERT INTO accounts (password, balance, account_number) VALUES (%d, %d, %d)", *pin, 10, *accountNumber);

    if (mysql_query(conn, query)) {
        fprintf(stderr, "Abfrage fehlgeschlagen: %s\n", mysql_error(conn));
        return 0;
    }
    return 1;
}

//close database connection
void closeDB() {
    if (conn != NULL) {
        mysql_close(conn);
        conn = NULL;
    }
}
