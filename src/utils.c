//
// Created by User on 19.05.2025.
//

#include "include/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>

//clear screen
void clearScreen() {
    system("cls");
}

void getHeader(char equals[], const char subject[]) {

    int subject_length = strlen(subject);
    int len = subject_length + 16;

    for (int i = 0; i < len; i++) {
        equals[i] = '=';
    }
    equals[len] = '\0';

    printf("\n\n%s\n", equals);
    printf("\t%s\n", subject);
    printf("%s\n\n", equals);
}

//control menu with arrow keys
char levelSelection(const char *lines[], int option_count, int *pos, const char subject[]) {
    char uInput;
    char equals[255];
    getHeader(equals, subject);

    do {
        clearScreen();
        getHeader(equals, subject);

        for (int i = 0; i < option_count; i++) {
            if (i == *pos) {
                printf(" >  %s", lines[i]);
            } else {
                printf("    %s", lines[i]);
            }
        }
        printf("\n");
        printf("===================================\n");
        printf("Mit W/S bewegen, ENTER bestaetigen");
        uInput = getch();

        if (uInput == 'w') {
            *pos = (*pos == 0) ? option_count - 1 : *pos - 1;
        } else if (uInput == 's') {
            *pos = (*pos == option_count - 1) ? 0 : *pos + 1;
        }
    } while (uInput != '\r');

    return uInput;
}
