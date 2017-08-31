#include <stdio.h>
#include <unistd.h>


int main() {
   printf("Current username is %s", getlogin());
   return 0;	
}
