#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	gettimeofday(&begin_t, NULL);

	if (fork() == 0) {
		execl("/bin/echo", "echo", "this is", "messege one", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}

	if (fork() == 0) {
		execl("/bin/echo", "echo", "this is", "messege two", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}

	if (fork() == 0) {
		execl("/bin/echo", "echo", "this is", "messege three", (char *)0);
		fprintf(stderr, "exec error\n");
		exit(1);
	}

	printf("Parent program ending\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
