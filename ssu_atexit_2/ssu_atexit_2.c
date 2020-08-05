#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

static void ssu_exit1(void);
static void ssu_exit2(void);
static void printTime(void);

int main(void)
{
	gettimeofday(&begin_t, NULL);

	if (atexit(printTime) != 0) {
		fprintf(stderr, "printTime error\n");
		exit(1);
	}

	if (atexit(ssu_exit2) != 0) {
		fprintf(stderr, "atexit error for ssu_exit2\n");
		exit(1);
	}

	if (atexit(ssu_exit1) != 0) {
		fprintf(stderr, "atexit error for ssu_exit1\n");
		exit(1);
	}

	if (atexit(ssu_exit1) != 0) {
		fprintf(stderr, "atexit error for ssu_exit1\n");
		exit(1);
	}

	printf("done\n");

	exit(0);
}

static void ssu_exit1(void)
{
	printf("ssu_exit1 handler\n");
}

static void ssu_exit2(void)
{
	printf("ssu_exit2 handler\n");
}

static void printTime(void)
{
	gettimeofday(&end_t, NULL);

	ssu_runtime(&begin_t, &end_t);
}
