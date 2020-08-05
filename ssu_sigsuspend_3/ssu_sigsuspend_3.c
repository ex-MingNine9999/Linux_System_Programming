#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <time.h>
#include "ssu_runtime.h"

void ssu_signal_handler(int signo);
void ssu_timestamp(char *str);

int main(void)
{
	struct sigaction sig_act;
	sigset_t blk_set;

	gettimeofday(&begin_t, NULL);

	sigfillset(&blk_set);
	sigdelset(&blk_set, SIGALRM);
	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler;
	sigaction(SIGALRM, &sig_act, NULL);
	ssu_timestamp("before sigsuspend()");

	alarm(5);

	sigsuspend(&blk_set);
	ssu_timestamp("after sigsuspend()");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_signal_handler(int signo)
{
	printf("int ssu_signal_handler() function\n");
}

void ssu_timestamp(char *str)
{
	time_t time_val;

	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}
