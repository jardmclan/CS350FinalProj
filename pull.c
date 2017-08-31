#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "encrypt.h"
#include "fileperm.h"

#include "wlfunctions.h"

void clear();

int main(int argc, char **argv) {
   
   if(argc != 2) {
      printf("usage: unlock [filename]\n");
      return 1;
   }

   if(access(argv[1], F_OK) == -1) {
      printf("File not found");
      return 1;
   }
   whitelist* wl = config_read("whitelist");

   char* user = getlogin();

   char *filename = argv[1];
   char hashname[strlen(filename) + 4];
   strcpy(hashname, filename);
   strcat(hashname, ".md5");
   char accname[strlen(filename) + 7];
   strcpy(accname, filename);
   strcat(accname, ".access");
   char coname[strlen(filename) + 3];
   strcpy(coname, filename);
   strcat(coname, ".co");

   int perm;
   
   perm = config_test(wl, filename, user);

   FILE *cof = fopen(coname, "r+");
   if(fgetc(cof) == 'o') {
      fclose(cof);
      printf("File already checked out, unable to get file.\n");
      return 2;
   }
   rewind(cof);

   if(perm == 0) {
      printf("You do not have any permissions for this file, please contact your system administrator for more information\n");
      destroy_whitelist(wl);
      //indicates no access priveleages
      return 2;
   }

   else {
      char fpass[128];
      printf("Please enter the password for this file\n");
      fgets(fpass, 128, stdin);
      if((char)fpass[strlen(fpass) - 1] != '\n')
      {
         clear();
      }
      //sets file access for operations
      setAccess(filename, 7, 0, 0);
      //write decrypted file to [filename].access
      if(decrypt(filename, fpass, hashname, accname)) {
         //sets user access to file based on rules (%8 in order to ignore superuser bit)
         setAccess(accname, perm%8, 0, 0);
         //relocks original file
         setAccess(filename, 0, 0, 0);
         fputc('o', cof);
         fclose(cof);
         printf("File pulled to %s!\n", accname);
      }
      
      else {
         setAccess(filename, 0, 0, 0);
         printf("Incorrect password, access denied\n");
         destroy_whitelist(wl);
         //indicates access denied
         return 3;
      }
   }

   destroy_whitelist(wl);

   return 0;
}

//clears stdin
void clear() {
   char c;
   while((c = getchar()) != '\n' && c != EOF);
}

