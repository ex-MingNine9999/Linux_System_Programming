#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include "ssu_runtime.h"

int main(void)
{
	gettimeofday(&begin_t, NULL);
	

	printf("Proccess ID		= %d\n", getpid());
	printf("Parent proccess ID	= %d\n", getppid());
	printf("Real User ID		= %d\n", getuid());
	printf("Effective User ID	= %d\n", geteuid());
	printf("Real Group ID		= %d\n", getgid());
	printf("Effective Group ID	= %d\n", getegid());

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
