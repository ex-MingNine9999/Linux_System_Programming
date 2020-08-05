#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <fcntl.h>
#include "ssu_runtime.h"

int main(void)
{
	int flag;

	gettimeofday(&begin_t, NULL);

	if ((flag = fcntl(STDOUT_FILENO, F_DUPFD)) == -1) {
		fprintf(stderr, "Error : Checking CLOSE_ON_EXEC\n");
		exit(1);
	}

	printf("CLOSE ON EXEC flog is = %d\n", flag);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
