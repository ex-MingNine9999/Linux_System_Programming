#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#ifndef MAP_FILE
#define MAP_FILE 0
#endif

void serror(char *str)
{
	fprintf(stderr, "%s\n", str);
	exit(1);
}

int main(int argc, char *argv[])
{
	int fdin, fdout;
	char *inp, *outp;
	struct stat statbuf;

	gettimeofday(&begin_t, NULL);

	if (argc != 3) {
		serror("usage: a.out [source file] [target file]");
	}

	if ((fdin = open(argv[1], O_RDONLY)) < 0) {
		serror("can't open file for reading");
	}

	if ((fdout = open(argv[2], O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
		serror("can't open file for writing");
	}

	if (fstat(fdin, &statbuf) < 0) {
		serror("fstat error");
	}

	if (lseek(fdout, statbuf.st_size - 1, SEEK_SET) == -1) {
		serror("lseek error");
	}

	if (write(fdout, " ", 1) != 1) {
		serror("write error");
	}

	if ((inp = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fdin, 0)) == (caddr_t) -1) {
		serror("mmap error for input");
	}

	if ((outp = mmap(0, statbuf.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fdout, 0)) == (caddr_t) -1) {
		serror("mmap error for output");
	}

	memcpy(outp, inp, statbuf.st_size);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
