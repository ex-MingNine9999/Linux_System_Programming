#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	gettimeofday(&begin_t, NULL);

	if (argc != 2) {
		fprintf(stderr, "usage: %s [Process ID]\n", argv[0]);
		exit(1);
	}
	else {
		kill(atoi(argv[1]), SIGKILL);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
