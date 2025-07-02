//
// Created by User on 15.05.2025.
//

#include <stdio.h>
#include "include/database.h"
#include "include/terminal.h"
#include "windows.h"

int main(void) {
    if (!initDB()) {
        printf("Datenbank nicht erreichbar");
        return 0;
    }
    mainTerminal();
    closeDB();
    Sleep(2000);
    return 0;
}
