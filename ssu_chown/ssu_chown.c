#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "ssu_runtime.h"

int main(void)
{
	struct stat statbuf;
	char *fname = "ssu_myfile";
	int fd;

	gettimeofday(&begin_t, NULL);

	if ((fd = open(fname, O_RDWR | O_CREAT, 0600)) < 0) {
		fprintf(stderr, "open error for %s\n", fname);
		exit(1);
	}

	close(fd);

	stat(fname, &statbuf);
	printf("# 1st stat call # UID:%d	GID:%d\n", statbuf.st_uid, statbuf.st_gid);

	if(chown(fname, 501, 300) < 0) {
		fprintf(stderr, "chown erorr for %s\n", fname);
		exit(1);
	}

	stat(fname, &statbuf);
	printf("# 2nd stat call # UID:%d	GID:%d\n", statbuf.st_uid, statbuf.st_gid);

	if (unlink(fname) < 0) {
		fprintf(stderr, "unlink error for %s\n", fname);
		exit(1);
	}

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
