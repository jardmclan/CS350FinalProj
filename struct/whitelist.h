/**
 * whitelist.h
 *
 *     @author: cs
 */

#ifndef WHITELIST_H_
#define WHITELIST_H_

#include "../wlcommon.h"
#include "fileref.h"

typedef struct whitelist {
	int nfiles;
	fileref** files;
} whitelist;

whitelist* create_whitelist() {
	//try to allocate
	whitelist* rval = malloc(sizeof(*rval));
	if (rval == NULL) {
		perror("create_whitelist malloc");
		exit errno;
	}

	//initialize members
	rval->nfiles = 0;
	rval->files = NULL;

	return rval;
}

void destroy_whitelist(whitelist* t) {
	int i;
	for (i = 0; i < t->nfiles; ++i) {
		destroy_fileref(&(*t->files[i]));
	}
	free(t->files);
	free(t);
}

void outwl(whitelist* wl, FILE* of) {
	int i;
	for (i = 0; i < wl->nfiles; ++i) {
		if (i > 0) fprintf(of, "%c", '\n');
		outfr(wl->files[i], of);
	}
}

void printwl(whitelist* wl) {
	int i;
	for (i = 0; i < wl->nfiles; ++i) {
		printfr(wl->files[i]);
		printf("%c", '\n');
	}
}

whitelist* read_whitelist(FILE* ifile, int lines) {

	//allocate full whitelist struct
	struct whitelist* rval = create_whitelist();
	if (rval == NULL) {
		perror("config_read create_whitelist");
		exit errno;
	}

	//set members
	rval->nfiles = lines;
	rval->files = malloc(rval->nfiles * sizeof(rval->files));

	//initialize line loop
	char* linebuf = NULL;
	size_t linebufsize = 0;
	size_t linegot = 0;
	int currentfile = 0;
	while (currentfile < rval->nfiles) {
		linegot = getdelim(&linebuf, &linebufsize, '\n', ifile);
		if (linegot > 0) {
			//shave off the delimiter
			undelimit(&linebuf, '\n');
			//debug message
			if (DEBUG) puts(linebuf);

			//open the line as a filestream so can use delimiter on it
			FILE* linebufstream = fmemopen(linebuf, strlen(linebuf), "r");
			if (linebufstream == NULL) {
				perror("config_read fmemopen");
				exit errno;
			}

			//allocate this round's fileref and read into it
			struct fileref* tfile = read_fileref(linebufstream);

			//finish up with the line stream
			fclose(linebufstream);

			rval->files[currentfile] = tfile;

			//reset for next loop
			free(linebuf);
			linebuf = NULL;
			linebufsize = 0;
		}
		++currentfile;
	}
	//cleanup line loop
	free(linebuf);

	//already set members above

	return rval;
}

fileref* find_fileref(whitelist* wl, char* path, int* returnindex) {
	if (wl == NULL) {
		*returnindex = -1;
		return NULL;
	}

	int i = 0;
	while (i < wl->nfiles) {
		//if found, exit without incrementing to leave i at the right path
		if (strcmp(path, wl->files[i]->path) == 0) break;
		++i;
	}
	//counter passed the end, path not found
	if (i == wl->nfiles) {
		*returnindex = -1;
		return NULL;
	}

	*returnindex = i;
	return wl->files[i];
}

void add_fileref(whitelist* wl, fileref* fr) {
	int t = 0;
	fileref* testfr = find_fileref(wl, fr->path, &t);
	if (testfr != NULL) {
		return;
	}

	//expand the array by 1 since we need a new entry
	fileref** ntfiles = realloc(wl->files, (wl->nfiles + 1) * sizeof(ntfiles));
	if (ntfiles == NULL) {
		perror("add_fileref realloc");
		exit(errno);
	} else {
		wl->files = ntfiles;
	}

	wl->files[wl->nfiles] = fr;
	++(wl->nfiles);
}

void rem_fileref(whitelist* wl, int index) {
	if (index > -1 && index < wl->nfiles) {

		fileref* target = wl->files[index];
		if (target == NULL) {
			perror("rem_fileref null index");
			return;
		}
		destroy_fileref(target);

		//shift all further ones back if this isn't the last
		int i = index;
		while (i < wl->nfiles - 1) {
			wl->files[i] = wl->files[i+1];
			++i;
		}

		//contract the array by 1
		fileref** ntfiles = realloc(wl->files, (wl->nfiles - 1) * sizeof(ntfiles));
		if (ntfiles == NULL && wl->nfiles > 1) {
			perror("rem_fileref realloc");
			exit(errno);
		} else {
			wl->files = ntfiles;
		}
		--(wl->nfiles);
	}
}

#endif

