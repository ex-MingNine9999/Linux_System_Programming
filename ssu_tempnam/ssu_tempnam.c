#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	char *arg_directory = NULL;
	char *arg_prefix = NULL;

	gettimeofday(&begin_t, NULL);

	if (argc != 3) {
		fprintf(stderr, "usage: %s <directory> <prefix>\n", argv[0]);
		exit(1);
	}

	arg_directory = argv[1][0] != ' ' ? argv[1] : NULL;
	arg_prefix = argv[2][0] != ' ' ? argv[2] : NULL;
	printf("created : %s\n", tempnam(arg_directory, arg_prefix));
	
	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
