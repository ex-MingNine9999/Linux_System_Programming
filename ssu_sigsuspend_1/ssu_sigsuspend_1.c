#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

int main(void)
{
	sigset_t old_set;
	sigset_t sig_set;

	gettimeofday(&begin_t, NULL);

	sigemptyset(&sig_set);
	sigaddset(&sig_set, SIGINT);
	sigprocmask(SIG_BLOCK, &sig_set, &old_set);
	sigsuspend(&old_set);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
