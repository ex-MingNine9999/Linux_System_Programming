#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t;
	struct stat statbuf;

	gettimeofday(&begin_t, NULL);

	if (argc != 2) {
		fprintf(stderr, "usage : %s <file>\n", argv[0]);
		exit(1);
	}

	if ((stat(argv[1], &statbuf)) < 0) {
		fprintf(stderr, "stat error\n");
		exit(1);
	}

	printf("%s is %lld bytes\n", argv[1], statbuf.st_size);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
