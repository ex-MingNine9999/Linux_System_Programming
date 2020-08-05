#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	pid_t pid;

	gettimeofday(&begin_t, NULL);

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid != 0) {
		exit(1);
	}

	printf("before pid = %d, sid = %d\n", getpid(), getsid(getpid()));
	setsid();
	printf("after pid = %d, sid = %d\n", getpid(), getsid(getpid()));

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
