#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(void)
{
	char *fname = "ssu_test.dat";
	char name[BUFFER_SIZE];
	FILE *fp;
	int age;

	gettimeofday(&begin_t, NULL);

	fp = fopen(fname, "r");
	fscanf(fp, "%s%d", name, &age);
	fclose(fp);

	fp = fopen(fname, "w");
	fprintf(fp, "%s is %d years old\n", name, age);
	fclose(fp);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
