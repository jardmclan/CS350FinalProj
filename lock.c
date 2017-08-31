#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "encrypt.h"
#include "fileperm.h"

#include "wlfunctions.h"

void clear();

int main(int argc, char **argv) {
   
   if(argc != 2) {
      printf("usage: lock [filename]\n");
      return 1;
   }

   if(access(argv[1], F_OK) == -1) {
      printf("Could not find specified file!\n");
      return 1;
   }

   //creates whitelist if does not exist
   fclose(fopen("whitelist", "a"));
   
   if(access("master.md5", F_OK) == -1) {
      printf("Administrative password not set up!\n");
      return 2;
   }

   whitelist* wl = config_read("whitelist");

   char c;
   char *filename = argv[1];
   char hashname[strlen(filename) + 4];
   strcpy(hashname, filename);
   strcat(hashname, ".md5");
   char coname[strlen(filename) + 3];
   strcpy(coname, filename);
   strcat(coname, ".co");

   FILE *cof = fopen(coname, "a");
   

   //NOTE: accept passwords with a max length of 128 chars, if they want a password longer than that, theyre insane anyway         
   char pass[128];         
   FILE *temp;

   //in practice could probably just provide this program root access only instead of password protection
   printf("Please enter the administrator password\n");
   fgets(pass, 128, stdin);

   if((char)pass[strlen(pass) - 1] != '\n')
   {
      clear();
   }
   temp = fopen("temp.p", "w");
   fprintf(temp, "%s", pass);
   fclose(temp);
   createMD5("temp.p", "temp.tmd5");
   remove("temp.p");
   

   //admin password will be hashed and stored in master.md5
   if(verify("temp.tmd5", "master.md5") == 1) {
      remove("temp.tmd5");

      
      int tempint = 0;
      if( find_fileref(wl, filename, &tempint) != NULL ) {
         fclose(cof);
         printf("File already locked.\nWould you like to change user permissions? (y/n)\n");
         scanf("%c", &c);
         if (c != '\n') {
            clear();
         }
         if(c == 'y') {
            char name[128];
            int perm;
            
            printf("What is the username of the user you would like to set access for?\n");
            fgets(name, 128, stdin);
            if((char)name[strlen(name) - 1] != '\n')
            {
               clear();
            }
            else {
               //removes newline
               name[strlen(name) - 1] = '\0';
            }

            printf("What permissions would you like to give this user?\nMust be an integer between 0 and 15\n0-7 being normal file permissions\n8-15 being file permissions plus superuser access (ability to modify the whitelist\n");
            while (scanf("%d", &perm) < 1 || perm > 15 || perm < 0) {
               clear();
               printf("Invalid permissions\n");
            }
            clear();
            config_set(wl, filename, name, perm);
            config_save(wl, "whitelist");
            printf("Permissions set!\n");
         }

         destroy_whitelist(wl);
         return 0;
      }
   
      else {
         fputc('i', cof);
         fclose(cof);
         char fpass[128];
         printf("Please enter the password you would like to use for this file (maximum 128 characters)\n");
         fgets(fpass, 128, stdin);
         if((char)fpass[strlen(fpass) - 1] != '\n')
         {
            clear();
         }

         //hashes will be stored in [filename].md5
         encrypt(filename, fpass, hashname);
         //current user is facilitation admin anyway, so gives full access, this shows that the file is under facilitation
         config_set(wl, filename, getlogin(), 15);
         config_save(wl, "whitelist");
         setAccess(filename, 0, 0, 0);

         printf("File %s locked!\n", filename);
      }
   }
   else {
      remove("temp.tmd5");
      printf("Access denied\n");
      destroy_whitelist(wl);
      return 3;
   }

   return 0;
}

//clears stdin
void clear() {
   char c;
   while((c = getchar()) != '\n' && c != EOF);
}

