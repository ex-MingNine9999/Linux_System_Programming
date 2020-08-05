#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main() {
	char* fname = "ssu_test.txt";
	int fd;
	struct timeval begin_t, end_t; 

	gettimeofday(&begin_t, NULL);
	
	if((fd = creat(fname, 0666)) < 0) {
		fprintf(stderr, "creat error %s\n", fname);
		exit(1);
	}
	else {
		close(fd);
		fd = open(fname, O_RDWR);
		printf("Successed!\n<%s> is new readable and writeable\n", fname);
	}
	
	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
