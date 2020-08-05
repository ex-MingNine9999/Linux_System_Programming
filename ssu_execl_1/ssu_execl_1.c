#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

int main(void)
{
	gettimeofday(&begin_t, NULL);

	printf("this is the origonal program\n");
	execl("./ssu_execl_test_1", "ssu_execl_test_1",
			"param1", "param2", "param3", (char *)0);
	printf("%s\n", "this line should never get printed\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
