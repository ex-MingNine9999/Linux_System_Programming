#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "ssu_runtime.h"

void ssu_timestamp(char *str);

int main(void)
{
	unsigned int ret;

	gettimeofday(&begin_t, NULL);

	ssu_timestamp("before sleep()");
	ret = sleep(10);
	ssu_timestamp("after sleep()");
	printf("sleep() returned %d\n", ret);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}

void ssu_timestamp(char *str)
{
	time_t time_val;

	time(&time_val);
	printf("%s the time is %s\n", str, ctime(&time_val));
}
