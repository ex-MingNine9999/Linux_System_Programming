#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	char buf[64];
	char *fname = "ssu_tmepfile";
	int fd;
	int length;

	gettimeofday(&begin_t, NULL);

	if ((fd = open(fname, O_RDWR | O_CREAT | O_TRUNC, 0600)) < 0) {
		fprintf(stderr, "first open error for %s\n", fname);
		exit(1);
	}

	if (unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	if (write(fd, "How are you?", 12) != 12) {
		fprintf(stderr, "write error\n");
		exit(1);
	}

	lseek(fd, 0, 0);

	if ((length = read(fd, buf, sizeof(buf))) < 0) {
		fprintf(stderr, "buf read error\n");
		exit(1);
	}
	else {
		buf[length] = 0;
	}

	printf("%s\n", buf);
	close(fd);

	if ((fd = open(fname, O_RDWR)) < 0) {
		fprintf(stderr, "second open error for %s\n", fname);

		gettimeofday(&end_t, NULL);
		ssu_runtime(&begin_t, &end_t);

		exit(1);
	}
	else {
		close(fd);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
