#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	int testfd;
	int fd;

	gettimeofday(&begin_t, NULL);

	fd = open("test.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	printf("fd: %d\n", fd);
	testfd = fcntl(fd, F_DUPFD, 5);
	printf("testfd : %d\n", testfd);
	testfd = fcntl(fd, F_DUPFD, 5);
	printf("testfd : %d\n", testfd);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
