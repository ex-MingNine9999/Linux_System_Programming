#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <sys/stat.h>
#include "ssu_runtime.h"

int main(void)
{
	int fd, state; 

	gettimeofday(&begin_t, NULL);

	state = seteuid(0);

	if (state < 0) {
		fprintf(stderr, "seteuid error\n");
		exit(1);
	}

	if ((fd = open("ssu_test.txt", O_CREAT | O_RDWR, S_IRWXU)) < 0) {
		fprintf(stderr, "open error\n");
		exit(1);
	}

	close(fd);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
