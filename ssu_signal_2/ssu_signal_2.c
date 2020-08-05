#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

static void ssu_signal_handler(int signo);

int main(void)
{
	gettimeofday(&begin_t, NULL);

	if (signal(SIGINT, ssu_signal_handler) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	if (signal(SIGTERM, ssu_signal_handler) == SIG_ERR) {
		fprintf(stderr, "cannot handler SIGTERM\n");
		exit(EXIT_FAILURE);
	}

	if (signal(SIGPROF, SIG_DFL) == SIG_ERR) {
		fprintf(stderr, "cannot reset SIGPROF\n");
		exit(EXIT_FAILURE);
	}

	if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
		fprintf(stderr, "cannot ignore SIGHUP\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		pause();
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

static void ssu_signal_handler(int signo) {
	if (signo == SIGINT) {
		printf("caught SIGINT\n");
	}
	else if (signo == SIGTERM) {
		printf("caught SIGTERM\n");
	}
	else {
		fprintf(stderr, "unexpected signal\n");
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
