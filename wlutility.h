/**
 * wlutility.h
 *
 *     @author: cs
 */

#ifndef WLUTILITY_H_
#define WLUTILITY_H_

#include "wlcommon.h"

int numlines(char* filename) {
    //open the file and test if it worked
    FILE* ifile = fopen(filename, "r");
    if (ifile == NULL) {
        perror("numlines fopen");
        exit errno;
    }

    int ch;
    int lines = 0;
    //try to grab the first character
    ch = fgetc(ifile);
    //if it's not eof after the first attempted read, it's got at least one line
    //a blank file has no lines, technically
    if (!(feof(ifile))) lines++;
    //now run until the end of the file, counting newlines
    while(!(feof(ifile))) {
        if (ch == '\n') lines++;
        ch = fgetc(ifile);
    }

    fclose(ifile);

    return lines;
}

/**
 * remove "delimiter" from the end of "str" if it's there
 */
void undelimit(char** str, char delimiter) {
    if ((*str)[strlen((*str)) - 1] == delimiter) {
        (*str)[strlen((*str)) - 1] = '\0';
    }
}

#endif

