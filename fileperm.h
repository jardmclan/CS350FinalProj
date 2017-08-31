/*
 * fileperm.h
 *
 *  Created on: Apr 24, 2016
 *      Author: cs
 */

#ifndef FILEPERM_H_
#define FILEPERM_H_
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

void setAccess(const char* file, int userPerm, int groupPerm, int otherPerm){


	if(!access(file, F_OK ))
		{

	    	struct stat st;
			mode_t fileperm;
			mode_t ufileperm=stat(file, &st);
			if (userPerm ==0)
				ufileperm &= ~(S_IRUSR)|~(S_IWUSR)|~(S_IXUSR);
			if (userPerm ==1)
				ufileperm=S_IXUSR;
			if (userPerm ==2)
				ufileperm=S_IWUSR;
			if (userPerm ==3)
				ufileperm=S_IWUSR|S_IXUSR;
			if (userPerm ==4)
				ufileperm = S_IRUSR;
			if (userPerm == 5)
				ufileperm = S_IRUSR|S_IXUSR;
			if (userPerm == 6)
				ufileperm = S_IRUSR|S_IWUSR;
			if (userPerm == 7)
				ufileperm = S_IRWXU;

			mode_t gfileperm=stat(file, &st);
			if (groupPerm == 0)
				gfileperm &= ~(S_IRGRP)|~(S_IWGRP)|~(S_IXGRP);
			if (groupPerm == 1)
				gfileperm = S_IXGRP;
			if (groupPerm == 2)
				gfileperm = S_IWGRP;
			if (groupPerm == 3)
				gfileperm = S_IWGRP|S_IXGRP;
			if (groupPerm == 4)
				gfileperm = S_IRGRP;
			if (groupPerm == 5)
				gfileperm = S_IRGRP|S_IXGRP;
			if (groupPerm == 6)
				gfileperm = S_IRGRP|S_IWGRP;
			if (groupPerm == 7)
				gfileperm = S_IRWXG;

			mode_t ofileperm=stat(file, &st);
			if (otherPerm == 0)
				ofileperm &= ~(S_IROTH)|~(S_IWOTH)|~(S_IXOTH);
			if (otherPerm == 1)
				ofileperm = S_IXOTH;
			if (otherPerm == 2)
				ofileperm = S_IWOTH;
			if (otherPerm == 3)
				ofileperm = S_IWOTH|S_IXOTH;
			if (otherPerm == 4)
				ofileperm = S_IROTH;
			if (otherPerm == 5)
				ofileperm = S_IROTH|S_IXOTH;
			if (otherPerm == 6)
				ofileperm = S_IROTH|S_IWOTH;
			if (otherPerm == 7)
				ofileperm = S_IRWXO;


			fileperm = ufileperm|gfileperm|ofileperm;
			//int result = 
			chmod(file, fileperm);
			//if (!(result<0))
				//printf("%s", file);
		}
		else
		{
			printf("The File %s not Found\n",file);
		}

}


#endif /* FILEPERM_H_ */
