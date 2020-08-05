#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	gettimeofday(&begin_t, NULL);

	printf("abort terminate this program\n");
	abort();
	printf("this line is never reached\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
