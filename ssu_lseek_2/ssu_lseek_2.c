#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define CREAT_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

char buf1[] = "123456790";
char buf2[] = "ABCDEFGHJ";

int main(void)
{
	struct timeval begin_t, end_t;
	char *fname = "ssu_hole.txt";
	int fd;

	gettimeofday(&begin_t, NULL);

	if((fd = creat(fname, CREAT_MODE)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}	

	if(write(fd, buf1, 12) != 12) {
		fprintf(stderr, "buf1 write error\n");
		exit(1);
	}

	if(lseek(fd, 15000, SEEK_SET) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}	

	if(write(fd, buf2, 12) < 0) {
		fprintf(stderr, "buf2 write error\n");
		exit(1);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
