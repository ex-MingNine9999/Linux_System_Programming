#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	char buf[BUFFER_SIZE];
	int pid;
	int pipe_fd[2];

	gettimeofday(&begin_t, NULL);

	pipe(pipe_fd);

	if ((pid = fork()) < 0) {
		fprintf(stderr, "fork error\n");
		exit(1);
	}
	else if (pid > 0) {
		printf(" PARENT: writing to the pipe \n");
		write(pipe_fd[1], "OSLAB", 6);
		printf(" PARENT: waiting\n");
		wait(NULL);
		printf("\nPARENT's ");
	}
	else {
		printf(" CHILD: reading from pipe \n");
		read(pipe_fd[0], buf, 6);
		printf(" CHILD: read \"%s\" \n", buf);

		printf("\nCHILD's ");
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
