#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	struct timeval begin_t, end_t;
	char character;
	int fd;
	int line_count = 0;

	gettimeofday(&begin_t, NULL);

	if ((fd = open("ssu_test.txt", O_RDONLY)) < 0) {
		fprintf(stderr, "open error %s\n", "ssu_test.txt");
		exit(1);
	}

	while (1) {
		if (read(fd, &character, 1) > 0) {
			if (character == '\n') {
				line_count++;
			}
		}
		else {
			break;
		}
	}

	printf("total line : %d", line_count);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
