#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval begin_t, end_t;
	char c;
	int fd;

	gettimeofday(&begin_t, NULL);

	if ((fd = open("ssu_test.txt", O_RDONLY)) < 0) {
		fprintf(stderr, "open error for %s\n", "ssu_test.txt");
		exit(1);
	}	

	while (1) {
		if (read(fd, &c, 1) > 0) {
			putchar(c);
		}
		else {
			break;
		}
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
