#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

void ssu_out(void);

int main(void)
{
	gettimeofday(&begin_t, NULL);

	if (atexit(ssu_out)) {
		fprintf(stderr, "atexit error\n");
		exit(1);
	}

	exit(0);
}

void ssu_out(void)
{
	printf("atexit succeeded!\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);
}
