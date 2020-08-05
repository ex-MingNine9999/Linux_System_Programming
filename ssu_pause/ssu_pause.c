#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

void ssu_alarm(int signo);

int main(void)
{
	gettimeofday(&begin_t, NULL);

	printf("Alarm Setting\n");
	signal(SIGALRM, ssu_alarm);
	alarm(2);

	while (1) {
		printf("done\n");
		pause();
		alarm(2);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_alarm(int signo)
{
	printf("alarm..!!\n");
}
