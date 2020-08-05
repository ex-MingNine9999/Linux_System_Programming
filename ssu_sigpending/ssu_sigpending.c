#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

int main(void)
{
	sigset_t pendingset;
	sigset_t sig_set;
	
	int count = 0;

	gettimeofday(&begin_t, NULL);

	sigfillset(&sig_set);
	sigprocmask(SIG_SETMASK, &sig_set, NULL);

	while (1) {
		printf("count : %d\n", count++);
		sleep(1);

		if (sigpending(&pendingset) == 0) {
			if (sigismember(&pendingset, SIGINT)) {
				printf("SIGINT가 블록되어 대기중. 무한루프 종료.\n");
				break;
			}
		}
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
