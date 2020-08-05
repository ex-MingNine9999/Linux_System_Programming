#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <pthread.h>
#include "ssu_runtime.h"

#define THREAD_NUM 8

struct thread_data {
	int thread_index;
	int sum;
	char *message;
};

void *ssu_printhello(void *arg);

struct thread_data thread_data_array[THREAD_NUM];
char *message[THREAD_NUM];

int main(void)
{
	pthread_t tid[THREAD_NUM];
	int sum;
	int i;

	gettimeofday(&begin_t, NULL);

	sum = 0;
	message[0] = "English: Hello Wordl!";
	message[1] = "French: Bonjour, le monde!";
	message[2] = "Spanish: Hola al mundo";
	message[3] = "Kligon: Nuq neH!";
	message[4] = "German: Guten Tag, Welt!";
	message[5] = "Russian: Zdravstvytye, mir!";
	message[6] = "Japan: Sekai e konnichiwa!";
	message[7] = "Latin: Orbis, te saluto!";

	for (i = 0; i < THREAD_NUM; i++) {
		sum = sum + i;
		thread_data_array[i].thread_index = i;
		thread_data_array[i].sum = sum;
		thread_data_array[i].message = message[i];
		printf("Creating thread %d\n", i);

		if (pthread_create(&tid[i], NULL, ssu_printhello, (void *)&thread_data_array[i]) != 0) {
			fprintf(stderr, "pthread_create error\n");
			exit(1);
		}
	}

	sleep(5);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void *ssu_printhello(void *arg)
{
	struct thread_data *data;
	char *message;
	int thread_index;
	int sum;

	sleep(1);
	data = (struct thread_data *)arg;
	thread_index = data->thread_index;
	sum = data->sum;
	message = data->message;
	printf("Thread %d: %s Sum=%d\n", thread_index, message, sum);
	return NULL;
}
