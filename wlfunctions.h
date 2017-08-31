/*
 * whitelist.h
 *
 *  Created on: Apr 10, 2016
 *      Author: cs
 */

#ifndef WLFUNCTIONS_H_
#define WLFUNCTIONS_H_

#include "wlcommon.h"
#include "wlutility.h"
#include "wlstruct.h"

/**
 * misc Functions
 */

whitelist* config_read(char* filename) {
	//open file and test if it worked
	FILE* ifile = fopen(filename, "r");
	if (ifile == NULL) {
		perror("config_read fopen");
		exit errno;
	}

	whitelist* rval = read_whitelist(ifile, numlines(filename));

	fclose(ifile);

	return rval;
}

/**
 * filepath must be in /path/to/file.ext format with the leading /
 */
char config_test(whitelist* wl, char* path, char* user) {
	if (wl == NULL) return 0;

	int t = 0;
	fileref* fr = find_fileref(wl, path, &t);
	if (fr == NULL) return 0;

	userperm* up = find_userperm(fr, user, &t);
	if (up == NULL) return 0;

	return up->perms;
}

int config_save(whitelist* wl, char* filename) {
	if (wl == NULL) {
		perror("config_save wl is null");
		return EXIT_FAILURE;
	}

	FILE* ofile = fopen(filename, "w");
	if (ofile == NULL) {
		perror("config_save fopen");
		return errno;
	}

	outwl(wl, ofile);

	fclose(ofile);

	return EXIT_SUCCESS;
}

void config_print(whitelist* wl) {
	puts("Printing Whitelist");
	if (wl == NULL) {
		puts("whitelist is null");
		return;
	}
	printwl(wl);
}

void config_rem_file(whitelist* wl, char* path) {
	if (wl == NULL) {
		perror("config_rem_file whitelist is null");
		return;
	}

	int ind = -2;
	fileref* foundfr = find_fileref(wl, path, &ind);
	if (foundfr == NULL) {
		perror("config_rem_file fileref is null");
		return;
	}

	if (ind == -2) {
		perror("config_rem_file index not set");
		return;
	}

	if (ind == -1) {
		perror("config_rem_file index not found");
		return;
	}

	rem_fileref(wl, ind);

}

void config_rem_user(whitelist* wl, char* path, char* username) {
	if (wl == NULL) {
		perror("config_rem_user whitelist is null");
		return;
	}

	int tind = -2;
	fileref* foundfr = find_fileref(wl, path, &tind);
	if (foundfr == NULL) {
		perror("config_rem_user fileref is null");
		return;
	}

	int ind = -2;
	userperm* foundup = find_userperm(foundfr, username, &ind);
	if (foundup == NULL) {
		perror("config_rem_user userperm is null");
		return;
	}

	if (ind == -2) {
		perror("config_rem_user index not set");
		return;
	}

	if (ind == -1) {
		perror("config_rem_user index not found");
		return;
	}

	rem_userperm(foundfr, ind);

	//check if the fileref is empty and remove it if it is, we don't need an empty fileref
	if (foundfr->nusers < 1) {
		config_rem_file(wl, path);
	}

}

void config_set(whitelist* wl, char* filepath, char* username, int perms) {
	if (wl == NULL) {
		perror("config_add whitelist is null");
		return;
	}

	//if we're setting the perms to 0, we don't need that user
	if (perms == 0) {
		config_rem_user(wl, filepath, username);
		return;
	}

	int t = 0;
	fileref* foundfr = find_fileref(wl, filepath, &t);
	if (foundfr != NULL) {
		//fileref exists, only might need to add the user and perms to it

		userperm* foundup = find_userperm(foundfr, username, &t);
		if (foundup != NULL) {
			//userperm already exists, check now to see if we need to destroy it
			foundup->perms = perms;
			return;
		}

		//otherwise, need to add the user
		userperm* addup = create_userperm();
		addup->name = strdup(username);
		addup->perms = perms;
		add_userperm(foundfr, addup);
		return;

	}

	//need to add the entire fileref, it doesn't exist
	fileref* addfr = create_fileref();
	addfr->path = strdup(filepath);
	userperm* addup = create_userperm();
	addup->name = strdup(username);
	addup->perms = perms;

	add_userperm(addfr, addup);
	add_fileref(wl, addfr);

	return;
}

#endif /* WHITELIST_H_ */
