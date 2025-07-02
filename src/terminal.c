//
// Created by User on 19.05.2025.
//

#include <stdio.h>
#include "include/terminal.h"
#include "conio.h"
#include <mysql/client_plugin.h>
#include <stdbool.h>
#include "include/database.h"
#include "include/utils.h"

//printing menu to create bank account
void createAccountTerminal() {
    clearScreen();
    int pin = 0;
    int tempPin = 1;
    int accNum = 0;
    char input[256];
    const char equals[256];
    const char subject[] = "FirstBank - Terminal";

    clearScreen();
    getHeader(equals, subject);

    do {
        clearScreen();
        getHeader(equals, subject);

        printf("Neue Kontonummer: ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        int valid = 1;
        for (int i = 0; input[i] != '\0'; i++) {
            if (!isdigit(input[i])) {
                valid = 0;
                break;
            }
        }

        if (!valid || strlen(input) != 5 || input[0] == '0') {
            clearScreen();
            getHeader(equals, subject);
            printf("Kontonummer muss genau 5 Ziffern enthalten. Keine fuehrende Null.\n");
            Sleep(3000);
            continue;
        }

        accNum = atoi(input);

        if (accountExists(&accNum)) {
            printf("Kontonummer existiert bereits.");
            Sleep(3000);
            pin = 0;
            tempPin = 1;
            continue;
        }

        char pinInput[16];
        clearScreen();
        getHeader(equals, subject);

        printf("Neue PIN: ");
        fgets(pinInput, sizeof(pinInput), stdin);
        tempPin = atoi(pinInput);

        printf("PIN bestaetigen: ");
        fgets(pinInput, sizeof(pinInput), stdin);
        pin = atoi(pinInput);

    } while (pin != tempPin);

    addAccountToDB(&pin, &accNum);

    clearScreen();

    getHeader(equals, subject);

    printf("Konto eroeffnet. Als Startguthaben erhalten Sie 10 EUR.");
    Sleep(3000);
    mainTerminal();
}

void fetchTransactionTerminal(const int *accNum) {
    char equals[256];
    const char subject[] = "FirstBank - Terminal";

    char uInput;

    int othAccNum = 0;
    char type[256];
    int amount = 0;
    char date[256];
    char description[256];

    clearScreen();
    getHeader(equals, subject);

    fetchTransactionData(accNum);

    printf("\n=====================\n");
    printf("ENTER bestaetigen");
    do {
        uInput = getch();
    } while (uInput != '\r');
    loggedInTerminal(accNum);
}

void createTransactionTerminal(const int *accNum) {
    char equals[256];
    const char subject[] = "FirstBank - Terminal";
    bool transactionConfirmed = false;
    int othAccNum = 0;
    int balance = 0;
    int transactionBalance = 0;
    char input[256];
    char description[256];

    do {
        clearScreen();
        getHeader(equals, subject);

        printf("Ziel-Kontonummer: ");
        fgets(input, sizeof(input), stdin);
        othAccNum = atoi(input);

        if (*accNum==othAccNum) {
            clearScreen();
            getHeader(equals, subject);
            printf("Keine Ueberweisung auf eigenes Konto moeglich.");
            Sleep(3000);
            continue;
        }

        if (!accountExists(&othAccNum)) {
            clearScreen();
            getHeader(equals, subject);
            printf("Es existiert kein Account mit dieser Kontonummer.\nFormat Kontonummern: 10001, 21007, 993100");
            Sleep(3000);
            continue;
        }

        printf("Betrag: ");
        fgets(input, sizeof(input), stdin);
        transactionBalance = atoi(input);

        fetchAccountData(accNum, &balance);
        if (balance < transactionBalance) {
            clearScreen();
            getHeader(equals, subject);
            printf("Maximaler Betrag: %d", balance);
            Sleep(3000);
            continue;
        }

        printf("Verwendungszweck: ");
        fgets(description, sizeof(description), stdin);

        transactionConfirmed = true;
        transAmount(accNum, &othAccNum, &transactionBalance);
        addTransactionToDB(accNum, &othAccNum, &transactionBalance, description);

        clearScreen();
        getHeader(equals, subject);

        printf("Transaktion abgeschlossen.");
        Sleep(3000);
    } while (!transactionConfirmed);
    loggedInTerminal(accNum);
}

//logged in section
void loggedInTerminal(const int *accNum) {
    const char equals[256];
    const char subject[] = "FirstBank - Terminal";
    const char *lines[] = {
        "Transaktion\n",
        "Transaktionshistorie\n",
        "Loeschen\n",
        "Abmelden\n"
    };
    int option_count = sizeof(lines) / sizeof(lines[0]);
    int pos = 0;
    int balance;
    char uInput;

    clearScreen();
    getHeader(equals, subject);

    fetchAccountData(accNum, &balance);

    do {
        printf("Kontonummer: %d\nKontostand: %d", *accNum, balance);
        printf("\n=====================\n");
        printf("ENTER bestaetigen");
        uInput = getch();


    } while (uInput != '\r');

    levelSelection(lines, option_count, &pos, subject);

    if (lines[pos] == "Transaktion\n") {
        createTransactionTerminal(accNum);
    } else if (lines[pos] == "Transaktionshistorie\n") {
        fetchTransactionTerminal(accNum);
    } else if (lines[pos] == "Loeschen\n") {
        deleteData(accNum);
        clearScreen();
        getHeader(equals, subject);
        printf("Account erfolgreich geloescht.");
        Sleep(3000);
        mainTerminal();
    } else if (lines[pos] == "Abmelden\n") {
        clearScreen();
        getHeader(equals, subject);
        printf("Abmeldung erfolgreich.");
        mainTerminal();
    } else {
        ;
    }
}

//login section
void loginTerminal() {
    char equals[256];
    const char subject[] = "FirstBank - Terminal";
    char input[256];
    int accNum = 0;
    int pin = 0;
    bool loggedIn = false;

    do {
        clearScreen();
        getHeader(equals, subject);

        printf("Kontonummer: ");
        fgets(input, sizeof(input), stdin);
        accNum = atoi(input);

        //check accNum exist
        if (!accountExists(&accNum)) {
            clearScreen();
            getHeader(equals, subject);
            printf("Es existiert kein Account mit dieser Kontonummer.\nFormat Kontonummern: 10001, 21007, 993100");
            Sleep(3000);
            continue;
        }

        clearScreen();
        getHeader(equals, subject);

        printf("PIN: ");
        fgets(input, sizeof(input), stdin);
        pin = atoi(input);

        if (!loginToAccount(&accNum, &pin)) {
            clearScreen();
            getHeader(equals, subject);
            printf("Kombination aus Kontonummer und PIN ist inkorrekt.");
            Sleep(3000);
            continue;
        }

        clearScreen();
        getHeader(equals, subject);

        printf("Login bestaetigt.");
        loggedIn = true;

    }while (!loggedIn);

    loggedInTerminal(&accNum);
}

//printing main menu
void mainTerminal() {
    const char *subject = "FirstBank - Terminal";
    const char *lines[] = {
        "Einloggen\n",
        "Eroeffnen\n",
        "Beenden\n"
    };
    int option_count = sizeof(lines) / sizeof(lines[0]);
    int pos = 0;

    //control main menu with arrow keys
    levelSelection(lines, option_count, &pos, subject);

    if (lines[pos] == "Einloggen\n") {
        loginTerminal();
    } else if (lines[pos] == "Eroeffnen\n") {
        createAccountTerminal();
    } else if (lines[pos] == "Beenden") {
        ;
    } else {
        ;
    }
}


