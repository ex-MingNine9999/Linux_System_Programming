#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include "ssu_runtime.h"

int main(void)
{
	sigset_t set;

	gettimeofday(&begin_t, NULL);

	sigemptyset(&set);
	sigaddset(&set, SIGINT);

	switch (sigismember(&set, SIGINT)) {
		case 1 :
			printf("SIGINT is included. \n");
			break;
		case 0 :
			printf("SIGINT is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
			break;
	}

	switch (sigismember(&set, SIGSYS)) {
		case 1 :
			printf("SIGSYS is included. \n");
			break;
		case 0 :
			printf("SIGSYS is not included. \n");
			break;
		default :
			printf("failed to call sigismember() \n");
			break;
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
