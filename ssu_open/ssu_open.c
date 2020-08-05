#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main() {
	char *fname = "ssu_test.txt";
	int fd;
	struct timeval begin_t, end_t;

	gettimeofday(&begin_t, NULL);

	if ((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}
	else {
		printf("Success!\nFileName : %s\nDescriptor : %d\n", fname, fd);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
