#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>

#define NAMESIZE 50

struct employee {
	char name[NAMESIZE];
	int salary;
	int pid;
};

int main(void)
{
	int fd;
	struct employee man[5];

	strcpy(man[0].name, "HongKilDong"), man[0].salary = 1500000;
	strcpy(man[1].name, "LeeSoonSin"), man[1].salary = 1900000;
	strcpy(man[2].name, "SongSeonHoon"), man[2].salary = 450000;
	strcpy(man[3].name, "BaekMaKang"), man[3].salary = 230000;
	strcpy(man[4].name, "KimJongHoon"), man[4].salary = 1500000;


	fd = open("employeefile", O_WRONLY | O_CREAT | O_TRUNC, 0644);
	
	for (int i = 0; i < 5; i++) {
		write(fd, &(man[i]), sizeof(struct employee));
	}

	exit(0);
}
