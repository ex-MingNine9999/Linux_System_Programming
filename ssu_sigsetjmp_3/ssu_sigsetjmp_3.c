#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <setjmp.h>
#include <signal.h>
#include "ssu_runtime.h"

static void ssu_signal_handler1(int signo);
static void ssu_signal_handler2(int signo);

sigjmp_buf jmp_buf1;
sigjmp_buf jmp_buf2;

int main(void)
{
	struct sigaction act_sig1;
	struct sigaction act_sig2;
	int i, ret;

	gettimeofday(&begin_t, NULL);

	printf("My PID is %d\n", getpid());
	ret = sigsetjmp(jmp_buf1, 1);

	if (ret == 0) {
		act_sig1.sa_handler = ssu_signal_handler1;
		sigaction(SIGINT, &act_sig1, NULL);
	}
	else if (ret == 3) {
		printf("------------------\n");
	}

	printf("Starting\n");
	sigsetjmp(jmp_buf2, 2);
	act_sig2.sa_handler = ssu_signal_handler2;
	sigaction(SIGUSR1, &act_sig2, NULL);

	for (i = 0; i < 20; i++) {
		printf("i = %d\n", i);
		sleep(1);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

static void ssu_signal_handler1(int signo){
	fprintf(stderr, "\nInterrupted\n");
	siglongjmp(jmp_buf1, 1);
}	

static void ssu_signal_handler2(int signo){
	fprintf(stderr, "\nSIGUSR1\n");
	siglongjmp(jmp_buf2, 2);
}
