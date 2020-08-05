#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ssu_runtime.h"

#define EXIT_CODE 1

int main(void)
{
	pid_t pid;
	int ret_val, status;

	gettimeofday(&begin_t, NULL);

	if ((pid = fork()) == 0) {
		printf("child: pid = %d ppid = %d exit_code = %d\n",
				getpid(), getppid(), EXIT_CODE);
		exit(EXIT_CODE);
	}

	printf("parent: waiting for child = %d\n", pid);
	ret_val = wait(&status);
	printf("parent: return value = %d, ", ret_val);
	printf(" child's status = %x", status);
	printf(" and shifted = %x\n", (status >> 8));
	exit(0);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
