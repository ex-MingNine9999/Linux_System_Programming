#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval begin_t, end_t;
	char *fname = "ssu_test.txt";
	off_t fsize;
	int fd;

	gettimeofday(&begin_t, NULL);

	if ((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if ((fsize = lseek(fd, 0, SEEK_END)) < 0) {
		fprintf(stderr, "lseek error\n");
		exit(1);
	}	

	printf("The size of <%s> is %lld bytes.\n", fname, fsize);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
