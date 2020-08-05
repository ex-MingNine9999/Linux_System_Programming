#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <string.h>
#include <sys/uio.h>
#include "ssu_runtime.h"

int main(int argc, char *argv[])
{
	struct iovec vec[2];
	char MSG1[] = "0123456789";
	char MSG2[] = "abcdefghijklmnopqrstuvwxyz";
	int str_len;

	gettimeofday(&begin_t, NULL);

	vec[0].iov_base = MSG1;
	vec[0].iov_len = strlen(MSG1);
	vec[1].iov_base = MSG2;
	vec[1].iov_len = strlen(MSG2);
	
	str_len = writev(1, vec, 2);

	printf("\n총 %d바이트 출력\n", str_len);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
