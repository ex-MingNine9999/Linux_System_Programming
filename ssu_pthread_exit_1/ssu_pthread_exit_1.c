#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "ssu_runtime.h"

#define THREAD_NUM 5

void *ssu_printhello(void *arg);

int main(void)
{
	pthread_t tid[THREAD_NUM];
	int i;

	gettimeofday(&begin_t, NULL);

	for (i = 0; i < THREAD_NUM; i++) {
		printf("In main: creating thread %d\n", i);

		if (pthread_create(&tid[i], NULL, ssu_printhello, (void *)&i) != 0) {
			fprintf(stderr, "pthread_create error\n");
			exit(1);
		}
	}

	pthread_exit(NULL);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void *ssu_printhello(void *arg) 
{
	int thread_index;

	thread_index = *((int *)arg);
	printf("Hello World! It's me, thread #%d\n", thread_index);
	pthread_exit(NULL);

	return NULL;
}
