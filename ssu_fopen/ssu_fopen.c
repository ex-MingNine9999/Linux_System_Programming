#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	char *fname = "ssu_test.txt";
	char *mode = "r";

	gettimeofday(&begin_t, NULL);

	if (fopen(fname, mode) == NULL) {
		fprintf(stderr, "fopen error for %s\n", fname);
		exit(0);
	}
	else {
		printf("Success	!\nFilename: <%s>, mode: <%s>\n", fname, mode);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
