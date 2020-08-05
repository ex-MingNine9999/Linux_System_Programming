#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

char glob_str[] = "write to standard output\n";
int glob_val = 10;

int main(void)
{
	pid_t pid;
	int loc_val = 100;

	gettimeofday(&begin_t, NULL);

	if (write(STDOUT_FILENO, glob_str, sizeof(glob_str) - 1) != sizeof(glob_str) - 1) {
		fprintf(stderr, "write error for stdout\n");
		exit(1);
	}

	printf("before fork\n");

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid == 0) {
		glob_val++;
		loc_val++;
	}
	else {
		sleep(3);
	}

	printf("\npid = %d, glob_val = %d, loc_val = %d\n", getpid(), glob_val, loc_val);

	gettimeofday(&end_t, NULL);
	printf("%d's runtime", getpid());
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
