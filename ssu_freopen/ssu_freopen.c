#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include "ssu_runtime.h"

int main(void)
{
	char *fname = "ssu_test.txt";
	int fd;
	int tmpOut;

	gettimeofday(&begin_t, NULL);

	printf("First printf : Hello OSLAB!!\n");
	
	tmpOut = dup(1);

	if ((fd = open(fname, O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	if (freopen(fname, "w", stdout) != NULL) {
		printf("Second printf : Hello OSLAB!!\n");
	}

	stdout = fdopen(tmpOut, "w");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
