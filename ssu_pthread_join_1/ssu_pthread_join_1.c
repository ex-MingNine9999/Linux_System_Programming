#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "ssu_runtime.h"

void *ssu_thread(void *arg);

int main(void)
{
	pthread_t tid1, tid2;
	int thread1 = 1;
	int thread2 = 2;
	int status;

	gettimeofday(&begin_t, NULL);

	if (pthread_create(&tid1, NULL, ssu_thread, (void *)&thread1) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	if (pthread_create(&tid2, NULL, ssu_thread, (void *)&thread2) != 0) {
		fprintf(stderr, "pthread_create error\n");
		exit(1);
	}

	pthread_join(tid1, (void *)&status);
	pthread_join(tid2, (void *)&status);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void *ssu_thread(void *arg) {
	int thread_index;
	int i;

	thread_index = *((int *)arg);

	for (i = 0; i < 5; i++) {
		printf("%d : %d\n", thread_index, i);
		sleep(1);
	}

	return NULL;
}
