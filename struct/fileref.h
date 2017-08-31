/**
 * fileref.h
 *
 *     @author: cs
 */

#ifndef FILEREF_H_
#define FILEREF_H_

#include "../wlcommon.h"
#include "userperm.h"

typedef struct fileref {
	char* path;
	int nusers;
	userperm** users;
} fileref;

fileref* create_fileref() {
	//try to allocate
	fileref* rval = malloc(sizeof(*rval));
	if (rval == NULL) {
		perror("create_fileref malloc");
		exit errno;
	}

	//initialize members
	rval->path = NULL;
	rval->nusers = 0;
	rval->users = NULL;

	return rval;
}

void destroy_fileref(fileref* t) {
	free(t->path);
	int i;
	for (i = 0; i < t->nusers; ++i) {
		destroy_userperm(&(*t->users[i]));
	}
	free(t->users);
	free(t);
}

void outfr(fileref* fr, FILE* of) {
	fprintf(of, "%s", fr->path);
	int i;
	for (i = 0; i < fr->nusers; ++i) {
		fprintf(of, "%c", ':');
		outup(fr->users[i], of);
	}
}

void printfr(fileref* fr) {
	printf("%s", fr->path);
	int i;
	for (i = 0; i < fr->nusers; ++i) {
		printf("%c", '\n');
		printf("%c", '\t');
		printup(fr->users[i]);
	}
}

fileref* read_fileref(FILE* iline) {

	//allocate full fileref struct
	fileref* rval = create_fileref();
	if (rval == NULL) {
		perror("read_fileref create_fileref");
		exit errno;
	}

	//initialize delimiter loop
	char* pairbuf = NULL;
	size_t pairbufsize = 0;
	size_t pairgot = 0;
	int currentuser = 0;

	//get first item for the file path
	pairgot = getdelim(&pairbuf, &pairbufsize, ':', iline);
	if (pairgot > 0) {
		//shave off the delimiter
		undelimit(&pairbuf, ':');
		if (DEBUG) puts(pairbuf); //debug

		rval->path = strdup(pairbuf);

		//reset for next loop
		free(pairbuf);
		pairbuf = NULL;
		pairbufsize = 0;
		pairgot = 0;
	}
	//continue with the loop, getting the remaining user,perms pairs
	while (1) {
		pairgot = getdelim(&pairbuf, &pairbufsize, ':', iline);
		if (pairgot > 0) {
			//shave off the delimiter
			undelimit(&pairbuf, ':');
			if (DEBUG) puts(pairbuf); //debug

			//expand the array by 1 since we need a new entry
			userperm** ntusers = realloc(rval->users, (currentuser + 1) * sizeof(ntusers));
			if (ntusers == NULL) {
				perror("read_fileref realloc");
				exit(errno);
			} else {
				rval->users = ntusers;
			}

			//open the chunk as a filestream so can use delimiter on it
			FILE* pairbufstream = fmemopen(pairbuf, strlen(pairbuf), "r");
			if (pairbufstream == NULL) {
				perror("read_fileref fmemopen");
				exit errno;
			}

			//push it into the array
			rval->users[currentuser] = read_userperm(pairbufstream);

			//finish up with the line stream
			fclose(pairbufstream);

			++currentuser;
		}
		if (feof(iline)) break;

		//reset for next loop
		free(pairbuf);
		pairbuf = NULL;
		pairbufsize = 0;
		pairgot = 0;
	}
	//cleanup delimiter loop
	free(pairbuf);

	//set members
	rval->nusers = currentuser;

	return rval;
}

userperm* find_userperm(fileref* fr, char* user, int* returnindex) {
	if (fr == NULL) {
		*returnindex = -1;
		return NULL;
	}

	int i = 0;
	while (i < fr->nusers) {
		//if found, exit without incrementing to leave j at the right user
		if (strcmp(user, fr->users[i]->name) == 0) break;
		++i;
	}
	//counter passed the end, user not found
	if (i == fr->nusers) {
		*returnindex = -1;
		return NULL;
	}

	*returnindex = i;
	return fr->users[i];
}

void add_userperm(fileref* fr, userperm* up) {
	int t = 0;
	userperm* testup = find_userperm(fr, up->name, &t);
	if (testup != NULL) {
		return;
	}

	//expand the array by 1 since we need a new entry
	userperm** ntusers = realloc(fr->users, (fr->nusers + 1) * sizeof(ntusers));
	if (ntusers == NULL) {
		perror("add_userperm realloc");
		exit(errno);
	} else {
		fr->users = ntusers;
	}

	fr->users[fr->nusers] = up;
	++(fr->nusers);
}

void rem_userperm(fileref* fr, int index) {
	if (index > -1 && index < fr->nusers) {

		userperm* target = fr->users[index];
		if (target == NULL) {
			perror("rem_userperm null index");
			return;
		}
		destroy_userperm(target);

		//shift all further ones back if this isn't the last
		int i = index;
		while (i < fr->nusers - 1) {
			fr->users[i] = fr->users[i+1];
			++i;
		}

		//contract the array by 1
		userperm** ntusers = realloc(fr->users, (fr->nusers - 1) * sizeof(ntusers));
		if (ntusers == NULL && fr->nusers > 1) {
			perror("rem_userperm realloc");
			exit(errno);
		} else {
			fr->users = ntusers;
		}
		--(fr->nusers);
	}
}

#endif

