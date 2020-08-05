
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
	
	
	char *file = "q3java.txt";
	
	/****** 두번째 매개변수 ******/
	/****** true : 기존 파일의 내용 이후부터 쓰여짐 ******/
	/****** false : 처음부터 쓰여짐 ******/
	
	int writer = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	if (writer < 0) {
		fprintf(stderr, "open error\n");
		exit(1);
	}
	
	write(writer, "2019 OSLAB\n", 11);
	write(writer, "Linux System Programming\n", 25);
	
	fsync(writer);
	printf("DONE\n");
	
	if(writer >= 0)
		close(writer);
	

	return 0;
}
