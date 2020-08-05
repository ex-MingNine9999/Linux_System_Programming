#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

int main(int argc, char *argv[])
{
	int fdin, fdout;
	void *src, *dst;
	struct stat statbuf;

	gettimeofday(&begin_t, NULL);

	if (argc != 3) {
		fprintf(stderr, "usage : %s <fromfile> <tofile>", argv[0]);
		exit(1);
	}

	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "can't open %s for reading\n", argv[1]);
		exit(1);
	}

	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0) {
		fprintf(stderr, "can't open %s for writing\n", argv[2]);
		exit(1);
	}

	if (fstat(fdin, &statbuf) < 0) {
		fprintf(stderr, "fstat() error\n");
		exit(1);
	}

	if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1) {
		fprintf(stderr, "lseek() error\n");
		exit(1);
	}
	if (write(fdout, "", 1) != 1) {
		fprintf(stderr, "write() error\n");
		exit(1);
	}

	if ((src = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == MAP_FAILED) {
		fprintf(stderr, "mmap() error for input");
		exit(1);
	}

	if ((dst = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == MAP_FAILED) {
		fprintf(stderr, "mmap() error for output");
		exit(1);
	}

	memcpy(dst, src, statbuf.st_size);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
