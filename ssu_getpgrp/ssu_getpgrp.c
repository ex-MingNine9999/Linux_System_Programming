#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	pid_t pgid;
	pid_t pid;

	gettimeofday(&begin_t, NULL);

	pid = getpid();
	pgid = getpgrp();

	printf("pid: %d, pgid: %d\n", pid, pgid);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
