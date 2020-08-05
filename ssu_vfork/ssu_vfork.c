#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include "ssu_runtime.h"

void ssu_local_func(void);

int main(void)
{
	gettimeofday(&begin_t, NULL);

	printf("Before vfork\n");
	ssu_local_func();
	printf("After ssu_local_func, my PID is %d\n", getpid());

	gettimeofday(&end_t, NULL);

	printf("\n%d's runtime", getpid());
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_local_func(void)
{
	pid_t pid;

	if ((pid = vfork()) == 0) {
		printf("I'm child. My PID is %d\n", getpid());
	}
	else if (pid > 0) {
		sleep(3);
	}
	else {
		fprintf(stderr, "vfork error\n");
	}
}
