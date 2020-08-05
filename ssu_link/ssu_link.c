#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	gettimeofday(&begin_t, NULL);

	if (argc < 3) {
		fprintf(stderr, "usage : %s <file1> <file2>\n", argv[0]);
		exit(1);
	}

	if (link(argv[1], argv[2]) == -1) {
		fprintf(stderr, "link error for %s\n", argv[1]);
		exit(1);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
