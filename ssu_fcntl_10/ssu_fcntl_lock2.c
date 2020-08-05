#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	struct flock lock;
	int fd;
	char command[100];

	gettimeofday(&begin_t, NULL);

	if ((fd = open(argv[1], O_RDWR)) == -1) {
		perror(argv[1]);
		exit(1);
	}
	lock.l_type = F_WRLCK;
	lock.l_whence = 0;
	lock.l_start = 0l;
	lock.l_len = 0l;

	if (fcntl(fd, F_SETLK, &lock) == -1) {
		if (errno == EACCES) {
			printf("%s busy -- try later\n", argv[1]);
			exit(2);
		}
		perror(argv[1]);
		exit(3);
	}

	sprintf(command, "vim %s\n", argv[1]);
	system(command);
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLK, &lock);
	close(fd);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
