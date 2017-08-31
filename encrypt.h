#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void encrypt(char *, char *, char *);
int decrypt(char *, char *, char *, char *);
void createMD5(char *, char *);
int leftrotate(int, int);
int verify(char *, char *);

//encrypts file with given password and stores hash in given location
void encrypt(char *file, char *password, char *hash) {
   createMD5(file, hash);
   int flen;
   char enc;
   char next;
   int pos;
   int plen = (int)strlen(password);
   int kpos = 0;
   char key[plen];
   strcpy(key, password);
   FILE *f = fopen(file, "r+b");
   //moves to end of file, and uses position to get file length
   fseek(f, 0, SEEK_END);
   flen = ftell(f);
   //moves back to start of file
   rewind(f);
   int i;
   for(i = 0; i < flen; i++) {
      pos = ftell(f);
      //gets next char
      next = fgetc(f);
      //resets position before char
      fseek(f, pos, SEEK_SET);
      //xors next character with corresponding key
      enc = next ^ key[kpos];
      //shifts key with raw data from current position
      key[kpos] = next;
      //wraps to beginning of key array if end of partition reached
      if(kpos == plen) {
         kpos = 0;
      }
      else {
         kpos++;
      }
      fputc(enc, f);
   }

   fclose(f);
}

//attempts to decrypt file with password and tests against given hash, decrypted file is outputted to dest
int decrypt(char *file, char *password, char *hash, char *dest) {
   int flen;
   char enc;
   char next;
   //int pos;
   int plen = (int)strlen(password);
   int kpos = 0;
   char key[plen];
   strcpy(key, password);
   FILE *f = fopen(file, "rb");
   FILE *de = fopen(dest, "wb");
   fseek(f, 0, SEEK_END);
   flen = ftell(f);
   rewind(f);
   int i;
   //almost the same procedure as decrypt, but recursively updates key with decrypted data
   for(i = 0; i < flen; i++) {
      next = fgetc(f);
      enc = next ^ key[kpos];
      key[kpos] = enc;
      if(kpos == plen) {
         kpos = 0;
      }
      else {
         kpos++;
      }
      fputc(enc, de);
   }
   fclose(f);
   fclose(de);

   int ret;
   //creates and verifies hash
   createMD5(dest, "temp");
   ret = verify(hash, "temp");
   //deletes decrypted file if password incorrect
   if(ret == 0) {
      remove("dest");
   }
   
   remove("temp");
   //returns successful decryption
   return ret;
}

//updates md5 digest for each 512 bit block in given file, and writes final digest to given hash file
void createMD5(char *hash, char *destination) {
   FILE *f = fopen(hash, "rb");
   FILE *dest = fopen(destination, "wb");
   int mblock[16];
   int flen;
   //setup values used in md5 update procedure
   int shift[64] = {7,12,17,22,7,12,17,22,7,12,17,22,7,12,17,22,
                    5,9,14,20,5,9,14,20,5,9,14,20,5,9,14,20,
                    4,11,16,23,4,11,16,23,4,11,16,23,4,11,16,23,
                    6,10,15,21,6,10,15,21,6,10,15,21,6,10,15,21};
   int con[64];
   int i;
   int j;
   for(i = 0; i < 64; i++) {
      con[i] = floor(2^32 * abs(sin(i + 1)));
   }
   
   int a = 0x67452301;
   int b = 0xefcdab89;
   int c = 0x98badcfe;
   int d = 0x10325476;
   int cA = a;
   int cB = b;
   int cC = c;
   int cD = d;

   int r;
   int g;

   int pos = 0;
   char word[4];

   fseek(f, 0, SEEK_END);
   flen = ftell(f);
   rewind(f);
   int blocks = ceil((double)flen/64);
   //update md5 digest for each block in file
   for(j = 0; j < blocks; j++) {
      //get blocks
      for(i = 0; i < 16; i++) {
         int k;
         //get words
         for(k = 0; k < 4; k++) {
            //while not at end of file use file characters
            if(pos < flen) {
               word[k] = fgetc(f);
            }
            //at end of file append 0's until bit length of current block = 448 (or append none if already past that)
            else {
               if(i*4 + k < 56) {
                  word[k] = 0;
               }
               //when block length >= 448 append file length mod 2^64 and use that
               else {
                  word[k] = (char)((flen*8)%(int)pow(2, 64) >> ((i*4 + k) - 56)*8);
               }
            }

            pos++;
         }
         //save words
         mblock[i] = (((((word[0] << 8) | word[1]) << 8) | word[2]) << 8) | word[3];
      }
      //md5 update procedure
      for(i = 0; i < 64; i++) {
         if(i < 16) {
            r = (cB & cC) | ((~cB) & cD);
            g = i;
         }
         else if(i < 32) {
            r = (cD & cB) | ((~cD) & cC);
            g = (5*i + 1)%16;
         }
         else if(i < 48) {
            r = cB ^ cC ^ cD;
            g = (3*i + 5)%16;
         }
         else {
            r = cC ^ (cB |(~cD));
            g = (7*i)%16;
         }
         int temp = cD;
         cD = cC;
         cC = cB;
         cB = cB + leftrotate(cA + r + con[i] + mblock[g], shift[i]);
         cA = temp;
      }
      a = a + cA;
      b = b + cB;
      c = c + cC;
      d = d + cD;
   }
   //md5 digest
   int final[4] = {a, b, c, d};
   //write digest to hash file
   fwrite(final, sizeof(int), 4, dest);
   fclose(f);
   fclose(dest);
}

//md5 procedure subroutine
int leftrotate(int x, int c) {
   return (x << c) | (x >> (32 - c));
}

//verifies if 2 hashfiles contain the same hash
int verify(char *hash1file, char *hash2file) {
   FILE *h1 = fopen(hash1file, "rb");
   FILE *h2 = fopen(hash2file, "rb");
   int flen1;
   int flen2;
   //get file length
   fseek(h1, 0, SEEK_END);     
   flen1 = ftell(h1);
   rewind(h1);
   fseek(h2, 0, SEEK_END);
   flen2 = ftell(h2);
   rewind(h2);
   //if files are not the same length return false
   if(flen1 != flen2) {
      return 0;
   }
   char a;
   char b;
   int i;
   //compare files char by char
   for(i = 0; i < flen1; i++) {
      a = fgetc(h1);
      b = fgetc(h2);
      if(a != b) {
         return 0;
      }
   }
   return 1;
   fclose(h1);
   fclose(h2);
}

