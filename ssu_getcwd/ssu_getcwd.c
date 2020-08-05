#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define PATH_MAX 1024

int main(void)
{
	char *pathname;

	gettimeofday(&begin_t, NULL);

	if (chdir("..") < 0) {
		fprintf(stderr, "chdir error\n");
		exit(1);
	}

	pathname = malloc(PATH_MAX);
	if (getcwd(pathname, PATH_MAX) == NULL) {
		fprintf(stderr, "getcwd error\n");
		exit(1);
	}

	printf("current directory : %s\n", pathname);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
