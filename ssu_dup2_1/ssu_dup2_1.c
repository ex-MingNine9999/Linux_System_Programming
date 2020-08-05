#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval begin_t, end_t;
	char *fname = "ssu_test.txt";
	int fd;
	int stdoutDes;

	gettimeofday(&begin_t, NULL);
	
	printf("%c", 0);
	stdoutDes = dup(1);
	if ((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "creat error for %s\n", fname);
		exit(1);
	}

	printf("First printf is on the screen.\n");
	dup2(fd, 1);
	printf("Second printf is on this file.\n");
		
	dup2(stdoutDes, 1);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
