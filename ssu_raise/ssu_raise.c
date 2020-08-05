#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

void ssu_signal_handler1(int signo);
void ssu_signal_handler2(int signo);

int main(void)
{
	gettimeofday(&begin_t, NULL);

	if (signal(SIGINT, ssu_signal_handler1) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGINT\n");
		exit(EXIT_FAILURE);
	}

	if (signal(SIGUSR1, ssu_signal_handler2) == SIG_ERR) {
		fprintf(stderr, "cannot handle SIGUSR1\n");
		exit(EXIT_FAILURE);
	}

	raise(SIGINT);
	raise(SIGUSR1);

	printf("maine return\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_signal_handler1(int signo)
{
	printf("SIGINT 시그널 발생\n");
}

void ssu_signal_handler2(int signo)
{
	printf("SIGUSR1 시그널 발생\n");
}

