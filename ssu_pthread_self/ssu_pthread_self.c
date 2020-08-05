#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "ssu_runtime.h"

void *ssu_thread(void *arg);

int main(void)
{
	pthread_t tid;

	gettimeofday(&begin_t, NULL);

	if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	printf("%u\n", (unsigned int)tid);

	if (pthread_create(&tid, NULL, ssu_thread, NULL) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	printf("%u\n", (unsigned int)tid);
	
	sleep(1);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void *ssu_thread(void *arg)
{
	pthread_t tid;

	tid = pthread_self();
	printf("->%u\n", (unsigned int)tid);
	
	return NULL;
}
