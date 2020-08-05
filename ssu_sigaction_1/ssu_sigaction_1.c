#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

void ssu_signal_handler(int signo) {
	printf("ssu_signal_handler control\n");
}

int main(void)
{
	struct sigaction sig_act;
	sigset_t sig_set;

	gettimeofday(&begin_t, NULL);

	sigemptyset(&sig_act.sa_mask);
	sig_act.sa_flags = 0;
	sig_act.sa_handler = ssu_signal_handler;
	sigaction(SIGUSR1, &sig_act, NULL);
	printf("before first kill()\n");
	kill(getpid(), SIGUSR1);
	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGUSR1);
	sigprocmask(SIG_SETMASK, &sig_set, NULL);
	printf("before second kill()\n");
	kill(getpid(), SIGUSR1);
	printf("after second kill()\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
