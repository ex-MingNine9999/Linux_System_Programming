#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	int fd;
	char buf[500000];

	fd = open("ssu_test1.txt", O_CREAT | O_WRONLY, 0644);

	for (int i = 0; i < 500000; i++) {
		buf[i] = i % 126 + 1;
	}

	write(fd, buf, sizeof(buf));
	

	return 0;
}
