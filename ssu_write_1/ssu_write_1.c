#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	struct timeval begin_t, end_t;
	char buf[BUFFER_SIZE];
	int length;

	gettimeofday(&begin_t, NULL);

	length = read(0, buf, BUFFER_SIZE);
	write(1, buf, length);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
