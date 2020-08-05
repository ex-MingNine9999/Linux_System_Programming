#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

void ssu_signal_handler(int signo);

int count = 0;

int main(void)
{
	gettimeofday(&begin_t, NULL);

	signal(SIGALRM, ssu_signal_handler);
	alarm(1);

	while (1);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_signal_handler(int signo)
{
	printf("alarm %d\n", count++);
	alarm(1);
}
