#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>
#include "ssu_employee.h"
#include "ssu_runtime.h"

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t;
	struct ssu_employee record;
	int fd;
	int record_num;

	gettimeofday(&begin_t, NULL);

	if (argc < 2) {
		fprintf(stderr, "Usage : %s file\n", argv[0]);
		exit(1);
	}

	if ((fd = open(argv[1], O_RDONLY)) < 0) {
		fprintf(stderr, "open error %s\n", argv[1]);
		exit(1);
	}

	while (1) {
		printf("Enter record number : ");
		scanf("%d", &record_num);

		if (record_num < 0) {
			break;
		}

		if ((lseek(fd, (long)record_num * sizeof(record), 0)) < 0) {
			fprintf(stderr, "lseek error\n");
			exit(1);
		}

		if (read(fd, (char *)&record, sizeof(record)) > 0) {
			printf("Employee : %s Salary : %d\n", record.name, record.salary);
		}
		else {
			printf("Record %d not found\n", record_num);
		}
	}

	close(fd);

	gettimeofday(&end_t, NULL);
	ssu_runtime(&begin_t, &end_t);

	exit(0);
}
