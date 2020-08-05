#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define MSG "message will be written to Terminal\n"

int main(void)
{
	int new_fd;

	gettimeofday(&begin_t, NULL);

	if ((new_fd = fcntl(STDOUT_FILENO, F_DUPFD, 3)) == -1) {
		fprintf(stderr, "Error : Copying File Descriptor\n");
		exit(1);
	}

	close(STDOUT_FILENO);
	write(3, MSG, strlen(MSG));

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
