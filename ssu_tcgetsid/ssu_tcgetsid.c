#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <errno.h>
#include <sys/types.h>
#include <termios.h>
#include "ssu_runtime.h"

int main(void)
{
	pid_t sid_stderr;
	pid_t sid_stdin;
	pid_t sid_stdout;

	gettimeofday(&begin_t, NULL);

	sid_stdin = tcgetsid(STDIN_FILENO);

	if (sid_stdin == -1) {
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else {
		printf("Session Leader for stdin: %d\n", sid_stdin);
	}

	sid_stdout = tcgetsid(STDOUT_FILENO);

	if (sid_stdout == -1) {
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else {
		printf("Session Leader for stdout: %d\n", sid_stdout);
	}

	sid_stderr = tcgetsid(STDERR_FILENO);

	if (sid_stderr == -1) {
		fprintf(stderr, "tcgetsid error\n");
		exit(1);
	}
	else {
		printf("Session Leader for stderr: %d\n", sid_stderr);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
