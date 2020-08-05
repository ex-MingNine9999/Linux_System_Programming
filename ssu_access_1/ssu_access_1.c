#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	int i;

	gettimeofday(&begin_t, NULL);

	if (argc < 2) {
		fprintf(stderr, "usage : %s <file1> <file2> ... <fileN>\n", argv[0]);
	}

	for (int i = 1; i < argc; i++) {
		if (access(argv[i], F_OK) < 0) {
			fprintf(stderr, "%s doesn't exist\n", argv[i]);
			continue;
		}

		if (access(argv[i], R_OK) == 0) {
			printf("User can read %s\n", argv[i]);
		}

		if (access(argv[i], W_OK) == 0) {
			printf("User can write %s\n", argv[i]);
		}

		if (access(argv[i], X_OK) == 0) {
			printf("User can execute %s\n", argv[i]);
		}
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
