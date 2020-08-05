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
	char *env[] = {
		"NAME = value",
		"nextname=nextvalue",
		"HOME=/home/oslab",
		(char *)0
	};

	gettimeofday(&begin_t, NULL);

	printf("this is the original program\n");
	execve("./ssu_execl_test_1", argv, env);
	printf("%s\n", "this line should never get printed\n");

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
