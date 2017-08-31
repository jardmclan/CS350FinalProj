#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "encrypt.h"
#include "fileperm.h"

#include "wlfunctions.h"

void clear();

int main(int argc, char **argv) {

   if(argc != 2) {
      printf("usage: modwl [filename]\n");
      return 1;
   }

   if(access(argv[1], F_OK) == -1) {
      printf("Could not find specified file!");
      return 1;
   }

   whitelist* wl = config_read("whitelist");
   char *filename = argv[1];

   if(!(config_test(wl, filename, getlogin()) > 7)) {
      printf("You do not have sufficient privelages to modify this file's whitelist.\nAccess denied.\n");
      return 3;
   }

   char name[128];
   int perm;
          
   printf("What is the username of the user you would like to set access for?\n");
   fgets(name, 128, stdin);
   if((char)name[strlen(name) - 1] != '\n') {
      clear();
   }
   else {
      //removes newline
      name[strlen(name) - 1] = '\0';
   }

   printf("What permissions would you like to give this user?\nMust be an integer between 0 and 15\n0-7 being normal file permissions\n8-15 being file permissions plus superuser access (ability to modify the whitelist entries\n");
   while (scanf("%d", &perm) < 1 || perm > 15 || perm < 0) {
      clear();
      printf("Invalid permissions\n");
   }
   clear();
   
   config_set(wl, filename, name, perm);
   config_save(wl, "whitelist");
   printf("Permissions set!\n");

   destroy_whitelist(wl);
   return 0;
}

//clears stdin
void clear() {
   char c;
   while((c = getchar()) != '\n' && c != EOF);
}
