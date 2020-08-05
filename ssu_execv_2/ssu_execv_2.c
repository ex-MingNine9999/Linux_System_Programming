#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	char *argv[] = {
		"ssu_execl_test_1", "param1", "param2", (char *)0
	};

	gettimeofday(&begin_t, NULL);

	printf("this is the original program\n");
	execv("./ssu_execl_test_1", argv);
	printf("%s\n", "This line should never get printed\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
