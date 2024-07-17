#include "ParaReader.h"

/*
 *  Get the index of next unblank char from a string.
 */
int getNextChar(char *str) {
    int rtn = 0;
    // Jump over all blanks
    while (str[rtn] == ' ')
        rtn++;
    return rtn;
}

/*
 *  Get next word from a string.
 */
void getNextWord(char *str, char *word) {
    // Jump over all blanks
    while (*str == ' ')
        str++;
    while (*str != ' ' && *str != '\0') {
        *word = *str;
        str++;
        word++;
    }
    *word = '\0';
}

