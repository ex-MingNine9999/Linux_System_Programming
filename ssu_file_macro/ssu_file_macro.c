#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	struct stat file_info;
	char *str;
	int i;

	gettimeofday(&begin_t, NULL);

	for (i = 1; i < argc; i++) {
		printf("name = %s, type = ", argv[i]);

		if (lstat(argv[i], &file_info) < 0) {
			fprintf(stderr, "lstat error\n");
			continue;
		}

		if (S_ISREG(file_info.st_mode))
			str = "regular";
		else if (S_ISDIR(file_info.st_mode))
			str = "directroy";
		else if (S_ISCHR(file_info.st_mode))
			str = "character special";
		else if (S_ISBLK(file_info.st_mode))
			str = "block special";
		else if (S_ISFIFO(file_info.st_mode))
			str = "FIFO";
		else if (S_ISLNK(file_info.st_mode))
			str = "symbolic link";
		else if (S_ISSOCK(file_info.st_mode))
			str = "socket";
		else 
			str = "unknown mode";
			
		printf("%s\n", str);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
