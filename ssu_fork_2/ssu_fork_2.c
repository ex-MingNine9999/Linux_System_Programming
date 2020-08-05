#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	pid_t pid;
	char character, first, last;
	long i;

	gettimeofday(&begin_t, NULL);

	if ((pid = fork()) > 0) {
		first = 'A';
		last = 'Z';
	}
	else if (pid == 0) {
		first = 'a';
		last = 'z';
	}
	else {
		fprintf(stderr, "%s\n", argv[0]);
		exit(1);
	}

	for (character = first; character <= last; character++) {
		for (i = 0; i <= 100000; i++)
			;

		write(1, &character, 1);
	}

	printf("\n");

	gettimeofday(&end_t, NULL);

	printf("\n%d's runtime", getpid());
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
