#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	extern char **environ;
	char **str;
	int i;

	gettimeofday(&begin_t, NULL);

	for (i = 0; i < argc; i++) {
		printf("argv[%d] : %s\n", i, argv[i]);
	}

	for (str = environ; *str != 0; str++) {
		printf("%s\n", *str);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
