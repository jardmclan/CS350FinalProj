/**
 * userperm.h
 *
 *     @author: cs
 */

#ifndef USERPERM_H_
#define USERPERM_H_

#include "../wlcommon.h"

typedef struct userperm {
    char* name;
    char perms;
} userperm;

userperm* create_userperm() {
    //try to allocate
    userperm* rval = malloc(sizeof(*rval));
    if (rval == NULL) {
        perror("create_userperm malloc");
        exit errno;
    }

    //initialize members
    rval->name = NULL;
    rval->perms = '\0';

    return rval;
}

void destroy_userperm(userperm* t) {
    free(t->name);
    free(t);
}

void outup(userperm* up, FILE* of) {
	fprintf(of, "%s", up->name);
	fprintf(of, "%c", ',');
	fprintf(of, "%d", up->perms);
}

void printup(userperm* up) {
    printf("%s: %d", up->name, up->perms);
}


userperm* read_userperm(FILE* ipair) {

    //allocate full userperm struct
    userperm* rval = create_userperm();
    if (rval == NULL) {
        perror("read_userperm create_userperm");
        exit errno;
    }

    //initialize delimiter loop for key(name)
    char* keyvalbuf = NULL;
    size_t keyvalbufsize = 0;
    size_t keyvalgot = 0;

    //get key
    keyvalgot = getdelim(&keyvalbuf, &keyvalbufsize, ',', ipair);
    if (keyvalgot > 0) {
        //shave off the delimiter
        undelimit(&keyvalbuf, ',');
        if (DEBUG) puts(keyvalbuf); //debug

        rval->name=strdup(keyvalbuf);
    }

    //reset for value(perms)
    free(keyvalbuf);
    keyvalbuf = NULL;
    keyvalbufsize = 0;
    keyvalgot = 0;

    //get value
    keyvalgot = getdelim(&keyvalbuf, &keyvalbufsize, ',', ipair);
    if (keyvalgot > 0) {
        //shave off the delimiter
        undelimit(&keyvalbuf, ',');
        if (DEBUG) puts(keyvalbuf); //debug

        char* end;
        rval->perms=strtol(keyvalbuf, &end, 10);
    }

    //reset full
    free(keyvalbuf);
    keyvalbuf = NULL;
    keyvalbufsize = 0;
    keyvalgot = 0;

    return rval;
}

#endif

